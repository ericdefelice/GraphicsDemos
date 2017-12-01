#pragma once

#include "GF_Math.h"

// -- color functions
double Blackbody(double wl, double temp);
GumshoeFramework10::Float3 XYZ2RGB(double X, double Y, double Z);
GumshoeFramework10::Float3 ComputeBlackbody(double temperature);

void ConvertsRGBToSpectrum(float* spectrum, uint32 numsamples, 
                           GumshoeFramework10::Float3 sRGB);
GumshoeFramework10::Float3 ConvertSpectrumTosRGB(float* spectrum, uint32 numsamples,
                                                uint32 lowerBound, uint32 upperBound,
                                                uint32 waveInc);