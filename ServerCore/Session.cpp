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
    return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes)
{
    switch (iocpEvent->eventType)
    {
    case EEventType::Connect:
        ProcessConnect();
        break;
    case EEventType::Disconnect:
        ProcessDisconnect();
        break;
    case EEventType::Recv:
        ProcessRecv(numOfBytes);
        break;
    case EEventType::Send:
        ProcessSend(numOfBytes);
        break;
    default:
        break;
    }
}

bool Session::RegisterConnect()
{
    if (IsConnected()) return false;

    if (GetService()->GetServiceType() != ServiceType::Client) return false; //먼저 연결을 시도하는건 클라이언트이기 때문에 서비스 타입이 서버면 잘못된 요청이다.

    if (SocketUtils::SetReuseAddress(_socket, true) == false) return false;

    if (SocketUtils::BindAnyAddress(_socket, 0) == false) return false;

    _connectEvent.Init();
    _connectEvent.owner = shared_from_this();

    DWORD numOfBytes = 0;
    SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
    if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
    {
        int32_t errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            _connectEvent.owner = nullptr;
            return false;
        }
    }

    return true;
}

bool Session::RegisterDisconnect()
{
    _disconnectEvent.Init();
    _disconnectEvent.owner = shared_from_this();

    if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
    {
        int32_t errorCode = WSAGetLastError();
        if(errorCode != WSA_IO_PENDING)
        {
            _disconnectEvent.owner = nullptr;
            return false;
        }
    }

    return true;
}

void Session::RegisterRecv()
{
    if (IsConnected() == false) return;
    
    _recvEvent.Init();
    _recvEvent.owner = shared_from_this();

    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos()); // 쓸 위치 
    wsaBuf.len = _recvBuffer.FreeSize(); // 읽을수있는길이

    DWORD numOfBytes = 0;
    DWORD flags = 0;
    if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
    {
        int32_t errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            HandleError(errorCode);
            _recvEvent.owner = nullptr;
        }
    }

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
    _connectEvent.owner = nullptr;

    _connected.store(true);

    // 세션 등록
    GetService()->AddSession(GetSessionRef());

    // 컨텐츠 코드에서 재정의
    OnConnected();

    // 수신 등록
    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    _disconnectEvent.owner = nullptr;

    OnDisconnected(); // 컨텐츠 코드에서 재정의
    GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32_t numOfBytes) 
{
    _recvEvent.owner = nullptr; // RELEASE_REF 

    if (numOfBytes == 0)
    {
        Disconnect(L"Recv 0");
        return;
    }

    if (_recvBuffer.OnWrite(numOfBytes) == false)
    {
        Disconnect(L"OnWrite Overflow");
        return;
    }

    int32_t dataSize = _recvBuffer.DataSize();
    int32_t processLen = OnRecv(_recvBuffer.ReadPos(), dataSize); // 컨텐츠 코드에서 재정의
    if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
    {
        Disconnect(L"OnRead Overflow");
        return;
    }

    // 커서 정리
    _recvBuffer.Clean();

    // 수신 등록
    RegisterRecv();
}

void Session::ProcessSend(int32_t numOfBytes)
{
    _sendEvent.owner = nullptr; 
    _sendEvent.sendBuffers.clear();  

    if (numOfBytes == 0)
    {
        Disconnect(L"Send 0");
        return;
    }

    // 컨텐츠 코드에서 재정의
    OnSend(numOfBytes);

    WriteLockGuard lG(_lock);
    //보내고 보낸것을 확인할때까진 send요청을 못하게 하고(대신 queue에 적재시킴), 큐가 비어있으면 권한을 풀고, 아니면 해당 스레드가 다시 send요청을 한다.
    if (_sendQueue.empty()) _sendRegistered.store(false); 
    else RegisterSend();
}

void Session::HandleError(int32_t errorCode)
{
    switch (errorCode)
    {
    case WSAECONNRESET:
    case WSAECONNABORTED:
        Disconnect(L"HandleError");
        break;
    default:
        // TODO : Log
        cout << "Handle Error : " << errorCode << endl;
        break;
    }
}


///// PacketSession /////

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32_t PacketSession::OnRecv(BYTE* buffer, int32_t len)
{
    int32_t processLen = 0;

    while (true)
    {
        int32_t dataSize = len - processLen;
        // 최소한 헤더는 파싱할 수 있어야 한다
        if (dataSize < sizeof(PacketHeader)) break;

        PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
        // 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
        if (dataSize < header.size) break;

        // 패킷 조립 성공
        OnRecvPacket(&buffer[processLen], header.size); // 자식 패킷세션들이 처리

        processLen += header.size;
    }

    return processLen;
}


