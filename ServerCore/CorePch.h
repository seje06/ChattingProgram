#pragma once

#include <cstdint>
#include <thread>

#include <Windows.h>
#include <iostream>
using namespace std;

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "CoreMacro.h"
#include "CoreTLS.h"

///// 컨테이너 /////

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>