#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "algo.h"

int main(int argc, char *argv[])
{
  std::string data = "/Users/nam/work/mu2e/STM/liverpool-data/100pulses.txt";
  if (argc >= 2) {
    data = argv[1];
  }

  double f = 1 - 0.999980;
  uint32_t offset = 100;
  uint32_t width = 200;

  std::ifstream input(data);
  std::string line;
  while (std::getline(input, line)){
    std::stringstream ss(line);
    std::string value;

    Waveform wf0;
    uint32_t i = 0;
    while (std::getline(ss, value, ' ')){
        wf0.at(i++) = std::stod(value);
    }

    Waveform mwd = MovingAverage(OffsetDifferentiate(Deconvolute(wf0, f), offset), width);

    // Print(wf0);
    // Print(mwd);
  }

  return 0;
}
