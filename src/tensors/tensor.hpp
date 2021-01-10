#ifndef _TENSOR_HPP
#define _TENSOR_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file tensor.hpp
///
/// \brief Implements all functionalities of tensors

//#include <expr/expr.hpp>
#include <tensors/tensorDecl.hpp>
#include <tensors/complex.hpp>
#include <resources/storLoc.hpp>
#include <tensors/componentsList.hpp>
#include <tensors/tensorFeat.hpp>
#include <tensors/tensorSlice.hpp>
#include <utilities/tuple.hpp>

namespace maze
{
  /// Short name for the tensor
 #define THIS					\
  Tensor<TensorComps<TC...>,F,SL,IsStackable>
  
  /// Tensor
  template <typename F,
	    StorLoc SL,
	    typename...TC,
	    Stackable IsStackable>
  struct THIS : public
    // Expr<THIS>,
    ComplexSubscribe<THIS>,
    TensorFeat<IsTensor,THIS>
  {
    // /// Import assign operator from expression
    // using Expr<THIS>::operator=;
    
    // INLINE_FUNCTION CUDA_HOST_DEVICE
    // decltype(auto) operator=(const THIS& oth)
    // {
    //   *this=
    // 	static_cast<const Expr<THIS>&>(oth);
      
    //   return
    // 	*this;
    // }
    
    /// Tensor should not be copied when taken as arguments in expression
    static constexpr bool takeAsArgByRef=
      true;
    
    /// Tensor can be assigned
    static constexpr bool canBeAssigned=
      true;
    
    /// Fundamental type
    using Fund=
      F;
    
    /// Components
    using Comps=
      TensorComps<TC...>;
    
    /// Get the I-th component
    template <int I>
    using Comp=
      std::tuple_element_t<I,Comps>;
    
    /// Determine whether this can be simdfified
    template <typename _Fund=Fund,
	      typename _Comps=Comps,
	      ENABLE_THIS_TEMPLATE_IF(std::tuple_size<_Comps>::value>0)>
    static constexpr bool _canBeSimdified()
    {
      /// Last component type
      using LastComp=
	Comp<sizeof...(TC)-1>;
      
      return
	LastComp::template canBeSimdified<Fund>;
    }
    
    /// Determine whether this can be simdfified
    template <typename _Fund=Fund,
	      typename _Comps=Comps,
	      ENABLE_THIS_TEMPLATE_IF(std::tuple_size<_Comps>::value==0)>
    static constexpr bool _canBeSimdified()
    {
      return
	false;
    }
    
    static constexpr bool canBeSimdified=
      _canBeSimdified<Fund,Comps>();
    
    /// Provide constant/not constant simdify method when not simdifiable
#define PROVIDE_SIMDIFY(CONST_ATTR)					\
    /*! Convert into simdified, CONST_ATTR case */			\
    template <typename _F=F,						\
	      ENABLE_THIS_TEMPLATE_IF					\
	      (not _canBeSimdified<_F>())>				\
    decltype(auto) simdify()						\
      CONST_ATTR							\
    {									\
      return								\
	*this;								\
    }
    
    PROVIDE_SIMDIFY(const);
    PROVIDE_SIMDIFY(/* non const */);
    
#undef PROVIDE_SIMDIFY
    
    /// Provide constant/not constant simdify method when simdifiable
    ///
    //// Last component is of the same size of simd_length
#define PROVIDE_SIMDIFY(CONST_ATTR)					\
    /*! Convert into simdified, CONST_ATTR case */			\
    template <typename _F=F,						\
	      ENABLE_THIS_TEMPLATE_IF					\
	      (_canBeSimdified<_F>())>					\
    auto simdify()							\
      CONST_ATTR							\
    {									\
      return								\
	Tensor<TupleAllButLast<TensorComps<TC...>>,Simd<F>,SL,Stackable::CANNOT_GO_ON_STACK> \
	((Simd<F>*)(this->getDataPtr()),this->data.getSize(),dynamicSizes); \
      }
    
    PROVIDE_SIMDIFY(const);
    PROVIDE_SIMDIFY(/* non const */);
    
#undef PROVIDE_SIMDIFY
    
    /// Provide eval method, converting to fundamental
#define PROVIDE_EVAL_METHOD(CONST_ATTR)					\
    /*! Operator to return direct access to data */			\
    template <typename Cp=Comps,					\
	      ENABLE_THIS_TEMPLATE_IF(std::tuple_size<Cp>::value==0)>	\
    CUDA_HOST_DEVICE constexpr INLINE_FUNCTION				\
    decltype(auto) eval()						\
      CONST_ATTR							\
    {									\
      return								\
	trivialAccess(0);						\
    }
    
    PROVIDE_EVAL_METHOD(/* not const */);
    PROVIDE_EVAL_METHOD(const);
    
#undef PROVIDE_EVAL_METHOD
    
    /// Storage Location
    static constexpr
    StorLoc storLoc=
      SL;
    
    /// Type to be used for the index
    using Index=
      std::common_type_t<int,TC...>;
    
    /// List of all statically allocated components
    using StaticComps=
      TupleFilter<SizeIsKnownAtCompileTime<true>::t,TensorComps<TC...>>;
    
    /// List of all dynamically allocated components
    using DynamicComps=
      TupleFilter<SizeIsKnownAtCompileTime<false>::t,TensorComps<TC...>>;
    
    /// Sizes of the dynamic components
    const DynamicComps dynamicSizes;
    
    /// Static size
    static constexpr Index staticSize=
      ((TC::SizeIsKnownAtCompileTime?
	TC::Base::sizeAtCompileTime:
       Index{1})*...);
    
    /// Size of the Tv component
    ///
    /// Case in which the component size is known at compile time
    template <typename Tv,
	      ENABLE_THIS_TEMPLATE_IF(Tv::SizeIsKnownAtCompileTime)>
    CUDA_HOST_DEVICE INLINE_FUNCTION
    constexpr auto compSize()
      const
    {
      return
	Tv::Base::sizeAtCompileTime;
    }
    
    /// Size of the Tv component
    ///
    /// Case in which the component size is not knwon at compile time
    template <typename Tv,
	      ENABLE_THIS_TEMPLATE_IF(not Tv::SizeIsKnownAtCompileTime)>
    constexpr CUDA_HOST_DEVICE INLINE_FUNCTION
    const auto& compSize()
      const
    {
      return
	std::get<Tv>(dynamicSizes);
    }
    
    /// Calculate the index - no more components to parse
    constexpr CUDA_HOST_DEVICE INLINE_FUNCTION
    Index orderedCompsIndex(Index outer) ///< Value of all the outer components
      const
    {
      return
	outer;
    }
    
    /// Calculate index iteratively
    ///
    /// Given the components (i,j,k) we must compute ((0*ni+i)*nj+j)*nk+k
    ///
    /// The parsing of the variadic components is done left to right, so
    /// to compute the nested bracket list we must proceed inward. Let's
    /// say we are at component j. We define outer=(0*ni+i) the result
    /// of inner step. We need to compute thisVal=outer*nj+j and pass it
    /// to the inner step, which incorporate iteratively all the inner
    /// components. The first step requires outer=0.
    template <typename T,
    	      typename...Tp>
    constexpr CUDA_HOST_DEVICE INLINE_FUNCTION
    Index orderedCompsIndex(Index outer,        ///< Value of all the outer components
			    T&& thisComp,       ///< Currently parsed component
			    Tp&&...innerComps)  ///< Inner components
      const
    {
      /// Remove reference and all attributes to access to types
      using Tv=
	std::decay_t<T>;
      
      /// Size of this component
      const auto thisSize=
	compSize<Tv>();
      
      //cout<<"thisSize: "<<thisSize<<endl;
      /// Value of the index when including this component
      const auto thisVal=
	outer*thisSize+thisComp;
      
      return
	orderedCompsIndex(thisVal,innerComps...);
    }
    
    /// Intermediate layer to reorder the passed components
    template <typename...T>
    constexpr CUDA_HOST_DEVICE INLINE_FUNCTION
    Index index(const TensorComps<T...>& comps)
      const
    {
      /// Build the index reordering the components
      return
	orderedCompsIndex(0,std::get<TC>(comps)...);
    }
    
    /// Determine whether the components are all static, or not
    static constexpr bool allCompsAreStatic=
      std::is_same<DynamicComps,std::tuple<>>::value;
    
    /// Computes the storage size at compile time, if knwon
    static constexpr Index storageSizeAtCompileTime=
      allCompsAreStatic?(Index)staticSize:(Index)DYNAMIC;
    
    /// Storage type
    using StorageType=
      TensorStorage<Fund,storageSizeAtCompileTime,SL,IsStackable>;
    
    /// Actual storage
    StorageType data;
    
    /// Returns the pointer to the data
    CUDA_HOST_DEVICE
    decltype(auto) getDataPtr()
      const
    {
      return
	data.getDataPtr();
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD_GPU(getDataPtr);
    
    // /// Returns the name of the type
    // static std::string nameOfType()
    // {
    //   return
    // 	std::string("Tensor<")+NAME_OF_TYPE(Fund)+","+storLocTag<SL>()+">";
    // }
    
    /// Initialize the dynamical component \t Out using the inputs
    template <typename Ds,   // Type of the dynamically allocated components
	      typename Out>  // Type to set
    CUDA_HOST_DEVICE
    Index initializeDynSize(const Ds& inputs, ///< Input sizes
			    Out& out)         ///< Output size to set
    {
      out=
	std::get<Out>(inputs);
      
      return
	out;
    }
    
    /// Compute the size needed to initialize the tensor and set it
    template <typename...Td,
	      typename...T>
    CUDA_HOST_DEVICE
    TensorComps<Td...> initializeDynSizes(TensorComps<Td...>*,
					T&&...in)
    {
      static_assert(sizeof...(T)==sizeof...(Td),"Number of passed dynamic sizes not matching the needed one");
      
      return {std::get<Td>(std::make_tuple(in...))...};
    }
    
    /// Initialize the tensor with the knowledge of the dynamic size
    template <typename...TD,
	      ENABLE_THIS_TEMPLATE_IF(sizeof...(TD)>=1)>
    Tensor(const TensorCompFeat<IsTensorComp,TD>&...tdFeat) :
      dynamicSizes{initializeDynSizes((DynamicComps*)nullptr,tdFeat.deFeat()...)},
      data(staticSize*(tdFeat.deFeat()*...))
    {
    }
    
    /// Initialize the tensor when no dynamic component is present
    template <typename...TD,
	      ENABLE_THIS_TEMPLATE_IF(sizeof...(TD)==0)>
    CUDA_HOST_DEVICE
    Tensor() :
      dynamicSizes{}
    {
    }
    
    /// Initialize the tensor when sizes are passed as a TensorComps
    template <typename...C>
    CUDA_HOST_DEVICE
    Tensor(const TensorComps<C...>& tc) :
      Tensor(std::get<C>(tc)...)
    {
    }
    
    
    /// Move constructor
    CUDA_HOST_DEVICE
    Tensor(Tensor<TensorComps<TC...>,Fund,SL>&& oth) : dynamicSizes(oth.dynamicSizes),data(std::move(oth.data))
    {
    }
    
    // /// Copy constructor
    // CUDA_HOST_DEVICE
    // Tensor(const Tensor& oth) :
    //   dynamicSizes(oth.dynamicSizes),
    //   data(oth.data.getSize())
    // {
    //   static_cast<Expr<Tensor>>(*this)=
    // 	static_cast<const Expr<Tensor>&>(oth);
    // }
    
    /// HACK
    ///
    /// \todo Why is a hack?
    template <typename...Dyn>
    CUDA_HOST_DEVICE
    Tensor(Fund* oth,
	 const Size& size,
	 const Dyn&...dynamicSizes) :
      dynamicSizes(dynamicSizes...),data(oth,size)
    {
    }
    
    // /// Build from a generic expression
    // template <typename T>
    // explicit Tensor(const Expr<T>& oth)
    // {
    //   *this=
    // 	oth;
    // }
    
    /// Provide trivial access to the fundamental data
    decltype(auto) trivialAccess(const Index& i)
      const
    {
      return
	data[i];
    }
    
    //PROVIDE_ALSO_NON_CONST_METHOD(trivialAccess);
    
    /// Gets access to the inner data
    // const Fund* getRawAccess()
    //   const
    // {
    //   return
    // 	&trivialAccess(0);
    // }
    
    // PROVIDE_ALSO_NON_CONST_METHOD(getRawAccess);
    
    /// Provide subscribe operator when returning a reference
    ///
    /// \todo move to tag dispatch, so we can avoid the horrible sfinae subtleties
#define PROVIDE_SUBSCRIBE_OPERATOR(CONST_ATTR,CONST_AS_BOOL)		\
    /*! Operator to take a const reference to a given component */	\
    template <typename C,						\
	      typename Cp=Comps,					\
	      ENABLE_THIS_TEMPLATE_IF(TupleHasType<C,Cp>)>		\
    CUDA_HOST_DEVICE INLINE_FUNCTION					\
    auto operator[](const TensorCompFeat<IsTensorComp,C>& cFeat)		\
      CONST_ATTR							\
    {									\
      /*! Subscribed components */					\
      using SubsComps=							\
	TensorComps<C>;							\
									\
      return								\
	TensorSlice<CONST_AS_BOOL,THIS,SubsComps>(*this,SubsComps(cFeat.deFeat())); \
    }
    
    PROVIDE_SUBSCRIBE_OPERATOR(/* not const */, false);
    PROVIDE_SUBSCRIBE_OPERATOR(const, true);
    
#undef PROVIDE_SUBSCRIBE_OPERATOR
    
  };
  
#undef THIS
}

#endif
