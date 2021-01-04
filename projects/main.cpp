#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <Maze.hpp>

using namespace maze;

/// Hashed grid
template <int NDim,
	  typename Index>
struct HashedGrid : public LxGrid<NDim,Index>
{
  const Index vol;
  
  Vector<Coords<NDim>,StorLoc::ON_CPU> coordsOfLx;
  
  HashedGrid(const Coords<NDim>& sizes) :
    LxGrid<NDim,Index>(sizes),
    vol(this->computeVol()),
    coordsOfLx(this->getCoordsOfAllLx())
  {
  }
  
  ~HashedGrid()
  {
  }
};

template <int NDim,
	  typename Index>
struct IndexedGrid
{
  const HashedGrid<NDim,Index>& grid;
  
  Index* idOfLxx;
  
  IndexedGrid(const HashedGrid<NDim,Index>& grid) : grid(grid)
  {
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
    HashedGrid<4,int> grid({3,2,2,2});
    
    LOGGER<<grid.computeSurfVol()<<endl;
  }
  
  finalizeMaze();
  
  return 0;
}
