#pragma once

#include "PacketRespondent.h"

template<>
class PacketRespondent<Protocol::C_CHAT_LOG>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_CHAT_LOG& pkt, OUT bool& isSuccess)
	{
		RESPONSE_START(session, pkt)



		RESPONSE_END()
	}
};

template<>
class PacketRespondent<Protocol::C_ROOM_OUT>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, Protocol::C_ROOM_OUT& pkt, OUT bool& isSuccess)
	{
		RESPONSE_START(session, pkt)



			RESPONSE_END()
	}
};