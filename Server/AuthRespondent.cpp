#include "pch.h"
#include "AuthRespondent.h"

Lock PacketRespondent<Protocol::C_LOGIN>::_lock;

#if MULTIPLE_CONNECT_TEST_MODE
atomic<int> PacketRespondent<Protocol::C_REGISTER>::registerCount = 1;
#endif
Lock PacketRespondent<Protocol::C_REGISTER>::_lock;