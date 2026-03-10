#include "pch.h"
#include "RoomHandler.h"

bool Room::AddUser(const string& userId, shared_ptr<PacketSession> user)
{
	WriteLockGuard guard(_lock);
	if (_users.size() == 0) return false;
	_users[userId] = user;

	return true;
}

void Room::RemoveUser(const string& userId)
{
	WriteLockGuard guard(_lock);
	_users.erase(userId);
	
	if (_users.size() == 0)
	{
		RoomHandler::RemoveRoom(_roomName);
	}
}


shared_ptr<Room> RoomHandler::GetRoom(const string& roomName)
{
	if (_rooms.find(roomName) == _rooms.end()) return shared_ptr<Room>();

	return _rooms[roomName];
}

shared_ptr<Room> RoomHandler::GetRoom(wstring& roomNameW)
{
	const string roomName = WCHARToUTF8(roomNameW.c_str());
	if (_rooms.find(roomName) == _rooms.end()) return shared_ptr<Room>();

	return _rooms[roomName];
}

shared_ptr<Room> RoomHandler::GetRoom(WCHAR* roomNameW)
{
	const string roomName = WCHARToUTF8(roomNameW);
	if (_rooms.find(roomName) == _rooms.end()) return shared_ptr<Room>();

	return _rooms[roomName];
}

map<const string, shared_ptr<Room>> RoomHandler::_rooms;
Lock RoomHandler::_lock;