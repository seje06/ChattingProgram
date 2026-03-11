#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "RoomHandler.h"
#include "DBConnectionPool.h"
#include "DBBind.h"


void ClientSession::OnConnected()
{
}

void ClientSession::OnSend(int32_t len)
{
}

void ClientSession::OnDisconnected()
{
	if (!_room.expired())
	{
		auto room = _room.lock();
		room->RemoveUser(_id);
	}
	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBBind<2, 0> dbBind(*dbConn, L"UPDATE chat.account SET is_online = ? WHERE id = ?;");
	
	bool isOnline= false;
	wstring id = Utf8ToWstring(_id);
	dbBind.BindParam(0, isOnline);
	dbBind.BindParam(1, (WCHAR*)id.data());

	ASSERT_CRASH(dbBind.Execute());

	GDBConnectionPool->Push(dbConn);
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32_t len)
{
	shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void ClientSession::SetRoom(shared_ptr<Room> room)
{
	_room = room;
}

void ClientSession::SetId(string id)
{
	_id = id;
}
