#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include "ContentsService.h"

#include <random> // 테스트용

void ServerSession::OnConnected()
{
	if(!ClientGlobal::GServerSession) ClientGlobal::GServerSession = static_pointer_cast<PacketSession>(shared_from_this());

	/*Protocol::C_TEST pkt;
	pkt.set_testnum(1);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);*/

#if MULTIPLE_CONNECT_TEST_MODE
	// 테스트 
	 // 1. 진짜 난수 생성기 (시드 설정용)
	std::random_device rd;
	// 2. Mersenne Twister 엔진 초기화 (rd()로 시드값 설정)
	std::mt19937 gen(rd());
	// 3. 0부터 99까지의 균일 분포 정의
	std::uniform_int_distribution<int> dis(0, 9999);
	int id = dis(gen);
	int pssd = dis(gen);
	Protocol::C_REGISTER pkt;
	pkt.set_id(to_string(id));
	pkt.set_pssd(to_string(pssd));
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
	//
#endif
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32_t len)
{
	shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ServerPacketHandler::HandlePacket(session, buffer, len);
}
