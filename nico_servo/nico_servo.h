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

#ifndef NICO_SERVO_H
#define NICO_SERVO_H

#include "nico_util.h"

#include <Adafruit_PWMServoDriver.h>

enum class ServoType { SG92R, MG90S };

struct ServoData {
    unsigned int usMin_; // microseconds
    unsigned int usMax_; // microseconds
    unsigned int maxAngle_;
};

//-----------------------------------------------------------------------------
class ServoDriver : public Base {
  public:
    static const size_t MAX_COUNT = 8;

    explicit ServoDriver(ServoType type, DebugMode debugMode);

    void setup(size_t index, double beginAngle, double endAngle);
    void init();

    bool enabled(size_t index) const { return dataVector_[index].enabled_; }
    double beginAngle(size_t index) const { return dataVector_[index].beginAngle_; }
    double endAngle(size_t index) const { return dataVector_[index].endAngle_; }
    double angle(size_t index) const { return dataVector_[index].angle_; }
    bool inRange(size_t index, double angle) const;

    void set(size_t index, double angle);
    void move(size_t index, double toAngle, double speed = 1.0);
    void moveToBegin(size_t index, double speed = 1.0);
    void moveToEnd(size_t index, double speed = 1.0);
    void moveAllToBegin();
    void moveAllToEnd();

  private:
    struct Data {
        bool enabled_ = false;
        double beginAngle_;
        double endAngle_;
        double angle_ = 0.0;
    };

    const ServoData& data_;
    Adafruit_PWMServoDriver driver_;
    Data dataVector_[MAX_COUNT];
};

//-----------------------------------------------------------------------------
class ServoManager : public Base {
  public:
    enum Action { NoAction, MoveToBegin, MoveToEnd };

    explicit ServoManager(ServoType type, DebugMode debugMode);

    struct Data {
      Action action_ = NoAction;
      unsigned int duration_; // ms to go from min to max
      unsigned long startTime_; // ms
    };

    void setup(size_t index, double beginAngle, double endAngle);
    void set(size_t index, unsigned int duration, unsigned int startTime);
    void set(size_t index, Action action);

    void init();
    void update();
    void clear();

  private:
    ServoDriver driver_;
    Data dataVector_[ServoDriver::MAX_COUNT];
};

#endif
