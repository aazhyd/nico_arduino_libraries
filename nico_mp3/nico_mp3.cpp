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

#include "nico_mp3.h"

#include <FreeStack.h>
#include <SPI.h>

//-----------------------------------------------------------------------------
MP3Player::MP3Player(SdFat& sd, DebugMode debugMode)
: Base(debugMode),
  sd_(sd)
{
}

void MP3Player::init()
{
  if (debugMode() != DebugMode::None) {
    Console::instance_ << "\n" << F("F_CPU = ") << F_CPU << "\n";
    Console::instance_ << F("Free RAM = ") << FreeStack() << F(" Should be a base line of 1028, on ATmega328 when using INTx\n");
  }

  if (debugMode() == DebugMode::DryRun) {
    return;
  }

  // init SD card
  if (not sd_.begin(SD_SEL, SPI_FULL_SPEED)) {
    sd_.initErrorHalt();
  }
  if (not sd_.chdir("/")) {
    sd_.errorHalt("sd.chdir");
  }

  // init MP3 player
  const uint8_t res = player_.begin();
  if (res != 0) {
    Console::instance_ << F("Error code: ") << res << F(" when trying to start MP3 player\n");
  }
}

void MP3Player::clear()
{
  filename_ = nullptr;
}

void MP3Player::setNext(const char* filename, unsigned int duration)
{
  filename_ = filename;
  if (filename_ == nullptr) {
    return;
  }

  timer_.reset(duration);

  if (debugMode() != DebugMode::None) {
    const double seconds = double(duration) / 1000; 
    Console::instance_ << Console::Time << F("next play ") << filename << F(" in ") << seconds << F("s\n");
  }
}

bool MP3Player::isPlaying()
{
  return (debugMode() != DebugMode::DryRun
    && player_.isPlaying());
}

void MP3Player::update()
{
  if (filename_ == nullptr
      || not timer_.elapsed()
      || isPlaying()) {
    return;
  }

  if (debugMode() != DebugMode::None) {
    Console::instance_ << Console::Time << F("play '") << filename_ << F("'\n");
  }

  if (debugMode() != DebugMode::DryRun) {
    const uint8_t res = player_.playMP3((char*)filename_);
    if(res != 0) {
      Console::instance_ << F("Error code: ") << res << F(" when trying to play track\n");
    }
  }

  filename_ = nullptr;
}
