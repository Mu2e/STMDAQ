//
// Created by Nam Tran on 1/31/20.
//

#include "algo.h"

Waveform Deconvolute(Waveform const &input, double f) {
  if (input.size() <= 2){
    return input;
  }

  Waveform output;
  output.at(0) = input.at(0);
  auto sum = input.at(0);
  for (uint64_t i = 1; i < input.size(); i++){
    output.at(i) = input.at(i) - f * sum;
    sum += input.at(i);
  }
  return output;
}

Waveform OffsetDifferentiate(Waveform const &input, uint32_t offset) {
  if (input.size() <= offset) {
    return input;
  }

  Waveform output;
  for (uint32_t i = offset; i < input.size(); ++i) {
    output.at(i) = input.at(i) - input.at(i - offset);
  }

  return output;
}


Waveform MovingAverage(Waveform const &input, uint32_t width) {
  if (input.size() <= width) {
    return input;
  }

  Waveform output;
  double sum = 0.;
  for (uint32_t i = 0; i < width; ++i) {
    sum += input.at(i);
  }
  output.at(0) = sum / width;

  for (uint32_t i = width; i < input.size(); ++i) {
    sum += input.at(i) - input.at(i - width);
    output.at(i) = sum / width;
  }

  return output;
}

#include <iostream>
void Print(Waveform const &input){
  for (uint32_t i = 0; i < input.size(); ++i) {
    std::cout << input.at(i) << " ";
  }
  std::cout << std::endl;
}
