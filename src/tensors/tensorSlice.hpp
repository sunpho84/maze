#ifndef _TENSOR_SLICE_HPP
#define _TENSOR_SLICE_HPP

/// \file tensorSlice.hpp
///
/// \brief Implements a sliced view of a tensor

#include <metaProgramming/feature.hpp>
//#include <expr/expr.hpp>
#include <tensors/complex.hpp>
#include <tensors/component.hpp>
#include <tensors/componentsList.hpp>
#include <tensors/tensorDecl.hpp>
#include <tensors/tensorFeat.hpp>
#include <utilities/tuple.hpp>

namespace maze
{
  DEFINE_FEATURE(IsTensorSlice);
  
  DEFINE_FEATURE_GROUP(TensorSliceFeat);
  
  namespace impl
  {
    /// Computes the resulting components
    template <typename T,
	      typename SC>
    struct TensorSliceComps
    {
      /// Visible components
      using Comps=
	TupleFilterOut<SC,typename T::Comps>;
    };
  }
  
  /// Sliced view of a tensor
  ///
  /// Forward implementation
  template <bool IsConst,         // Const or not
	    typename T,           // Tensor
	    typename S,           // Subscribed componenents
	    typename ExtComps=typename impl::TensorSliceComps<T,S>::Comps,
	    typename ExtFund=typename T::Fund,
	    bool CanBeCastToFund=std::tuple_size<ExtComps>::value==0>
  struct TensorSlice;
  
  /// Shortcut for TensorSlice
#define THIS					\
  TensorSlice<IsConst,T,TensorComps<Sc...>,ExtComps,ExtFund,CanBeCastToFund>
  
  /// Sliced view of a tensor
  template <bool IsConst,  // Const or not
	    typename T,    // Tensor
	    typename...Sc, // Subscribed components
	    typename ExtComps,
	    typename ExtFund,
	    bool CanBeCastToFund>
  struct THIS : public
    // Expr<THIS>,
    ComplexSubscribe<THIS>,
  //AssignFromFundProvider<not IsConst,THIS,ExtFund>,
    // ToFundCastProvider<CanBeCastToFund,THIS,ConstIf<IsConst,ExtFund>,FundCastByRefVal::BY_REF>,
    TensorSliceFeat<IsTensorSlice,THIS>
  {
    /// Keep note of the template par
    static constexpr bool canBeCastToFund=
      CanBeCastToFund;
    
    // /// Import assign operator from expression
    // using Expr<THIS>::operator=;
    
    /// A slice can be copied easily
    static constexpr bool takeAsArgByRef=
      false;
    
    /// Holds info on whether the slice is constant
    static constexpr bool isConst=
      IsConst;
    
    /// A slice can be assigned provided is not const
    static constexpr bool canBeAssigned=
      not IsConst;
    
    // /// Import assign operator from fund
    // template <typename O=ExtFund,
    // 	      bool CBA=canBeAssigned,
    // 	      ENABLE_THIS_TEMPLATE_IF(CBA)>
    // const O& operator=(const O& o)
    // {
    //   return
    // 	this->eval()=
    // 	o;
    // }
    // using AssignFromFundProvider<canBeAssigned,THIS,ExtFund>::operator=;
    
    /// Fundamental type
    using Fund=
      ExtFund;
    
    /// Resulting components
    using Comps=
      ExtComps;
    
    /// Original tensor to which we refer
    using OrigTensor=
      T;
    
    /// Reference to original tensor
    const OrigTensor& t;
    
    /// Subscribed components
    using SubsComps=
      TensorComps<Sc...>;
    
    /// Subscribed components
    const SubsComps subsComps;
    
    /// Get components size from the tensor
    template <typename C>
    INLINE_FUNCTION constexpr
    decltype(auto) compSize() const
    {
      return
	this->t.template compSize<C>();
    }
    
    /// Provide subscribe operator when returning a reference
#define PROVIDE_SUBSCRIBE_OPERATOR(CONST_ATTR,CONST_AS_BOOL)		\
    /*! Operator to take a const slice to a given component */		\
    template <typename C,						\
	      typename Cp=Comps>					\
    CUDA_HOST_DEVICE INLINE_FUNCTION					\
    auto operator[](const TensorCompFeat<IsTensorComp,C>& cFeat) CONST_ATTR	\
    {									\
									\
      /*! Nested reference subscribed components */			\
      auto nestedSubsComps=						\
	std::tuple_cat(subsComps,TensorComps<C>{cFeat.deFeat()});		\
									\
      /*! Type used to hold all components */				\
      using NestedSubsComps=						\
	decltype(nestedSubsComps);					\
    									\
      /*! Reference type */						\
      using R=								\
	TensorSlice<CONST_AS_BOOL or IsConst,T,NestedSubsComps>;		\
      									\
      return								\
	R(this->t,nestedSubsComps);					\
    }
    
    PROVIDE_SUBSCRIBE_OPERATOR(/* not const */, false);
    PROVIDE_SUBSCRIBE_OPERATOR(const, true);
    
#undef PROVIDE_SUBSCRIBE_OPERATOR
    
    /// Provide eval method, converting to fundamental
#define PROVIDE_EVAL_METHOD(CONST_ATTR)					\
    /*! Operator to return direct access to data */			\
    template <typename Cp=Comps,					\
	      typename Ret=ConstIf<IsConst,Fund>&,			\
	      ENABLE_THIS_TEMPLATE_IF(std::tuple_size<Cp>::value==0)>	\
    CUDA_HOST_DEVICE INLINE_FUNCTION					\
    Ret eval()								\
      CONST_ATTR							\
    {									\
      return								\
	(Ret)(t.trivialAccess(t.index(subsComps)));			\
    }
    
    PROVIDE_EVAL_METHOD(/* not const */);
    PROVIDE_EVAL_METHOD(const);
    
#undef PROVIDE_EVAL_METHOD
    
    /// Create from slice and list of subscribed components
    CUDA_HOST_DEVICE
    TensorSlice(const TensorFeat<IsTensor,T>& t,
	      const SubsComps& subsComps) :
      t(t.deFeat()),subsComps(subsComps)
    {
    }
    
    /// Return a tensor pointing to the offsetted data, with the resulting component
    CUDA_HOST_DEVICE INLINE_FUNCTION constexpr
    auto carryOver() const
    {
      /// Number of dynamic components must be zero
      constexpr int nDynComps=
	std::tuple_size<TupleFilter<SizeIsKnownAtCompileTime<false>::t,Comps>>::value;
      
      static_assert(nDynComps==0,"Not supported if residual dynamic components are present");
      
      /// Offset to data
      const auto offset=
	t.index(fillTuple<typename T::Comps>(subsComps));
      
      /// Data with offset
      auto carriedData=
	t.getDataPtr()+
	offset;
      
      return
	Tensor<Comps,typename T::Fund,T::storLoc,Stackable::CANNOT_GO_ON_STACK>(carriedData,t.data.getSize());
    }
  };
  
#undef THIS
}

#endif
