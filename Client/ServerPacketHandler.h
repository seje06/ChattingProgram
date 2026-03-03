#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(shared_ptr<PacketSession>&, BYTE*, int32_t)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16_t
{
	PKT_C_TEST = 1000,
	PKT_S_TEST = 1001,
};

//TODO : 자동화
// Custom Handlers
bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len);
bool Handle_S_TEST(shared_ptr<PacketSession>& session, Protocol::S_TEST& pkt);

class ServerPacketHandler
{
public:
	//TODO : 초기화
	static void Init()
	{
		for (int32_t i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[0] = Handle_INVALD;
		}
		GPacketHandler[PKT_S_TEST] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
		}

	static bool HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_TEST&pkt) { return MakeSendBuffer(pkt, PKT_C_TEST); }

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




