#pragma once

#ifndef RGB_MATRIX_ENABLE
#define HAL_USE_GPT TRUE
#define SN32_GPT_USE_CT16B1 TRUE
#include_next <halconf.h>
#else
#define HAL_USE_PWM TRUE
#include_next <halconf.h>
#endif
