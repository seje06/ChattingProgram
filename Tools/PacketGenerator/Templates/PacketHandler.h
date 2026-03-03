#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(shared_ptr<PacketSession>&, BYTE*, int32_t)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16_t
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

//TODO : 자동화
// Custom Handlers
bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len);

{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(shared_ptr<PacketSession>& session, Protocol::{{pkt.name}}& pkt);
{%- endfor %}

class {{output}}
{
public:
	//TODO : 초기화
	static void Init()
	{
		for (int32_t i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[0] = Handle_INVALD;
		}

{%- for pkt in parser.recv_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len) {return HandlePacket<Protocol::{{pkt.name}}>(Handle_{{pkt.name}}, session, buffer, len); };
{%- endfor %}
		}

	static bool HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

{%- for pkt in parser.send_pkt %}
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::{{pkt.name}}&pkt) { return MakeSendBuffer(pkt, PKT_{{pkt.name}}); }
{%- endfor %}

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





