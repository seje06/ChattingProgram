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
			dbBind1.BindParam(2, (WCHAR*)outRoomName);
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
		int outRoomId;
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

		{
			DBBind<2, 0> dbBind(*dbConn, L"UPDATE chat.account SET current_room_id = ? WHERE id = ?;");
			int roomId = 0;
			id = Utf8ToWstring(pkt.id());

			// 해당 유저의 룸아이디를 널로 변경.
			dbBind.BindParam(0, roomId);
			dbBind.SetParamNull(0); // 해당 파라미터 널처리
			dbBind.BindParam(1, (WCHAR*)id.data());
			ASSERT_CRASH(dbBind.Execute());
		}
			// 룸Id을 통해 룸의 유저 카운트 감소.
		{
			DBBind<1, 0> dbBind(*dbConn, L"UPDATE chat.room SET user_count = user_count - 1 WHERE room_id = ?;");
			dbBind.BindParam(0, outRoomId);
			ASSERT_CRASH(dbBind.Execute());
		}
		// 룸의 유저카운트가 0보다 작으면 제거
		{
			DBBind<1, 0> dbBind(*dbConn, L"DELETE FROM chat.room WHERE room_id = ? AND user_count <= 0;");
			dbBind.BindParam(0, outRoomId);
			ASSERT_CRASH(dbBind.Execute());
		}
		// room세션에서 id를 통해 해당 유저 제거
		auto room = RoomHandler::GetRoom(outRoomId);
		room->RemoveUser(pkt.id());
		
		// room세션에서 나갔다는 패킷 전달
		Protocol::S_ROOM_OUT pktS;
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
		session->Send(sendBuffer);

		// 남은 유저들에게 refresh room pkt 날림
		{
			Protocol::S_REFRESH_ROOM multiPktS;

			DBBind<1, 1> dbBind(*dbConn, L"SELECT id FROM chat.account WHERE current_room_id = ?;");
			dbBind.BindParam(0, outRoomId);
			WCHAR outId[14];
			dbBind.BindCol(0, outId);
			ASSERT_CRASH(dbBind.Execute());
			while (dbBind.Fetch())
			{
				multiPktS.add_userids(WCHARToUTF8(outId)); //브로드캐스트 pkt
			}
			room->BroadCast(multiPktS);
		}

		cout << "" << endl;

		RESPONSE_END()
	}
};