#pragma once
class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override;

	virtual void OnRecvPacket(BYTE* buffer, int32_t len) override;
	virtual void OnSend(int32_t len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		// cout << "Disconnected" << endl;
	}
};

