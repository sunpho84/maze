#ifndef _INDEX_SHUFFLER_HPP
#define _INDEX_SHUFFLER_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file indexShuffler.hpp

#include <resources/vector.hpp>

namespace maze
{
  /// Wraps the vector used to remap indices
  ///
  /// Useful to avoid the accidental conversion between indices and to
  /// keep track of the input index
  template <typename In,
	    typename Out// ,
	    // ENABLE_THIS_TEMPLATE_IF(std::is_convertible_v<In,int> and std::is_convertible_v<Out,int>)
	    >
  struct IndexShuffler
  {
    // non va usato un vector ma un tensor, cosi' possiamo sottosccriverlo con tutte le componenti in una botta
#warning questo va rivisitato
    
    /// Stores the lookup table
    Vector<Out> lookupTable;
    
    /// Constructor taking size and filler
    template <typename F,
	      ENABLE_THIS_TEMPLATE_IF(std::is_invocable_v<F,In> and
	      			      std::is_convertible_v<std::invoke_result_t<F,In>,Out>)>
    void fill(F f)
    {
      lookupTable.fill(f);
    }
    
    /// Constructor taking size and filler
    template <typename F,
	      ENABLE_THIS_TEMPLATE_IF(std::is_invocable_v<F,In>//  and
				      // std::is_convertible_v<std::invoke_result_t<F,In>,Out>
				      )>
    IndexShuffler(const size_t& nEl,
		  F f) :
      lookupTable(nEl)
    {
      fill(f);
    }
    
    /// Constructor taking vector
    explicit IndexShuffler(const Vector<Out>& lookupTable) :
      lookupTable(lookupTable)
    {
    }
    
    /// Default constructor
    IndexShuffler(const size_t n=0) :
      lookupTable(n)
    {
    }
    
    /// Returns the index
    const Out& operator()(const In& in) const
    {
      return lookupTable[in];
    }
    
    /// Verifies the indexing
    void verify() const
    {
      /// Number of elements
      const size_t& n=
	lookupTable.size();
      
      /// Check that a given element is used only once
      Vector<Out> used(n,Out(n) /* as initializer */);
      
      for(size_t in=0;in<lookupTable.size();in++)
	{
	  /// Take the out index
	  const Out& out=
	    (*this)(in);
	  
	  if(out<0 or out>=n)
	    CRASHER<<"id of element "<<in<<": "<<out<<" out of range [0;"<<n<<")"<<endl;
	  
	  if(used[out]!=n)
	    CRASHER<<"id of element "<<in<<": "<<out<<" already used by element: "<<used[out]<<endl;
	  
	  used[out]=in;
	}
    }
    
    /// Returns the transpose shuffler
    IndexShuffler<Out,In> transpose() const
    {
      /// Number of elements
      const size_t& n=
	lookupTable.size();
      
      /// Result
      Vector<In> res(n,n /* as initializer */);
      
      for(size_t i=0;i<n;i++)
	{
	  /// Index of the given element
	  const Out out=(*this)(i);
#warning not working
	  
	  //	  res[out]=i;
      }
      
      return static_cast<IndexShuffler<Out,In>>(res);
    }
  };
}

#endif
