#pragma once


class SendBuffer
{
public:
	SendBuffer(BYTE* buffer, uint32_t, uint32_t allocSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer; }
	uint32_t AllocSize() { return _allocSize; }
	uint32_t WriteSize() { return _writeSize; }
	void Close(uint32_t writeSize);

private:
	BYTE* _buffer;
	uint32_t _allocSize = 0;
	uint32_t _writeSize = 0;
};

