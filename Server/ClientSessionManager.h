#pragma once

class ClientSession;

class ClientSessionManager
{
public:
	void Add(shared_ptr<ClientSession> session);
	void Remove(shared_ptr<ClientSession> session);
	void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	
private:
	Lock _lock;
	set<shared_ptr<ClientSession>> _sessions;
};

extern ClientSessionManager GSessionManager;

