#pragma once
#include "DBConnection.h"

template<int32_t C>
struct FullBits { enum { value = (1 << (C - 1)) | FullBits<C - 1>::value }; };

template<>
struct FullBits<1> { enum { value = 1 }; };

template<>
struct FullBits<0> { enum { value = 0 }; };

template<int32_t ParamCount, int32_t ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, const WCHAR* query) :_dbConnection(dbConnection), _query(query)
	{
		::memset(_paramIndex, 0, sizeof(_paramIndex));
		::memset(_columnIndex, 0, sizeof(_columnIndex));
		_paramFlag = 0;
		_columnFlag = 0;
		dbConnection.Unbind();
	}

	bool Validate()
	{
		return _paramFlag == FullBits<ParamCount>::value && _columnFlag == FullBits<ColumnCount>::value;
	}

	bool Execute(bool doHandleError = true)
	{
		ASSERT_CRASH(Validate());
		return _dbConnection.Execute(_query, doHandleError);
	}

	bool Fetch()
	{
		return _dbConnection.Fetch();
	}

public:

	void SetParamNull(int32_t idx)
	{
		_paramFlag |= (1LL << idx);      // Validate 통과용
		_paramIndex[idx] = SQL_NULL_DATA;
	}

	bool IsColNull(int32_t idx) const
	{
		return _columnIndex[idx] == SQL_NULL_DATA;
	}

	template<typename T>
	void BindParam(int32_t idx, T& value)
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32_t idx, const WCHAR* value)
	{
		_dbConnection.BindParam(idx + 1, value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T, int32_t N>
	void BindParam(int32_t idx, T(&value)[N])
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, sizeof(T)*N, & _paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	void BindParam(int32_t idx, T* value, int32_t N)
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, sizeof(T) * N, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	void BindCol(int32_t idx, T& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<int32_t N>
	void BindCol(int32_t idx, WCHAR(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, sizeof(value), &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindCol(int32_t idx, WCHAR* value, int32_t len)
	{
		_dbConnection.BindCol(idx + 1, value, len -1, & _columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<typename T, int32_t N>
	void BindCol(int32_t idx, T(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, sizeof(T) * N, _columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

protected:
	DBConnection& _dbConnection;
	const WCHAR* _query;
	SQLLEN _paramIndex[ParamCount > 0 ? ParamCount : 1];
	SQLLEN _columnIndex[ColumnCount > 0 ? ColumnCount : 1];
	uint64_t _paramFlag;
	uint64_t _columnFlag;
};

