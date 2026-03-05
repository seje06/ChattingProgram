#include "pch.h"
#include "ClientPacketHandler.h"
#include "DBConnectionPool.h"
#include "DBBind.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
{
    return false;
}

bool Handle_C_TEST(shared_ptr<PacketSession>& session, Protocol::C_TEST& pkt)
{
    {
        WriteLockGuard guard(GTestLock);
        if (GTestSep.size() > 0) return true;
        GTestSep.insert(L"user1");
    }

    function<void()> func = []() {
        DBConnection* dbConn = GDBConnectionPool->Pop();
        DBBind<4, 0> dbBind(*dbConn, L"INSERT INTO chat.account(id, password, nickname) VALUES(?, ?, ?, ?); ");

        WCHAR id[30] = L"user1";
        dbBind.BindParam(0, id);
        WCHAR password[30] = L"1234";
        dbBind.BindParam(1, password);
        WCHAR nickname[30] = L"김세진";
        dbBind.BindParam(2, nickname);
        TIMESTAMP_STRUCT ts = { 2026, 3, 5 };
        dbBind.BindParam(3, ts);

        ASSERT_CRASH(dbBind.Execute());

        GDBConnectionPool->Push(dbConn);
        };

    shared_ptr<Job> job = make_shared<Job>(0, std::move(func));
    shared_ptr<JobQueue> jobQueue = make_shared<JobQueue>();
    
    jobQueue->Push(job);
    JobQueue::GlobalPush(jobQueue);

    return true;
}
