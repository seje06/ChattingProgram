#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
//#include "Session.h"

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
	return false;
}

void Listener::CloseSocket()
{
}

HANDLE Listener::GetHandle()
{
	return HANDLE();
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes)
{
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
}
