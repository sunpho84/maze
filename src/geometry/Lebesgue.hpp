#ifndef _LEBESGUE_HPP
#define _LEBESGUE_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file Lebesgue.hpp

#include <geometry/lxGrid.hpp>
#include <utilities/factorize.hpp>

namespace maze
{
  /// Structure to compute Lebesgue index
  template <int NDim,
	    typename Index>
  struct LxOfLebesgueCalculator
  {
    /// Reference grid
    const LxGrid<NDim,Index>& grid;
    
    /// Factors needed to compute Leb index
    std::vector<std::vector<int>> factors;
    
    /// Compute the Lx of a given Lebesgue
    Index operator()(const Index& _Leb) const
    {
      Index Leb=_Leb;
      
      /// Number of factors
      const size_t& nFactors=factors.size();
      Coords<NDim> c,t1,t2,t3;
      
      for(int i=0;i<NDim;i++)
	{
	  t1[i]=t2[i]=c[i]=0;
	  t3[i]=(NDim*nFactors-1)%nFactors;
	}
      
      /// Convert to mixed base
      int xMixedBase[NDim][NDim*nFactors];
      memset(xMixedBase,0,sizeof(xMixedBase));
      
      for(int i=0;i<(int)(NDim*nFactors);i++)
	{
	  int mu=NDim-1-(i%NDim);
	  int f=factors[t1[mu]][mu];
	  
	  xMixedBase[mu][t2[mu]]=Leb%f;
	  Leb/=f;
	  
	  t1[mu]=(t1[mu]+1)%nFactors;
	  t2[mu]++;
	}
      
      //build coordinate in lx format
      for(int mu=0;mu<NDim;mu++)
	for(int j=NDim*nFactors-1;j>=0;j--)
	  {
	    c[mu]=xMixedBase[mu][j]+factors[t3[mu]][mu]*c[mu];
	    t3[mu]=(t3[mu]+nFactors-1)%nFactors;
	  }
      
      return grid.computeLxOfCoords(c);
    }
    
    /// Constructor
    LxOfLebesgueCalculator(const LxGrid<NDim,Index>& grid) : grid(grid)
    {
      /// Get nmax_fact
      int nMaxFacts=0;
      for(int mu=0;mu<NDim;mu++)
	nMaxFacts=std::max(nMaxFacts,(int)factorize(grid.sizes[mu]).size());
      
      /// Set all factors to 1
      factors.resize(nMaxFacts,std::vector<int>(NDim));
      for(int i=0;i<nMaxFacts;i++)
	for(int mu=0;mu<NDim;mu++)
	  factors[i][mu]=1;
      
      //put all the non-1 factors
      for(int mu=0;mu<NDim;mu++)
	{
	  const std::vector<int> listFactMu=factorize(grid.sizes[mu]);
	  const int nFacts=listFactMu.size();
	  const int nFacts1=nMaxFacts-nFacts;
	  for(int ifact=0;ifact<nFacts;ifact++)
	    factors[nFacts1+ifact][mu]=listFactMu[ifact];
	}
    }
  };
}

#endif
