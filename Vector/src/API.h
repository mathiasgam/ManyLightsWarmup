#pragma once

#ifdef API_PLATFORM_WINDOW
		#ifdef API_BUILD_DLL
	#define API __declspec(dllexport)
		#else
	#define API __declspec(dllexport)
		#endif
#else
	#error does only support windows so far
#endif