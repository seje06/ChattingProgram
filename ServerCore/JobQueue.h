#pragma once
#include "Job.h"
#include "LockQueue.h"

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(uint32_t delaySeconds, CallBackFunc&& callback)
	{
		Push(make_shared<Job>(delaySeconds, std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(uint32_t delaySeconds, Ret(T::* memFunc(Args...)), Args... args) //JobQueue 클래스를 상속받은 클래스에다 자신의 함수만 가능
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(make_shared<Job>(delaySeconds, owner, memFunc, std::forward<Args>(args)...));
	}

	void ClearJobs() { _jobs.Clear(); _priorityJobs.Clear(); }

public:
	void Push(shared_ptr<Job> job);
	void Execute();

	static void GlobalPush(shared_ptr<JobQueue> jobQueue);
	static shared_ptr<JobQueue> GlobalPop();

protected:
	LockQueue<shared_ptr<Job>> _jobs;
	PriorityLockQueue<shared_ptr<Job>> _priorityJobs;
	
	atomic<uint32_t> _jobCount = 0;
	//atomic<uint32_t> _priorityJobCount = 0;

	static LockQueue<shared_ptr<JobQueue>>  GJobQueues;
};

