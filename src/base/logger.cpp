#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file logger.hpp
///
/// \brief Implement the banner

#define EXTERN_LOGGER
# include <base/logger.hpp>

#include <unistd.h>
#include <sys/ioctl.h>

namespace maze
{
  void printBanner()
  {
    /// Width of the message
    const int messageWidth=67;
    
    // Window size
    struct winsize w;
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
    
    /// Check if we are on a terminal
    const bool isTerminal=isatty(STDOUT_FILENO);
    
    /// Terminal output size
    const int width=isTerminal?(w.ws_col):(messageWidth+10);
    
    if(width>=messageWidth)
      {
	/// Padding size
	const int n=(width-messageWidth)/2;
	char sp[n+1];
	for(int i=0;i<n;i++) sp[i]=' ';
	sp[n]='\0';
	
	LOGGER<<endl;
	LOGGER<<sp<<"      ___           ___           ___           ___     "<<sp<<endl;
	LOGGER<<sp<<"     /__/\\         /  /\\         /  /\\         /  /\\    "<<sp<<endl;
	LOGGER<<sp<<"    |  |::\\       /  /::\\       /  /::|       /  /:/_   "<<sp<<endl;
	LOGGER<<sp<<"    |  |:|:\\     /  /:/\\:\\     /  /:/:|      /  /:/ /\\  "<<sp<<endl;
	LOGGER<<sp<<"  __|__|:|\\:\\   /  /:/~/::\\   /  /:/|:|__   /  /:/ /:/_ "<<sp<<endl;
	LOGGER<<sp<<" /__/::::| \\:\\ /__/:/ /:/\\:\\ /__/:/ |:| /\\ /__/:/ /:/ /\\"<<sp<<endl;
	LOGGER<<sp<<" \\  \\:\\~~\\__\\/ \\  \\:\\/:/__\\/ \\__\\/  |:|/:/ \\  \\:\\/:/ /:/"<<sp<<endl;
	LOGGER<<sp<<"  \\  \\:\\        \\  \\::/          |  |:/:/   \\  \\::/ /:/ "<<sp<<endl;
	LOGGER<<sp<<"   \\  \\:\\        \\  \\:\\          |  |::/     \\  \\:\\/:/  "<<sp<<endl;
	LOGGER<<sp<<"    \\  \\:\\        \\  \\:\\         |  |:/       \\  \\::/   "<<sp<<endl;
	LOGGER<<sp<<"     \\__\\/         \\__\\/         |__|/         \\__\\/    "<<sp<<endl;
      }
  };
}
