#pragma once



class Lock
{
	enum ELockState : uint8_t
	{
		WRITE_STATE,
		READ_STATE,
		NOTTING_STATE,
	};

public:
	void WriteLock();
	void WriteUnLock();
	void ReadLock();
	void ReadUnLock();

private:


private:
	atomic<bool> _canWriteLock = true;
	atomic<ELockState> _lockState = NOTTING_STATE;
	atomic<uint16_t> _readThreadCount = 0;
	atomic<uint16_t> _writeLockCount = 0;
	atomic<int> _ownerThreadId = -1;

	bool _threadFirstRead[THREAD_COUNT] = {false};
};

