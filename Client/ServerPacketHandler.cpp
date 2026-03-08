#include "pch.h"
#include "ServerPacketHandler.h"
#include "ContentsService.h"
#include "Model.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
{
	return false;
}

bool Handle_S_TEST(shared_ptr<PacketSession>& session, Protocol::S_TEST& pkt)
{
	if (pkt.testnum() == 2)
	{
		cout << "success login!!"<<endl;
	}
	return true;
}

bool Handle_S_LOGIN(shared_ptr<PacketSession>& session, Protocol::S_LOGIN& pkt)
{
	AuthData data;
	data.isSuccess = pkt.issuccess();
	data.id = CA2W(pkt.id().c_str(), CP_UTF8);
	data.pasd = CA2W(pkt.pssd().c_str(), CP_UTF8);

	ContentsService<AuthModel>::Execute(AuthModel(data));

	return true;
}

bool Handle_S_REGISTER(shared_ptr<PacketSession>& session, Protocol::S_REGISTER& pkt)
{
	AuthData data;
	data.isSuccess = pkt.issuccess();
	data.id = CA2W(pkt.id().c_str(), CP_UTF8);
	data.pasd = CA2W(pkt.pssd().c_str(), CP_UTF8);

	ContentsService<AuthModel>::Execute(AuthModel(data));

	return true;
}
