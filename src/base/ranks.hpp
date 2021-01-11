#ifndef _RANKS_HPP
#define _RANKS_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file ranks.hpp
///
/// MPI parallelization

#include <mpi.h>

#ifndef EXTERN_RANK
# define EXTERN_RANK extern
#define INIT_RANK_TO(...)
#else
# define INIT_RANK_TO(...) (__VA_ARGS__)
#endif

namespace maze
{
  namespace resources
  {
    /// Current rank
    EXTERN_RANK int thisRank INIT_RANK_TO(0);
    
    /// Total number of ranks
    EXTERN_RANK int nRanks INIT_RANK_TO(1);
    
    /// Rank that prints
    EXTERN_RANK int printingRank INIT_RANK_TO(0);
  }
  
  /// Returns current rank
  inline const int& thisRank()
  {
    return resources::thisRank;
  }
  
  /// Returns the total number of ranks
  inline const int& nRranks()
  {
    return resources::nRanks;
  }
  
  /// Returns true on master rank
  inline bool isMasterRank()
  {
    return thisRank()==0;
  }
  
  /// Returns the printing rank
  inline const int& getPrintingRank()
  {
    return resources::printingRank;
  }
  
  /// Set the printing rank
  inline void setPrintingRank(const int& i)
  {
    resources::printingRank=i;
  }
  
  /// Check if this rank is printing
  inline bool thisRankPrints()
  {
    return getPrintingRank()==thisRank();
  }
  
  /// Initialize MPI
  void initRanks(int& narg,char **&arg);
  
  /// Finalize MPI
  void finalizeRanks();
  
  /// Abort all ranks
  void ranksAbort(const int& errNo);
  
  /// Barrier
  void ranksBarrier();
  
}

#undef EXTERN_RANK
#undef INIT_RANK_TO

#endif
