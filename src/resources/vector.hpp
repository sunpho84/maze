#ifndef _VECTOR_HPP
#define _VECTOR_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file vector.hpp

#include <resources/memoryManager.hpp>

namespace maze
{
  /// Provides a static size implementation of a vector
  template <typename E,
	    StorLoc SL>
  struct Vector
  {
    /// Number of elements
    const size_t nEl;
    
    /// Pointer to data
    E* data;
    
    /// Const access to elements
    const E& operator[](const size_t& i) const
    {
      return data[i];
    }
    
    /// Non const access to elements
    E& operator[](const size_t& i)
    {
      return data[i];
    }
    
    /// Construct getting number of elements and pointer
    Vector(const size_t& nEl,
	   E* data) : nEl(nEl),data(data)
    {
    }
    
    /// Move constructor
    Vector(Vector&& oth) : nEl(oth.nEl),data(oth.data)
    {
      oth.data=nullptr;
    }
    
    /// Construct getting number of elements
    Vector(const size_t& nEl) :
      Vector(nEl,memoryManager<SL>()->template provide<E>(nEl))
    {
    }
    
    /// Destructor
    ~Vector()
    {
      memoryManager<SL>()->release(data);
    }
  };
}

#endif
