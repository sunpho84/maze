#ifndef _TEMPLATE_ENABLER_HPP
#define _TEMPLATE_ENABLER_HPP

/// \file templateEnabler.hpp
///
/// \brief Provides a SFINAE to be used in template par list

#include <type_traits>
#include <utility>

//#include <gpu/cudaMacros.hpp>

namespace maze
{
  /// Provides a SFINAE to be used in template par list
  ///
  /// This follows
  /// https://stackoverflow.com/questions/32636275/sfinae-with-variadic-templates
  /// as in this example
  /// \code
  /// template <typename D,
  ///           ENABLE_THIS_TEMPLATE_IF(std::is_same<D,int>::value)>
  /// void foo(D i) {} // fails if D is not int
  /// \endcode
#define ENABLE_THIS_TEMPLATE_IF(...)	\
  std::enable_if_t<(__VA_ARGS__),void*> =nullptr
  
  
  /////////////////////////////////////////////////////////////////
  
  //to be moved elsewhere
  
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
