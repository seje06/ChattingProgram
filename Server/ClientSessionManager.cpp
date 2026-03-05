#include "pch.h"
#include "ClientSessionManager.h"
#include "ClientSession.h"

ClientSessionManager GSessionManager;

void ClientSessionManager::Add(shared_ptr<ClientSession> session)
{
	WriteLockGuard guard(_lock);
	_sessions.insert(session);
}

void ClientSessionManager::Remove(shared_ptr<ClientSession> session)
{
	WriteLockGuard guard(_lock);
	_sessions.erase(session);
}

void ClientSessionManager::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	WriteLockGuard guard(_lock);

	for (shared_ptr<ClientSession> session : _sessions)
	{
		session->Send(sendBuffer);
	}
}
