#ifndef _VERSION_INFO_HPP
#define _VERSION_INFO_HPP

/// \file versionInfo.hpp

#include <fstream>

namespace maze
{
  namespace resources
  {
    /// Compile time, detected when including the Maze.hpp file
    extern char PROG_COMPILE_TIME[];
    
    /// Compile date, detected when including the Maze.hpp file
    extern char PROG_COMPILE_DATE[];
  }
  
  void printVersionAndCompileFlags(std::ofstream& out);
}

#undef EXTERN_VERSION_INFO

#endif
