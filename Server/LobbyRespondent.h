#pragma once

// 임시
namespace Protocol
{
	class C_REFRESH
	{
	public:
	};
	class C_CREATE_ROOM
	{
	public:
		const string& roomname() const { return string(); }
		void set_roomname(const string& name) {}
	};
	class S_CREATE_ROOM
	{
	public:
		bool issuccess() { return true; }
		void set_issuccess(bool isSuccess) { ; }

		const string& roomname() const { return string(); }
		void set_roomname(const string& name) {}
	};
	class C_JOIN_ROOM
	{
	public:
		const string& id() const { return string(); }
		void set_id(const string& name){}
		const string& roomname() const { return string(); }
		void set_roomname(const string& name) {}
	};
	class S_JOIN_ROOM
	{
	public:
		bool issuccess() { return true; }
		void set_issuccess(bool isSuccess) { ; }

		const string& roomname() const { return string(); }
		void set_roomname(const string& name) {}
	};
}

template<>
class PacketRespondent<Protocol::C_REFRESH>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_REFRESH& pkt, OUT bool& isSuccess) 
	{
		RESPONSE_START(session, pkt)
			

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
				DBBind<1, 0> dbBind(*dbConn, L"INSERT INTO chat.room (room_name) VALUES(?);");
				dbBind.BindParam(0, (WCHAR*)roomName.data());
			}

			//auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
			//session->Send(sendBuffer);

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

			Protocol::S_CREATE_ROOM pktS;
			pktS.set_roomname(pkt.roomname());
			bool canEnter = dbBind.Fetch(); // 같은 이름의 방이 존재해야 입장가능 
			pktS.set_issuccess(canEnter);
			if (canEnter)
			{
				// 유저 정보 업데이트
				DBBind<2, 0> dbBind(*dbConn, L"UPDATE chat.account SET current_room_id = ? WHERE id = ?;");
				dbBind.BindParam(0, outRoomId);
				wstring id = Utf8ToWstring(pkt.id());
				dbBind.BindParam(1, (WCHAR*)id.data());
				ASSERT_CRASH(dbBind.Execute());
				// 룸 업데이트
				DBBind<1, 0> dbBind(*dbConn, L"UPDATE chat.room SET user_count = `user_count` + 1 WHERE room_id = ?;");
				dbBind.BindParam(0, outRoomId);
				ASSERT_CRASH(dbBind.Execute());
			}

		//auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pktS);
		//session->Send(sendBuffer);

		RESPONSE_END()
	}
};
