/**
 * Copyright (c) 2023 Nicolas Hadacek
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef NICO_PROXIMITY_H
#define NICO_PROXIMITY_H

#include "nico_util.h"

#include <SharpIR.h>

//-----------------------------------------------------------------------------
// The bulk of the code is to ignore measurement glitches: 5 measurements are kept
// and only the 3 measurements closest to the mean are used
class SharpProximityDetector : public Base {
  public:
    SharpProximityDetector(SharpIR::sensorCode code, unsigned int pin, unsigned int minDist, unsigned int maxDist, DebugMode debugMode = DebugMode::None);

    void init();

    unsigned int getMinDistance() const { return minDist_; } // cm
    unsigned int getMaxDistance() const { return maxDist_; } // cm
    unsigned int getDistance(); // cm

  private:
    static const size_t Num_ = 5;

    const unsigned int pin_;
    const unsigned int minDist_;
    const unsigned int maxDist_;
    SharpIR sharp_;
    BeatKeeper beatKeeper_{100};
    Array<unsigned int, Num_> dists_;

    unsigned int computeDistance() const;
    size_t getDistanceIndexFurthestFromMean(size_t skipIndex) const;
};

//-----------------------------------------------------------------------------
// Range: 2cm - 450cm (best: 10cm - 250cm)
// Use at most at a 60ms interval
class HCSR04 : public Base {
  public:
    HCSR04(unsigned int triggerPin, unsigned int echoPin, DebugMode debugMode = DebugMode::None);
    
    void init();
    unsigned int getDistance(); // cm
    
  private:
    const unsigned int triggerPin_;
    const unsigned int echoPin_;
    BeatKeeper beatKeeper_{100};
    unsigned int dist_ = 0;
};

#endif
