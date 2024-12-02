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

#include "nico_proximity.h"

//-----------------------------------------------------------------------------
SharpProximityDetector::SharpProximityDetector(SharpIR::sensorCode code, unsigned int pin, unsigned int minDist, unsigned int maxDist, DebugMode debugMode)
: Base(debugMode),
  pin_(pin),
  minDist_(minDist),
  maxDist_(maxDist),
  sharp_(code, pin)
{
  for (size_t i = 0; i < Num_; ++i) {
    dists_.push_back(0);
  }
}

void SharpProximityDetector::init()
{
  pinMode(pin_, INPUT);
}

unsigned int SharpProximityDetector::getDistance()
{
  if (dists_.empty()
      || beatKeeper_.getNumBeats() != 0) {
    // shift previous measurements
    for (size_t i = Num_ - 1; i > 0; --i) {
      dists_[i] = dists_[i - 1];
    }

    // new measurement
    const bool avoidBurstRead = false;
    dists_[0] = sharp_.getDistance(avoidBurstRead); // ~80 to 10cm

    if (debugMode() != DebugMode::None) {
      Console::instance_ << F("distance: ") << computeDistance() << F("cm (");
      for (size_t i = 0; i < Num_; ++i) {
        Console::instance_ << " " << dists_[i];
      }
      Console::instance_ << F(" )\n");
    }
  }

  return computeDistance();
}

unsigned int SharpProximityDetector::computeDistance() const
{
  // find two measurements furthest from mean
  const size_t index1 = getDistanceIndexFurthestFromMean(Num_);
  const size_t index2 = getDistanceIndexFurthestFromMean(index1);

  // return mean without these two measurements
  unsigned int mean = 0;
  for (size_t i = 0; i < Num_; ++i) {
    if (i != index1 && i != index2) {
      mean += dists_[i];
    }
  }
  mean /= (Num_ - 2);
  return mean;
}

size_t SharpProximityDetector::getDistanceIndexFurthestFromMean(
  size_t skipIndex) const
{
  // compute mean
  unsigned int mean = 0;
  for (size_t i = 0; i < Num_; ++i) {
    mean += dists_[i];
  }
  mean /= Num_;

  // find measurement furthest from mean
  size_t index = Num_;
  for (size_t i = 0; i < dists_.size(); ++i) {
    if (i == skipIndex) {
      continue;
    }

    if (index == Num_) {
      index = i;
      continue;
    }

    const int di = (int)dists_[i] - mean;
    const int dindex = (int)dists_[index] - mean;
    if (abs(di) > abs(dindex)) {
      index = i;
    }
  }
  return index;
}

//-----------------------------------------------------------------------------
HCSR04::HCSR04(unsigned int triggerPin, unsigned int echoPin, DebugMode debugMode)
: Base(debugMode), triggerPin_(triggerPin), echoPin_(echoPin)
{
}

void HCSR04::init()
{
    pinMode(triggerPin_, OUTPUT);
    pinMode(echoPin_, INPUT);
}

unsigned int HCSR04::getDistance()
{
    if (beatKeeper_.getNumBeats() != 0) {
        // send trigger
        digitalWrite(triggerPin_, LOW);
        delayMicroseconds(2);
        digitalWrite(triggerPin_, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPin_, LOW);
        
        // receive echo
        noInterrupts();
        const float d = pulseIn(echoPin_, HIGH, 23529.4); // timeout in us for 400cm
        interrupts();
        
        dist_ = d / 58.8235;
        
        if (debugMode() != DebugMode::None) {
            Console::instance_ << F("distance: ") << dist_ << F("cm\n");
        }
    }
    
    return dist_;
}
