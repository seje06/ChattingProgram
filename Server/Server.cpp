
#include "pch.h"
#include "ThreadManager.h"
#include "IocpCore.h"
#include "Service.h"
#include "ClientSession.h"
#include "DBConnection.h"
#include "DBBind.h"
#include "DBConnectionPool.h"

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
    // db 연결 시도
    ASSERT_CRASH(GDBConnectionPool->Connect(1, L"DRIVER={MySQL ODBC 8.0 Unicode Driver};SERVER=localhost;PORT=3306;DATABASE=chat;UID=root;PWD=1234;"));

    DBConnection* dbConn = GDBConnectionPool->Pop();
    auto query = L"DROP TABLE IF EXISTS `chat`.`account`;";
    ASSERT_CRASH(dbConn->Execute(query));
    query = LR"(
    CREATE TABLE `chat`.`account` (
        `id` VARCHAR(30) NOT NULL,
        `password` VARCHAR(255) NOT NULL,
        `nickname` VARCHAR(30) NOT NULL,
        `create_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
        PRIMARY KEY (`id`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
)";
    ASSERT_CRASH(dbConn->Execute(query));

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
