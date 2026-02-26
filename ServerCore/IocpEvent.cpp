#include "pch.h"
#include "IocpEvent.h"

IocpEvent::IocpEvent(EEventType type) : eventType(type)
{
	Init();
}

void IocpEvent::Init()
{
	// 쓰레기값을 읽지않게 초기화해준다.
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
