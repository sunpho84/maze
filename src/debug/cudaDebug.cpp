#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file debug.hpp

#include <cstdarg>

#ifdef USE_CUDA
# include <cuda_runtime.h>
#endif

#include <debug/crasher.hpp>

namespace maze
{
#if USE_CUDA
  void internalDecryptCudaError(const int lineNo,const char* fileName,const char* funcName,const cudaError_t rc,const char *templ,...)
  {
    if(rc!=cudaSuccess)
      {
	char mess[1024];
	va_list ap;
	va_start(ap,templ);
	va_end(ap);
	
	vsprintf(mess,templ,ap);
	Crasher(lineNo,fileName,funcName)<<mess<<", cuda raisded error: "<<cudaGetErrorString(rc)<<endl;
      }
  }
#endif
}
