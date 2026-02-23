#pragma once



class Lock
{
	enum ELockState : int8_t
	{
		WRITE_STATE = -1,
		NOTTING_STATE,
		// 0 > 은 read state
	};

public:
	void WriteLock();
	void WriteUnLock();
	void ReadLock();
	void ReadUnLock();

private:
	atomic<uint16_t> _writerPending = 0;
	atomic<uint16_t> _writeRecursion = 0;
	atomic<int> _lockState = NOTTING_STATE;
	atomic<int> _ownerThreadId = -1;
	
#ifdef _DEBUG
	atomic<uint16_t> _readDepthArray[THREAD_COUNT] = { 0 };

	void ValidateThreadId() // threadID가 최대치를 넘진않았는지 검증
	{
		if (LThreadId < 0 || LThreadId >= THREAD_COUNT) CRASH("Invalid LThreadId");
	}
#endif
};

