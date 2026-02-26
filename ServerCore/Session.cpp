#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "SendBuffer.h"

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
    _socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

void Session::Send(shared_ptr<SendBuffer> sendBuffer)
{
    if (IsConnected() == false) return;

    bool registerSend = false;

    //Send 요청이 여러개 들어올수 있어서 락을 걸어준다.
    {
        WriteLockGuard lG(_lock);

        _sendQueue.push(sendBuffer);
        if (_sendRegistered.exchange(true) == false) // 여러스레드에서 send요청이 와도 처음 요청한 쪽에서 register권한을 갖게해줌
        {
            registerSend = true;
        }
    }
    if (registerSend) RegisterSend(); // 권한을 가져서 register 실행
    
}

bool Session::Connect()
{
    return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
    if (_connected.exchange(false) == false) return;

    wcout << "Disconnect : " << cause << endl;

    RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
    return HANDLE();
}

void Session::Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes)
{
}

bool Session::RegisterConnect()
{
    if (IsConnected())
        return false;

    if (GetService()->GetServiceType() != ServiceType::Client)
        return false;

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::BindAnyAddress(_socket, 0/*남는거*/) == false)
        return false;
}

bool Session::RegisterDisconnect()
{
    return false;
}

void Session::RegisterRecv()
{
}

void Session::RegisterSend()
{
    if (IsConnected() == false) return;

    _sendEvent.owner = shared_from_this(); // disk driver로부터 완료패킷 받았을때, send완료 후 로직을 처리해줄 IocpObject인 자신을 owner로 넣어줌

    
    // 보낼 데이터를 sendEvent에 전달
    {
        WriteLockGuard lG(_lock);

        while (_sendQueue.empty() == false)
        {
            shared_ptr<SendBuffer> sendBuffer = _sendQueue.front();

            _sendQueue.pop();
            _sendEvent.sendBuffers.push_back(sendBuffer);
        }
    }  //락이 끝나고 send 요청이 생기면 다시 queue에 들어감
   
    vector<WSABUF> wsaBufs;
    wsaBufs.reserve(_sendEvent.sendBuffers.size());
    for (shared_ptr<SendBuffer> sendBuffer : _sendEvent.sendBuffers)
    {
        WSABUF wsaBuf;
        wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
        wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
        wsaBufs.push_back(wsaBuf);
    }

    DWORD numOfBytes = 0;
    if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
    {
        int32_t errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            HandleError(errorCode);
            _sendEvent.owner = nullptr;
            _sendEvent.sendBuffers.clear();
            _sendRegistered.store(false);
        }
    }
}

void Session::ProcessConnect()
{
}

void Session::ProcessDisconnect()
{
}

void Session::ProcessRecv(int32_t numOfBytes)
{
}

void Session::ProcessSend(int32_t numOfBytes)
{
}

void Session::HandleError(int32_t errorCode)
{
}
