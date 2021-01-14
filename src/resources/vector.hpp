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
    size_t _nEl;
    
    /// Return the number of elements
    const size_t& size() const
    {
      return _nEl;
    }
    
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
    
    /// Find last where occurring
    template <typename F>
    size_t lastPositionWhere(F&& f) const
    {
      /// Store the number of elements
      const size_t nEl=
	this->size();
      
      /// Result
      size_t res=nEl;
      
      while((--res)<nEl and not f(data[res]));
      
      return std::min(res,nEl);
    }
    
    /// Find last element where occurrs
    template <typename F>
    const E& lastElementWhere(F&& f) const
    {
      return data[lastPositionWhere(std::forward<F>(f))];
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(lastElementWhere);
    
    /// Copy constructor
    Vector(const Vector& oth) : Vector(oth._nEl)
    {
      for(size_t i=0;i<this->size();i++)
	data[i]=oth[i];
    }
    
    /// Move constructor
    Vector(Vector&& oth) : _nEl(oth._nEl),data(oth.data)
    {
      oth.data=nullptr;
    }
    
    /// Move assignment
    Vector& operator=(Vector&& oth)
    {
      std::swap(_nEl,oth._nEl);
      std::swap(data,oth.data);
      
      return *this;
    }
    
    /// Construct getting number of elements
    Vector(const size_t& nEl) :
      Vector(nEl,memoryManager<SL>()->template provide<E>(nEl))
    {
    }
    
    /// Construct getting a filler
    template <typename F,
	      ENABLE_THIS_TEMPLATE_IF(std::is_invocable_v<const F&,size_t>)>
    void fill(const F& f)
    {
      for(size_t iEl=0;iEl<this->size();iEl++)
	data[iEl]=f(iEl);
    }
    
    /// Construct getting a filler
    template <typename F,
	      ENABLE_THIS_TEMPLATE_IF(std::is_invocable_v<const F&,size_t>)>
    Vector(const size_t& nEl,
	   const F& f) :
      Vector(nEl)
    {
      fill(f);
    }
    
    /// Construct getting a value
    Vector(const size_t& nEl,
	   const E& e) :
      Vector(nEl)
    {
      for(size_t iEl=0;iEl<nEl;iEl++)
	data[iEl]=e;
    }
    
    /// Construct getting number of elements and pointer
    Vector(const size_t& nEl,
	   E* data) : _nEl(nEl),data(data)
    {
    }
    
    /// Default construct
    Vector() : _nEl(0),data(nullptr)
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
