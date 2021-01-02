#include <Maze.hpp>

using namespace maze;

struct a
{
  void b() const
  {
  }
  
  PROVIDE_ALSO_NON_CONST_METHOD(b);
  
};

PROVIDE_HAS_MEMBER(b);



int main()
{
  int i=hasMember_b<a>;
  
  return 0;
}
