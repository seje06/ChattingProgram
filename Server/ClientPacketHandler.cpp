#include "pch.h"
#include "ClientPacketHandler.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "PacketRespondentGroub.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALD(shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
{
    return false;
}

bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_LOGIN& pkt)
{
    bool isSuccess;
    PacketRespondent response(session, pkt, OUT isSuccess);
    return isSuccess;
}

bool Handle_C_REGISTER(shared_ptr<PacketSession>& session, Protocol::C_REGISTER& pkt)
{
    bool isSuccess;
    PacketRespondent response(session, pkt, OUT isSuccess);
    return isSuccess;

}

bool Handle_C_REFRESH_LOBBY(shared_ptr<PacketSession>& session, Protocol::C_REFRESH_LOBBY& pkt)
{
    bool isSuccess;
    PacketRespondent response(session, pkt, OUT isSuccess);
    return isSuccess;
}

bool Handle_C_CREATE_ROOM(shared_ptr<PacketSession>& session, Protocol::C_CREATE_ROOM& pkt)
{
    bool isSuccess;
    PacketRespondent response(session, pkt, OUT isSuccess);
    return isSuccess;
}

bool Handle_C_JOIN_ROOM(shared_ptr<PacketSession>& session, Protocol::C_JOIN_ROOM& pkt)
{
    bool isSuccess;
    PacketRespondent response(session, pkt, OUT isSuccess);
    return isSuccess;
}

bool Handle_C_CHAT_LOG(shared_ptr<PacketSession>& session, Protocol::C_CHAT_LOG& pkt)
{
    bool isSuccess;
    PacketRespondent response(session, pkt, OUT isSuccess);
    return isSuccess;
}

bool Handle_C_ROOM_OUT(shared_ptr<PacketSession>& session, Protocol::C_ROOM_OUT& pkt)
{
    bool isSuccess;
    PacketRespondent response(session, pkt, OUT isSuccess);
    return isSuccess;
}
