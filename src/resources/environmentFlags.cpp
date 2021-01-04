#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file environmentFlags.cpp
///
/// \brief Routines needed to read environment variables

#define EXTERN_ENVIRONMENT_FLAGS
# include <resources/environmentFlags.hpp>

#include <sstream>

#include <base/logger.hpp>
#include <unroll/forEachInTuple.hpp>

namespace maze
{
  void readAllFlags()
  {
    LOGGER<<endl;
    forEachInTuple(flagList,[](auto& f)
			  {
			    /// Flag to be parsed
			    auto& flag=*std::get<0>(f);
			    
			    /// Default value
			    const auto& def=std::get<1>(f);
			    
			    /// Tag to be used to parse
			    const char* tag=std::get<2>(f);
			    
			    /// Description to be used to parse
			    const char* descr=std::get<3>(f);
			    
			    LOGGER<<"Flag "<<tag<<" ("<<descr<<") ";
			    
			    /// Search for the tag
			    const char* p=getenv(tag);
			    
			    if(p!=NULL)
			      {
				/// Convert from string
				std::istringstream is(p);
				is>>flag;
				
				LOGGER<<"changed from "<<def<<" to: ";
			      }
			    else
			      {
				LOGGER<<"set to default value: ";
				flag=def;
			      }
			    LOGGER<<flag<<endl;
			  });
  }
}
