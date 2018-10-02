
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <direct.h>
#define GetCurrentDir _getcwd
#endif
#ifdef __linux__
#define VK_USE_PLATFORM_XCB_KHR
#include "xcb/xcb.h"
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include "vulkan/vulkan.h"

// Debugging output macros
#ifdef _WIN32

/*
bit 0 - foreground blue
bit 1 - foreground green
bit 2 - foreground red
bit 3 - foreground intensity

bit 4 - background blue
bit 5 - background green
bit 6 - background red
bit 7 - background intensity
*/

#define BREAK_ON_WARNING

#ifdef _MSC_VER
#define VDU_DBG_SEVERE(msg) { \
			std::stringstream ss; \
			ss << msg; \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b11001111); \
			std::cout << __FILE__ << " Line: " << __LINE__ << std::endl; \
			std::cout << "! SEVERE  -"; \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b00001111); \
			std::cout << " " << ss.str() << std::endl; \
			MessageBoxA(NULL, LPCSTR(ss.str().c_str()), "Severe Error!", MB_OK); \
			DebugBreak(); } 
#else
#define VDU_DBG_SEVERE(msg) { \
			std::stringstream ss; \
			ss << msg; \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b11001111); \
			std::cout << "! SEVERE  -"; \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b00001111); \
			std::cout << " " << ss.str() << std::endl; \
			MessageBoxA(NULL, LPCSTR(ss.str().c_str()), "Severe Error!", MB_OK); \
			__builtin_trap(); }
#endif
#ifdef BREAK_ON_WARNING
#define VDU_DBG_WARNING(msg) { \
		std::stringstream ss; \
		ss << msg; \
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b01101111); \
		std::cout << __FILE__ << " Line: " << __LINE__ << std::endl; \
		std::cout << "! Warning -"; \
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b00001111); \
		std::cout << " " << ss.str() << std::endl << std::endl << std::endl << std::endl; \
		DebugBreak(); }
#else
#define VDU_DBG_WARNING(msg) { \
		std::stringstream ss; \
		ss << msg; \
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b01101111); \
		std::cout << __FILE__ << " Line: " << __LINE__ << std::endl; \
		std::cout << "! Warning -"; \
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b00001111); \
		std::cout << " " << ss.str() << std::endl << std::endl << std::endl << std::endl; } 
#endif

#define VDU_DBG_INFO(msg) { \
		std::stringstream ss; \
		ss << msg; \
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b00011111); \
		std::cout << " Info     -"; \
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0b00001111); \
		std::cout << " " << ss.str() << std::endl; } 
#else
#ifdef __linux__
#define VDU_DBG_SEVERE(msg) { \
			std::stringstream ss; \
			ss << msg; \
			std::cout << "\033[1;31m! SEVERE  -\033[0m"; \
			std::cout << " " << ss.str() << std::endl; }
#define VDU_DBG_WARNING(msg) { \
			std::stringstream ss; \
			ss << msg; \
			std::cout << "\033[1;33m! WARNING  -\033[0m"; \
			std::cout << " " << ss.str() << std::endl; }

#define VDU_DBG_INFO(msg) { \
			std::stringstream ss; \
			ss << msg; \
			std::cout << "\033[1;32m! INFO  -\033[0m"; \
			std::cout << " " << ss.str() << std::endl; }
#endif
#endif

#ifndef VDU_NO_VALIDATION
#define VDU_WITH_VALIDATION
#endif

#define VDU_VK_CHECK_RESULT(f, message) { \
	auto result = f; \
	if (result != VK_SUCCESS) { \
		std::string errorMsg = "Encountered "; \
		switch(result){ \
			case(VK_ERROR_OUT_OF_HOST_MEMORY): \
				errorMsg += "VK_ERROR_OUT_OF_HOST_MEMORY: "; break; \
			case(VK_ERROR_OUT_OF_DEVICE_MEMORY): \
				errorMsg += "VK_ERROR_OUT_OF_DEVICE_MEMORY: "; break; \
			case(VK_ERROR_INITIALIZATION_FAILED): \
				errorMsg += "VK_ERROR_INITIALIZATION_FAILED: "; break; \
			case(VK_ERROR_DEVICE_LOST): \
				errorMsg += "VK_ERROR_DEVICE_LOST: "; break; \
			case(VK_ERROR_MEMORY_MAP_FAILED): \
				errorMsg += "VK_ERROR_MEMORY_MAP_FAILED: "; break; \
			case(VK_ERROR_LAYER_NOT_PRESENT): \
				errorMsg += "VK_ERROR_LAYER_NOT_PRESENT: "; break; \
			case(VK_ERROR_EXTENSION_NOT_PRESENT): \
				errorMsg += "VK_ERROR_EXTENSION_NOT_PRESENT: "; break; \
			case(VK_ERROR_FEATURE_NOT_PRESENT): \
				errorMsg += "VK_ERROR_FEATURE_NOT_PRESENT: "; break; \
			case(VK_ERROR_INCOMPATIBLE_DRIVER): \
				errorMsg += "VK_ERROR_INCOMPATIBLE_DRIVER: "; break; \
			case(VK_ERROR_TOO_MANY_OBJECTS): \
				errorMsg += "VK_ERROR_TOO_MANY_OBJECTS: "; break; \
			case(VK_ERROR_FORMAT_NOT_SUPPORTED): \
				errorMsg += "VK_ERROR_FORMAT_NOT_SUPPORTED: "; break; \
			case(VK_ERROR_FRAGMENTED_POOL): \
				errorMsg += "VK_ERROR_FRAGMENTED_POOL: "; break; \
			default: \
				errorMsg += "UNKNOWN VK_ERROR: "; break; \
		} \
		m_logicalDevice->_internalReportError(result, errorMsg + message); \
	} \
}

/// Utilities includes

#include <limits>

#include <functional>

#include <utility>
// pair, make_pair

#include <initializer_list>

#include <assert.h>

/// Strings includes

#include <string>

/// Containers includes

#include <vector>

#include <list>

#include <forward_list>

#include <set>

#include <unordered_set>

#include <map>

#include <unordered_map>

#include <queue>

#include <array>

/// Iterators includes

#include <iterator>

/// IO includes

#include <ios>

#include <iostream>

#include <fstream>

#include <cstdio>

#include <sstream>