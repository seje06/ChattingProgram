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

    return true;
}

bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& pkt)
{
    function<void()> func = [session, pkt]() {
        DBConnection* dbConn = GDBConnectionPool->Pop();
        DBBind<3, 1> dbBind(*dbConn, L"SELECT id FROM chat.account WHERE id = ? And password = ? And is_online = ?;");

        wstring id = Utf8ToWstring(pkt.id());
        dbBind.BindParam(0, (WCHAR*)id.data());
        wstring pssd = Utf8ToWstring(pkt.pssd());
        dbBind.BindParam(1, (WCHAR*)pssd.data());
        bool isOnline = false;
        dbBind.BindParam(2, isOnline);

        WCHAR outId[14];
        dbBind.BindCol(0, OUT outId);

        ASSERT_CRASH(dbBind.Execute());
        
        Protocol::S_LOGIN pktS;
        bool canLogin = dbBind.Fetch();
        if (canLogin)
        {
            DBBind<2, 0> dbBind(*dbConn, L"UPDATE chat.account SET is_online = ? WHERE id = ?;");

            bool isOnline = true;
            dbBind.BindParam(0, isOnline);

            dbBind.BindParam(1, (WCHAR*)id.data());

            ASSERT_CRASH(dbBind.Execute());
        }
        GDBConnectionPool->Push(dbConn);

        pktS.set_issuccess(canLogin); // 해당하는 아이디 있고, 오프라인상태면 성공
        pktS.set_id(pkt.id());
        pktS.set_pssd(pkt.pssd());
        
        auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
        session->Send(sendBuffer);

        };

    shared_ptr<Job> job = make_shared<Job>(0, std::move(func));
    shared_ptr<JobQueue> jobQueue = make_shared<JobQueue>();

    jobQueue->Push(job);
    JobQueue::GlobalPush(jobQueue);

    return true;
}

bool Handle_C_REGISTER(shared_ptr<PacketSession>& session, Protocol::C_REGISTER& pkt)
{
    function<void()> func = [session, pkt]() {
        DBConnection* dbConn = GDBConnectionPool->Pop();
        DBBind<1, 1> dbBind(*dbConn, L"SELECT id FROM chat.account WHERE id = ?;");

        wstring id = Utf8ToWstring(pkt.id());
        dbBind.BindParam(0, (WCHAR*)id.data());

        WCHAR outId[14];
        dbBind.BindCol(0,OUT outId);

        ASSERT_CRASH(dbBind.Execute());

        Protocol::S_LOGIN pktS;
        bool isExist = dbBind.Fetch();
        pktS.set_issuccess(!isExist); // 해당하는 아이디 없으면 성공
        pktS.set_id(pkt.id());
        pktS.set_pssd(pkt.pssd());

        if (!isExist)
        {
            DBBind<4, 0> dbBind(*dbConn, L"INSERT INTO chat.account (id, password, create_date, is_online) VALUES(?, ?, ?, ?);");

            dbBind.BindParam(0, (WCHAR*)id.data());
            wstring passd = Utf8ToWstring(pktS.pssd());
            dbBind.BindParam(1, (WCHAR*)passd.data());
            auto ts = GetCurrentTimeStamp();
            dbBind.BindParam(2, ts);
            bool isOnline = false;
            dbBind.BindParam(3, isOnline);

            

            ASSERT_CRASH(dbBind.Execute());

            GDBConnectionPool->Push(dbConn);
        }

        auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
        session->Send(sendBuffer);

        };

    shared_ptr<Job> job = make_shared<Job>(0, std::move(func));
    shared_ptr<JobQueue> jobQueue = make_shared<JobQueue>();

    jobQueue->Push(job);
    JobQueue::GlobalPush(jobQueue);

    return true;

}
