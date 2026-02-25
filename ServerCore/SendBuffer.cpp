#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(BYTE* buffer, uint32_t, uint32_t allocSize)
	: _buffer(buffer), _allocSize(allocSize)
{

}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32_t writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
}
