#include "pch.h"
#include "Job.h"

Job::Job(uint32_t delaySeconds, CallBackFunc&& callBack) : _callBack(std::move(callBack))
{
	_executeTick = GetTickCount64() + delaySeconds; // 이후 실행될 시간 저장
}
