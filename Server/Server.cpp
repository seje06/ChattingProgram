
#include "pch.h"
#include "ThreadManager.h"
#include "IocpCore.h"
#include "Service.h"
#include "ClientSession.h"
#include "DBConnection.h"
#include "DBBind.h"
#include "DBConnectionPool.h"
#include "ClientPacketHandler.h"
#include <cstdlib>

enum
{
    WORKER_TICK = 64
};

void WorkerThread(shared_ptr<ServerService>& service)
{
    while (service->IsRunning())
    {
        LEndTickCount = ::GetTickCount64() + WORKER_TICK;

        service->GetIocpCore()->Dispatch(10);

        JobQueue::ExcuteGlobalJobs();
    }
}

shared_ptr<ServerService> service;

void OnClose() 
{
    service->CloseService();

    GThreadManager->Join();

    delete GCoreGlobal;
}

int main()
{
    SetConsoleOutputCP(65001);

    GCoreGlobal = new CoreGlobal();

    // db 연결 시도
    ASSERT_CRASH(GDBConnectionPool->Connect(30, L"DRIVER={MySQL ODBC 8.0 Unicode Driver};SERVER=localhost;PORT=3306;DATABASE=chat;UID=root;PWD=1234;"));

    DBConnection* dbConn = GDBConnectionPool->Pop();
    const wchar_t * query;

    query = L"DROP TABLE IF EXISTS `chat`.`log`;";
    ASSERT_CRASH(dbConn->Execute(query,true));
    query = L"DROP TABLE IF EXISTS `chat`.`account`;";
    ASSERT_CRASH(dbConn->Execute(query, true));
    query = L"DROP TABLE IF EXISTS `chat`.`room`;";
    ASSERT_CRASH(dbConn->Execute(query, true));
    
    // 룸 테이블 생성
    query = LR"(
    CREATE TABLE IF NOT EXISTS `chat`.`room` (
        `room_id` INT NOT NULL AUTO_INCREMENT,
        `room_name` VARCHAR(30) NOT NULL,
        `user_count` INT NOT NULL DEFAULT 1,
        PRIMARY KEY (`room_id`),
        UNIQUE KEY `uq_room_name` (`room_name`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
    )";
    ASSERT_CRASH(dbConn->Execute(query, true));

    // 계정 테이블 생성
    query = LR"(
        CREATE TABLE IF NOT EXISTS `chat`.`account` (
        `id` VARCHAR(30) NOT NULL,
        `password` VARCHAR(255) NOT NULL,
        `create_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
        `is_online` BOOL NOT NULL DEFAULT 0,
        `current_room_id` INT NULL,
        PRIMARY KEY (`id`),
        INDEX `idx_account_current_room_id` (`current_room_id`),
        CONSTRAINT `fk_account_room`
            FOREIGN KEY (`current_room_id`) 
            REFERENCES `chat`.`room` (`room_id`) 
            ON DELETE SET NULL
            ON UPDATE CASCADE
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
)";
    ASSERT_CRASH(dbConn->Execute(query, true));

    //채팅 기록 테이블 생성
    query = LR"(
    CREATE TABLE IF NOT EXISTS `chat`.`log` (
        `log_id` INT NOT NULL AUTO_INCREMENT,
        `account_id` VARCHAR(30),
        `room_id` INT,
        `message` TEXT NOT NULL,
        `send_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
        PRIMARY KEY (`log_id`),
        CONSTRAINT `fk_log_account`
            FOREIGN KEY (`account_id`) REFERENCES `chat`.`account` (`id`)
            ON DELETE CASCADE ON UPDATE CASCADE,
        CONSTRAINT `fk_log_room`
            FOREIGN KEY (`room_id`) REFERENCES `chat`.`room` (`room_id`)
            ON DELETE CASCADE ON UPDATE CASCADE
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
)";
    ASSERT_CRASH(dbConn->Execute(query, true));

    GDBConnectionPool->Push(dbConn);

    ClientPacketHandler::Init();

    service =  std::make_shared<ServerService>(
        NetAddress(L"127.0.0.1", 7777),
        make_shared<IocpCore>(),
        make_shared<ClientSession>,
        100);
    atexit(OnClose);
    ASSERT_CRASH(service->Start());

    for (int32_t i = 0; i < 10; i++)
    {
        GThreadManager->Launch([]()
            {
                WorkerThread(service);
            });
    }

    WorkerThread(service);
}
