#ifndef _WORLD_HPP
#define _WORLD_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file world.hpp

#include <tensors/component.hpp>

namespace maze
{
  /// Structure to incapsulate the directions and oriented movements
  ///
  /// In the naming scheme, Lx denotes a site
  template <int _NDims=4>
  struct World
  {
    /// Number of dimensions
    static constexpr int nDims=
      _NDims;
    
    /// Number of oriented directions
    static constexpr int nOrientedDirs=
      2*nDims;
    
    DECLARE_COMPONENT(Direction,int,nDims,direction);
  };
}

#endif
