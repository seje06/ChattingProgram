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
            // 문자열 변환
            wstring id = Utf8ToWstring(pkt.id());
            wstring pssd = Utf8ToWstring(pkt.pssd());
            auto ts = GetCurrentTimeStamp();
            bool isOnline = false;

            Protocol::S_REGISTER pktS; // S_LOGIN 대신 S_REGISTER로 명칭 확인 필요
            pktS.set_id(pkt.id());
            pktS.set_pssd(pkt.pssd());


            // 바로 INSERT 시도 (SELECT 과정을 생략하여 DB 왕복 횟수 절반으로 감소)
            DBBind<4, 0> insertBind(*dbConn, L"INSERT INTO chat.account (id, password, create_date, is_online) VALUES(?, ?, ?, ?);");
            insertBind.BindParam(0, (WCHAR*)id.data());
            insertBind.BindParam(1, (WCHAR*)pssd.data());
            insertBind.BindParam(2, ts);
            insertBind.BindParam(3, isOnline);
            // Execute 결과로 성공/실패(중복) 판단
            if (insertBind.Execute(false))
            {
                pktS.set_issuccess(true);
            }
            else
            {
                pktS.set_issuccess(false);
            }

            // 3. 패킷 전송
            auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
            session->Send(sendBuffer);

#if MULTIPLE_CONNECT_TEST_MODE
            // 4. 테스트 카운팅 
            int currentCount = ++registerCount;
            if (currentCount % 100 == 0)
            {
                cout << currentCount << "번째 회원가입 처리 중 (ID: " << pkt.id() << ")" << endl;
            }
#endif
        RESPONSE_END()
	}

private:
#if MULTIPLE_CONNECT_TEST_MODE
    static atomic<int> registerCount;
#endif
    static Lock _lock;
};

