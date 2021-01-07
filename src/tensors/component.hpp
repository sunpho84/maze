#ifndef _COMPONENT_HPP
#define _COMPONENT_HPP

/// \file component.hpp
///
/// \brief Implements the basic feature of a tensor component
///
/// A tensor component is a class with a given signature, representing
/// the tensor index of a certain kind S. Any tensor is represented by
/// a list of component. Each component can be of Row or Column type,
/// and can be listed more than once. Each occurrency is distinguished
/// by the parameter Which.
///
/// To declare a new tensor component, create a class inheriting from
/// TensCompIdx with the appropriated signature.

#include <unroll/inliner.hpp>
#include <metaProgramming/feature.hpp>
#include <resources/simdTypes.hpp>
#include <tensors/componentSize.hpp>
#include <tensors/componentSignature.hpp>

#include <array>

namespace maze
{
  DEFINE_FEATURE(IsTensComp);
  
  DEFINE_FEATURE_GROUP(TensCompFeat);
  
  /// Short name for the TensComp
#define THIS					\
  TensComp<S,RC,Which>
  
  /// Tensor component defined by base type S
  template <typename S,
	    RwCl RC=ROW,
	    int Which=0>
  struct TensComp :
    public TensCompFeat<IsTensComp,THIS>
  {
    /// Transposed component
    using Transp=
      TensComp<S,transp<RC>,Which>;
    
    /// Base type
    using Base=
      S;
    
    /// Value type
    using Index=
      typename S::Index;
    
    /// Value
    Index i;
    
    /// Row or column
    static constexpr
    RwCl rC=
      RC;
    
    /// Index of the component
    static constexpr
    int which=
      Which;
    
    /// Check if the size is known at compile time
    static constexpr
    bool SizeIsKnownAtCompileTime=
      Base::sizeAtCompileTime!=DYNAMIC;
    
    /// Determine if this type can be simdified
    ///
    /// Internal implementation when F is a type for which simd exists
    template <typename F,
	      ENABLE_THIS_TEMPLATE_IF(simdOfTypeExists<F>)>
    static constexpr
    bool _canBeSimdified()
    {
      return
	SizeIsKnownAtCompileTime and
	((Base::sizeAtCompileTime%simdLength<F>)==0);
    }
    
    /// Determine if this type can be simdified
    ///
    /// Internal implementation when F is not a type for which simd exists
    template <typename F,
	      ENABLE_THIS_TEMPLATE_IF(not simdOfTypeExists<F>)>
    static constexpr
    bool _canBeSimdified()
    {
      return
	false;
    };
    
    /// Determine if this type can be simdified
    template <typename F>
    static constexpr
    bool canBeSimdified=
      _canBeSimdified<F>();
    
    /// Init from value
    INLINE_FUNCTION CUDA_HOST_DEVICE
    explicit constexpr TensComp(const Index& i=0) : i(i)
    {
    }
    
    /// Convert to actual value
    CUDA_HOST_DEVICE constexpr
    operator Index&()
    {
      return
	i;
    }
    
    /// Convert to actual value with const attribute
    INLINE_FUNCTION constexpr CUDA_HOST_DEVICE
    operator const Index&()
      const
    {
      return
	i;
    }
    
    /// Transposed index
    INLINE_FUNCTION CUDA_HOST_DEVICE constexpr
    auto transp()
      const
    {
      return
	Transp{i};
    }
    
    /// Assignment operator
    INLINE_FUNCTION CUDA_HOST_DEVICE constexpr
    TensComp& operator=(const Index& oth)
    {
      i=oth;
      
      return
	*this;
    }
  };
  
#undef THIS
  
  /// Promotes the argument i to a COMPONENT, through a function with given NAME
#define DECLARE_COMPONENT_FACTORY(NAME,COMPONENT...)		\
  template <typename T>						\
  INLINE_FUNCTION constexpr CUDA_HOST_DEVICE			\
  COMPONENT NAME(T&& i)						\
  {								\
    return							\
      COMPONENT(i);						\
  }
  
  /// Declare a component with no special feature
  ///
  /// The component has no row/column tag or index, so it can be
  /// included only once in a tensor
#define DECLARE_COMPONENT(NAME,TYPE,SIZE,FACTORY)		\
  DECLARE_COMPONENT_SIGNATURE(NAME,TYPE,SIZE);			\
  								\
  /*! NAME component */						\
  using NAME=							\
    TensComp<NAME ## Signature,ANY,0>;				\
								\
  DECLARE_COMPONENT_FACTORY(FACTORY,NAME)
  
  /// Declare a component which can be included more than once
  ///
  /// The component has a row/column tag, and an additional index, so
  /// it can be included twice in a tensor
#define DECLARE_ROW_OR_CLN_COMPONENT(NAME,TYPE,SIZE,FACTORY)	\
  DECLARE_COMPONENT_SIGNATURE(NAME,TYPE,SIZE);			\
  								\
  /*! NAME component */						\
  template <RwCl RC=ROW,					\
	    int Which=0>					\
  using NAME ## RC=TensComp<NAME ## Signature,RC,Which>;	\
								\
  /*! Row kind of NAME component */				\
  using NAME ## Row=NAME ## RC<ROW,0>;				\
								\
  /*! Column kind of NAME component */				\
  using NAME ## Cln=NAME ## RC<CLN,0>;				\
  								\
  /*! Default NAME component is Row */				\
  using NAME=NAME ## Row;					\
  								\
  DECLARE_COMPONENT_FACTORY(FACTORY ## Row,NAME ## Row);	\
								\
  DECLARE_COMPONENT_FACTORY(FACTORY ## Cln,NAME ## Cln);	\
								\
  DECLARE_COMPONENT_FACTORY(FACTORY,NAME)
}

#endif
