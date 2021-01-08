#ifndef _TENSOR_DECL_HPP
#define _TENSOR_DECL_HPP

/// \file tensorDecl.hpp
///
/// \brief Declaration of tensor class

#include <tensors/tensorStorage.hpp>

namespace maze
{
  /// Default storage for a tensor
  constexpr StorLoc DefaultStorage=
    StorLoc::
#ifdef USE_CUDA
    ON_GPU
#else
    ON_CPU
#endif
    ;
  
  /// Tensor with Comps components, of Fund fundamental type
  ///
  /// Forward definition to capture actual components
  template <typename Comps,
	    typename Fund=double,
	    StorLoc SL=DefaultStorage,
	    Stackable IsStackable=Stackable::MIGHT_GO_ON_STACK>
  struct Tensor;
}

#endif
