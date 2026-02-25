#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class SendBuffer;

class Session : public IocpObject
{
	enum
	{
		BUFFER_SIZE = 4096
	};

	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	Session();
	virtual ~Session();

public:
	void Send(shared_ptr<SendBuffer> sendBuffer);
	bool Connect(); // 클라가 서버쪽으로 연결
	void Disconnect(const WCHAR* cause);
	
	shared_ptr<Service> GetService() { return _service.lock(); }
	void SetService(shared_ptr<Service> service) { _service = service; }

public:

	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }
	shared_ptr<Session> GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:

	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32_t numOfBytes = 0) override;

private:

	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32_t numOfBytes);
	void ProcessSend(int32_t numOfBytes);

	void HandleError(int32_t errorCode);

protected:

	virtual void OnConnected(){ }
	virtual int32_t OnRecv(BYTE* buffer, int32_t len) { return len; }
	virtual void OnSend(int32_t len) { }
	virtual void OnDisconnected() { }

private:
	weak_ptr<Service> _service; // 순환참조 때문에 weak
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	atomic<bool> _connected = false;

private:
	Lock _lock;

	RecvBuffer _recvBuffer;

	queue<shared_ptr<SendBuffer>> _sendQueue;
	atomic<bool> _sendRegistered = false;

private:

	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};

struct PacketHeader
{
	uint16_t size;
	uint16_t id;
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	shared_ptr<PacketSession> GetPacketSession() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32_t OnRecv(BYTE* buffer, int32_t len) sealed;
	virtual void OnRecvPacket(BYTE* buffer, int32_t len) abstract;
};