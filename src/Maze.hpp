#ifndef _MAZE_HPP
#define _MAZE_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file Maze.hpp
///
/// \brief Include all headers

#include <Base.hpp>
#include <Debug.hpp>
#include <Geometry.hpp>
#include <MetaProgramming.hpp>
#include <Resources.hpp>
#include <Threads.hpp>

/// \brief Main namespace for the library
namespace maze
{
  /// Hides internal implementation not to be used directly
  namespace resources
  {
   /// Holds the time at which the program has been compiled
    char PROG_COMPILE_TIME[]=__TIME__;
    
    /// Holds the date at which the program has been compiled
    char PROG_COMPILE_DATE[]=__DATE__;
  }
  
  /// Initialize the library
  template <typename F>
  inline void initMaze(F&& f,int& narg,char **&arg)
  {
    initRanks(narg,arg);
    
    printBanner();
    
    readAllFlags();
    
    printVersionAndCompileFlags(LOGGER);
    
    possiblyWaitToAttachDebugger();
    
    cpuMemoryManager=new CPUMemoryManager;
    //cpuMemoryManager->disableCache();
    
    //initCuda();
    
#ifdef USE_CUDA
    gpuMemoryManager=new GPUMemoryManager;
#endif
    
    ThreadPool::poolStart(f,narg,arg);
  }
  
  /// Finalizes
  inline void finalizeMaze()
  {
    ThreadPool::poolStop();
    
    delete cpuMemoryManager;
    
#ifdef USE_CUDA
    delete gpuMemoryManager;
#endif
    
    LOGGER<<endl<<"Ariciao!"<<endl<<endl;
    
    finalizeRanks();
  }
}

#endif
