#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <Maze.hpp>

using namespace maze;

template <int NDim>
using Coords=std::array<int,NDim>;

template <int NDim,
	  typename Index>
struct Grid
{
  const Coords<NDim> sizes;
  
  const Index vol;
  
  Coords<NDim>* coordsOfLx;
  
  Grid(const Coords<NDim>& sizes) : sizes(sizes),vol(computeVol())
  {
    coordsOfLx=cpuMemoryManager->provide<Coords<NDim>>(vol);
    
    for(Index site=0;site<vol;site++)
      coordsOfLx[site]=computeCoordsOfLx(site);
  }
  
  ~Grid()
  {
    cpuMemoryManager->release(coordsOfLx);
  }
  
  Index computeVol() const
  {
    Index out=1;
    
    for(int mu=0;mu<NDim;mu++)
      out=out*sizes[mu];
    
    return out;
  }
  
  Index computeLxOfCoords(const Coords<NDim>& coords) const
  {
    Index out=0;
    
    for(int mu=0;mu<NDim;mu++)
      out=out*sizes[mu]+coords[mu];
    
    return out;
  }
  
  Coords<NDim> computeCoordsOfLx(Index site) const
  {
    Coords<NDim> coords;
    
    for(int mu=NDim-1;mu>=0;mu--)
      {
	coords[mu]=site%sizes[mu];
	site/=sizes[mu];
      }
    
    return coords;
  }
};

struct a
{
  void b() const
  {
  }
  
  PROVIDE_ALSO_NON_CONST_METHOD(b);
  
};

namespace maze
{
  PROVIDE_HAS_MEMBER(b);
}


int main(int narg,char** arg)
{
  initMaze([](int narg,char** arg){},narg,arg);
  
  {
    Grid<4,int> grid({2,2,2,2});
    
    LOGGER<<grid.computeVol()<<endl;
  }
  
  finalizeMaze();
  
  return 0;
}
