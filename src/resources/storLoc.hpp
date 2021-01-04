#ifndef _STOR_LOC_HPP
#define _STOR_LOC_HPP


#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file storLoc.hpp

namespace maze
{
  /// Position where to store the data: device or host
  enum class StorLoc{ON_CPU,ON_GPU};
  
  /// Tag do distinguish CPU or GPU storage
  ///
  /// Forward definition
  template <StorLoc SL>
  constexpr const char* storLocTag()
  {
    switch(SL)
      {
      case StorLoc::ON_CPU:
	return "CPU";
	break;
      case StorLoc::ON_GPU:
      default:
	return "GPU";
	break;
      }
  }
}

#endif
