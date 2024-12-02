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

#ifndef NICO_UTIL_H
#define NICO_UTIL_H

#include <Arduino.h>
#include <Array.h>

//-----------------------------------------------------------------------------
enum class DebugMode { None, Print, DryRun };

//-----------------------------------------------------------------------------
class Timer {
  public:
    void reset(unsigned int duration); // ms
    bool elapsed() const;    

  private:
    unsigned long time_ = 0;
};

//-----------------------------------------------------------------------------
class BeatKeeper {
  public:
    BeatKeeper(unsigned int duration = 0); // ms
   
    void reset(unsigned int duration); // ms
    size_t getNumBeats();

  private:
    unsigned int duration_ = 0;
    unsigned long startTime_ = 0;
    size_t totalNumBeats_ = 0;
};

//-----------------------------------------------------------------------------
class TimeAveragedValue {
  public:
    double get() const;
    void add(double val, unsigned int halfLife);
  
  private:
    unsigned int setTime_ = 0;
    double val_ = 0.0;
};

//-----------------------------------------------------------------------------
class Console {
  public:
    enum Special { Time };

    static void init();
    static Console instance_;

    Console& operator<<(const char* str);
    Console& operator<<(const __FlashStringHelper* str);
    Console& operator<<(unsigned long val);
    Console& operator<<(long val);
    Console& operator<<(unsigned int val);
    Console& operator<<(int val);
    Console& operator<<(double val);
    Console& operator<<(Special special);

  private:
    double prevSeconds_ = 0;
};
 
//-----------------------------------------------------------------------------
class Base {
  public:
    explicit Base(DebugMode debugMode) : debugMode_(debugMode) {}

    DebugMode debugMode() const { return debugMode_; }

  private:
    const DebugMode debugMode_;
};

#endif
