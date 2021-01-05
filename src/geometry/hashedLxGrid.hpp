#ifndef _HASHED_LX_GRID_HPP
#define _HASHED_LX_GRID_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file hashedLxGrid.hpp

#include <geometry/lxGrid.hpp>

namespace maze
{
  /// Hashed grid
  template <int NDim,
	    typename Index>
  struct HashedLxGrid : public LxGrid<NDim,Index>
  {
    /// Grid volume
    const Index vol;
    
    /// Lookup table of coordinates
    const Vector<Coords<NDim>> coordsOfLx;
    
    /// Construct from sizes
    HashedLxGrid(const Coords<NDim>& sizes) :
      LxGrid<NDim,Index>(sizes),
      vol(this->computeVol()),
      coordsOfLx(this->getCoordsOfAllLx())
    {
    }
  };
}

#endif
