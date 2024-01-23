#include <cstdlib>
#include <iostream>

#include "Server.h"
#include "Models.h"

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: auction-house <port>\n";
      return 1;
    }

    Server s(ioContext, std::atoi(argv[1]));
    ioContext.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}