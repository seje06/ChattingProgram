#pragma once
#include "ServerPacketHandler.h"

template<typename T>
class ContentsService
{
public:
	
	ContentsService(function<void(T)>& callback)
	{
		_callback = callback;
	}
	template<typename U>
	ContentsService(U&& pkt, function<void(T)>& callback)
	{
		_callback = callback;

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		ClientGlobal::GServerSession->Send(sendBuffer);
		//Send(pkt);

	}
	/*ContentsService(Protocol::C_REGISTER& pkt, function<void(T)>& callback)
	{
		_callback = callback;

		Send(pkt);
	}*/

	static void Execute(T model, bool isClear=true)
	{
		WriteLockGuard guard(_lock);

		if (_callback)_callback(model);
		if(isClear)_callback = nullptr;

	}

private:
	/*template<typename U>
	void Send(U&& pkt)
	{
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		ClientGlobal::GServerSession->Send(sendBuffer);
	}*/
private:
	static function<void(T)> _callback;

	// 테스트용
	static Lock _lock; 
};
template<typename T>
function<void(T)> ContentsService<T>::_callback = nullptr;
template<typename T>
Lock ContentsService<T>::_lock;

