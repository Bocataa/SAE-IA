/**
  ******************************************************************************
  * @file    conv2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    14 december 2022
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _CONV2D_1_H_
#define _CONV2D_1_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      32
#define INPUT_HEIGHT        15
#define INPUT_WIDTH         15
#define CONV_FILTERS        64
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       1
#define CONV_STRIDE_X       1

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )


typedef float conv2d_1_output_type[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS];

#if 0
void conv2d_1(
  const number_t input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const number_t kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS], // IN

  const number_t bias[CONV_FILTERS],						                // IN

  number_t output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]);               // OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT

#endif//_CONV2D_1_H_