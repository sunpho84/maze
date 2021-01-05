#ifndef _MATH_HPP
#define _MATH_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file math.hpp

namespace maze
{
  // Return the log2 of N
  template <typename I>
  I log2N(I N)
  {
    /// Result
    I log2N=0;
    
    do log2N++;
    while ((2<<log2N)<N);
    
    return log2N;
  }
}

#endif
