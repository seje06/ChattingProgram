#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "DBConnectionPool.h"
#include "Lock.h"

ThreadManager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;

CoreGlobal* GCoreGlobal = nullptr;

set<wstring> GTestSep;
Lock GTestLock;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
	GSendBufferManager = new SendBufferManager();
	GDBConnectionPool = new DBConnectionPool();

	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GSendBufferManager;
	delete GDBConnectionPool;

	SocketUtils::Clear();
}