#pragma once

#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8_t
{
	Server, // 서버입장 - 특징은 연결을 받는쪽
	Client // 클라입장 - 특징은 연결을 시도하는쪽
};

class SendBuffer;
class Session;

using SessionFactory = function<shared_ptr<Session>(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, shared_ptr<IocpCore> core, SessionFactory factory, int32_t maxSessionCount = 1);
	virtual ~Service();
	virtual bool Start() abstract;
	bool CanStart() { return _sessionFactory != nullptr; }

	virtual void CloseService();
	void SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	shared_ptr<Session> CreateSession();
	void AddSession(shared_ptr<Session> session);
	void ReleaseSession(shared_ptr<Session> session);
	int32_t	GetCurrentSessionCount() { return _sessionCount; }
	int32_t	GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType	GetServiceType() { return _type; }
	NetAddress GetNetAddress() { return _netAddress; }
	shared_ptr<IocpCore>& GetIocpCore() { return _iocpCore; }

protected:
	Lock _lock;
	ServiceType	_type;
	NetAddress _netAddress = {};
	shared_ptr<IocpCore> _iocpCore;

	set<shared_ptr<Session>> _sessions;
	int32_t	_sessionCount = 0;
	int32_t	_maxSessionCount = 0;
	SessionFactory _sessionFactory;
};


////// ClientService //////

class ClientService : public Service
{
	ClientService(NetAddress targetAddress, shared_ptr<IocpCore> core, SessionFactory factory, int32_t maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool Start() override;
};


///// ServerService /////

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, shared_ptr<IocpCore> core, SessionFactory factory, int32_t maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool Start() override;
	virtual void CloseService() override;

private:
	shared_ptr<Listener> _listener = nullptr;
};