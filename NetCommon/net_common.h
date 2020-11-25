#pragma once

// Common libraries and procesor defines

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
//#include <optional> // c++17 functionality only
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

#ifdef _WIN32 // Define that the Windows API is to be used (does not mean just use win32) and fix ASIO compiler warning
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE // Define the use of ASIO as non-boost
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include "net_message.h"