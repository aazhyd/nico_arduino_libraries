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

#include "nico_util.h"

//-----------------------------------------------------------------------------
void Timer::reset(unsigned int duration)
{
  time_ = millis() + duration;
}

bool Timer::elapsed() const
{
  return (millis() > time_);
}

//-----------------------------------------------------------------------------
BeatKeeper::BeatKeeper(unsigned int duration)
{
  reset(duration);
}

void BeatKeeper::reset(unsigned int duration)
{
  duration_ = duration;
  startTime_ = millis();
  totalNumBeats_ = 0;
}

size_t BeatKeeper::getNumBeats()
{
    if (duration_ == 0) {
        return 0;
    }

    const unsigned long time = millis();
    if (time < startTime_) { // rollover
        reset(duration_);
        return 0;
    }

    const size_t totalNumBeats = (time - startTime_) / duration_;
    const size_t numBeats = totalNumBeats - totalNumBeats_;
    totalNumBeats_ = totalNumBeats;
    return numBeats;
}

//-----------------------------------------------------------------------------
double TimeAveragedValue::get() const
{
  return val_;
}

void TimeAveragedValue::add(double val, unsigned int halfLife)
{
  if (halfLife == 0) {
    val_ = val;
    return;
  }
  
  const unsigned int duration = millis() - setTime_;
  const double factor = exp(-0.693 * duration / halfLife);
  //Console::instance_ << duration << "/" << halfLife << " " << factor << "\n";
  val_ = factor * val_ + (1 - factor) * val;
  setTime_ = millis();
}

//-----------------------------------------------------------------------------
void Console::init()
{
  Serial.begin(115200);
  instance_ << F("console inited\n");
}

Console& Console::operator<<(const char* str)
{
  if (str != nullptr) {
    Serial.print(str);
  }
  return *this;
}

Console& Console::operator<<(const __FlashStringHelper* str)
{
  if (str != nullptr) {
    Serial.print(str);
  }
  return *this;
}

Console& Console::operator<<(unsigned long val)
{
  Serial.print(val);
  return *this;
}

Console& Console::operator<<(long val)
{
  Serial.print(val);
  return *this;
}

Console& Console::operator<<(unsigned int val)
{
  Serial.print(val);
  return *this;
}

Console& Console::operator<<(int val)
{
  Serial.print(val);
  return *this;
}

Console& Console::operator<<(double val)
{
  Serial.print(val);
  return *this;
}

Console& Console::operator<<(Special special)
{
  switch (special) {
    case Time: {
      const double seconds = double(millis()) / 1000;
      *this << "[" << seconds;
      if (prevSeconds_ > 0) {
        const double delta = seconds - prevSeconds_;
        *this << " " << delta;
      }
      *this << "] ";
      prevSeconds_ = seconds;
      break;
    }
  }
  return *this;
}

Console Console::instance_;
