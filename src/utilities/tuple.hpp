#ifndef _TUPLE_HPP
#define _TUPLE_HPP

/// \file tuple.hpp
///
/// \brief Functionalities on top of std::tuple

#include <tuple>
#include <utility>

#include <type_traits>
#include <utilities/math.hpp>

namespace maze
{
  namespace impl
    {
      /// Helper to filter a tuple on the basis of a predicate
      ///
      /// Filter a single element, forward declaration
      template <bool,
		typename>
      struct _TupleFilterEl;
      
      /// Helper to filter a tuple on the basis of a predicate
      ///
      /// Filter a single element: True case, in which the type passes
      /// the filter
      template <typename T>
      struct _TupleFilterEl<true,T>
      {
	/// Helper type, used to cat the results
	using type=
	  std::tuple<T>;
	
	/// Filtered value
	const type value;
	
	/// Construct, taking a tuple type and filtering the valid casis
	template <typename Tp>
	_TupleFilterEl(Tp&& t) : ///< Tuple to filter
	  value{std::get<T>(t)}
	{
	}
      };
      
      /// Helper to filter a tuple on the basis of a predicate
      ///
      /// Filter a single element: False case, in which the type does
      /// not pass the filter
      template <typename T>
      struct _TupleFilterEl<false,T>
      {
	/// Helper empty type, used to cat the results
	using type=std::tuple<>;
	
	/// Empty value
	const type value{};
	
	/// Construct without getting the type
	template <typename Tp>
	_TupleFilterEl(Tp&& t) ///< Tuple to filter
	{
	}
      };
  }
  
  /// Returns a tuple, filtering out on the basis of a predicate
  template <template <class> class F,          // Predicate to be applied on the types
	    typename...T>                      // Types contained in the tuple to be filtered
  auto tupleFilter(const std::tuple<T...>& t) ///< Tuple to filter
  {
    return
      std::tuple_cat(impl::_TupleFilterEl<F<T>::value,T>{t}.value...);
  }
  
  /// Type obtained applying the predicate filter F on the tuple T
  template <template <class> class F,
	    typename T>
  using TupleFilter=
    decltype(tupleFilter<F>(*(T*)nullptr));
  
  /////////////////////////////////////////////////////////////////
  
  namespace impl
  {
    /// Directly provides the result of filtering out from a tuple a give
    ///
    /// Forward definition
    template <typename F,
	      typename Tp>
    struct _TupleFilterOut;
    
    /// Cannot use directly the TupleFilter, because of some template template limitation
    template <typename...Fs,
	      typename...Tps>
    struct _TupleFilterOut<std::tuple<Fs...>,std::tuple<Tps...>>
    {
      /// Predicate to filter out
      template <typename T>
      struct Filter
      {
	/// Predicate result, counting whether the type match
	static constexpr bool value=
	  ((std::is_same<T,Fs>::value+...)==0);
      };
      
      /// Returned type
      typedef TupleFilter<Filter,std::tuple<Tps...>> type;
    };
  }
  
  /// Directly provides the result of filtering out the types of the tuple F from Tuple Tp
  template <typename F,
	    typename Tp>
  using TupleFilterOut=
    typename impl::_TupleFilterOut<F,Tp>::type;
  
  /////////////////////////////////////////////////////////////////
  
  namespace impl
  {
    /// Predicate returning whether the type is present in the list a given number of times N
    ///
    /// Forward definition
    template <int N,       // Number of times that the type must be present
	      typename Tp> // Tuple in which to search
    struct _TypeIsInList;
    
    /// Predicate returning whether the type is present in the list
    template <int N,
	      typename...Tp>
    struct _TypeIsInList<N,std::tuple<Tp...>>
    {
      /// Internal implementation
      template <typename T>  // Type to search
      struct t
      {
	/// Predicate result
	static constexpr bool value=
	  ((std::is_same<T,Tp>::value+...)==N);
      };
    };
  }
  
  /// Returns whether the type T is in the tuple Tp N times
  template <typename T,
	    typename Tp,
	    int N=1>
  constexpr bool TupleHasType=
    impl::_TypeIsInList<N,Tp>::template t<T>::value;
  
  /// Returns a tuple containing all types common to the two tuples
  template <typename TupleToSearch,
	    typename TupleBeingSearched>
  using TupleCommonTypes=
    TupleFilter<impl::_TypeIsInList<1,TupleToSearch>::template t,TupleBeingSearched>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns a tuple filled with the arguments of another tuple
  ///
  /// The arguments not passed are null-initialized
  template <typename ResTuple,     ///< Tuple type to be returned, to be provided
	    typename...Args>       ///< Tuple arguments to be filled in
  ResTuple fillTuple(const std::tuple<Args...>& args) ///< Tuple containing the arguments to be passed
  {
    /// Returned tuple
    ResTuple res;
    
    [](auto...){}((std::get<Args>(res)=std::get<Args>(args))...);
    
    return
      res;
  }
  
  /// Returns a tuple filled with a list of arguments
  ///
  /// The arguments not passed are null-initialized
  template <typename ResTuple,     ///< Tuple type to be returned, to be provided
	    typename...Args>       ///< Tuple arguments to be filled in
  ResTuple fillTuple(const Args&...args) ///< Tuple containing the arguments to be passed
  {
    /// Returned tuple
    ResTuple res;
    
    [](auto...){}((std::get<Args>(res)=args)...);
    
    return
      res;
  }
  namespace impl
  {
    template <typename I,
	      typename T>
    struct _TupleElOfList;
    
    template <std::size_t...Is,
	      typename T>
    struct _TupleElOfList<std::index_sequence<Is...>,T>
    {
      using type=
	std::tuple<std::tuple_element_t<Is,T>...>;
    };
  }
  
  /////////////////////////////////////////////////////////////////
  
  /// Type of the tuple obtained removing last element
  template <typename Tp>
  using TupleAllButLast=
    typename impl::_TupleElOfList<std::make_index_sequence<std::tuple_size<Tp>::value-1>,Tp>::type;
  
  namespace impl
  {
    /// Type of the tuple obtained removing first element
    ///
    /// Helper function
    template <typename Head,
	      typename...Tail>
    auto _tupleAllButFirstHelper(std::tuple<Head,Tail...>*) -> std::tuple<Tail...>
    {
    }
  }
  
  /// Type of the tuple obtained removing first element
  template <typename Tp>
  using TupleAllButFirst=
    decltype(impl::_tupleAllButFirstHelper((Tp*)nullptr));
  
  /// Type of the tuple obtained catting two tuples
  template <typename...TP>
  using TupleCat=
    decltype(std::tuple_cat(std::declval<TP>()...));
  
  /////////////////////////////////////////////////////////////////
  
  namespace impl
  {
    /// Position of type T in the tuple
    ///
    /// Internal implementation
    template <typename T,
	      typename...L>
    constexpr int _posOfType(T*,
			    std::tuple<L...>*)
    {
      /// Holds whethere is the same
      constexpr bool isSame[]=
	{std::is_same<T,L>::value...};
      
      int i=0;
      while(i<(int)sizeof...(L) and not isSame[i])
	i++;
      
      return i;
    }
  }
  
  /// Position of type T in the tuple Tp
  ///
  /// Returns tuple size if not found
  template <typename T,
	    typename Tp>
  constexpr int posOfType=
    impl::_posOfType((T*)nullptr,(Tp*)nullptr);
  
  /////////////////////////////////////////////////////////////////
  
  namespace impl
  {
    /// Replace a single type group, by checking if one of the replacements applies
    template <typename T,
	      typename...Repl>
    struct _TupleGroupTypesHelper2
    {
      /// All types of the replacement
      using AllRepl=
	TupleCat<Repl...>;
      
      /// All first match of the replacement
      using AllKeys=
	std::tuple<std::tuple_element_t<0,Repl>...>;
      
      /// All other matches
      using AllNonKeys=
	TupleCat<TupleAllButFirst<Repl>...>;
      
      /// Number of non-keys
      static constexpr int nNonKeys=
	std::tuple_size_v<AllNonKeys>;
      
      /// Is T non-key (not first of a group)
      static constexpr bool isNonKey=
	posOfType<T,AllNonKeys>!=nNonKeys;
      
      /// Index of the key matching T, if any
      static constexpr int iKey=
	posOfType<T,AllKeys>;
      
      /// Number of keys
      static constexpr int nKeys=
	std::tuple_size_v<AllKeys>;
      
      /// Is T key (not first of a group)
      static constexpr bool isKey=
	iKey!=nKeys;
      
      /// Resulting type
      using type=
	std::conditional_t<isNonKey,
			   std::tuple<>,
			   std::conditional_t<isKey,
					      std::tuple<std::tuple_element_t<iKey,std::tuple<Repl...,void>>>,
					      std::tuple<T>>>;
    };
    
    /// Helper class to group types
    ///
    /// Forward declaration
    template <typename Tp,
	      typename...F>
    struct _TupleGroupTypesHelper;
    
    /// Helper class to group types
    template <typename...Tp,
	      typename...F>
    struct _TupleGroupTypesHelper<std::tuple<Tp...>,F...>
    {
      /// Resuting type
      using type=
	TupleCat<typename _TupleGroupTypesHelper2<Tp,F...>::type...>;
    };
  }
  
  /// Group types in a tuple according to a list of groups
  ///
  /// \example
  ///
  ///  using A=std::tuple<int,char,int64_t,uint32_t>;
  ///  using B=std::tuple<char,int64_t>;
  ///  using C=TupleGroupTypes<A,B>; /// std::tuple<int,std::tuple<char,int64_t>,uint32_t>;
  template <typename Tp,
	    typename...F>
  using TupleGroupTypes=
    typename impl::_TupleGroupTypesHelper<Tp,F...>::type;
}

#endif
