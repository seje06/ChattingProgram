#pragma once
#include "DBConnection.h"
#include "DBModel.h"

/*--------------------
	DBSynchronizer
---------------------*/

class DBSynchronizer
{
	enum
	{
		PROCEDURE_MAX_LEN = 10000
	};

	enum UpdateStep : uint8_t
	{
		DropIndex,
		AlterColumn,
		AddColumn,
		CreateTable,
		DefaultConstraint,
		CreateIndex,
		DropColumn,
		DropTable,
		StoredProcecure,

		Max
	};

	enum ColumnFlag : uint8_t
	{
		Type = 1 << 0,
		Nullable = 1 << 1,
		Identity = 1 << 2,
		Default = 1 << 3,
		Length = 1 << 4,
	};

public:
	DBSynchronizer(DBConnection& conn) : _dbConn(conn) {}
	~DBSynchronizer();

	bool		Synchronize(const WCHAR* path);

private:
	void		ParseXmlDB(const WCHAR* path);
	bool		GatherDBTables();
	bool		GatherDBIndexes();
	bool		GatherDBStoredProcedures();

	void		CompareDBModel();
	void		CompareTables(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Table> xmlTable);
	void		CompareColumns(shared_ptr<DBModel::Table> dbTable, shared_ptr<DBModel::Column> dbColumn, shared_ptr<DBModel::Column> xmlColumn);
	void		CompareStoredProcedures();

	void		ExecuteUpdateQueries();

private:
	DBConnection& _dbConn;

	vector<shared_ptr<DBModel::Table>>			_xmlTables;
	vector<shared_ptr<DBModel::Procedure>>		_xmlProcedures;
	set<wstring>							_xmlRemovedTables;

	vector<shared_ptr<DBModel::Table>>			_dbTables;
	vector<shared_ptr<DBModel::Procedure>>		_dbProcedures;

private:
	set<wstring>							_dependentIndexes;
	vector<wstring>						_updateQueries[UpdateStep::Max];
};