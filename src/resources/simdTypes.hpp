#ifndef _SIMD_TYPES_HPP
#define _SIMD_TYPES_HPP

/// \file simdTypes.hpp
///
/// \brief Dispatch the intrinsic datatype

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#ifndef DISABLE_X86_INTRINSICS
# include <immintrin.h>
#endif

#include <cstring>
#include <type_traits>

#include <tensors/arithmeticTensor.hpp>

namespace maze
{
  /// Kinds of instruction set
  enum InstSet{NONE,MMX,AVX,AVX512};
  
  /// Determine whether the simd type exists for the passed type
  template <typename T>
  [[ maybe_unused ]]
  static constexpr bool simdOfTypeExists=
    std::is_same<T,float>::value or
    std::is_same<T,double>::value;
  
  namespace impl
  {
    /// SIMD type for a given instruction set and fundamental
    ///
    /// Forward definition
    template <InstSet IS,
	      typename Fund>
    struct _Simd;
    
    /// Provides the proper datatype for the given fundamental and instruction set
#define PROVIDE_SIMD(INST_SET,FUND,TYPE...)		\
    /*! SIMD FUND for instruction set INST_SET */	\
    template <>						\
    struct _Simd<INST_SET,FUND>				\
    {							\
      /*! Provide the type*/				\
      using Type=TYPE;					\
    }
    
    PROVIDE_SIMD(NONE,float,ArithmeticArray<float,1>);
    PROVIDE_SIMD(NONE,double,ArithmeticArray<double,1>);
    
#ifndef DISABLE_X86_INTRINSICS
    
    PROVIDE_SIMD(AVX,float,__m256);
    PROVIDE_SIMD(AVX,double,__m256d);
    
    PROVIDE_SIMD(MMX,float,__m128);
    PROVIDE_SIMD(MMX,double,__m128d);
    
    PROVIDE_SIMD(AVX512,float,__m512);
    PROVIDE_SIMD(AVX512,double,__m512d);
    
#endif
    
#undef PROVIDE_SIMD
    
    /// Actual intrinsic to be used
    template <typename Fund>
    using ActualSimd=
      typename impl::_Simd<SIMD_INST_SET,Fund>::Type;
  }
  
  /// Length of a SIMD vector
  template <typename Fund>
  constexpr int simdLength=
    sizeof(impl::ActualSimd<Fund>)/sizeof(Fund);
  
  /// Simd datatype
  template <typename Fund>
  using Simd=
#ifndef COMPILING_FOR_DEVICE
    impl::ActualSimd<Fund>
#else
    ArithmeticArray<Fund,simdLength<Fund>>
#endif
    ;
}

#endif
