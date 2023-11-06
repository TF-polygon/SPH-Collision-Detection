#pragma once
#ifndef __CORE_H__
#define __CORE_H__

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/string_cast.hpp"

#include "GL/glew.h"
#include "GL/glut.h"

typedef unsigned int uint;
using   namespace    std;

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <tchar.h>
#include <cctype>
#include <mutex>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define TABLE_SIZE					1000000
#define THREAD_COUNT				8
#define PI							3.14159265f
#define DATA_LENGTH					255

#define RASPBERRY_PI_IP_ADDRESS		"10.106.3.50"
#define RASPBERRY_PI_PORT			22
#endif