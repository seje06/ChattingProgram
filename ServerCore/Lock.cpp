#include "pch.h"
#include "Lock.h"



void Lock::WriteLock()
{
	if (_ownerThreadId.load() == LThreadId)
	{
		_lockState.store(WRITE_STATE);
		_writeLockCount.fetch_add(1);
		return;
	}

	while (true)
	{
		ELockState expected = NOTTING_STATE;
		if (_lockState.compare_exchange_strong(expected, WRITE_STATE))
		{
			_ownerThreadId.store(LThreadId);
			_writeLockCount.fetch_add(1);
			return;
		}
	}
}

void Lock::WriteUnLock()
{

}

void Lock::ReadLock()
{
	if (_ownerThreadId.load() == LThreadId)
	{
		if (_threadFirstRead[LThreadId] == false)
		{
			_threadFirstRead[LThreadId] = true;
			_readThreadCount.fetch_add(1);
		}
		return;
	}

	atomic<bool>* canLock = reinterpret_cast<atomic<bool>*>(&_lockState); // WRITE는 0, READ와 NOTTING은 1, 2 기때문에 true면 read하기위해 형변환을 해줌
	while (true)
	{
		bool expected = true;
		int expectedId = _ownerThreadId;
		if (canLock->compare_exchange_strong(expected, true)) // WRITE가 아니면 상태에 READ를 넣어주고 락을 하지않고 진행
		{
			if (_threadFirstRead[LThreadId] == false)
			{
				_threadFirstRead[LThreadId] = true;
				_readThreadCount.fetch_add(1);
			}

			
			if (_readThreadCount.load() == 1)
			{
				_ownerThreadId.compare_exchange_strong(expectedId, LThreadId);
			}
			else
			{
				_ownerThreadId.exchange(-2);
			}

			return;
		}
	}
}

void Lock::ReadUnLock()
{
}
