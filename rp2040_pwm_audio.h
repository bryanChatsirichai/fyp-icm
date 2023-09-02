/*----------------------------------------------------------------------------/
  RP2040 PWM Audio

  support platform:
    RP2040 with Arduino-Pico(https://github.com/earlephilhower/arduino-pico)

  Original Source:  
    https://github.com/aloseed/RP2040-PWM-Audio

   Licence:
      Copyright (c) 2022 alocchi
      This software is released under the MIT License, see LICENSE.

   Author:
      [alocchi](https://twitter.com/aloseed_blog)

   Blog:
      [aloseed](https://aloseed.com)
/----------------------------------------------------------------------------*/
#pragma once

// PWM出力するピンを指定します。
void pwm_audio_init(uint);

// 8KHz 8bit モノラルのrawデータをセットします。
// 配列(rawデータ)と配列長
void set_sound(uint8_t *s, uint32_t len);

// set_soundでセットしたデータをPWMで再生します。
void play_pwm_audio();