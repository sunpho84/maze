#ifndef _GDB_ATTACH_HPP
#define _GDB_ATTACH_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file gdbAttach.hpp

#include <fstream>

#ifndef EXTERN_GDB_ATTACH
# define EXTERN_GDB_ATTACH extern
#endif

namespace maze
{
  /// Wait to attach gdb
  EXTERN_GDB_ATTACH bool waitToAttachDebuggerFlag;
  
  /// Implements the trap to debug
  void possiblyWaitToAttachDebugger();
  
  /// Print version, configuration and compilation time
  void printVersionAndCompileFlags(std::ofstream& out);
}

#endif
