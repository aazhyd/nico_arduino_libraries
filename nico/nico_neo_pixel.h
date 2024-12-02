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
class NeoPixelRawArray : public Base {
  public:
    NeoPixelRawArray(size_t size, unsigned int pin, unsigned int type, DebugMode debugMode);

    size_t size() const { return pixels_.numPixels(); }

    void init();
    virtual void clear();
    void set(size_t index, const Color&);
    void show();

  private:
    Adafruit_NeoPixel pixels_;
};

//-----------------------------------------------------------------------------
class NeoPixelBaseArray : public Base {
  public:
    NeoPixelBaseArray(NeoPixelRawArray& array, size_t offset, size_t size, DebugMode debugMode);

    bool empty() const;

    void clear();
    void update();

    void add(const SnakeSetup& setup);
    void add(const PulseSetup& setup);
    void add(const RandomSetup& setup);
    void set(size_t i, const Color& color);

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

    NeoPixelRawArray& array_;
    const size_t offset_;
    const size_t size_;
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
class NeoPixelArray : public NeoPixelRawArray {
  public:
    NeoPixelArray(size_t numPixels, unsigned int pin, unsigned int type, DebugMode debugMode);
    
    void add(const SnakeSetup& setup);
    void add(const PulseSetup& setup);
    void add(const RandomSetup& setup);
    
    virtual void clear();
    void update();

  private:
    NeoPixelBaseArray array_;
};

//-----------------------------------------------------------------------------
class NeoPixel : public NeoPixelRawArray {
  public:
    NeoPixel(unsigned int pin, unsigned int type, DebugMode debugMode);

    void setColor(const Color& color);
    void addPattern(Pattern* pattern);
    void clearPatterns();
    void update();

  private:
    Array<Pattern*, 2> patterns_;
};

//-----------------------------------------------------------------------------
class BoardPixel : public NeoPixel {
  public:
    BoardPixel();
    
  private:
    BlinkPattern pattern_;
};

#endif
