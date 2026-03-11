#include "pch.h"
#include "RoomHandler.h"
#include "ClientSession.h"
#include "DBConnectionPool.h"
#include "DBBind.h"

Room::Room(const int& roomId, const string& userId, const shared_ptr<PacketSession>& user) : _roomId(roomId)
{
	WriteLockGuard guard(_lock);
	_users[userId] = user;
}

bool Room::AddUser(const string& userId, shared_ptr<PacketSession> user)
{
	WriteLockGuard guard(_lock);
	if (_users.size() == 0) return false;
	_users[userId] = user;

	dynamic_pointer_cast<ClientSession>(user)->SetRoom(shared_from_this());
	return true;
}

void Room::RemoveUser(const string& userId)
{
	WriteLockGuard guard(_lock);

	DBConnection* dbConn = GDBConnectionPool->Pop();
	
	wstring id = Utf8ToWstring(userId);
	{
		DBBind<2, 0> dbBind(*dbConn, L"UPDATE chat.account SET current_room_id = ? WHERE id = ?;");
		int32_t roomId = 0;
		id = Utf8ToWstring(userId);

		// 해당 유저의 룸아이디를 널로 변경.
		dbBind.BindParam(0, roomId);
		dbBind.SetParamNull(0); // 해당 파라미터 널처리
		dbBind.BindParam(1, (WCHAR*)id.data());
		ASSERT_CRASH(dbBind.Execute());
	}
	// 룸Id을 통해 룸의 유저 카운트 감소.
	{
		DBBind<1, 0> dbBind(*dbConn, L"UPDATE chat.room SET user_count = user_count - 1 WHERE room_id = ?;");
		int roomId = _roomId;
		dbBind.BindParam(0, roomId);
		ASSERT_CRASH(dbBind.Execute());
	}
	// 룸의 유저카운트가 0보다 작으면 제거
	{
		DBBind<1, 0> dbBind(*dbConn, L"DELETE FROM chat.room WHERE room_id = ? AND user_count <= 0;");
		int roomId = _roomId;
		dbBind.BindParam(0, roomId);
		ASSERT_CRASH(dbBind.Execute());
	}

	ASSERT_CRASH(_users.find(userId) != _users.end())

	dynamic_pointer_cast<ClientSession>(_users[userId])->SetRoom(nullptr);
	
	_users.erase(userId);
	
	if (_users.size() == 0)
	{
		RoomHandler::RemoveRoom(_roomId);
	}
	else
	{
		// 남은 유저들에게 refresh room pkt 날림
		
		Protocol::S_REFRESH_ROOM multiPktS;

		DBBind<1, 1> dbBind(*dbConn, L"SELECT id FROM chat.account WHERE current_room_id = ?;");
		int roomId = _roomId;
		dbBind.BindParam(0, roomId);
		WCHAR outId[14];
		dbBind.BindCol(0, outId);
		ASSERT_CRASH(dbBind.Execute());
		while (dbBind.Fetch())
		{
			multiPktS.add_userids(WCHARToUTF8(outId)); //브로드캐스트 pkt
		}
		BroadCast(multiPktS);
	}

	GDBConnectionPool->Push(dbConn);
}


shared_ptr<Room> RoomHandler::GetRoom(const int& roomName)
{
	if (_rooms.find(roomName) == _rooms.end()) return shared_ptr<Room>();

	return _rooms[roomName];
}

map<const int, shared_ptr<Room>> RoomHandler::_rooms;
Lock RoomHandler::_lock;