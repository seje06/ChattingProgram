#pragma once

#include "PacketRespondent.h"

template<>
class PacketRespondent<Protocol::C_CHAT_LOG>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_CHAT_LOG& pkt, OUT bool& isSuccess)
	{
		RESPONSE_START(session, pkt)
			const wchar_t* query = LR"(
				SELECT 
					A.current_room_id, 
					R.room_name 
				FROM 
					`chat`.`account` A
				INNER JOIN 
					`chat`.`room` R ON A.current_room_id = R.room_id
				WHERE 
					A.id = ?;
			)";
			DBBind<1, 2> dbBind(*dbConn, query); //유저가 속한 룸아이디랑 룸 이름 가져오기
			wstring id = Utf8ToWstring(pkt.id());
			dbBind.BindParam(0, id.c_str());
			WCHAR outRoomId[14];
			WCHAR outRoomName[14];
			dbBind.BindCol(0, OUT outRoomId);
			dbBind.BindCol(1, OUT outRoomName);

			ASSERT_CRASH(dbBind.Execute());
			ASSERT_CRASH(dbBind.Fetch());

			// 로그 테이블에 로그 정보 저장
			DBBind<3, 0> dbBind1(*dbConn, L"INSERT INTO chat.log (account_id, room_id, message) VALUES(?, ?, ?);");
			dbBind1.BindParam(0, id.c_str());
			dbBind1.BindParam(1, outRoomId);
			dbBind1.BindParam(2, outRoomName);
			ASSERT_CRASH(dbBind1.Execute());

			//브로드캐스트로 로그 해당 방에 뿌리기
			Protocol::S_CHAT_LOG pktS;
			pktS.set_id(pkt.id());
			pktS.set_log(pkt.log());
			shared_ptr<Room> room = RoomHandler::GetRoom(outRoomName);
			room->BroadCast(pktS);

		RESPONSE_END()
	}
};

template<>
class PacketRespondent<Protocol::C_ROOM_OUT>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_ROOM_OUT& pkt, OUT bool& isSuccess)
	{
		RESPONSE_START(session, pkt)
			// 아이디, 룸 네임, 받아야함 
			// 해당 유저의 룸아이디를 널로 변경.
			// 
			// 이후 룸네임을 통해 룸의 유저 카운트 감소.
			// room에서 id를 통해 해당 유저 제거
			// 남은 유저들에게 refresh room pkt 날림
			Protocol::S_REFRESH_ROOM pkt; pkt.set_userids();
			DBBind<1, 1> dbBind3(*dbConn, L"SELECT id FROM chat.account WHERE current_room_id = ?;");
			dbBind3.BindParam(0, outRoomId);
			WCHAR outId[14];
			dbBind3.BindCol(0, outId);
			ASSERT_CRASH(dbBind3.Execute());
			while (dbBind3.Fetch())
			{
				pktS.add_userids(WCHARToUTF8(outId));
				multiPktS.add_userids(WCHARToUTF8(outId)); //브로드캐스트 pkt
			}

		RESPONSE_END()
	}
};