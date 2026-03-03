#pragma once

extern thread_local int LThreadId;
extern thread_local class JobQueue* LCurrentJobQueue;
extern thread_local uint64_t LEndTickCount;
extern thread_local shared_ptr<class SendBufferChunk> LSendBufferChunk;