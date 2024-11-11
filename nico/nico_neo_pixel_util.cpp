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

#include "nico_neo_pixel_util.h"

//-----------------------------------------------------------------------------
Color::Color(const Color& color, double gamma)
{
  add(color, gamma);
}

void Color::add(const Color& color, double gamma)
{
  r_ += round(gamma * color.r_);
  g_ += round(gamma * color.g_);
  b_ += round(gamma * color.b_);
  w_ += round(gamma * color.w_);
}

//-----------------------------------------------------------------------------
BlinkPattern::BlinkPattern(const Color& color1, const Color& color2, unsigned int period)
: color1_(color1),
  color2_(color2),
  beatKeeper_(period)
{
}

void BlinkPattern::increment()
{
  index_ += beatKeeper_.getNumBeats();
  index_ %= 2;
}

void BlinkPattern::setColor(size_t /*index*/, Color& color) const
{
  color = (index_ == 0) ? color1_ : color2_;
}

//-----------------------------------------------------------------------------
PulsePattern::PulsePattern(unsigned int period, double minGamma)
: period_(period),
  minGamma_(minGamma),
  beatKeeper_(beatPeriod_) // ms
{
}

void PulsePattern::increment()
{
  if (period_ == 0) {
    return; // safety
  }

  count_ += beatKeeper_.getNumBeats();
}

void PulsePattern::setColor(size_t /*index*/, Color& color) const
{
  const double level = (beatPeriod_ * count_) % period_;
  const double half = (double)period_ / 2;
  double gamma = (level <= half ) ? level / half : 2.0 - level / half;
  gamma = gamma * (1.0 - minGamma_) + minGamma_;
  color = Color(color, gamma);
}
