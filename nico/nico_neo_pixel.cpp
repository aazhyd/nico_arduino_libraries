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

#include "nico_neo_pixel.h"

//-----------------------------------------------------------------------------
NeoPixelBaseArray::NeoPixelBaseArray(
  size_t       numPixels,
  unsigned int pin,
  unsigned int type,
  DebugMode    debugMode)
: Base(debugMode),
  pixels_(numPixels, pin, type + NEO_KHZ800)
{
}

void NeoPixelBaseArray::init()
{
  // turn off all pixels even in DebugMode::DryRun
  pixels_.begin();
  pixels_.show();
}

void NeoPixelBaseArray::clear()
{
  patterns_.clear();
  pixels_.clear();
  pixels_.show();
}

void NeoPixelBaseArray::addPattern(Pattern* pattern)
{
  if (pattern != nullptr) {
    patterns_.push_back(pattern);
  }
}

void NeoPixelBaseArray::set(size_t index, const Color& color)
{
    uint32_t c = pixels_.Color(color.r_, color.g_, color.b_, color.w_);
    c = pixels_.gamma32(c);
    pixels_.setPixelColor(index, c);
}

void NeoPixelBaseArray::show()
{
  if (debugMode() != DebugMode::DryRun) {
    pixels_.show();
  }
}

void NeoPixelBaseArray::update()
{
  for (size_t i = 0; i < patterns_.size(); ++i) {
    patterns_[i]->increment();
  }

  for (size_t i = 0; i < getNumPixels(); ++i) {
      Color color;
      for (size_t i = 0; i < patterns_.size(); ++i) {
        patterns_[i]->setColor(i, color);
      }
    set(i, color);
  }

  show();
}
//-----------------------------------------------------------------------------
NeoPixelArray::NeoPixelArray(
  size_t       numPixels,
  unsigned int pin,
  unsigned int type,
  DebugMode    debugMode)
: NeoPixelBaseArray(numPixels, pin, type, debugMode)
{
}

bool NeoPixelArray::empty() const
{
   return (snakeDataVector_.empty() && pulseDataVector_.empty() && randomDataVector_.empty());
}

void NeoPixelArray::clear()
{
  NeoPixelBaseArray::clear();
  snakeDataVector_.clear();
  pulseDataVector_.clear();
  randomDataVector_.clear();
}

void NeoPixelArray::add(const SnakeSetup& setup)
{
  SnakeData data;
  data.setup_ = setup;
  data.beatKeeper_.reset(setup.duration_);
  data.index_ = setup.offset_;
  snakeDataVector_.push_back(data);
}

void NeoPixelArray::add(const PulseSetup& setup)
{
  PulseData data;
  data.setup_ = setup;
  data.beatKeeper_.reset(setup.duration_);
  pulseDataVector_.push_back(data);
}

void NeoPixelArray::add(const RandomSetup& setup)
{
  RandomData data;
  data.setup_ = setup;
  data.beatKeeper_.reset(setup.duration_);
  randomDataVector_.push_back(data);
}

void NeoPixelArray::update()
{
  bool needUpdate = false;
  for (size_t k = 0; k < snakeDataVector_.size(); ++k) {
    needUpdate |= increment(snakeDataVector_[k]);
  }
  for (size_t k = 0; k < pulseDataVector_.size(); ++k) {
    needUpdate |= increment(pulseDataVector_[k]);
  }
  for (size_t k = 0; k < randomDataVector_.size(); ++k) {
    needUpdate |= increment(randomDataVector_[k]);
  }
  if (not needUpdate) {
    return;
  }

  for (size_t i = 0; i < getNumPixels(); ++i) {
    Color color;
    for (size_t k = 0; k < snakeDataVector_.size(); ++k) {
      addColor(i, snakeDataVector_[k], color);
    }
    for (size_t k = 0; k < pulseDataVector_.size(); ++k) {
      addColor(i, pulseDataVector_[k], color);
    }
    for (size_t k = 0; k < randomDataVector_.size(); ++k) {
      addColor(i, randomDataVector_[k], color);
    }

//    Console::instance_ << color.r_ <<  " " << color.g_ << " " << color.b_ << " " << color.w_ << "\n";
    set(i, color);
  }

  show();
}

bool NeoPixelArray::increment(SnakeData& data) const
{
  const size_t numBeats = data.beatKeeper_.getNumBeats();
  if (numBeats == 0) {
    return false;
  }

  for (size_t i = 0; i < numBeats; ++i) {
    incrementPixelIndex(data.index_, data.setup_.dir_);
  }
  return true;
}

bool NeoPixelArray::increment(PulseData& data) const
{
  const size_t numBeats = data.beatKeeper_.getNumBeats();
  if (numBeats == 0) {
    return false;
  }

  data.level_ += numBeats;
  data.level_ %= 2;
  return true;
}

bool NeoPixelArray::increment(RandomData& data) const
{
  if (data.setup_.count_ == 0) {
    return false;
  }

  const size_t numBeats = data.beatKeeper_.getNumBeats();
  if (numBeats == 0) {
    return false;
  }

  const size_t pixelIndex = random(0, getNumPixels());
  if (data.index_ == data.pixelIndexes_.size()) {
    data.pixelIndexes_.push_back(pixelIndex);
  } else {
    data.pixelIndexes_[data.index_] = pixelIndex;
  }
  data.index_ = (data.index_ + 1) % data.setup_.count_;
  return true;
}

void NeoPixelArray::addColor(size_t i, const SnakeData& data, Color& color) const
{
  const size_t dist = getPixelDistance(i, data.index_, data.setup_.dir_);
  double gamma = 0.0;
  if (dist < data.setup_.length_) {
    gamma = (1.0 - data.setup_.fadeFactor_ * (double)dist / (getNumPixels() - 1));
  }
  color.add(data.setup_.color_, gamma);
}

void NeoPixelArray::addColor(size_t /*i*/, const PulseData& data, Color& color) const
{
  const double gamma = (data.level_ == 0) ? 0.0 : 1.0;
  color.add(data.setup_.color_, gamma);
}

void NeoPixelArray::addColor(size_t i, const RandomData& data, Color& color) const
{
  for (size_t k = 0; k < data.pixelIndexes_.size(); ++k) {
    if (data.pixelIndexes_[k] == i) {
      color = data.setup_.color_;
      return;
    }
  }
  color = data.setup_.backgroundColor_;
}

void NeoPixelArray::incrementPixelIndex(size_t& index, Direction dir) const
{
  if (dir == CCW) {
    index = (index == 0) ? getNumPixels() - 1 : index - 1;
  } else {
    index = (index == getNumPixels() -1) ? 0 : index + 1;
  }
}

size_t NeoPixelArray::getPixelDistance(size_t i, size_t index, Direction dir) const
{
  if (dir == CCW) {
    return (i >= index) ? i - index : getNumPixels() + i - index;
  } else {
    return (i <= index) ? index - i : getNumPixels() + index - i;
  }
}

//-----------------------------------------------------------------------------
NeoPixel::NeoPixel(
  unsigned int pin,
  unsigned int type,
  DebugMode    debugMode)
: NeoPixelBaseArray(1, pin, type, debugMode)
{
}

void NeoPixel::setColor(const Color& color)
{
  clear();
  set(0, color);
  show();
}
