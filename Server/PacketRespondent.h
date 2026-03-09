#pragma once

#pragma once
#include "ClientPacketHandler.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "RoomHandler.h"

//DB 바인드 부터 하면 된다.
#define RESPONSE_START(session, pkt)						\
	isSuccess = false;										\
															\
	function<void()> func = [session, pkt]() {				\
		DBConnection* dbConn = GDBConnectionPool->Pop();	

#define RESPONSE_END()											  \
	GDBConnectionPool->Push(dbConn);							  \
	};															  \
	shared_ptr<Job> job = make_shared<Job>(0, std::move(func));	  \
	shared_ptr<JobQueue> jobQueue = make_shared<JobQueue>();	  \
																  \
	jobQueue->Push(job);										  \
	JobQueue::GlobalPush(jobQueue);								  \
																  \
	isSuccess = true;	

template<typename T>
class PacketRespondent
{
public:

	PacketRespondent(shared_ptr<PacketSession>& session, T& pkt, OUT bool& isSuccess) {}
};

										  
	