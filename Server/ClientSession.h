#pragma once
class ClientSession : public PacketSession
{
public:
	~ClientSession()
	{

	}

	virtual void OnConnected()override;
	virtual void OnSend(int32_t len)override;
	virtual void OnDisconnected()override;
	virtual void OnRecvPacket(BYTE* buffer, int32_t len)override;
};
