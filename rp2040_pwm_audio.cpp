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
#include <Arduino.h>
#include "hardware/pwm.h"

// 音源周波数
static const uint freq = 8000;
// 周波数スケール
static const uint freq_scale = 3;
// PWM wrap値
static const uint16_t wrap = 256;
static uint slice_num;

static uint pwm_pin;
static uint32_t t;
static uint8_t *sound;
static uint32_t sound_len;
static volatile bool isAudioPlay;

// PWMオーディオ再生ハンドラ(Interrupt Service Routine)
void pwm_audio_isr()
{
  static uint32_t count = 0; // raw count

  if (isAudioPlay && t == 0)
  {
    count = 0;
  }
  else if (t >= sound_len)
  {
    isAudioPlay = false;
    pwm_set_irq_enabled(slice_num, false);
    pwm_set_gpio_level(pwm_pin, 0);
    return;
  }

  if (count % freq_scale == 0)
  {
    pwm_set_gpio_level(pwm_pin, *(sound + t));
    t++;
  }
  count++;
  pwm_clear_irq(slice_num);
}

void set_sound(uint8_t *s, uint32_t len)
{
  sound = s;
  sound_len = len;
}

void play_pwm_audio()
{
  t = 0;
  isAudioPlay = true;
  pwm_clear_irq(slice_num);
  pwm_set_irq_enabled(slice_num, true);
  while (isAudioPlay)
    yield();
}

void pwm_audio_init(uint pin)
{
  pwm_pin = pin;
  isAudioPlay = false;

  // PWMオーディオ初期化処理
  // PWM初期化
  gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
  slice_num = pwm_gpio_to_slice_num(pwm_pin);
  pwm_set_clkdiv(slice_num, clock_get_hz(clk_sys) / (float)(wrap * freq * freq_scale));
  pwm_set_wrap(slice_num, wrap);
  pwm_set_enabled(slice_num, true);

  // PWM割り込み初期化
  irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_audio_isr);
  irq_set_enabled(PWM_IRQ_WRAP, true);
}