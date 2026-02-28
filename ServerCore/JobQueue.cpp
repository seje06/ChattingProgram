#include "pch.h"
#include "JobQueue.h"

LockQueue<shared_ptr<JobQueue>> JobQueue::GJobQueues;

void JobQueue::Push(shared_ptr<Job> job)
{
	int32_t prevCount = prevCount = _jobCount.fetch_add(1);
	if (job->GetExecuteTick() <= GetTickCount64()) _jobs.Push(job);
	else _priorityJobs.Push(job);

	if (prevCount == 0)
	{
		if (LCurrentJobQueue == nullptr) Execute();
		else GlobalPush(shared_from_this());
	}
}

void JobQueue::Execute()
{
	//priority_queue는 시간이 다된것만 처리한다.
}

void JobQueue::GlobalPush(shared_ptr<JobQueue> jobQueue)
{
	GJobQueues.Push(jobQueue);

}

shared_ptr<JobQueue> JobQueue::GlobalPop()
{
	return GJobQueues.Pop();
}
