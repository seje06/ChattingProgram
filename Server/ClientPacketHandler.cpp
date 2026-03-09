#include "pch.h"
#include "ClientPacketHandler.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "PacketRespondent.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
{
    return false;
}

bool Handle_C_TEST(shared_ptr<PacketSession>& session, Protocol::C_TEST& pkt)
{

    return true;
}

bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& pkt)
{
    bool isSuccess;
    PacketRespondent<Protocol::C_LOGIN> response(session, pkt, OUT isSuccess);

    return isSuccess;
}

bool Handle_C_REGISTER(shared_ptr<PacketSession>& session, Protocol::C_REGISTER& pkt)
{
    bool isSuccess;
    PacketRespondent<Protocol::C_REGISTER> response(session, pkt, OUT isSuccess);

    return isSuccess;

}
