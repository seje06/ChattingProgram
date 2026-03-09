#pragma once
#include "PacketRespondent.h"

template<>
class PacketRespondent<Protocol::C_REFRESH_LOBBY>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_REFRESH_LOBBY& pkt, OUT bool& isSuccess)
	{
		RESPONSE_START(session, pkt)
			Protocol::S_REFRESH_LOBBY pktS;
			pktS.set_issuccess(true);
			DBBind<0, 2> dbBind(*dbConn, L"SELECT room_name, user_count FROM chat.room;");
			WCHAR outRoomName[14];
			int64_t outUser;
			dbBind.BindCol(0, outRoomName);
			dbBind.BindCol(1, outUser);
			ASSERT_CRASH(dbBind.Execute());

			while (dbBind.Fetch())
			{
				pktS.add_roomnames(WCHARToUTF8(outRoomName));
				pktS.add_usercounts(outUser);
			}
			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
			session->Send(sendBuffer);

		RESPONSE_END()
	}
};

template<>
class PacketRespondent<Protocol::C_CREATE_ROOM>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_CREATE_ROOM& pkt, OUT bool& isSuccess)
	{
		RESPONSE_START(session, pkt)
			DBBind<1, 1> dbBind(*dbConn, L"SELECT room_id FROM chat.room WHERE room_id = ?;");
			wstring roomName = Utf8ToWstring(pkt.roomname());
			dbBind.BindParam(0, (WCHAR*)roomName.data());
			WCHAR outRoomId[14];
			dbBind.BindCol(0, OUT outRoomId);

			ASSERT_CRASH(dbBind.Execute());

			Protocol::S_CREATE_ROOM pktS;
			pktS.set_roomname(pkt.roomname());
			bool canCreate = !dbBind.Fetch(); // 같은 이름의 방이 존재하지 않아야 생성가능
			pktS.set_issuccess(canCreate);
			if (canCreate)
			{
				//룸 생성
				DBBind<1, 0> dbBind1(*dbConn, L"INSERT INTO chat.room (room_name) VALUES(?);");
				dbBind1.BindParam(0, (WCHAR*)roomName.data());
				ASSERT_CRASH(dbBind.Execute());
				//유저 정보 업데이트
				DBBind<2, 0> dbBind2(*dbConn, L"UPDATE chat.account SET current_room_id = ? WHERE id = ?;");
				dbBind2.BindParam(0, outRoomId);
				wstring id = Utf8ToWstring(pkt.id());
				dbBind2.BindParam(1, (WCHAR*)id.data());
				ASSERT_CRASH(dbBind2.Execute());

				shared_ptr<Room> room = make_shared<Room>(pkt.roomname(), pkt.id(), session);
				RoomHandler::AddRoom(room);
			}

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
			session->Send(sendBuffer);

		RESPONSE_END()
	}
};

template<>
class PacketRespondent<Protocol::C_JOIN_ROOM>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_JOIN_ROOM& pkt, OUT bool& isSuccess)
	{
		RESPONSE_START(session, pkt)
			DBBind<1, 2> dbBind(*dbConn, L"SELECT room_id, room_name FROM chat.room WHERE room_name = ?;");
			wstring roomName = Utf8ToWstring(pkt.roomname());
			dbBind.BindParam(0, (WCHAR*)roomName.data());
			WCHAR outRoomId[14];
			dbBind.BindCol(0, OUT outRoomId);
			WCHAR outRoomName[14];
			dbBind.BindCol(1, OUT outRoomName);

			ASSERT_CRASH(dbBind.Execute());

			Protocol::S_JOIN_ROOM pktS;
			pktS.set_roomname(pkt.roomname());
			bool canJoin = dbBind.Fetch(); // 같은 이름의 방이 존재해야 입장가능 
			pktS.set_issuccess(canJoin);
			if (canJoin)
			{
				Protocol::S_REFRESH_ROOM multiPktS;


				// 유저 정보 업데이트
				DBBind<2, 0> dbBind1(*dbConn, L"UPDATE chat.account SET current_room_id = ? WHERE id = ?;");
				dbBind1.BindParam(0, outRoomId);
				wstring id = Utf8ToWstring(pkt.id());
				dbBind1.BindParam(1, (WCHAR*)id.data());
				ASSERT_CRASH(dbBind1.Execute());
				// 룸 업데이트
				DBBind<1, 0> dbBind2(*dbConn, L"UPDATE chat.room SET user_count = `user_count` + 1 WHERE room_id = ?;");
				dbBind2.BindParam(0, outRoomId);
				ASSERT_CRASH(dbBind2.Execute());
				// 방에들어갈테니 유저 이름들 pktS에 넣어주기
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
				// 룸에 세션을 추가해주고, 룸정보 업데이트되었으니까 브로드캐스트해주기
				shared_ptr<Room> room = RoomHandler::GetRoom(pkt.roomname());
				ASSERT_CRASH(room);
				room->AddUser(pkt.id(), session);
				room->BroadCast(multiPktS, session);
			}

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
		session->Send(sendBuffer);

		RESPONSE_END()
	}
};
