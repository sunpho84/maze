#ifndef _TENSOR_STORAGE_HPP
#define _TENSOR_STORAGE_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <resources/memoryManager.hpp>
#include <resources/storLoc.hpp>
#include <tensors/componentSize.hpp>

/// \file tensorStorage.hpp
///
/// \todo Maybe use Vector for dynamic storage

namespace maze
{
  /// Stackability
  enum class Stackable{CANNOT_GO_ON_STACK,MIGHT_GO_ON_STACK};
  
  /// Basic storage, to use to detect storage
  template <typename T>
  struct BaseTensorStorage
  {
    PROVIDE_DEFEAT_METHOD(T);
  };
  
  /// Class to store the data
  template <typename Fund,            // Fundamental type
	    Size StaticSize,          // Size konwn at compile time
	    StorLoc SL,               // Location where to store data
	    Stackable IsStackable=    // Select if can go or not on stack
	    Stackable::MIGHT_GO_ON_STACK>
  struct TensorStorage
  {
    /// Structure to hold dynamically allocated data
    struct DynamicStorage
    {
      /// Hold info if it is a reference
      bool isRef;
      
      /// Storage
      Fund* data;
      
      /// Allocated size
      Size dynSize;
      
      /// Returns the size
      constexpr Size getSize()
	const
      {
	return
	  dynSize;
      }
      
      /// Returns the pointer to data
      CUDA_HOST_DEVICE
      decltype(auto) getDataPtr() const
      {
	return
	  data;
      }
      
      PROVIDE_ALSO_NON_CONST_METHOD_GPU(getDataPtr);
      
      /// Construct allocating data
      DynamicStorage(const Size& dynSize) :
	isRef(false),
	data(memoryManager<SL>()->template provide<Fund>(dynSize)),
	dynSize(dynSize)
      {
      }
      
      /// "Copy" constructor, actually taking a reference
      CUDA_HOST_DEVICE
      DynamicStorage(const DynamicStorage& oth) :
	isRef(true),
	dynSize(oth.dynSize),
	data(oth.data)
      {
      }
      
      /// Create a reference starting from a pointer
      CUDA_HOST_DEVICE
      DynamicStorage(Fund* oth,
		     const Size& dynSize) :
	isRef(true),
	data(oth),
	dynSize(dynSize)
      {
      }
      
      /// Move constructor
      CUDA_HOST_DEVICE
      DynamicStorage(DynamicStorage&& oth) :
	isRef(oth.isRef),
	data(oth.data),
	dynSize(oth.dynSize)
      {
	oth.isRef=true;
	oth.data=nullptr;
	oth.dynSize=0;
      }
      
      /// Move assignment
      CUDA_HOST_DEVICE
      DynamicStorage& operator=(DynamicStorage&& oth)
      {
	std::swap(isRef,oth.isRef);
	std::swap(data,oth.data);
	std::swap(dynSize,oth.dynSize);
	
	return *this;
      }
      
      #ifndef COMPILING_FOR_DEVICE
      /// Destructor deallocating the memory
      ~DynamicStorage()
      {
	if(not isRef)
	  memoryManager<SL>()->release(data);
      }
#endif
    };
    
    /// Structure to hold statically allocated data
    struct StackStorage
    {
      /// Returns the size
      constexpr Size getSize()
	const
      {
	return
	  StaticSize;
      }
      
      /// Storage
      Fund data[StaticSize];
      
      /// Return the pointer to inner data
      INLINE_FUNCTION CUDA_HOST_DEVICE
      const Fund* getDataPtr()
	const
      {
	return
	  data;
      }
      
      PROVIDE_ALSO_NON_CONST_METHOD_GPU(getDataPtr);
      
      /// Constructor: since the data is statically allocated, we need to do nothing
      CUDA_HOST_DEVICE
      StackStorage(const Size& size=0)
      {
      }
      
      /// Copy constructor
      CUDA_HOST_DEVICE
      explicit StackStorage(const StackStorage& oth)
      {
	memcpy(this->data,oth.data,StaticSize);
      }
      
      // /// Move constructor is deleted
      // CUDA_HOST_DEVICE
      // StackStorage(StackStorage&&) =delete;
    };
    
    /// Threshold beyond which allocate dynamically in any case
    static constexpr
    Size MAX_STACK_SIZE=2304;
    
    /// Decide whether to allocate on the stack or dynamically
    static constexpr
    bool stackAllocated=
      (StaticSize!=DYNAMIC) and
      (IsStackable==Stackable::MIGHT_GO_ON_STACK) and
      (StaticSize*sizeof(Fund)<=MAX_STACK_SIZE) and
      ((CompilingForDevice==true  and SL==StorLoc::ON_GPU) or
       (CompilingForDevice==false and SL==StorLoc::ON_CPU));
    
    /// Actual storage class
    using ActualStorage=
      std::conditional_t<stackAllocated,StackStorage,DynamicStorage>;
    
    /// Storage of data
    ActualStorage data;
    
    /// Returns the pointer to data
    CUDA_HOST_DEVICE
    decltype(auto) getDataPtr() const
    {
      return data.getDataPtr();
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD_GPU(getDataPtr);
    
    /// Returns the size
    constexpr Size getSize()
      const
    {
      return
	data.getSize();
    }
    
    /// Construct taking the size to allocate
    TensorStorage(const Size& size) ///< Size to allocate
      : data(size)
    {
    }
    
    /// Creates starting from a reference
    CUDA_HOST_DEVICE
    TensorStorage(Fund* oth,
		const Size& size) :
      data(oth,size)
    {
      static_assert(stackAllocated==false,"Only dynamic allocation is possible when creating a reference");
    }
    
    /// Construct taking the size to allocate
    TensorStorage()
    {
      static_assert(stackAllocated,"If not stack allocated must pass the size");
    }
    
    /// Copy constructor
    CUDA_HOST_DEVICE
    TensorStorage(const TensorStorage& oth) : data(oth.data)
    {
    }
    
    /// Move constructor
    CUDA_HOST_DEVICE
    TensorStorage(TensorStorage&& oth) : data(std::move(oth.data))
    {
    }
    
    /// Copy assignemnt
    CUDA_HOST_DEVICE
    TensorStorage& operator=(const TensorStorage& oth)
    {
      data=oth.data;
      
      return *this;
    }
    
    /// Move assignemnt
    CUDA_HOST_DEVICE
    TensorStorage& operator=(TensorStorage&& oth)
    {
      data=std::move(oth.data);
      
      return *this;
    }
    
    /// Access to a sepcific value via subscribe operator
    template <typename T>                  // Subscribed component type
    const Fund& operator[](const T& t)   ///< Subscribed component
      const
    {
      return
	data.data[t];
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(operator[]);
  };
}

#endif
