#ifndef _KERNEL_HPP
#define _KERNEL_HPP

/// \file kernel.hpp
///
/// \brief Provide and dispatch kernels

namespace maze
{
  /// Introduces the body of a loop
#if defined USE_CUDA
# define KERNEL_LAMBDA_BODY(A)			\
  [=] CUDA_HOST_DEVICE (A) mutable
#else
# define KERNEL_LAMBDA_BODY(A)			\
  [&] (A) __attribute__((always_inline))
#endif
}

#endif
