#pragma once

#define MULTIPLE_CONNECT_TEST_MODE 0

#define THREAD_MAX_COUNT 32

#define NAMESPACE_BEGIN(name) namespace name{
#define NAMESPACE_END		  }

///// Crash /////

#define CRASH(cause)						\
{											\
	uint32_t* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

