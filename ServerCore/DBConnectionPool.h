#pragma once
#include "DBConnection.h"


/*------------------
   DBConnectionPool
-------------------*/

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool Connect(int32_t connectionCount, const WCHAR* connectionString);
	void Clear();

	DBConnection* Pop();
	void Push(DBConnection* connection);

private:
	Lock _lock;
	SQLHENV _environment = SQL_NULL_HANDLE;
	vector<DBConnection*> _connections;
};

