#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"


void ClientSession::OnConnected()
{
}

void ClientSession::OnSend(int32_t len)
{
}

void ClientSession::OnDisconnected()
{
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32_t len)
{
	shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ClientPacketHandler::HandlePacket(session, buffer, len);
}
