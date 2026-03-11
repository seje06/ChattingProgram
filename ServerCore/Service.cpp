#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

Service::Service(ServiceType type, NetAddress address, shared_ptr<IocpCore> core, SessionFactory factory, int32_t maxSessionCount)
	: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{
	_isRunning = true;
}

Service::~Service()
{
}

void Service::CloseService()
{
	_isRunning = false;
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

shared_ptr<Session> Service::CreateSession()
{
	shared_ptr<Session> session = _sessionFactory(); // 해당 서비스에 설정해준 세션생성
	session->SetService(shared_from_this());

	if (_iocpCore->Register(session) == false)
	{
		cout << GetLastError() << endl;
		return nullptr; //세션이 보유중인 소켓 관찰시키기
	}
	return session;
}

void Service::AddSession(shared_ptr<Session> session) // 연결완료된 세션을 추가해줌
{
	WriteLockGuard lock(_lock); 
	_sessionCount++;
	_sessions.insert(session);
}

void Service::ReleaseSession(shared_ptr<Session> session) // 연결끊긴 세션 제거
{
	WriteLockGuard lock(_lock);
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

ClientService::ClientService(NetAddress targetAddress, shared_ptr<IocpCore> core, SessionFactory factory, int32_t maxSessionCount)
	: Service(ServiceType::Client, targetAddress, core,factory, maxSessionCount)
{
}

bool ClientService::Start()
{
	if (CanStart() == false) return false;

	const int32_t sessionCount = GetMaxSessionCount();
	for (int32_t i = 0; i < sessionCount; i++)
	{
		shared_ptr<Session> session = CreateSession();
		if (session->Connect() == false) return false; //서버로 연결 시도
	}

	return true;
}

ServerService::ServerService(NetAddress targetAddress, shared_ptr<IocpCore> core, SessionFactory factory, int32_t maxSessionCount)
	: Service(ServiceType::Server, targetAddress, core, factory, maxSessionCount)
{

}

bool ServerService::Start()
{
	if (CanStart() == false) return false;

	_listener = make_shared<Listener>();
	if (_listener == nullptr) return false;

	shared_ptr<ServerService> service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false) return false; // 서버의 리슨실행 후 어셉트 실행

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}
