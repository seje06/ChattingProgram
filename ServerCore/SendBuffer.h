#pragma once


class SendBuffer
{
public:
	SendBuffer();

private:
	BYTE* _buffer;
	uint32_t _allocSize = 0;
	uint32_t _writeSize = 0;
};

