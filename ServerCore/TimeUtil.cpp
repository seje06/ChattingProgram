
#include "pch.h"
#include "TimeUtil.h"

TIMESTAMP_STRUCT GetCurrentTimeStamp()
{
	TIMESTAMP_STRUCT ts;
	std::time_t now = std::time(nullptr);
	//std::tm* ltm = std::localtime(&now);
	std::tm ltm;
	localtime_s(&ltm, &now);
	ts.year = ltm.tm_year + 1900; // tm_year는 1900년 이후 경과 년수
	ts.month = ltm.tm_mon + 1;    // tm_mon은 0~11 범위
	ts.day = ltm.tm_mday;
	ts.hour = ltm.tm_hour;
	ts.minute = ltm.tm_min;
	ts.second = ltm.tm_sec;
	ts.fraction = 0;

	return ts;
}