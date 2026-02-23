#include "pch.h"
#include "RecvBuffer.h"


RecvBuffer::RecvBuffer(int32_t bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32_t dataSize = DataSize();
	if (dataSize == 0) // 읽을수 있는 데이터 다읽었을때
	{
		_readPos = _writePos = 0;
	}
	else // 남아있을때
	{
		if (FreeSize() < _bufferSize) //여유 사이즈가 partition버퍼보다 작을때만
		{
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32_t numOfBytes)
{
	if (numOfBytes > DataSize())
	{
		return false;
	}

	_readPos += numOfBytes;

	return true;
}

bool RecvBuffer::OnWrite(int32_t numOfBytes)
{
	if (numOfBytes > FreeSize())
	{
		return false;
	}

	_writePos += numOfBytes;

	return true;
}
