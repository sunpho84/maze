#ifndef _INDEX_SHUFFLER_HPP
#define _INDEX_SHUFFLER_HPP

#include "tensors/component.hpp"
#include "tensors/componentsList.hpp"
#include "tensors/tensorDecl.hpp"
#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file indexShuffler.hpp

#include "tensors/loopOnAllComponentsValues.hpp"
#include "tensors/tensor.hpp"

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
    /// Lookup table
    using LookupTable=
      Tensor<In,Out>;
    
    /// Index to be used to access in
    using LookupIndex=
      typename LookupTable::Index;
    
    /// Stores the lookup table
    LookupTable lookupTable;
    
    /// Constructor taking size and filler
    template <typename F,
	      ENABLE_THIS_TEMPLATE_IF(std::is_invocable_v<F,In> and
	      			      std::is_convertible_v<std::invoke_result_t<F,In>,Out>)>
    void fill(F f)
    {
      /// Index to access tenssor
      using Index=
	typename LookupTable::Index;
      
      loopOnAllComponentsValues(lookupTable,[this,&f](const Index& i,const In& in)
					    {
					      lookupTable(in)=f(in);
					    });
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
    explicit IndexShuffler(const LookupTable& lookupTable) :
      lookupTable(lookupTable)
    {
    }
    
    /// Move constructor
    IndexShuffler(IndexShuffler&& oth) :
      lookupTable(std::move(oth.lookupTable))
    {
    }
    
    /// Move assignment
    IndexShuffler& operator=(IndexShuffler&& oth)
    {
      std::swap(lookupTable,oth.lookupTable);
      
      return *this;
    }
    
    /// Copy constructor
    IndexShuffler(const IndexShuffler& oth) :
      lookupTable(oth.lookupTable)
    {
    }
    
    /// Copy assignment
    IndexShuffler& operator=(const IndexShuffler& oth)
    {
      lookupTable=oth.lookupTable;
      
      return *this;
    }
    
    /// Default constructor
    IndexShuffler(const In& inMax) :
      lookupTable(inMax)
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
      /// Check that a given element is used only once
      Tensor<Out,int> used;
      
      const LookupIndex n=
	used.size();
      
      loopOnAllComponentsValues(used,[&used,&n](const LookupIndex& index,const Out& out)
					    {
					      used(out)=n;
					    });
      
      loopOnAllComponentsValues(lookupTable,[this,&used,&n](const LookupIndex& index,const In& in)
					    {
					      /// Take the out index
					      const Out& out=
						(*this)(in);
	  
					      if(used[out]!=n)
						CRASHER<<"id of element "<<in<<": "<<out<<" already used by element: "<<used[out]<<endl;
					      
					      used[out]=in;
					    });
    }
    
    /// Returns the transpose shuffler
    IndexShuffler<Out,In> transpose() const
    {
      /// Result
      Tensor<Out,In> res;//(n,n /* as initializer */);
      
      loopOnAllComponentsValues(lookupTable,[this,&res](const LookupIndex& i,const In& in)
					    {
					      const Out out=(*this)(in);
					      res(out)=in;
					    });
      
      return static_cast<IndexShuffler<Out,In>>(res);
    }
  };
}

#endif
