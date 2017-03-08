#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H
//======================================================================================
// Filename: Common.h
// Description:
//======================================================================================

#include "BUILD_OPTIONS.h"
//======================================================================================
// Includes
//======================================================================================
#include <assert.h>
#include <string>
#include <Windows.h> //@ToDo: Replace with Platform?
//======================================================================================
// Types
//======================================================================================

typedef char				s8;
typedef short				s16;
typedef int					s32;
typedef __int64				s64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned __int64	u64;

typedef float				f32;
typedef double				f64;

//======================================================================================
// Limits
//======================================================================================

#define U8_MAX				(u8)-1
#define U16_MAX				(u16)-1
#define U32_MAX				(u32)-1
#define U64_MAX				(u64)-1

#define F32_MIN				FLT_MIN
#define F32_MAX				FLT_MAX


//====================================================================================================
// Macros
//====================================================================================================

#if defined(_DEBUG)

#include "BUILD_OPTIONS.h"
//@ToDo: Fix platfrom specific print // Log to file?
#define LOG(format, ...)\
	{\
		{\
			char buffer[1024];\
			sprintf_s(buffer, 1024, (#format), __VA_ARGS__);\
			std::string message;\
			message += (buffer);\
			message += "\n";\
			OutputDebugStringA(message.c_str());\
		}\
	}
#else
#define LOG(format, ...)
#endif

//----------------------------------------------------------------------------------------------------

#if defined(_DEBUG)
#define ASSERT(condition, format, ...)\
	{\
		if (!(condition))\
		{\
			LOG(format, __VA_ARGS__)\
			DebugBreak();\
		}\
	}
#define VERIFY(condition, format, ...)\
	{\
		if (!(condition))\
		{\
			LOG(format, __VA_ARGS__)\
			DebugBreak();\
		}\
	}
#else
#define ASSERT(condition, format, ...)
#define VERIFY(condition, format, ...) condition;
#endif

//----------------------------------------------------------------------------------------------------

#define NONCOPYABLE(type)\
	type(const type&) = delete;\
	type& operator=(const type&) = delete;\
	type(const type&&) = delete;\
	type& operator=(const type&&) = delete;


#define SAVE_DELETE( obj )\
	delete obj;\
	obj = nullptr;
//======================================================================================
// Common Functions
//======================================================================================

#endif // !ENGINE_COMMON_H




