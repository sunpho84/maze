#ifndef _H_CUBE_INDEXER_HPP
#define _H_CUBE_INDEXER_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file hCubeIndexer.hpp

#include <lattice/hCube.hpp>
#include <lattice/indexShuffler.hpp>
#include <metaProgramming/feature.hpp>

namespace maze
{
  /// Base expression
  template <typename T>
  struct Expr;
  DEFINE_FEATURE(IndexDeducerFromLx);
  
  DEFINE_FEATURE(LxIndexDeducer);
  
  /// Keep a lookup table for lx to a given index
  template <typename Index,
	    typename HC>
  struct HCubeIndexer
  {
    /// Number of dimensions
    static constexpr int nDims=
      HC::NDims;
    
    /// Reference to the hypercube
    const HC& hCube;
    
    /// Lexicographic index
    using LxIndex=
      typename HC::Index;
    
    /// Index of a given lexicographic site
    IndexShuffler<Index,LxIndex> idOfLx;
    
    /// Lexicographic site of a given index
    IndexShuffler<LxIndex,Index> lxOfId;
    
    /// Constructor taking a reference hypercube and a function to compute id from lx
    template <typename F>
    HCubeIndexer(const HC& hCube,
		 const IndexDeducerFromLx<F> &indexFromLx) :
      hCube(hCube),
      idOfLx(hCube.vol)
    {
      idOfLx.fill(indexFromLx.deFeat());
      lxOfId=idOfLx.transpose();
    }
    
    template <typename F>
    HCubeIndexer(const HC& hCube,
		 const LxIndexDeducer<F> &indexOfLx) :
      hCube(hCube),
      lxOfId(hCube.vol)
    {
      lxOfId.fill(indexOfLx.deFeat());
      idOfLx=lxOfId.transpose();
    }
  };
  
  /// Returns an indexer, deducing the index from Lx with the function
  template <typename Index,
	    typename HC,
	    typename F>
  HCubeIndexer<Index,HC> getHCubeIndexer(const HC& hCube,
					 const F& f)
  {
    return HCubeIndexer<Index,HC>(hCube,f);
  }
}

#endif
