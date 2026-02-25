#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

Service::Service(ServiceType type, NetAddress address, shared_ptr<IocpCore> core, SessionFactory factory, int32_t maxSessionCount)
	: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
}

void Service::CloseService()
{
	// TODO
}

void Service::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	WriteLockGuard lG(_lock);
	for (const auto& session : _sessions)
	{
		session->Send(sendBuffer);
	}
}