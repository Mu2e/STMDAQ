//
// Created by Nam Tran on 1/31/20.
//

#ifndef MWD_CPP_ALGO_H
#define MWD_CPP_ALGO_H

#define MAX_LENGTH 1024

#include <array>
typedef std::array<double, MAX_LENGTH> Waveform;

Waveform Deconvolute(Waveform const &input, double f);

Waveform OffsetDifferentiate(Waveform const &input, uint32_t offset);

Waveform MovingAverage(Waveform const &input, uint32_t width);

void Print(Waveform const &input);

#endif //MWD_CPP_ALGO_H
