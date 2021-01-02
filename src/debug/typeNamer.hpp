#ifndef _TYPENAMER_HPP
#define _TYPENAMER_HPP

/// \file typeNamer.hpp
///
/// \brief Given a type, it provides a string with it

#include <metaProgramming/hasMember.hpp>
#include <metaProgramming/templateEnabler.hpp>

#include <string>

namespace maze
{
  PROVIDE_HAS_MEMBER(nameOfType);
  
  /// Generic call to related method for a class type
  template <typename T,
	    ENABLE_THIS_TEMPLATE_IF(not hasMember_nameOfType<T>)>
  std::string nameOfType(T*)
  {
    /// Mangled name
    std::string name=
      typeid(T).name();
    
#ifdef HAVE_CXXABI_H
    
    /// Status returned
    int status=0;
    
    /// Demangled name
    char* demangled=
      abi::__cxa_demangle(name.c_str(),nullptr,nullptr,&status);
    
    if(status==0)
      name=demangled;
    
    if(status!=-1)
      free(demangled);
    
#endif
    
    return
      name;
  }
  
  /// Generic call to related method for a class type
  template <typename T,
	    ENABLE_THIS_TEMPLATE_IF(hasMember_nameOfType<T>)>
  std::string nameOfType(T*)
  {
    return
      std::decay_t<T>::nameOfType();
  }
  
  /// Returns the name of a type
#define NAME_OF_TYPE(A...) \
  maze::nameOfType((A*){nullptr})
}

#endif
