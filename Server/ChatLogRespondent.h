#pragma once



template<>
class PacketRespondent<>
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, T& pkt, OUT bool& isSuccess) {}
};

