#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

void ServerSession::OnConnected()
{
	
	Protocol::C_TEST pkt;
	pkt.set_testnum(1);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
	
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32_t len)
{
	shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ServerPacketHandler::HandlePacket(session, buffer, len);
}
