#include "pch.h"
#include "AuthRespondent.h"

Lock PacketRespondent<Protocol::C_LOGIN>::_lock;

Lock PacketRespondent<Protocol::C_REGISTER>::_lock;