#ifndef _TAG_DISPATCH_HPP
#define _TAG_DISPATCH_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file tagDispatch.hpp
///
/// \brief Implements a mechanism to choose overloaded functions and
/// methods at compile time

namespace maze
{
  /// Provides a simple struct with the given name
  ///
  /// Can be used to distinguish function in place of SFINAE
#define DECLARE_DISPATCHABLE_TAG(NAME) \
  struct NAME			      \
  {				      \
  }
  
  /// Creates the dispatcher, to be used inside function
#define DISPATCH(DISPATCH_TAG)				\
  DISPATCH_TAG()
}

#endif
