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
			dbBind.BindParam(0, (WCHAR*)id.data());
			int outRoomId;
			WCHAR outRoomName[14];
			dbBind.BindCol(0, OUT outRoomId);
			dbBind.BindCol(1, OUT outRoomName);

			ASSERT_CRASH(dbBind.Execute());
			ASSERT_CRASH(dbBind.Fetch());

			// 로그 테이블에 로그 정보 저장
			DBBind<3, 0> dbBind1(*dbConn, L"INSERT INTO chat.log (account_id, room_id, message) VALUES(?, ?, ?);");
			dbBind1.BindParam(0, (WCHAR*)id.data());
			dbBind1.BindParam(1, outRoomId);
			wstring log = Utf8ToWstring(pkt.log());
			dbBind1.BindParam(2, (WCHAR*)log.data());
			ASSERT_CRASH(dbBind1.Execute());

			//브로드캐스트로 로그 해당 방에 뿌리기
			Protocol::S_CHAT_LOG pktS;
			pktS.set_id(pkt.id());
			pktS.set_log(pkt.log());
			shared_ptr<Room> room = RoomHandler::GetRoom(outRoomId);
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
		int32_t outRoomId;
		// 룸 아이디 가져오기
		wstring id = Utf8ToWstring(pkt.id());
		{
			DBBind<1, 1> dbBind(*dbConn, L"SELECT current_room_id FROM chat.account WHERE id = ?;");
			
			dbBind.BindParam(0, (WCHAR*)id.data());
			dbBind.BindCol(0, OUT outRoomId);
			ASSERT_CRASH(dbBind.Execute());
			ASSERT_CRASH(dbBind.Fetch());
			ASSERT_CRASH(!dbBind.IsColNull(0));
		}

		// room세션에서 id를 통해 해당 유저 제거 후 브로드캐스트
		auto room = RoomHandler::GetRoom(outRoomId);
		room->RemoveUser(pkt.id());
		
		// room세션에서 나갔다는 패킷 전달
		Protocol::S_ROOM_OUT pktS;
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
		session->Send(sendBuffer);

		RESPONSE_END()
	}
};