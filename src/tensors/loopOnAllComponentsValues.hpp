#ifndef _LOOPONALLCOMPONENTSVALUES_HPP
#define _LOOPONALLCOMPONENTSVALUES_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file loopOnAllComponentsValues.hpp
///
/// \todo by now it's limited to tensors, but we should quickly
/// generalize to expr once we setup them

#include <tensors/tensor.hpp>

namespace maze
{
  namespace impl
  {
    /// Loop on all components value
    ///
    /// Internal implementation, no residual component
    template <typename...SubsComps, // Subscribed components
	      typename T,           // Tensor
	      typename F,
	      typename I>
    INLINE_FUNCTION
    void _loopOnAllComponentsValues(TensorComps<>*,
				    T& t,  ///< Tensor
				    F&& f, ///< Function to be invocated
				    I& i,  ///< Function to be executed
				    SubsComps&&...subsComps) ///< Subscribed components
    {
      f(i,std::forward_as_tuple(subsComps...));
      i++;
    }
    
    /// Loop on all components value
    ///
    /// Internal implementation looping on most external component
    template <typename HeadComp,    // First not subscribed components
	      typename...TailComps, // Others
	      typename...SubsComps, // Subscribed components
	      typename T,           // Tensor
	      typename F,
	      typename I>
    INLINE_FUNCTION
    void _loopOnAllComponentsValues(TensorComps<HeadComp,TailComps...>*,
				    T& t,  ///< Tensor
				    F&& f, ///< Function to be invocated
				    I& i,  ///< Function to be executed
				    SubsComps&&...subsComps) ///< Subscribed components
    {
      /// Rest of the components to loop
      using ResiualComps=
	TensorComps<TailComps...>;
      
      for(HeadComp h(0);h<t.template compSize<HeadComp>();h++)
	_loopOnAllComponentsValues((ResiualComps*)nullptr,t,std::forward<F>(f),i,std::forward<SubsComps>(subsComps)...,h);
    }
  }
  
  /// Loop on all components
  ///
  /// Must be invocated with a function accepting the index, and the components in a tuple format
  template <typename T,
	    typename F>
  INLINE_FUNCTION
  void loopOnAllComponentsValues(T&& t,F&& f)
  {
    /// BaseType
    using _T=
      typename std::decay_t<T>;
    
    /// Index
    typename _T::Index i(0);
    
    impl::_loopOnAllComponentsValues((typename _T::Comps*)nullptr,std::forward<T>(t),std::forward<F>(f),i);
  }
}


#endif
