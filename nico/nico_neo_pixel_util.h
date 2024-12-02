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

#ifndef NICO_NEO_PIXEL_UTIL_H
#define NICO_NEO_PIXEL_UTIL_H

#include "nico_util.h"

//-----------------------------------------------------------------------------
enum Direction { CW, CCW };

//-----------------------------------------------------------------------------
struct Color {
  Color() {}
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) : r_(r), g_(g), b_(b), w_(w) {}
  Color(const Color& color, double gamma);
    
  static const Color black_;

  void add(const Color& color, double gamma);

  uint8_t r_ = 0;
  uint8_t g_ = 0;
  uint8_t b_ = 0;
  uint8_t w_ = 0;
};

//-----------------------------------------------------------------------------
class Pattern {
  public:
    virtual void increment() = 0;
    virtual void setColor(size_t index, Color& color) const = 0;
};

//-----------------------------------------------------------------------------
class SolidPattern : public Pattern {
  public:
    SolidPattern(const Color& color) : color_(color) {}

    virtual void increment() {}
    virtual void setColor(size_t /*index*/, Color& color) const { color = color_; }

  private:
    const Color color_;
};

//-----------------------------------------------------------------------------
class BlinkPattern : public Pattern {
  public:
    BlinkPattern(const Color& color1, const Color& color2, unsigned int period); // ms

    virtual void increment();
    virtual void setColor(size_t index, Color& color) const;

  private:
    const Color color1_;
    const Color color2_;
    BeatKeeper  beatKeeper_;
    size_t index_ = 0;
};

//-----------------------------------------------------------------------------
class PulsePattern : public Pattern {
  public:
    PulsePattern(unsigned int period, double minGamma = 0.0); // ms

    virtual void increment();
    virtual void setColor(size_t index, Color& color) const;

  private:
    static const unsigned int beatPeriod_ = 50; // ms
    const unsigned int period_;
    const double minGamma_;
    BeatKeeper beatKeeper_;
    size_t count_ = 0;
};

//-----------------------------------------------------------------------------
struct SnakeSetup {
  Color color_;
  size_t offset_;
  Direction dir_;
  size_t length_;
  double fadeFactor_;
  unsigned int duration_; // ms
};

struct PulseSetup {
  Color color_;
  unsigned int duration_; // ms
};

struct RandomSetup {
  Color color_;
  Color backgroundColor_;
  size_t count_;
  unsigned int duration_; // ms
};

#endif
