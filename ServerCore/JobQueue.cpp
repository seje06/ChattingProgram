#include "pch.h"
#include "JobQueue.h"

LockQueue<shared_ptr<JobQueue>> JobQueue::GJobQueues;

void JobQueue::Push(shared_ptr<Job> job)
{
	int32_t prevCount =  _jobCount.fetch_add(1);
	if (job->GetExecuteTick() <= GetTickCount64()) _jobs.Push(job);
	else _priorityJobs.Push(job);

	if (prevCount == 0) // 큐 실행하는 스레드 없을때만
	{
		// 해당 스레드가 excute상태에서 한번더 Push를 통해 들어오게되서 excute를 또 처리하게 되면, 한 스레드가 너무 오래 작업하기 때문에 그것을 방지
		if (LCurrentJobQueue == nullptr) Execute();
		else GlobalPush(shared_from_this());
	}
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;
	while (true)
	{
		vector<shared_ptr<Job>> jobs;
		_jobs.PopAll(OUT jobs);

		uint32_t jobCount = jobs.size();
		for (int i = 0; i < jobCount; i++)
		{
			jobs[i]->Execute();
		}

		while (shared_ptr<Job> job = _priorityJobs.Top()) // delay가 존재하는 job처리
		{
			if (job->GetExecuteTick() > GetTickCount64()) break; // 젤 위에 있는게 딜레이가 아직 남아있으면 break;
			job->Execute();
			_priorityJobs.Pop();
			jobCount++;
		}

		if (_jobCount.fetch_sub(jobCount) == jobCount) // 다처리했으면 리턴
		{
			LCurrentJobQueue = nullptr;
			return;
		}
		else if (LEndTickCount <= GetTickCount64()) // 처리는 다 못했지만 할당된 작업시간이 다됐으면 글로벌 큐에 넣어놓고 리턴
		{
			GlobalPush(shared_from_this());
			LCurrentJobQueue = nullptr;
			return;
		}
	}
}

void JobQueue::ExcuteGlobalJobs()
{
	while (shared_ptr<JobQueue> jobQueue = GlobalPop())
	{
		jobQueue->Execute();

		if (LEndTickCount <= GetTickCount64()) //할당된 작업시간이 다됐으면 리턴
		{
			return;
		}
	}
}
