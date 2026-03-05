#pragma once

extern class ThreadManager* GThreadManager;
extern class SendBufferManager* GSendBufferManager;
extern class DBConnectionPool* GDBConnectionPool;

extern set<wstring> GTestSep;
extern class Lock GTestLock;