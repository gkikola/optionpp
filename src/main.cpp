#include "optionpp.hpp"

int main(int argc, char* argv[])
{
  OptionParser opt = {
      {'?', "help", "", "Display help text"},
      {0, "version", "", "Display program version"}
  };
  return 0;
}
