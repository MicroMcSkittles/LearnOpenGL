#pragma once
#include <iostream>
#include <functional>

// Message level controls what messages you receive
// 2 = Messages, Warnings, and Errors
// 1 = Warnings and Errors,
// 0 = Errors
// Doesn't do anything in release mode
#define MESSAGE_LEVEL 0

// Message priority tells MESSAGE what messages it can let through
// Doesn't do anything in release mode
#define MESSAGE_PRIORITY 2
#define TOP_MESSAGE_PRIORITY  0 // Will always show a message if messages are enabled
#define HIGH_MESSAGE_PRIORITY 1
#define NORM_MESSAGE_PRIORITY 2
#define LOW_MESSAGE_PRIORITY  3

#pragma region Miscellaneous

// Make sure MESSAGE_LEVEL is within the correct bounds
#if MESSAGE_LEVEL > 2 || MESSAGE_LEVEL < 0

#error MESSAGE_LEVEL must be at least 0 and at most 2

#endif

// Make sure MESSAGE_PRIORITY is within the correct bounds
#if MESSAGE_PRIORITY > 3 || MESSAGE_PRIORITY < 0

#error MESSAGE_PRIORITY must be at least 0 and at most 3

#endif

#define BIT(x) 1 << x

#define BIND_ARGS_1(func) std::bind(func, std::placeholders::_1)
#define BIND_ARGS_2(func) std::bind(func, std::placeholders::_1, std::placeholders::_2)
#define BIND_ARGS_3(func) std::bind(func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#define CLASS_BIND_ARGS_1(func) std::bind(&func, this, std::placeholders::_1)
#define CLASS_BIND_ARGS_2(func) std::bind(&func, this, std::placeholders::_1, std::placeholders::_2)
#define CLASS_BIND_ARGS_3(func) std::bind(&func, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#pragma endregion

#pragma region ErrorHandling

#ifdef DEBUG

#define ERROR(msg, ext) { \
	std::cout << "Error occurred in function ( " << __func__ \
		<< " ) at line ( " << __LINE__ << " ):" << std::endl; \
	std::cout << (char)9 << msg << std::endl << std::endl; \
	if(ext) exit(-1); \
}

#define FATAL_ERROR(msg) ERROR(msg, true)

#define SOFT_ERROR(msg) ERROR(msg, false)

#elif RELEASE

#define ERROR(msg, ext) exit(-1);

#define FATAL_ERROR(msg) ERROR("", true)

#define SOFT_ERROR(msg)

#endif

#pragma endregion

#pragma region Messages&Warnings

#ifdef DEBUG

// Messages ===========
#if MESSAGE_LEVEL == 2

#define MESSAGE(msg) { \
	std::cout << "Message from function ( " << __func__ \
		<< " ) at line ( " << __LINE__ << " ):" << std::endl; \
	std::cout << (char)9 << msg << std::endl << std::endl; \
}

#define TOP_MESSAGE(msg) MESSAGE(msg)

#if MESSAGE_PRIORITY >= LOW_MESSAGE_PRIORITY

#define LOW_MESSAGE(msg) MESSAGE(msg)

#else

#define LOW_MESSAGE(msg)

#endif

#if MESSAGE_PRIORITY >= NORM_MESSAGE_PRIORITY

#define NORM_MESSAGE(msg) MESSAGE(msg)

#else

#define NORM_MESSAGE(msg)

#endif

#if MESSAGE_PRIORITY >= HIGH_MESSAGE_PRIORITY

#define HIGH_MESSAGE(msg) MESSAGE(msg)

#else

#define HIGH_MESSAGE(msg)

#endif

#else

#define MESSAGE(msg)

#define TOP_MESSAGE(msg)

#define LOW_MESSAGE(msg)

#define NORM_MESSAGE(msg)

#define HIGH_MESSAGE(msg)

#endif

// Warnings ===========
#if MESSAGE_LEVEL == 2 || MESSAGE_LEVEL == 1

#define WARNING(msg) { \
	std::cout << "Warning from function ( " << __func__ \
		<< " ) at line ( " << __LINE__ << " ):" << std::endl; \
	std::cout << (char)9 << msg << std::endl << std::endl; \
}

#else

#define WARNING(msg)

#endif

#elif RELEASE

#define MESSAGE(msg)

#define WARNING(msg)

#define TOP_MESSAGE(msg)

#define LOW_MESSAGE(msg)

#define NORM_MESSAGE(msg)

#define HIGH_MESSAGE(msg)

#endif

#pragma endregion
