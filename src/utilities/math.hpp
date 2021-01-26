#ifndef _MATH_HPP
#define _MATH_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file math.hpp

#include <utility>

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
  
  /// Adds an offset to an index sequence
  template <std::size_t O,
	    std::size_t...Is>
  std::index_sequence<(O+Is)...> addOffset(std::index_sequence<Is...>)
  {
    return {};
  }
  
  /// Creates an index sequence with an offset
  template <std::size_t O,
	    std::size_t N>
  auto makeIndexSequenceWithOffset()
  {
    return addOffset<O>(std::make_index_sequence<N>{});
  }
  
  /// Creates a range integer sequence excluding last
  template <std::size_t F,
	    std::size_t L>
  auto makeRange()
  {
    return addOffset<F>(std::make_index_sequence<L-F>{});
  }
  
}

#endif
