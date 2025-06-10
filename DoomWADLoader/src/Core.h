#pragma once
#include <iostream>
#include <string>

#ifdef DEBUG


#define ERROR(msg, ext) \
{ \
std::cout << "Error with function ( " << __func__ << " ) in file ( " << __FILE__ << " ) at line ( " << __LINE__ << " ):" << std::endl; \
std::cout << msg << std::endl << std::endl; \
if(ext) exit(-1); \
} 

#define FATAL_ERROR(msg) ERROR(msg, true)

#define SOFT_ERROR(msg) ERROR(msg, false)

#elif RELEASE

#define HANDLE_ERROR(msg, ext)

#define FATAL_ERROR(msg)

#define SOFT_ERROR(msg)

#endif // DEBUG
