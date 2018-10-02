
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include "vulkan/vulkan.h"

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
		m_logicalDevice->_internalReportVkError(result, errorMsg + message); \
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