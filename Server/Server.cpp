
#include "pch.h"
#include "ThreadManager.h"
#include "IocpCore.h"
#include "Service.h"
#include "ClientSession.h"


enum
{
    WORKER_TICK = 64
};

void WorkerThread(shared_ptr<ServerService>& service)
{
    while (true)
    {
        LEndTickCount = ::GetTickCount64() + WORKER_TICK;

        service->GetIocpCore()->Dispatch(10);

        JobQueue::ExcuteGlobalJobs();
    }
}

int main()
{
    shared_ptr<ServerService> service(new ServerService(
        NetAddress(L"127.0.0.1", 7777),
        make_shared<IocpCore>(),
        make_shared<ClientSession>,
        100));

    ASSERT_CRASH(service->Start());

    for (int32_t i = 0; i < 5; i++)
    {
        GThreadManager->Launch([&service]()
            {
                WorkerThread(service);
            });
    }

    WorkerThread(service);
    
    GThreadManager->Join();
}
