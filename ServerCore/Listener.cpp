#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		delete acceptEvent;
	}
}

bool Listener::StartAccept(shared_ptr<ServerService> service)
{
	_service = service;
	if (_service == nullptr) return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET) return false;

	if (_service->GetIocpCore()->Register(shared_from_this()) == false) return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false) return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false) return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false) return false;

	if (SocketUtils::Listen(_socket) == false) return false;

	const int32_t acceptCount = _service->GetMaxSessionCount();
	for (int32_t i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = new AcceptEvent();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes)
{
	ASSERT_CRASH(iocpEvent->eventType == EEventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	shared_ptr<Session> session = _service->CreateSession(); //클라세션
	
	acceptEvent->Init();
	acceptEvent->session = session;


	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32_t errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent) // 연결된 클라의 세션을 설정한후 recv를 이후 걸어주고,  해당 acceptEvent는 재활용된다.
{
	shared_ptr<Session> session = acceptEvent->session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32_t sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}
