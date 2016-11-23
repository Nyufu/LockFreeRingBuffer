#pragma warning( push, 0 )
#if __clang__ && defined(GTEST) && GTEST == 1
#define chdir		_chdir 
#define fdopen		_fdopen
#define read		_read
#define write		_write
#define close		_close
#define dup			_dup
#define dup2		_dup2
#define creat		_creat

#define O_RDONLY	_O_RDONLY
#define O_APPEND	_O_APPEND
#endif

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )