#pragma once
#undef UNICODE
//libraries
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>//std lib
#include <string>
#include <vector>
#include <thread>
#include <map>
#include <fstream>
#include <list>
#include <strsafe.h>
#include <stack>
//#include <ctime>
#include <time.h>
#include "mswsock.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT "25565"

//server definitons
#define WIN32_LEAN_AND_MEAN
#define MAX_WORKER_THREAD   16


//Variable declarations
	//server

//users
std::atomic_bool ActiveUsers = false;
std::atomic_bool NewMessage = false;

std::list<std::string> usermessages;//
WSABUF GlobalBuf;
//std::stack<std::string>  usermessages;

//incoming features
std::string UsersOnline[10];