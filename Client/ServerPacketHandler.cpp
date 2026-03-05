#include "pch.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
{
	return false;
}

bool Handle_S_TEST(shared_ptr<PacketSession>& session, Protocol::S_TEST& pkt)
{
	return true;
}
