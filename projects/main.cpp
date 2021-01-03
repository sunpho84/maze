#include <Maze.hpp>

using namespace maze;

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
  
  return 0;
}
