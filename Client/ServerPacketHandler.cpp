#include "pch.h"
#include "ServerPacketHandler.h"
#include "ContentsService.h"
#include "Model.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
{
	return false;
}

bool Handle_S_LOGIN(shared_ptr<PacketSession>& session, Protocol::S_LOGIN& pkt)
{
	AuthData data;
	data.isSuccess = pkt.issuccess();
	data.id = CA2W(pkt.id().c_str(), CP_UTF8);
	data.pasd = CA2W(pkt.pssd().c_str(), CP_UTF8);

	ContentsService<AuthModel>::Execute(AuthModel(data));

	return true;
}

bool Handle_S_REGISTER(shared_ptr<PacketSession>& session, Protocol::S_REGISTER& pkt)
{
	AuthData data;
	data.isSuccess = pkt.issuccess();
	data.id = CA2W(pkt.id().c_str(), CP_UTF8);
	data.pasd = CA2W(pkt.pssd().c_str(), CP_UTF8);

	ContentsService<AuthModel>::Execute(AuthModel(data));

	return true;
}

bool Handle_S_REFRESH_LOBBY(shared_ptr<PacketSession>& session, Protocol::S_REFRESH_LOBBY& pkt)
{
	RefreshLobbyData data;
	data.isSuccess = pkt.issuccess();
	for (int i = 0; i < pkt.roomnames().size(); i++)
	{
		CString roomName = CA2W(pkt.roomnames().Get(i).c_str(), CP_UTF8);
		int userCount = pkt.usercounts().Get(i);
		data.roomNames.push(roomName);
		data.userCounts.push(userCount);
	}
	ContentsService<RefreshLobbyModel>::Execute(RefreshLobbyModel(data));

	return true;
}

bool Handle_S_CREATE_ROOM(shared_ptr<PacketSession>& session, Protocol::S_CREATE_ROOM& pkt)
{
	CreateRoomData data;
	data.isSuccess = pkt.issuccess();
	data.roomName = CA2W(pkt.roomname().c_str(), CP_UTF8);

	ContentsService<CreateRoomModel>::Execute(CreateRoomModel(data));

	return true;
}

bool Handle_S_JOIN_ROOM(shared_ptr<PacketSession>& session, Protocol::S_JOIN_ROOM& pkt)
{
	JoinRoomData data;
	data.isSuccess = pkt.issuccess();
	data.roomName = CA2W(pkt.roomname().c_str(), CP_UTF8);
	for (int i = 0; i < pkt.userids().size(); i++)
	{
		CString userId = CA2W(pkt.userids().Get(i).c_str(), CP_UTF8);
		data.userIds.push(userId);
	}
	ContentsService<JoinRoomModel>::Execute(JoinRoomModel(data));

	return true;
}

bool Handle_S_REFRESH_ROOM(shared_ptr<PacketSession>& session, Protocol::S_REFRESH_ROOM& pkt)
{
	RefreshRoomData data;
	for (int i = 0; i < pkt.userids().size(); i++)
	{
		CString id = CA2W(pkt.userids().Get(i).c_str(), CP_UTF8);
		data.userIds.push(id);
	}
	ContentsService<RefreshRoomModel>::Execute(RefreshRoomModel(data),false);

	return true;
}

bool Handle_S_CHAT_LOG(shared_ptr<PacketSession>& session, Protocol::S_CHAT_LOG& pkt)
{
	ChatLogData data;
	data.id = CA2W(pkt.id().c_str(), CP_UTF8);
	data.log= CA2W(pkt.log().c_str(), CP_UTF8);

	ContentsService<ChatLogModel>::Execute(ChatLogModel(data), false);

	return true;
}

bool Handle_S_ROOM_OUT(shared_ptr<PacketSession>& session, Protocol::S_ROOM_OUT& pkt)
{
	ContentsService<RoomOutModel>::Execute(RoomOutModel(RoomOutData()));

	return true;
}
