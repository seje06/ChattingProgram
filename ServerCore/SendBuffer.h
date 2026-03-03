#pragma once

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(shared_ptr<SendBufferChunk> owner, BYTE* buffer, uint32_t allocSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer; }
	uint32_t AllocSize() { return _allocSize; }
	uint32_t WriteSize() { return _writeSize; }
	void Close(uint32_t writeSize);

private:
	BYTE* _buffer;
	uint32_t _allocSize = 0;
	uint32_t _writeSize = 0;
	shared_ptr<SendBufferChunk> _owner;
};

///// SendBufferChunk /////

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void Reset();
	shared_ptr<SendBuffer> Open(uint32_t allocSize);
	void Close(uint32_t writeSize);

	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32_t FreeSize() { return static_cast<uint32_t>(_buffer.size() - _usedSize); }

private:
	array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
	bool _open = false;
	uint32_t _usedSize = 0;
};

///// SendBufferManager /////

class SendBufferManager
{
public:
	shared_ptr<SendBuffer> Open(uint32_t size);

private:
	shared_ptr<SendBufferChunk> Pop();
	void Push(shared_ptr<SendBufferChunk> buffer);

	static void PushGlobal(SendBufferChunk* buffer);

private:
	Lock _lock;
	vector<shared_ptr<SendBufferChunk>> _sendBufferChunks;
};