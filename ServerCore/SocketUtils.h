#pragma once
#include "NetAddress.h"

class SocketUtils
{
public:
	static LPFN_CONNECTEX ConnectEx; // IOCP기반에서 사용될 비동기 coonect 함수 포인터
	static LPFN_DISCONNECTEX DisconnectEx; // IOCP기반에서 사용될 비동기 discoonect 함수 포인터
	static LPFN_ACCEPTEX AcceptEx; // IOCP기반에서 사용될 비동기 accept 함수 포인터

public:
	static void Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn); // 비동기 확장함수 바인딩
	static SOCKET CreateSocket();  // TCP기반 소켓 생성

	static bool SetLinger(SOCKET socket, uint16_t onoff, uint16_t linger); // 소켓닫을때 어케 닫을지
	static bool SetReuseAddress(SOCKET socket, bool flag); // 서버 재실행 할때 같은 포트로 재바인딩할때 안될때가 있어서, 관련 설정
	static bool SetRecvBufferSize(SOCKET socket, int32_t size); // 커널에 recvBuffer사이즈 요청
	static bool SetSendBufferSize(SOCKET socket, int32_t size); // 커널에 sendBuffer사이즈 요청
	static bool SetTcpNoDelay(SOCKET socket, bool flag); // 네이글알고리즘(패킷 모아서 보내기) 사용할지말지 설정
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);  // acceptEx 로 얻은 accept소켓에 Listen 소켓 컨텍스트를 복사

	static bool Bind(SOCKET socket, NetAddress netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16_t port);
	static bool Listen(SOCKET socket, int32_t backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32_t level, int32_t optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}

