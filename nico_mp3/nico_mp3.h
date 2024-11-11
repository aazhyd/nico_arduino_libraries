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

#ifndef NICO_MP3_H
#define NICO_MP3_H

#include "nico_util.h"

#include <SdFat.h>
#include <vs1053_SdFat.h>

//-----------------------------------------------------------------------------
class MP3Player : public Base {
  public:
    MP3Player(SdFat& sd, DebugMode debugMode);

    void init();
    void clear();
    void update();

    bool readyForNext() const { return (filename_ == nullptr); }
    void setNext(const char* filename, unsigned int duration);

  private:
    SdFat& sd_;
    vs1053 player_;
    Timer timer_;
    const char* filename_ = nullptr;

    bool isPlaying();
};

#endif
