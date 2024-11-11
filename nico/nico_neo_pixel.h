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

#ifndef NICO_NEO_PIXEL_H
#define NICO_NEO_PIXEL_H

#include "nico_neo_pixel_util.h"

#include <Adafruit_NeoPixel.h>

// NeoPixel on RP2040: GRB
// Individual NeoPixel: RGB

//-----------------------------------------------------------------------------
enum Direction { CW, CCW };

//-----------------------------------------------------------------------------
class NeoPixelBaseArray : public Base {
  public:
    NeoPixelBaseArray(size_t numPixels, unsigned int pin, unsigned int type, DebugMode debugMode);

    size_t getNumPixels() const { return pixels_.numPixels(); }

    void init();
    virtual void clear();
    virtual void update();

    void addPattern(Pattern* pattern);

  protected:
    void set(size_t index, const Color&);
    void show();

  private:
    Adafruit_NeoPixel pixels_;
    Array<Pattern*, 2> patterns_;
};

//-----------------------------------------------------------------------------
class NeoPixelArray : public NeoPixelBaseArray {
  public:
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

    NeoPixelArray(size_t numPixels, unsigned int pin, unsigned int type, DebugMode debugMode);

    bool empty() const;

    virtual void clear();
    virtual void update();

    void add(const SnakeSetup& setup);
    void add(const PulseSetup& setup);
    void add(const RandomSetup& setup);

  private:
    struct SnakeData {
      SnakeSetup setup_;
      BeatKeeper beatKeeper_;
      size_t index_ = 0;
    };

    struct PulseData {
      PulseSetup setup_;
      BeatKeeper beatKeeper_;
      unsigned int level_ = 0;
    };

    struct RandomData {
      RandomSetup setup_;
      BeatKeeper beatKeeper_;
      Array<size_t, 8> pixelIndexes_;
      size_t index_ = 0;
    };

    Adafruit_NeoPixel pixels_;
    Array<SnakeData, 4> snakeDataVector_;
    Array<PulseData, 1> pulseDataVector_;
    Array<RandomData, 1> randomDataVector_;

    bool increment(SnakeData& data) const;
    bool increment(PulseData& data) const;
    bool increment(RandomData& data) const;
    void addColor(size_t i, const SnakeData& data, Color& color) const;
    void addColor(size_t i, const PulseData& data, Color& color) const;
    void addColor(size_t i, const RandomData& data, Color& color) const;
    void incrementPixelIndex(size_t& index, Direction dir) const;
    size_t getPixelDistance(size_t i, size_t index, Direction dir) const;
};

//-----------------------------------------------------------------------------
class NeoPixel : public NeoPixelBaseArray {
  public:
    NeoPixel(unsigned int pin, unsigned int type, DebugMode debugMode);

    void setColor(const Color& color);
};

#endif
