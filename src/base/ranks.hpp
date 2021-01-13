#ifndef _RANKS_HPP
#define _RANKS_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file ranks.hpp
///
/// MPI parallelization

#include <mpi.h>

#include <tensors/component.hpp>

#ifndef EXTERN_RANK
# define EXTERN_RANK extern
#define INIT_RANK_TO(...)
#else
# define INIT_RANK_TO(...) (__VA_ARGS__)
#endif

namespace maze
{
  DECLARE_COMPONENT(MpiRank,int32_t,DYNAMIC,mpiRank);
  
  namespace resources
  {
    /// Current rank
    EXTERN_RANK MpiRank thisRank INIT_RANK_TO(0);
    
    /// Total number of ranks
    EXTERN_RANK MpiRank nRanks INIT_RANK_TO(1);
    
    /// Rank that prints
    EXTERN_RANK MpiRank printingRank INIT_RANK_TO(0);
  }
  
  /// Returns current rank
  inline const MpiRank& thisRank()
  {
    return resources::thisRank;
  }
  
  /// Returns the total number of ranks
  inline const MpiRank& nRranks()
  {
    return resources::nRanks;
  }
  
  /// Returns true on master rank
  inline bool isMasterRank()
  {
    return thisRank()==0;
  }
  
  /// Returns the printing rank
  inline const MpiRank& getPrintingRank()
  {
    return resources::printingRank;
  }
  
  /// Set the printing rank
  inline void setPrintingRank(const MpiRank& i)
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
