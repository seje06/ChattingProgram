#pragma once

enum class EEventType : uint8_t
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

class IocpEvent : public OVERLAPPED // iocp 결과를 식별하기 위한 구조체, 커널은 IO작업 에 필요한 데이터를 읽거나 과정또는 결과를 쓴다.
{
public:
	IocpEvent(EEventType type);
public:
	EEventType eventType;
	shared_ptr<class IocpObject> owner;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EEventType::Connect) {}
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EEventType::Disconnect) {}
};

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EEventType::Accept) {}

public:
	//SessionRef	session = nullptr;
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EEventType::Recv) {}
};

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EEventType::Send) {}

	vector<shared_ptr<class SendBuffer>> sendBuffers;
};