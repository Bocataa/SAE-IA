/**
  ******************************************************************************
  * @file    maxpool2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    21 april 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _MAX_POOLING2D_1_H_
#define _MAX_POOLING2D_1_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS  64
#define INPUT_HEIGHT    13
#define INPUT_WIDTH     13
#define POOL_SIZE_Y     2
#define POOL_SIZE_X     2
#define POOL_STRIDE_Y   2
#define POOL_STRIDE_X   2
#define POOL_PAD_Y      0 // Unsupported
#define POOL_PAD_X      0 // Unsupported
#define POOL_HEIGHT	    ( ( (INPUT_HEIGHT - POOL_SIZE_Y + (2*POOL_PAD_Y) ) / POOL_STRIDE_Y ) + 1 )
#define POOL_WIDTH	    ( ( (INPUT_WIDTH - POOL_SIZE_X + (2*POOL_PAD_X) ) / POOL_STRIDE_X ) + 1 )

typedef float max_pooling2d_1_output_type[POOL_HEIGHT][POOL_WIDTH][INPUT_CHANNELS];

#if 0
void max_pooling2d_1(
  const number_t input[INPUT_SAMPLES][INPUT_CHANNELS], 	    // IN
  number_t output[POOL_LENGTH][INPUT_CHANNELS]); 	// OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef POOL_SIZE_X
#undef POOL_SIZE_Y
#undef POOL_STRIDE_X
#undef POOL_STRIDE_Y
#undef POOL_PAD_X
#undef POOL_PAD_Y
#undef POOL_HEIGHT
#undef POOL_WIDTH

#endif//_MAX_POOLING2D_1_H_