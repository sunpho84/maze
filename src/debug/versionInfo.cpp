#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file versionInfo.cpp

#define EXTERN_VERSION_INFO
# include <debug/versionInfo.hpp>

#include <git_info.hpp>

namespace maze
{
  void printVersionAndCompileFlags(std::ofstream& out)
  {
    using std::endl;
    
    out<<endl;
    out<<"Git hash: "<<GIT_HASH<<", last commit at "<<GIT_TIME<<" with message: \""<<GIT_LOG<<"\""<<endl;
    out<<"Configured at "<<CONFIGURE_TIME<<" with flags: "<<CONFIGURE_FLAGS<<endl;
    out<<"Library compiled at "<<__TIME__<<" of "<<__DATE__<<
      ", executable compiled at "<<resources::PROG_COMPILE_TIME<<" of "<<resources::PROG_COMPILE_DATE<<endl;
    out<<endl;
  }
}
