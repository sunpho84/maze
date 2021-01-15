#ifndef _LEBESGUE_HPP
#define _LEBESGUE_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file Lebesgue.hpp

#include <lattice/hCubeIndexer.hpp>
#include <utilities/factorize.hpp>

namespace maze
{
  /// Structure to compute Lebesgue index
  template <typename HC,
	    typename LebIndex>
  struct LxOfLebesgueCalculator :
    public LxIndexDeducer<LxOfLebesgueCalculator<HC,LebIndex>>
  {
    /// Reference hCube
    const HC& hCube;
    
    /// Number of dimensions
    static constexpr int nDims=
      HC::nDims;
    
    /// Index type
    using LxIndex=
      typename HC::Index;
    
    /// Factors needed to compute Leb index
    std::vector<std::vector<int>> factors;
    
    /// Compute the Lx of a given Lebesgue
    LxIndex operator()(const LebIndex& _Leb) const
    {
      LebIndex Leb=_Leb;
      
      /// Number of factors
      const size_t& nFactors=factors.size();
      Coords<nDims> c,t1,t2,t3;
      
      for(int i=0;i<nDims;i++)
	{
	  t1[i]=t2[i]=c[i]=0;
	  t3[i]=(nDims*nFactors-1)%nFactors;
	}
      
      /// Convert to mixed base
      int xMixedBase[nDims][nDims*nFactors];
      memset(xMixedBase,0,sizeof(xMixedBase));
      
      for(int i=0;i<(int)(nDims*nFactors);i++)
	{
	  int mu=nDims-1-(i%nDims);
	  int f=factors[t1[mu]][mu];
	  
	  xMixedBase[mu][t2[mu]]=Leb%f;
	  Leb/=f;
	  
	  t1[mu]=(t1[mu]+1)%nFactors;
	  t2[mu]++;
	}
      
      //build coordinate in lx format
      for(int mu=0;mu<nDims;mu++)
	for(int j=nDims*nFactors-1;j>=0;j--)
	  {
	    c[mu]=xMixedBase[mu][j]+factors[t3[mu]][mu]*c[mu];
	    t3[mu]=(t3[mu]+nFactors-1)%nFactors;
	  }
      
      return hCube.computeLxOfCoords(c);
    }
    
    /// Constructor
    LxOfLebesgueCalculator(const HC& hCube) : hCube(hCube)
    {
      /// Get nmax_fact
      int nMaxFacts=0;
      for(int mu=0;mu<nDims;mu++)
	nMaxFacts=std::max(nMaxFacts,(int)factorize(hCube.sizes[mu]).size());
      
      /// Set all factors to 1
      factors.resize(nMaxFacts,std::vector<int>(nDims));
      for(int i=0;i<nMaxFacts;i++)
	for(int mu=0;mu<nDims;mu++)
	  factors[i][mu]=1;
      
      //put all the non-1 factors
      for(int mu=0;mu<nDims;mu++)
	{
	  const std::vector<int> listFactMu=factorize(hCube.sizes[mu]);
	  const int nFacts=listFactMu.size();
	  const int nFacts1=nMaxFacts-nFacts;
	  for(int ifact=0;ifact<nFacts;ifact++)
	    factors[nFacts1+ifact][mu]=listFactMu[ifact];
	}
    }
  };
  
  /// Returns a Lebesgue index calculator
  template <typename LebIndex,
	    typename HC>
  auto getLxOfLebesgueCalculator(const HC& hCube)
  {
    return LxOfLebesgueCalculator<HC,LebIndex>(hCube);
  }
  
  /// Returns an index over an hypercube
  template <typename LebSite,
	    typename HC>
  auto getLebesgueIndexer(const HC& hCube)
  {
    return getHCubeIndexer<LebSite>(hCube,getLxOfLebesgueCalculator<LebSite>(hCube));
  }
}

#endif
