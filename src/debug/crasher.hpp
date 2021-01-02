#ifndef _CRASHER_HPP
#define _CRASHER_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file crasher.hpp
///
/// \brief Crash with style

#ifndef EXTERN_DEBUG
 
 /// Make external if put in front of a variable
 ///
 /// Actual allocation is done in the cpp file
# define EXTERN_DEBUG extern
 
#endif

#ifdef USE_CUDA
# include <cuda_runtime.h>
#endif

#ifdef HAVE_CXXABI_H
# include <cxxabi.h>
#endif

#include <fstream>

#include <base/logger.hpp>

namespace maze
{
  /// Wait to attach gdb
  EXTERN_DEBUG bool waitToAttachDebuggerFlag;
  
  /// Write the list of called routines
  void printBacktraceList(std::ofstream&);
  
  /// A Structure allowing to pass the crashing message and crash at the end
  struct Crasher
  {
    /// Failing line
    const int lineNo;
    
    /// Failing file name
    const char* fileName;
    
    /// Failing function name
    const char* funcName;
    
    /// Constructor taking the line number, the file name and the function name as an input
    Crasher(const int lineNo,const char* fileName,const char* funcName) : lineNo(lineNo),fileName(fileName),funcName(funcName)
    {
      errLogger()<<"\x1b[31m" "ERROR on line "<<lineNo<<" of file \""<<fileName<<"\", function \""<<funcName<<"\", message error: \"";
    }
    
    /// Overload calling with any operand
    template <typename T>
    Crasher& operator<<(T&& t)
    {
      errLogger()<<std::forward<T>(t);
      
      return
	*this;
    }
    
    /// Overload calling with a function, to capure endl
    Crasher& operator<<(std::ostream&(*p)(std::ostream&))
    {
      errLogger()<<p;
      
      return
	*this;
    }
    
    /// Destroy exiting
    ~Crasher()
    {
      errLogger()<<"\".\n\x1b[0m";
      printBacktraceList(errLogger());
      ranksAbort(0);
    }
  };
  
/// Invoke the crasher, passing line, file and function
#define CRASHER Crasher(__LINE__,__FILE__,__FUNCTION__)

 
  
  /// Implements the trap to debug
  void possiblyWaitToAttachDebugger();
  
  /// Print version, configuration and compilation time
  void printVersionAndCompileFlags(std::ofstream& out);
  
  
}

#undef EXTERN_DEBUG

#endif
