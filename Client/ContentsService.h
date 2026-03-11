#pragma once
#include "ServerPacketHandler.h"

template<typename T>
class RequestService
{
public:
	
	RequestService(function<void(T)>& callback)
	{
		_callback = callback;
	}
	template<typename U>
	RequestService(U&& pkt, function<void(T)>& callback)
	{
		_callback = callback;

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		ClientGlobal::GServerSession->Send(sendBuffer);

	}

	static void Subscribe(function<void(T)>& callback)
	{
		_callback = callback;
	}

	template<typename U>
	static void Request(U&& pkt, function<void(T)>& callback)
	{
		_callback = callback;

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		ClientGlobal::GServerSession->Send(sendBuffer);

	}

	static void Execute(T&& model, bool isClear=true)
	{
		WriteLockGuard guard(_lock);

		if (_callback)_callback(model);
		if(isClear)_callback = nullptr;

	}

private:
	static function<void(T)> _callback;

	// 테스트용
	static Lock _lock; 
};
template<typename T>
function<void(T)> RequestService<T>::_callback = nullptr;
template<typename T>
Lock RequestService<T>::_lock;

