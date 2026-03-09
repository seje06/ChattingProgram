#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(shared_ptr<PacketSession>&, BYTE*, int32_t)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16_t
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_REGISTER = 1002,
	PKT_S_REGISTER = 1003,
	PKT_C_REFRESH_LOBBY = 1004,
	PKT_S_REFRESH_LOBBY = 1005,
	PKT_C_CREATE_ROOM = 1006,
	PKT_S_CREATE_ROOM = 1007,
	PKT_C_JOIN_ROOM = 1008,
	PKT_S_JOIN_ROOM = 1009,
	PKT_S_REFRESH_ROOM = 1010,
	PKT_C_CHAT_LOG = 1011,
	PKT_S_CHAT_LOG = 1012,
	PKT_C_ROOM_OUT = 1013,
	PKT_S_ROOM_OUT = 1014,
};

//TODO : 자동화
// Custom Handlers
bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len);
bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& pkt);
bool Handle_C_REGISTER(shared_ptr<PacketSession>& session, Protocol::C_REGISTER& pkt);
bool Handle_C_REFRESH_LOBBY(shared_ptr<PacketSession>& session, Protocol::C_REFRESH_LOBBY& pkt);
bool Handle_C_CREATE_ROOM(shared_ptr<PacketSession>& session, Protocol::C_CREATE_ROOM& pkt);
bool Handle_C_JOIN_ROOM(shared_ptr<PacketSession>& session, Protocol::C_JOIN_ROOM& pkt);
bool Handle_C_CHAT_LOG(shared_ptr<PacketSession>& session, Protocol::C_CHAT_LOG& pkt);
bool Handle_C_ROOM_OUT(shared_ptr<PacketSession>& session, Protocol::C_ROOM_OUT& pkt);

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
		GPacketHandler[PKT_C_LOGIN] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_REGISTER] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_REGISTER>(Handle_C_REGISTER, session, buffer, len); };
		GPacketHandler[PKT_C_REFRESH_LOBBY] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_REFRESH_LOBBY>(Handle_C_REFRESH_LOBBY, session, buffer, len); };
		GPacketHandler[PKT_C_CREATE_ROOM] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_CREATE_ROOM>(Handle_C_CREATE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_C_JOIN_ROOM] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_JOIN_ROOM>(Handle_C_JOIN_ROOM, session, buffer, len); };
		GPacketHandler[PKT_C_CHAT_LOG] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_CHAT_LOG>(Handle_C_CHAT_LOG, session, buffer, len); };
		GPacketHandler[PKT_C_ROOM_OUT] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::C_ROOM_OUT>(Handle_C_ROOM_OUT, session, buffer, len); };
		}

	static bool HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_LOGIN&pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REGISTER&pkt) { return MakeSendBuffer(pkt, PKT_S_REGISTER); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REFRESH_LOBBY&pkt) { return MakeSendBuffer(pkt, PKT_S_REFRESH_LOBBY); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_CREATE_ROOM&pkt) { return MakeSendBuffer(pkt, PKT_S_CREATE_ROOM); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_JOIN_ROOM&pkt) { return MakeSendBuffer(pkt, PKT_S_JOIN_ROOM); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REFRESH_ROOM&pkt) { return MakeSendBuffer(pkt, PKT_S_REFRESH_ROOM); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_CHAT_LOG&pkt) { return MakeSendBuffer(pkt, PKT_S_CHAT_LOG); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ROOM_OUT&pkt) { return MakeSendBuffer(pkt, PKT_S_ROOM_OUT); }

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




