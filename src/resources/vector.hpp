#ifndef _VECTOR_HPP
#define _VECTOR_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file vector.hpp

#include <utility>

#include <resources/memoryManager.hpp>

namespace maze
{
  /// Provides a static size implementation of a vector
  template <typename E,
	    StorLoc SL=CurrentArchStorLoc>
  struct Vector
  {
    /// Number of elements
    size_t nEl;
    
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
    
    /// Copy constructor
    Vector(const Vector& oth) : Vector(oth.nEl)
    {
      for(size_t i=0;i<nEl;i++)
	data[i]=oth[i];
    }
    
    /// Move constructor
    Vector(Vector&& oth) : nEl(oth.nEl),data(oth.data)
    {
      oth.data=nullptr;
    }
    
    /// Move assignment
    Vector& operator=(Vector&& oth)
    {
      std::swap(nEl,oth.nEl);
      std::swap(data,oth.data);
      
      return *this;
    }
    
    /// Construct getting number of elements
    Vector(const size_t& nEl) :
      Vector(nEl,memoryManager<SL>()->template provide<E>(nEl))
    {
    }
    
    /// Construct getting a filler
    Vector(const size_t& nEl,
	   std::function<E(const size_t&)> f) :
      Vector(nEl)
    {
      for(size_t iEl=0;iEl<nEl;iEl++)
	data[iEl]=f(iEl);
    }
    
    /// Construct getting a filler
    Vector(const size_t& nEl,
	   const E& e) :
      Vector(nEl)
    {
      for(size_t iEl=0;iEl<nEl;iEl++)
	data[iEl]=e;
    }
    
    /// Construct getting number of elements and pointer
    Vector(const size_t& nEl,
	   E* data) : nEl(nEl),data(data)
    {
    }
    
    /// Default construct
    Vector() : nEl(0),data(nullptr)
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
