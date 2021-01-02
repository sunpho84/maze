#ifndef _TIMER_HPP
#define _TIMER_HPP

/// \file timer.hpp
///
/// \brief Time between instants

#include <chrono>

namespace maze
{
  /// Measure time
  using Instant=
    std::chrono::time_point<std::chrono::steady_clock>;
  
  /// Returns the current time
  inline Instant takeTime()
  {
    return
      std::chrono::steady_clock::now();
  }
  
  /// Difference between two instants
  inline double timeDiffInSec(const Instant& end,   ///< Starting moment
			      const Instant& start) ///< Ending moment
  {
    return
      std::chrono::duration<double>(end-start).count();
  }
}

#endif
