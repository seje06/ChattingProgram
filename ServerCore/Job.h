#pragma once
#include <functional>

using CallBackFunc = std::function<void()>;

class Job
{
public:
	Job(uint32_t delaySeconds, CallBackFunc&& callBack);

	template<typename T, typename Ret, typename... Args>
	Job(uint32_t delaySeconds, shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_executeTick = GetTickCount64() + delaySeconds; // 이후 실행될 시간 저장
		_callBack = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}

	bool operator <(const Job& job) const
	{
		return this->_executeTick > job._executeTick;
	}

	uint64_t GetExecuteTick() { return _executeTick; }

private:

	uint64_t _executeTick;
	CallBackFunc _callBack;
};

