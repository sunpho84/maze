#ifndef _NON_CONST_METHOD_HPP
#define _NON_CONST_METHOD_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file nonConstMethod.hpp
///
/// \brief Provides non const version of a method

#include <metaProgramming/constnessChanger.hpp>

namespace maze
{
  /// First part of the non-const method provider
#define _PROVIDE_ALSO_NON_CONST_METHOD_BEGIN				\
  /*! Overload the \c NAME const method passing all args             */ \
  template <typename...Ts> /* Type of all arguments                  */
  
  /// Body OF non-const method provider
#define _PROVIDE_ALSO_NON_CONST_METHOD_BODY(NAME)			\
  decltype(auto) NAME(Ts&&...ts) /*!< Arguments                      */ \
  {									\
    return asMutable(asConst(*this).NAME(std::forward<Ts>(ts)...)); \
  }
  
  /// Provides also a non-const version of the method \c NAME
  ///
  /// See Scott Meyers
  /// https://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func
  /// One or more const method NAME must be already present, the
  /// correct one will be chosen perfectly forwarding the arguments
  ///
  ///Example
  ///
  /// \code
  // class mace
  /// {
  ///   double e{0};
  ///
  /// public:
  ///
  ///   const double& get() const
  ///   {
  ///     return e;
  ///   }
  ///
  ///   PROVIDE_ALSO_NON_CONST_METHOD(get);
  /// };
  /// \endcode
#define PROVIDE_ALSO_NON_CONST_METHOD(NAME)				\
  _PROVIDE_ALSO_NON_CONST_METHOD_BEGIN					\
  _PROVIDE_ALSO_NON_CONST_METHOD_BODY(NAME)
  
#define PROVIDE_ALSO_NON_CONST_METHOD_GPU(NAME)				\
  _PROVIDE_ALSO_NON_CONST_METHOD_BEGIN					\
  CUDA_HOST_DEVICE							\
  _PROVIDE_ALSO_NON_CONST_METHOD_BODY(NAME)
}

#endif
