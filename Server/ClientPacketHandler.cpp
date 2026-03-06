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
    int num;
    
    {
        WriteLockGuard guard(GTestLock);
        if (pkt.testnum() != 1) return true;
        num = GTestSep.size() + 1;
        GTestSep.insert(to_wstring(num));
    }

    function<void()> func = [session, num]() {
        DBConnection* dbConn = GDBConnectionPool->Pop();
        DBBind<4, 0> dbBind(*dbConn, L"INSERT INTO chat.account(id, password, nickname, create_date) VALUES(?, ?, ?, ?);");

        wstring id = to_wstring(num);
        dbBind.BindParam(0, (WCHAR*)id.data());
        WCHAR password[] = L"1234";
        dbBind.BindParam(1, (WCHAR*)password);
        WCHAR nickname[] = L"김세진";
        dbBind.BindParam(2, (WCHAR*)nickname);
        TIMESTAMP_STRUCT ts = { 2026, 3, 5 };
        dbBind.BindParam(3, ts);

        ASSERT_CRASH(dbBind.Execute());
        
        GDBConnectionPool->Push(dbConn);

        Protocol::S_TEST pkt;
        pkt.set_testnum(2);
        auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        session->Send(sendBuffer);
        };

    shared_ptr<Job> job = make_shared<Job>(0, std::move(func));
    shared_ptr<JobQueue> jobQueue = make_shared<JobQueue>();
    
    jobQueue->Push(job);
    JobQueue::GlobalPush(jobQueue);

    return true;
}
