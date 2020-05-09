#include <assert.h>

#include <iostream>

#include "app/quface.h"
#include "lib/utils.h"

using namespace suanzi;
int main(int argc, char **argv) {
  int disType = -1;
  if (argc == 3) {
    std::string argv1 = std::string(argv[1]);
    std::string argv2 = std::string(argv[2]);
    if (argv1 == "-display") {
      if (argv2 == "mipi_tx") disType = 0;
      if (argv2 == "hdmi") disType = 1;
    }
  }

  if (disType == -1) {
    std::cerr << "Usage: " << argv[0] << " -display [mipi_tx | hdmi]"
              << std::endl;
    return -1;
  }

  Quface engine;
  engine.initialize("device_info.json", "facemodel.bin", disType);
  engine.run();

  return 0;
}
