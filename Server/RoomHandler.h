#pragma once
#include "ClientPacketHandler.h"

class Room : public enable_shared_from_this<Room>
{
public:
	Room(const int& roomId, const string& userId, const shared_ptr<PacketSession>& user);

	bool AddUser(const string& userId, shared_ptr<PacketSession> user);
	void RemoveUser(const string& userId);
	int GetUserCount() { WriteLockGuard guard(_lock); return _users.size(); }

	template<typename T> 
	void BroadCast(T&& pkt, shared_ptr<PacketSession> excluUser = nullptr)
	{
		WriteLockGuard guard(_lock);

		auto it = _users.begin();
		while (it != _users.end())
		{
			if (excluUser)
			{
				if (excluUser != it->second)
				{
					auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
					it->second->Send(sendBuffer);
				}
			}
			else
			{
				auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
				it->second->Send(sendBuffer);
			}
			it++;
		}
	}

	const int GetRoomId() { return _roomId; }
private:
	map<const string, shared_ptr<PacketSession>> _users;
	Lock _lock;
	const int _roomId;
};

class RoomHandler
{
public:
	static shared_ptr<Room> GetRoom(const int& roomName);

	static void AddRoom(shared_ptr<Room>& room) { WriteLockGuard guard(_lock); _rooms[room->GetRoomId()] = room; }

	friend class Room;

private:
	static void RemoveRoom(const int& roomName) { WriteLockGuard guard(_lock); _rooms.erase(roomName); }

private:
	static map<const int, shared_ptr<Room>> _rooms;
	static Lock _lock;
};

