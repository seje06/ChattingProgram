#include "pch.h"
#include "Lock.h"



void Lock::WriteLock()
{
	if (_ownerThreadId.load() == LThreadId)  // 같은 스레드에서 write 했을때
	{
#ifdef _DEBUG
		ValidateThreadId();
		if (_readDepthArray[LThreadId] !=0 ) CRASH("READ -> WRITE");
#endif
		_writeRecursion.fetch_add(1);
		return;
	}

	_writerPending.fetch_add(1);

	int spin = 0;
	while (true)
	{
		int expected = NOTTING_STATE;
		if (_lockState.compare_exchange_strong(expected, WRITE_STATE))
		{
			_ownerThreadId.store(LThreadId);
			_writeRecursion.store(1);
			_writerPending.fetch_sub(1);
			return;
		}
		else
		{
			if (++spin < 100) _mm_pause(); // 100번까진 저전력으로 돌림
			else this_thread::yield(); // 이후 다른 작업가능한 다른 쓰레드가 있다면 대기상태
		}
	}
}

void Lock::WriteUnLock()
{
	if (_ownerThreadId.load() != LThreadId) // 잘못된 write unlock일 경우
	{
		CRASH("invalid unlock");
		return;
	}

	uint16_t prev = _writeRecursion.fetch_sub(1);
	if (prev == 0) // 재귀 unlock 언더플로우일때
	{
		CRASH("write recursion underflow");
	}
	else if (prev == 1)
	{
		_ownerThreadId.store(-1);
		_lockState.exchange(NOTTING_STATE);
	}
}

void Lock::ReadLock()
{
	// write 에서 같은 스레드가 read로 들어온 경우 통과
	if (_ownerThreadId.load() == LThreadId)
	{
#ifdef _DEBUG
		ValidateThreadId();
		_readDepthArray[LThreadId].fetch_add(1);
#endif
		return;
	}

	int spin = 0;
	while (true)
	{
		if (_writerPending.load() > 0)// write를 우선으로 하기위해 write요청이 오면 read는 대기
		{
			if (++spin < 100) _mm_pause(); // 100번까진 저전력으로 돌림
			else this_thread::yield(); // 이후 다른 작업가능한 다른 쓰레드가 있다면 대기상태

			continue;
		}

		int expected = _lockState.load();
		if (expected == WRITE_STATE) // write중이면 대기
		{
			if (++spin < 100) _mm_pause(); // 100번까진 저전력으로 돌림
			else this_thread::yield(); // 이후 다른 작업가능한 다른 쓰레드가 있다면 대기상태

			continue;
		}

		if (_lockState.compare_exchange_strong(expected, expected + 1))
		{
#ifdef _DEBUG
			ValidateThreadId();
			_readDepthArray[LThreadId].fetch_add(1);
#endif
			return;
		}
		else
		{
			if (++spin < 500) _mm_pause(); // cas를 시도했다는건 write가 아닐 확률이 더높다 생각하기에 500번까지 늘림
			else this_thread::yield(); // 이후 다른 작업가능한 다른 쓰레드가 있다면 대기상태

			continue;
		}
	}
}

void Lock::ReadUnLock()
{
#ifdef _DEBUG
	ValidateThreadId();
	auto prev = _readDepthArray[LThreadId].fetch_sub(1);
	if (prev == 0) CRASH("read depth underflow"); //재귀 unlock 언더플로우일때
#endif
	// 같은 스레드의 write에서 들어온 read가 아니면 카운트 감소
	if (_ownerThreadId.load() != LThreadId)
	{
		int prev = _lockState.fetch_sub(1);
		if (prev <= 0) CRASH("lockState underflow");

		return;
	}
}
