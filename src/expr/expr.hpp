#ifndef _EXPR_EXPR_HPP
#define _EXPR_EXPR_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <tuple>
#include <type_traits>

#include "debug/crasher.hpp"
#include "debug/typeNamer.hpp"
#include "metaProgramming/cudaMacros.hpp"
#include "metaProgramming/nonConstMethod.hpp"
#include "metaProgramming/tagDispatch.hpp"
#include "tensors/complex.hpp"
#include "tensors/component.hpp"
#include "tensors/componentsList.hpp"
#include "unroll/forEachInTuple.hpp"
#include "unroll/inliner.hpp"
#include "utilities/tuple.hpp"

/// \file expr/expr.hpp

/// \brief Base type for expression carrying tensor components
///
/// The Expr type is at the basis of any tensor expression. It
/// contains the subscribe operators.
///
/// Two operators are provided:
///
/// - operator[] accepts a single component, and never tries to
/// partially evaluate the expression,
///
/// - operator() accepts one or more components, and tries to
/// partially evaluate the expression, if possible.
///
/// In all cases, a full evaluation is always performed when no
/// component would be left after subscribing.
///

#include <metaProgramming/feature.hpp>

namespace maze
{
  /// Base expression
  template <typename T,
	    typename ExpTCs>
  struct Expr
  {
    
  private:
    
    PROVIDE_DEFEAT_METHOD(T);
    
    DECLARE_DISPATCHABLE_TAG(FULLY_EVALUATE);
    DECLARE_DISPATCHABLE_TAG(PARTIALLY_EVALUATE);
    DECLARE_DISPATCHABLE_TAG(BIND);
    
    /// Subscribe evaluating the expression
    template <typename...TC>
    INLINE_FUNCTION CUDA_HOST_DEVICE
    decltype(auto)_subscribe(FULLY_EVALUATE,
			     const TensorCompFeat<TC>&...unorderedTc)
      const
    {
      /// Reordered components
      auto orderedTc=
	fillTuple<TensorComps<TC...>>(unorderedTc.deFeat()...);
      
      return deFeat().eval(orderedTc);
    }
    
    /// Subscribe partially evaluating
    ///
    /// Call the partialEval method
    template <typename...TC>
    INLINE_FUNCTION CUDA_HOST_DEVICE
    void _subscribe(PARTIALLY_EVALUATE,
		   const TensorCompFeat<TC>&...tc)
      const
    {
      CRASHER<<"Not implemented yet"<<endl;
      /// TBI
    }
    
    /// Subcribe binding the component
    ///
    /// Returns a bound expression
    template <typename...TC>
    INLINE_FUNCTION CUDA_HOST_DEVICE
    void _subscribe(BIND,
		   const TensorCompFeat<TC>&...tc)
      const
    {
      CRASHER<<"Not implemented yet"<<endl;
      ///TBI
      }
    
    /// Number of components
    static constexpr int nComps=
      std::tuple_size_v<ExpTCs>;
    
  public:
    
    /// Subscrbe a single component
    template <typename TC>
    INLINE_FUNCTION CUDA_HOST_DEVICE
    decltype(auto) operator[](const TensorCompFeat<TC>& tc) const
    {
      static_assert((TupleHasType<TC,ExpTCs>),"Type not present in the expression");
      
      /// Check whether the subscribing lead to a full evaluation
      static constexpr bool fullyEval=
	(nComps==1);
      
      using HowToEvaluate=
	std::conditional_t<fullyEval,FULLY_EVALUATE,BIND>;
      
      static_assert(std::is_same_v<HowToEvaluate,FULLY_EVALUATE>,"Cannot bind yet");
      
      return _subscribe(DISPATCH(HowToEvaluate),
			tc.deFeat());
    }
    
    /// Subscribe many components
    template <typename...TC>
    INLINE_FUNCTION CUDA_HOST_DEVICE
    decltype(auto) operator()(const TensorCompFeat<TC>&...tc) const
    {
      static_assert((TupleHasType<TC,ExpTCs>&...),"Types not present in the expression");
      
      /// Number of subscribed components
      static constexpr int nSubComps=
	sizeof...(tc);
      
      /// Check whether the subscribing lead to a full evaluation
      static constexpr bool fullyEval=
	(nSubComps==nComps);
      
      using HowToEvaluate=
	std::conditional_t<fullyEval,FULLY_EVALUATE,BIND>;
      
      static_assert(std::is_same_v<HowToEvaluate,FULLY_EVALUATE>,"Cannot bind yet");
      
      return _subscribe(DISPATCH(HowToEvaluate),
			tc.deFeat()...);
    }
    
    /// Subscribe many components in a tuple format
    template <typename...TC>
    INLINE_FUNCTION CUDA_HOST_DEVICE
    decltype(auto) operator()(const TensorComps<TC>&...tc) const
    {
      return (*this)(std::get<TC>(tc)...);
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD_GPU(operator());
    
    PROVIDE_ALSO_NON_CONST_METHOD_GPU(operator[]);
    
    /////////////////////////////////////////////////////////////////
    
    /// Check assignability of rhs into lhs
    static void assertExprCanBeAssigned()
    {
    }
    
    /// Assignment operator implementation
    ///
    /// Do not call directly: no self-assignemnt check is performed
    template <typename R,
	      typename RCs>
    T& _assign(const Expr<R,RCs>& rhs)
    {
      static_assert(T::canBeAssigned,"Trying to assign to a non-assignable expression");
      
      /// Dynamic components of THIS
      using DynTCs=
	GetDynamicCompsOfTensorComps<ExpTCs>;
      
      forEachInTuple(DynTCs(),[this,&rhs](const auto& t) -> void
			      {
				/// Component under analysis
				using C=std::decay_t<decltype(t)>;
				
				const auto thisCompSize=
				  rhs.deFeat().template compSize<C>();
				
				const auto rhsCompSize=
				  this->deFeat().template compSize<C>();
				
				if(thisCompSize!=rhsCompSize)
				  CRASHER<<"Dynamic component "<<nameOfType((C*)nullptr)<<" of lhs has size "<<thisCompSize<<" when rhs has size "<<rhsCompSize<<endl;
			      });
      
      loopOnAllComponentsValues(this->deFeat(),
				[this,&rhs](auto,const auto&...comps)
				{
				  /// Take components in a tule format
				  const auto c=
				    make_tuple(comps...);
				  
				  this->deFeat()(c)=rhs.deFeat()(c);
				});
      
      return this->deFeat();
    }
    
    /// Provides the assignment operator=, to be imported
    template <typename R,
	      typename RCs>
    T& operator=(const Expr<R,RCs>& rhs)
    {
      return this->_assign(rhs);
    }
  };
}

#endif
