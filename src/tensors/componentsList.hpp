#ifndef _COMPONENTS_LIST_HPP
#define _COMPONENTS_LIST_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file componentsList.hpp
///
/// \brief Implements a list of components

#include <utilities/tuple.hpp>
#include <tensors/component.hpp>

namespace maze
{
  /// Collection of components
  template <typename...Tc>
  using TensorComps=
    std::tuple<Tc...>;
  
  /// Returns the number of components of a tensComp
  template <typename T>
  constexpr int nOfComps=
    std::tuple_size<typename T::Comps>::value;
  
  namespace impl
  {
    /// Provides the result of filtering from a list of components the Row or Column
    ///
    /// Forward definition
    template <RwCl F,
	      typename TC>
    struct _TensorCompsFilterRwCl;
    
    /// Cannot use directly the TupleFilter, because of some template template limitation
    template <RwCl F,
	      typename...Tc>
    struct _TensorCompsFilterRwCl<F,TensorComps<Tc...>>
    {
      /// Predicate to filter
      ///
      /// Forward definition
      template <typename T>
      struct Filter;
      
      /// Predicate to filter
      template <typename S,
		RwCl RC,
		int Which>
      struct Filter<TensorComp<S,RC,Which>>
      {
	/// Predicate result, counting whether the type match
	static constexpr
	bool value=
	  (RC==F);
      };
      
      /// Returned type
      typedef TupleFilter<Filter,TensorComps<Tc...>> type;
    };
  }
  
  /// Filter all Row components
  template <typename TC>
  using TensorCompsFilterRow=
    typename impl::_TensorCompsFilterRwCl<RwCl::ROW,TC>::type;
  
  /// Filter all Col components
  template <typename TC>
  using TensorCompsFilterCln=
    typename impl::_TensorCompsFilterRwCl<RwCl::CLN,TC>::type;
  
  /// Filter all Any components
  template <typename TC>
  using TensorCompsFilterAny=
    typename impl::_TensorCompsFilterRwCl<RwCl::ANY,TC>::type;
  
  /// Gets the dynamic components of a tensComps
  template <typename TC>
  constexpr decltype(auto) getDynamicCompsOfTensorComps(TC&& tc)
  {
    return tupleFilter<SizeIsKnownAtCompileTime<false>::t>(std::forward<TC>(tc));
  }
  
  /// Gets the dynamic component types of a TensorComps
  template <typename TC>
  using GetDynamicCompsOfTensorComps=
    decltype(getDynamicCompsOfTensorComps(TC{}));
  
  /// Gets the fixed size components of a tensComps
  template <typename TC>
  constexpr decltype(auto) getFixedSizeCompsOfTensorComps(TC&& tc)
  {
    return tupleFilter<SizeIsKnownAtCompileTime<true>::t>(std::forward<TC>(tc));
  }
  
  /// Gets the fixed size component types of a TensorComps
  template <typename TC>
  using GetFixedSizeCompsOfTensorComps=
    decltype(getFixedSizeCompsOfTensorComps(TC{}));
  
  /////////////////////////////////////////////////////////////////
  
  namespace impl
  {
    /// Transposes a list of components
    ///
    /// Actual implementation, forward declaration
    template <typename TC>
    struct _TensorCompsTransp;
    
    /// Transposes a list of components
    ///
    /// Actual implementation
    template <typename...S,
	      RwCl...RC,
	      int...Which>
    struct _TensorCompsTransp<TensorComps<TensorComp<S,RC,Which>...>>
    {
      /// Resulting type
      using type=
	TensorComps<TensorComp<S,transp<RC>,Which>...>;
    };
  }
  
  /// Transposes a list of components
  template <typename TC>
  using TensorCompsTransp=
    typename impl::_TensorCompsTransp<TC>::type;
}

#endif
