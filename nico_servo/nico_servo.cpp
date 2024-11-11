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

#include "nico_servo.h"

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you have!
#define OSC_FREQ   27000000
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

const ServoData SERVO_DATA[2] {
    { 1000, 2000, 180 }, // SG92R
    {  500, 2500, 180 }  // MG90S
};

//-----------------------------------------------------------------------------
ServoDriver::ServoDriver(ServoType type, DebugMode debugMode)
: Base(debugMode),
  data_(SERVO_DATA[(size_t)type]),
  driver_(0x40) // I2C address
{
}

void ServoDriver::setup(size_t index, double beginAngle, double endAngle)
{
  Data& data = dataVector_[index];
  data.enabled_ = true;
  data.beginAngle_ = beginAngle;
  data.endAngle_ = endAngle;
}

void ServoDriver::init()
{
  if (debugMode() != DebugMode::DryRun) {
    driver_.begin();
    driver_.setOscillatorFrequency(OSC_FREQ);
    driver_.setPWMFreq(SERVO_FREQ);
  }
}

bool ServoDriver::inRange(size_t index, double angle) const
{
  const Data& data = dataVector_[index];
  if (not data.enabled_) {
    return false;
  }
  
  const double minAngle = std::min(data.beginAngle_, data.endAngle_);
  const double maxAngle = std::max(data.beginAngle_, data.endAngle_);
  return (angle >= minAngle
    && angle <= maxAngle);
}

void ServoDriver::set(size_t index, double angle)
{
  if (not inRange(index, angle)) {
    if (debugMode() != DebugMode::None) {
      Console::instance_ << F("set angle for ") << index << F(" out of range: ") << angle << "\n";
    }
    return; // safety
  }

  const uint16_t usec = data_.usMin_ + angle * (data_.usMax_ - data_.usMin_) / data_.maxAngle_;
  if (debugMode() != DebugMode::DryRun) {
    driver_.writeMicroseconds(index, usec);
  }
  dataVector_[index].angle_ = angle;

  if (debugMode() != DebugMode::None) {
    Console::instance_ << F("set angle for ") <<index << ": " << angle << "\n";
  }
}

void ServoDriver::move(size_t index, double toAngle, double speed)
{
  if (not inRange(index, toAngle)
      || speed == 0.0) {
    return; // safety
  }

  const double inc = speed * data_.maxAngle_ / (data_.usMax_ - data_.usMin_);
  double angle = dataVector_[index].angle_ + inc;
  if (angle < toAngle) {
    for (; angle < toAngle; angle += inc) {
      set(index, angle);
    }
  } else {
    for (; angle > toAngle; angle -= inc) {
      set(index, angle);
    }
  }

  // make sure requested angle is set
  if (angle != toAngle) {
    set(index, toAngle);
  }
}

void ServoDriver::moveToBegin(size_t index, double speed)
{
  move(index, dataVector_[index].beginAngle_, speed);
}

void ServoDriver::moveAllToBegin()
{
  for (size_t i = 0; i < MAX_COUNT; ++i) {
    moveToBegin(i, 1.0);
  }
}

void ServoDriver::moveToEnd(size_t index, double speed)
{
  move(index, dataVector_[index].endAngle_, speed);
}

void ServoDriver::moveAllToEnd()
{
  for (size_t i = 0; i < MAX_COUNT; ++i) {
    moveToEnd(i, 1.0);
  }
}

//-----------------------------------------------------------------------------
ServoManager::ServoManager(ServoType type, DebugMode debugMode)
: Base(debugMode),
  driver_(type, debugMode)
{
}

void ServoManager::setup(size_t index, double beginAngle, double endAngle)
{
  driver_.setup(index, beginAngle, endAngle);
}

void ServoManager::init()
{
  driver_.init();
}

void ServoManager::set(size_t index, unsigned int duration, unsigned int startTime)
{
  dataVector_[index].action_ = NoAction;
  dataVector_[index].duration_ = duration;
  dataVector_[index].startTime_ = millis() + startTime;
}

void ServoManager::set(size_t index, Action action)
{
  dataVector_[index].action_ = action;
}

void ServoManager::clear()
{
    for (size_t i = 0; i < ServoDriver::MAX_COUNT; ++i) {
      dataVector_[i] = {};
    }
}

void ServoManager::update()
{
  const unsigned long time = millis();

  for (size_t i = 0; i < ServoDriver::MAX_COUNT; ++i) {
    if (not driver_.enabled(i)) {
      continue;
    }

    switch (dataVector_[i].action_) {
      case MoveToBegin:
        driver_.moveToBegin(i);
        continue;
      case MoveToEnd:
        driver_.moveToEnd(i);
        continue;
      case NoAction:
        break;
    }

    if (dataVector_[i].duration_ == 0) {
      continue;
    }

    const unsigned long startTime = dataVector_[i].startTime_;
    if (time <= startTime) {
      continue;
    }

    const double endAngle = driver_.endAngle(i);
    const double beginAngle = driver_.beginAngle(i);
    const double angleRange = (endAngle - beginAngle);
    const double deltaAnglePerMs = angleRange / dataVector_[i].duration_;
    const unsigned long duration = (time - startTime);
    double deltaAngle = deltaAnglePerMs * duration;
//    Console::instance_ << "angleRange=" << angleRange << " deltaAnglePerMs=" << deltaAnglePerMs
//      << " duration=" << duration << " deltaAngle=" << deltaAngle << "\n";
    deltaAngle = std::fmod(deltaAngle, 2.0 * angleRange);
//    Console::instance_ << deltaAngle << "\n";
    if (std::fabs(deltaAngle) > std::fabs(angleRange)) {
        deltaAngle = 2.0 * angleRange - deltaAngle;
    }
    const double angle = beginAngle + deltaAngle;
    driver_.set(i, angle);
  }
}
