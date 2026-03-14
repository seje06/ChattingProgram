#pragma once
#include "PacketRespondent.h"
#include "ClientSession.h"

template<>
class PacketRespondent<Protocol::C_LOGIN>
{
public:

    PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& pkt, OUT bool& isSuccess)
	{
        RESPONSE_START(session, pkt)
            DBBind<3, 1> dbBind(*dbConn, L"SELECT id FROM chat.account WHERE id = ? And password = ? And is_online = ?;");

            wstring id = Utf8ToWstring(pkt.id());
            dbBind.BindParam(0, (WCHAR*)id.data());
            wstring pssd = Utf8ToWstring(pkt.pssd());
            dbBind.BindParam(1, (WCHAR*)pssd.data());
            bool isOnline = false;
            dbBind.BindParam(2, isOnline);

            WCHAR outId[14];
            dbBind.BindCol(0, OUT outId);

            
            Protocol::S_LOGIN pktS;
            bool canLogin;

            {
                WriteLockGuard guard(_lock); // 동시에 로그인이 가능하다고 확인하고 둘다 해당 계정으로 로그인 시도할 수 있기 때문에 select후 update완료 까진 락을 걸어줘야함.

                ASSERT_CRASH(dbBind.Execute());


                canLogin = dbBind.Fetch();
                if (canLogin)
                {
                    DBBind<2, 0> dbBind1(*dbConn, L"UPDATE chat.account SET is_online = ? WHERE id = ?;");

                    bool isOnline = true;
                    dbBind1.BindParam(0, isOnline);

                    dbBind1.BindParam(1, (WCHAR*)id.data());

                    ASSERT_CRASH(dbBind1.Execute());

                    dynamic_pointer_cast<ClientSession>(session)->SetId(pkt.id());
                }
            }
            pktS.set_issuccess(canLogin); // 해당하는 아이디 있고, 오프라인상태면 성공
            pktS.set_id(pkt.id());
            pktS.set_pssd(pkt.pssd());

            auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
            session->Send(sendBuffer);

            RESPONSE_END()
	}
private:

    static Lock _lock;
};


template<>
class PacketRespondent<Protocol::C_REGISTER>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_REGISTER& pkt, OUT bool& isSuccess)
	{
        RESPONSE_START(session, pkt)
            DBBind<1, 1> dbBind(*dbConn, L"SELECT id FROM chat.account WHERE id = ?;");

            wstring id = Utf8ToWstring(pkt.id());
            dbBind.BindParam(0, (WCHAR*)id.data());

            WCHAR outId[14];
            dbBind.BindCol(0, OUT outId);

            Protocol::S_LOGIN pktS;
          
            {
                WriteLockGuard guard(_lock); // 동시에 회원가입이 가능하다고 확인하고 둘다 해당 계정으로 가입을 시도할 수 있기 때문에 select후 Insert완료 까진 락을 걸어줘야함.

                ASSERT_CRASH(dbBind.Execute());

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
                }
            }
            auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
            session->Send(sendBuffer);

            RESPONSE_END()
	}

private:

    static Lock _lock;
};

