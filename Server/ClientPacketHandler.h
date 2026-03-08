#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(shared_ptr<PacketSession>&, BYTE*, int32_t)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16_t
{
	PKT_C_TEST = 1000,
	PKT_S_TEST = 1001,
	PKT_C_LOGIN = 1002,
	PKT_S_LOGIN = 1003,
	PKT_C_REGISTER = 1004,
	PKT_S_REGISTER = 1005,
};

//TODO : 자동화
// Custom Handlers
bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len);
bool Handle_C_TEST(shared_ptr<PacketSession>& session, Protocol::C_TEST& pkt);
bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& pkt);
bool Handle_C_REGISTER(shared_ptr<PacketSession>& session, Protocol::C_REGISTER& pkt);

class ClientPacketHandler
{
public:
	//TODO : 초기화
	static void Init()
	{
		for (int32_t i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[0] = Handle_INVALD;
		}
		GPacketHandler[PKT_C_TEST] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_TEST>(Handle_C_TEST, session, buffer, len); };
		GPacketHandler[PKT_C_LOGIN] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_REGISTER] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_REGISTER>(Handle_C_REGISTER, session, buffer, len); };
		}

	static bool HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_TEST&pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_LOGIN&pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REGISTER&pkt) { return MakeSendBuffer(pkt, PKT_S_REGISTER); }

private:

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
		{
			return false;
		}

		return func(session, pkt);
	}

	template<typename T>
	static shared_ptr<SendBuffer> MakeSendBuffer(T& pkt, uint16_t pktId)
	{
		const uint16_t dataSize = static_cast<uint16_t>(pkt.ByteSizeLong());
		const uint16_t packetSize = dataSize + sizeof(PacketHeader);

		shared_ptr<SendBuffer> sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));

		sendBuffer->Close(packetSize);

		return sendBuffer;
	}

};




