#ifndef _CUDA_DEBUG_HPP
#define _CUDA_DEBUG_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file cudaDebug.hpp
///
/// \brief Debug cuda error

# ifdef USE_CUDA
namespace maze
{
  /// Put line, file and function in the actual call
# define DECRYPT_CUDA_ERROR(...) \
  internalDecryptCudaError(__LINE__,__FILE__,__FUNCTION__,__VA_ARGS__)
  
  /// Crash with a cuda error
  void internalDecryptCudaError(const int lineNo,        ///< Line of error
				const char *fileName,    ///< Filename
				const char* function,    ///< Function where the error occurred
				const cudaError_t rc,    ///< Error code
				const char *templ,...);  ///< Message template
}
# endif

#endif
