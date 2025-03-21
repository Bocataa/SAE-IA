#define SINGLE_FILE
/**
  ******************************************************************************
  * @file    defines.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, Université Côte d'Azur, LEAT, France
  * @version 2.1.0
  * @date    10 january 2024
  * @brief   Global C pre-processor definitions to use to build all source files (incl. CMSIS-NN)
  */

/* CMSIS-NN round mode definition */
#if defined(WITH_CMSIS_NN) || defined(WITH_NMSIS_NN)


#define ARM_NN_TRUNCATE 1
#define RISCV_NN_TRUNCATE 1

#endif // defined(WITH_CMSIS_NN) || defined(WITH_NMSIS_NN)
/**
  ******************************************************************************
  * @file    number.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    2 february 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NUMBER_H__
#define __NUMBER_H__

#include <stdint.h>
#include <stddef.h>
#include <math.h>

#ifdef TRAPV_SHIFT
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

#define _clamp_to(type, number) clamp_to_number_t_ ## type (number)
#define clamp_to(type, number) _clamp_to(type, number)
#define _scale(type, number, scale_factor, round_mode) scale_number_t_ ## type (number, scale_factor, round_mode)
#define scale(type, number, scale_factor, round_mode) _scale(type, number, scale_factor, round_mode)
#define _scale_and_clamp_to(type, number, scale_factor, round_mode) scale_and_clamp_to_number_t_ ## type (number, scale_factor, round_mode)
#define scale_and_clamp_to(type, number, scale_factor, round_mode) _scale_and_clamp_to(type, number, scale_factor, round_mode)

typedef enum {
  ROUND_MODE_NONE,
  ROUND_MODE_FLOOR,
  ROUND_MODE_NEAREST,
} round_mode_t;

// Idea 1: Write the smallest min max interval of the net, could be an issue for hybrid int type network
// Idea 2: listing any interval and add type in name in a switch case like <- better but painfull
// #define NUMBER_MIN		// Max value for this numeric type
// #define NUMBER_MAX		// Min value for this numeric type

// // Idea 1: List of all types and write any corresponding function 
// typedef  number_t;		// Standard size numeric type used for weights and activations
// typedef  long_number_t;	// Long numeric type used for intermediate results

#define NUMBER_MIN_INT32_T -2147483648
#define NUMBER_MAX_INT32_T 2147483647

static inline int64_t min_int32_t(
    int64_t a,
    int64_t b) {
	if (a <= b)
		return a;
	return b;
}

static inline int64_t max_int32_t(
    int64_t a,
    int64_t b) {
	if (a >= b)
		return a;
	return b;
}

static inline int64_t scale_number_t_int32_t(
  int64_t number, int scale_factor, round_mode_t round_mode) {


  if (scale_factor <= 0) {
#ifdef TRAPV_SHIFT
    // Check for possible overflow of left shift
    if (number > INT64_MAX >> -scale_factor) {
      fprintf(stderr,
              "Error: scale() overflow, number=%ld, scale_factor=%d, limit=%d\n",
              number,
              scale_factor,
              INT16_MAX >> -scale_factor);
      assert(number <= INT64_MAX >> -scale_factor);
    }
#endif
    // No rounding to apply when shifting left
    return number << - scale_factor;
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return number >> scale_factor;
  }
}
static inline int32_t clamp_to_number_t_int32_t(
  int64_t number) {
	return (int32_t) max_int32_t(
      NUMBER_MIN_INT32_T,
      min_int32_t(
        NUMBER_MAX_INT32_T, number));
}
static inline int32_t scale_and_clamp_to_number_t_int32_t(
  int64_t number, int scale_factor, round_mode_t round_mode) {
#ifdef WITH_CMSIS_NN
  // Not really CMSIS-NN but use SSAT anyway
  if (scale_factor <= 0) {
    // No rounding to apply when shifting left
    return __SSAT(number << - scale_factor, sizeof(int32_t) * 8);
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return __SSAT(number >> scale_factor, sizeof(int32_t) * 8);
  }
#else
  number = scale_number_t_int32_t(number, scale_factor, round_mode);
  return clamp_to_number_t_int32_t(number);
#endif
}

#define NUMBER_MIN_INT16_T -32768
#define NUMBER_MAX_INT16_T 32767

static inline int32_t min_int16_t(
    int32_t a,
    int32_t b) {
	if (a <= b)
		return a;
	return b;
}

static inline int32_t max_int16_t(
    int32_t a,
    int32_t b) {
	if (a >= b)
		return a;
	return b;
}

static inline int32_t scale_number_t_int16_t(
  int32_t number, int scale_factor, round_mode_t round_mode) {


  if (scale_factor <= 0) {
#ifdef TRAPV_SHIFT
    // Check for possible overflow of left shift
    if (number > INT32_MAX >> -scale_factor) {
      fprintf(stderr,
              "Error: scale() overflow, number=%d, scale_factor=%d, limit=%d\n",
              number,
              scale_factor,
              INT16_MAX >> -scale_factor);
      assert(number <= INT32_MAX >> -scale_factor);
    }
#endif
    // No rounding to apply when shifting left
    return number << - scale_factor;
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return number >> scale_factor;
  }
}
static inline int16_t clamp_to_number_t_int16_t(
  int32_t number) {
	return (int16_t) max_int16_t(
      NUMBER_MIN_INT16_T,
      min_int16_t(
        NUMBER_MAX_INT16_T, number));
}
static inline int16_t scale_and_clamp_to_number_t_int16_t(
  int32_t number, int scale_factor, round_mode_t round_mode) {
#ifdef WITH_CMSIS_NN
  // Not really CMSIS-NN but use SSAT anyway
  if (scale_factor <= 0) {
    // No rounding to apply when shifting left
    return __SSAT(number << - scale_factor, sizeof(int16_t) * 8);
  } else {
    if (round_mode == ROUND_MODE_NEAREST) {
      number += (1 << (scale_factor - 1)); // +0.5 in fixed-point
    }
    return __SSAT(number >> scale_factor, sizeof(int16_t) * 8);
  }
#else
  number = scale_number_t_int16_t(number, scale_factor, round_mode);
  return clamp_to_number_t_int16_t(number);
#endif
}




static inline void int64_t_to_float(int64_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = (float)tabint[i] / (1<<scale_factor);
  }
}

static inline void int32_t_to_float(int32_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = (float)tabint[i] / (1<<scale_factor);
  }
}

static inline void int16_t_to_float(int16_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = ((float)tabint[i]) / (1<<scale_factor);
  }
}

static inline void int8_t_to_float(int8_t * tabint, float * tabfloat, long tabdim, int scale_factor){
  for (int i=0; i<tabdim; i++){
    tabfloat[i] = ((float)tabint[i]) / (1<<scale_factor);
  }
}
#endif //__NUMBER_H__

#ifdef __cplusplus
} // extern "C"
#endif
/**
  ******************************************************************************
  * @file    conv2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    14 december 2022
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _CONV2D_H_
#define _CONV2D_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      3
#define INPUT_HEIGHT        32
#define INPUT_WIDTH         32
#define CONV_FILTERS        8
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )


typedef int16_t conv2d_output_type[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS];

#if 0
void conv2d(
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

#endif//_CONV2D_H_
/**
  ******************************************************************************
  * @file    conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "conv2d.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_CHANNELS      3
#define INPUT_HEIGHT        32
#define INPUT_WIDTH         32
#define CONV_FILTERS        8
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2
#define CONV_GROUPS         1
#define CHANNELS_PER_GROUP  (INPUT_CHANNELS / CONV_GROUPS)
#define FILTERS_PER_GROUP   (CONV_FILTERS / CONV_GROUPS)

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )

#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void conv2d(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const NUMBER_T kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS / CONV_GROUPS], // IN

  const NUMBER_T bias[CONV_FILTERS],						                // IN

  NUMBER_T output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]) {               // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short pos_x, pos_y, z, k; 	// loop indexes for output volume
  unsigned short x, y;
  int input_x, input_y;
  LONG_NUMBER_T	kernel_mac;
  LONG_NUMBER_T tmp;
  static LONG_NUMBER_T	output_acc[CONV_OUTHEIGHT][CONV_OUTWIDTH];

  for (k = 0; k < CONV_FILTERS; k++) { 
    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        output_acc[pos_y][pos_x] = 0;

        for (z = 0; z < INPUT_CHANNELS / CONV_GROUPS; z++) {
          kernel_mac = 0; 
            
          for (y = 0; y < CONV_KERNEL_SIZE_Y; y++) {
            input_y = pos_y * CONV_STRIDE_Y - ZEROPADDING_TOP + y;

            for (x = 0; x < CONV_KERNEL_SIZE_X; x++) {
              input_x = pos_x * CONV_STRIDE_X - ZEROPADDING_LEFT + x;

              if (input_x < 0 || input_x >= INPUT_WIDTH || input_y < 0 || input_y >= INPUT_HEIGHT) // ZeroPadding2D
                tmp = 0;
              else
                tmp = (LONG_NUMBER_T)input[input_y][input_x][z + (k / FILTERS_PER_GROUP) * CHANNELS_PER_GROUP] * (LONG_NUMBER_T)kernel[k][y][x][z];
              kernel_mac = kernel_mac + tmp;
            }
          }

          output_acc[pos_y][pos_x] = output_acc[pos_y][pos_x] + kernel_mac;

        }
      }
    }

    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        // Scale for possible additional precision of bias
        output_acc[pos_y][pos_x] = scale(NUMBER_T, output_acc[pos_y][pos_x],  WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Scale bias to match accumulator
        output_acc[pos_y][pos_x] += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


#ifdef ACTIVATION_LINEAR
        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // Activation function: ReLU
        if (output_acc[pos_y][pos_x] < 0) {
          output[pos_y][pos_x][k] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
        if (output_acc[pos_y][pos_x] > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
          output_acc[pos_y][pos_x] = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
        }
#endif
          output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_HEIGHT*INPUT_WIDTH*INPUT_CHANNELS];
#ifdef WITH_CMSIS_NN
  arm_convolve_HWC_q15_basic_nonsquare(
#elif defined(WITH_NMSIS_NN)
  riscv_convolve_HWC_q15_basic_nonsquare(
#endif
                                      (q15_t*)input, //Im_in
                                      INPUT_WIDTH, //dim_im_in_x
                                      INPUT_HEIGHT, //dim_im_in_y
                                      INPUT_CHANNELS, //ch_im_in
                                      (q15_t*)kernel, //wt
                                      CONV_FILTERS, //ch_im_out
                                      CONV_KERNEL_SIZE_X, //dim_kernel_x
                                      CONV_KERNEL_SIZE_Y, //dim_kernel_y
                                      ZEROPADDING_LEFT, //padding_x, left and right must be equal
                                      ZEROPADDING_TOP, //padding_y, top and bottom must be equal
                                      CONV_STRIDE_X, //stride_x
                                      CONV_STRIDE_Y, //stride_y
                                      (q15_t*)bias, //bias
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR, //bias_shift
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, //out_shift
                                      (q15_t*)output, //Im_out
                                      CONV_OUTWIDTH, //dim_im_out_x
                                      CONV_OUTHEIGHT, //dim_im_out_y
                                      bufferA, //bufferA
                                      NULL //bufferB, unused
                                      );
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef CONV_GROUPS
#undef CHANNELS_PER_GROUP
#undef FILTERS_PER_GROUP
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_CHANNELS     3
#define CONV_FILTERS       8
#define CONV_KERNEL_SIZE_Y 3
#define CONV_KERNEL_SIZE_X 3
#define CONV_GROUPS        1


const int16_t conv2d_bias[CONV_FILTERS] = {3, 3, 4, 6, 31, -9, 4, 4}
;


const int16_t conv2d_kernel[CONV_FILTERS][CONV_KERNEL_SIZE_Y][CONV_KERNEL_SIZE_X][INPUT_CHANNELS / CONV_GROUPS] = {{{{-14, -15, 3}
, {-3, 11, 13}
, {-4, 40, 10}
}
, {{-7, -1, 7}
, {-4, 6, -27}
, {12, 17, 40}
}
, {{-12, -15, 34}
, {-50, -20, -26}
, {-43, -4, -2}
}
}
, {{{-23, -23, -41}
, {1, 13, 7}
, {-18, 37, 10}
}
, {{25, 9, -19}
, {16, -40, -2}
, {3, -21, 1}
}
, {{-8, 31, 15}
, {-1, -34, -12}
, {-15, 10, -17}
}
}
, {{{-28, 36, 8}
, {-9, -17, 18}
, {-13, -34, -25}
}
, {{13, 50, 17}
, {22, 35, 5}
, {-13, -13, 9}
}
, {{-19, 14, 8}
, {-4, 6, -14}
, {-22, -5, -16}
}
}
, {{{-44, -21, -15}
, {-3, -15, -36}
, {-31, 5, -11}
}
, {{5, 23, -7}
, {14, 3, -10}
, {-31, -17, 2}
}
, {{17, 12, 27}
, {32, 24, 25}
, {4, 12, 40}
}
}
, {{{15, 3, -27}
, {-12, 3, -13}
, {16, 6, -19}
}
, {{53, -10, 18}
, {19, 8, -19}
, {11, 19, 22}
}
, {{45, 9, -27}
, {-15, -5, -19}
, {-24, -26, -11}
}
}
, {{{-36, -2, -23}
, {-1, -28, 17}
, {-7, -25, -19}
}
, {{-22, -27, 5}
, {-2, -18, 8}
, {-40, -23, -29}
}
, {{29, 27, -4}
, {-20, 31, -22}
, {26, 3, -33}
}
}
, {{{-22, 8, 1}
, {27, -33, 25}
, {-3, -17, -29}
}
, {{13, -38, 2}
, {8, -6, 9}
, {29, 3, -14}
}
, {{-23, -15, -32}
, {27, -32, 14}
, {39, -23, -26}
}
}
, {{{-9, -30, 16}
, {-12, 29, 43}
, {22, -4, 28}
}
, {{-16, -9, -33}
, {-16, 34, 0}
, {6, 21, 30}
}
, {{-42, -10, -15}
, {19, -4, 9}
, {-25, -26, 17}
}
}
}
;

#undef INPUT_CHANNELS
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_GROUPS
/**
  ******************************************************************************
  * @file    batchnorm2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _BATCH_NORMALIZATION_H_
#define _BATCH_NORMALIZATION_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      8
#define INPUT_HEIGHT        15
#define INPUT_WIDTH         15

typedef int16_t batch_normalization_output_type[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS];

#if 0
void batch_normalization(
  const number_t input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],  // IN
  const number_t kernel[INPUT_CHANNELS],                // IN
  const number_t bias[INPUT_CHANNELS],                  // IN
  batch_normalization_output_type output);                // OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_HEIGHT
#undef INPUT_WIDTH

#endif//_BATCH_NORMALIZATION_H_
/**
  ******************************************************************************
  * @file    batchnorm2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "batch_normalization.h"
#include "number.h"
#endif

#define INPUT_CHANNELS      8
#define INPUT_HEIGHT        15
#define INPUT_WIDTH         15
#define ACTIVATION_LINEAR

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void batch_normalization(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],  // IN
  const NUMBER_T kernel[INPUT_CHANNELS],                // IN
  const NUMBER_T bias[INPUT_CHANNELS],                  // IN
  batch_normalization_output_type output) {                // OUT

  LONG_NUMBER_T tmp;

  for (size_t y = 0; y < INPUT_HEIGHT; y++) {
    for (size_t x = 0; x < INPUT_WIDTH; x++) {
      for (size_t z = 0; z < INPUT_CHANNELS; z++) {
        tmp = (LONG_NUMBER_T)input[y][x][z] * (LONG_NUMBER_T)kernel[z];

        // Scale for possible additional precision of bias
        tmp = scale(NUMBER_T, tmp, WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        // Scale bias to match accumulator
        tmp += scale(NUMBER_T, (LONG_NUMBER_T)bias[z], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Activation function
#ifdef ACTIVATION_LINEAR
        // Linear (MEANS NONE)
        output[y][x][z] = scale_and_clamp_to(NUMBER_T, tmp, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // ReLU
        if (tmp < 0) {
          output[y][x][z] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
          if (tmp > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
            tmp = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
          }
#endif
          output[y][x][z] = scale_and_clamp_to(NUMBER_T, tmp, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
}

#undef INPUT_CHANNELS
#undef INPUT_HEIGHT
#undef INPUT_WIDTH
#undef ACTIVATION_LINEAR
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
/**
  ******************************************************************************
  * @file    weights/batchnorm2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

const int16_t batch_normalization_bias[8] = {-39, 1, -104, -116, -257, -12, -13, -90}
;
const int16_t batch_normalization_kernel[8] = {1740, 2665, 1084, 1179, 802, 3694, 2148, 752}
;
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

#define INPUT_CHANNELS      8
#define INPUT_HEIGHT        15
#define INPUT_WIDTH         15
#define CONV_FILTERS        32
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )


typedef int16_t conv2d_1_output_type[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS];

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
/**
  ******************************************************************************
  * @file    conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "conv2d_1.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_CHANNELS      8
#define INPUT_HEIGHT        15
#define INPUT_WIDTH         15
#define CONV_FILTERS        32
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2
#define CONV_GROUPS         1
#define CHANNELS_PER_GROUP  (INPUT_CHANNELS / CONV_GROUPS)
#define FILTERS_PER_GROUP   (CONV_FILTERS / CONV_GROUPS)

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )

#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void conv2d_1(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const NUMBER_T kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS / CONV_GROUPS], // IN

  const NUMBER_T bias[CONV_FILTERS],						                // IN

  NUMBER_T output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]) {               // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short pos_x, pos_y, z, k; 	// loop indexes for output volume
  unsigned short x, y;
  int input_x, input_y;
  LONG_NUMBER_T	kernel_mac;
  LONG_NUMBER_T tmp;
  static LONG_NUMBER_T	output_acc[CONV_OUTHEIGHT][CONV_OUTWIDTH];

  for (k = 0; k < CONV_FILTERS; k++) { 
    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        output_acc[pos_y][pos_x] = 0;

        for (z = 0; z < INPUT_CHANNELS / CONV_GROUPS; z++) {
          kernel_mac = 0; 
            
          for (y = 0; y < CONV_KERNEL_SIZE_Y; y++) {
            input_y = pos_y * CONV_STRIDE_Y - ZEROPADDING_TOP + y;

            for (x = 0; x < CONV_KERNEL_SIZE_X; x++) {
              input_x = pos_x * CONV_STRIDE_X - ZEROPADDING_LEFT + x;

              if (input_x < 0 || input_x >= INPUT_WIDTH || input_y < 0 || input_y >= INPUT_HEIGHT) // ZeroPadding2D
                tmp = 0;
              else
                tmp = (LONG_NUMBER_T)input[input_y][input_x][z + (k / FILTERS_PER_GROUP) * CHANNELS_PER_GROUP] * (LONG_NUMBER_T)kernel[k][y][x][z];
              kernel_mac = kernel_mac + tmp;
            }
          }

          output_acc[pos_y][pos_x] = output_acc[pos_y][pos_x] + kernel_mac;

        }
      }
    }

    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        // Scale for possible additional precision of bias
        output_acc[pos_y][pos_x] = scale(NUMBER_T, output_acc[pos_y][pos_x],  WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Scale bias to match accumulator
        output_acc[pos_y][pos_x] += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


#ifdef ACTIVATION_LINEAR
        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // Activation function: ReLU
        if (output_acc[pos_y][pos_x] < 0) {
          output[pos_y][pos_x][k] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
        if (output_acc[pos_y][pos_x] > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
          output_acc[pos_y][pos_x] = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
        }
#endif
          output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_HEIGHT*INPUT_WIDTH*INPUT_CHANNELS];
#ifdef WITH_CMSIS_NN
  arm_convolve_HWC_q15_basic_nonsquare(
#elif defined(WITH_NMSIS_NN)
  riscv_convolve_HWC_q15_basic_nonsquare(
#endif
                                      (q15_t*)input, //Im_in
                                      INPUT_WIDTH, //dim_im_in_x
                                      INPUT_HEIGHT, //dim_im_in_y
                                      INPUT_CHANNELS, //ch_im_in
                                      (q15_t*)kernel, //wt
                                      CONV_FILTERS, //ch_im_out
                                      CONV_KERNEL_SIZE_X, //dim_kernel_x
                                      CONV_KERNEL_SIZE_Y, //dim_kernel_y
                                      ZEROPADDING_LEFT, //padding_x, left and right must be equal
                                      ZEROPADDING_TOP, //padding_y, top and bottom must be equal
                                      CONV_STRIDE_X, //stride_x
                                      CONV_STRIDE_Y, //stride_y
                                      (q15_t*)bias, //bias
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR, //bias_shift
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, //out_shift
                                      (q15_t*)output, //Im_out
                                      CONV_OUTWIDTH, //dim_im_out_x
                                      CONV_OUTHEIGHT, //dim_im_out_y
                                      bufferA, //bufferA
                                      NULL //bufferB, unused
                                      );
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef CONV_GROUPS
#undef CHANNELS_PER_GROUP
#undef FILTERS_PER_GROUP
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_CHANNELS     8
#define CONV_FILTERS       32
#define CONV_KERNEL_SIZE_Y 3
#define CONV_KERNEL_SIZE_X 3
#define CONV_GROUPS        1


const int16_t conv2d_1_bias[CONV_FILTERS] = {-8, 11, 13, 6, 13, -1, -19, 23, 16, 3, 19, 17, 21, -7, 10, 6, -4, 15, 4, 17, 2, 10, 7, 2, -12, 8, 6, 4, 1, 6, -10, 24}
;


const int16_t conv2d_1_kernel[CONV_FILTERS][CONV_KERNEL_SIZE_Y][CONV_KERNEL_SIZE_X][INPUT_CHANNELS / CONV_GROUPS] = {{{{6, 19, -13, -7, -19, 24, -32, 5}
, {-25, -12, 21, 6, -2, 16, -22, -3}
, {-17, 1, 24, -7, -15, -4, 3, -6}
}
, {{24, 1, -37, 11, 1, 20, 9, 2}
, {8, 4, -19, -30, 0, 14, 6, 33}
, {-5, 14, 3, -24, 12, 16, 28, 25}
}
, {{-1, -12, 19, 5, -15, 19, -3, -13}
, {-19, 1, -4, 3, 0, -4, -2, -18}
, {0, 0, -10, -4, 4, 25, 1, -24}
}
}
, {{{5, -4, 13, 5, -26, -3, -7, 23}
, {-9, 4, 15, 15, -12, -19, -16, 12}
, {4, 11, 4, -4, 3, -23, 5, 11}
}
, {{5, 3, -7, -16, -6, -24, -4, 31}
, {-3, -40, 14, -34, 0, -1, -13, 29}
, {-7, -10, 8, -26, 21, -10, 8, 18}
}
, {{-17, 7, -13, -1, 1, -2, -10, -2}
, {-5, 9, -18, -10, -5, -34, 11, -11}
, {7, 8, -19, -25, -9, -13, -2, 6}
}
}
, {{{-32, -32, 19, 8, -1, -24, 22, 6}
, {-21, -5, 14, 14, -14, -11, 3, -16}
, {17, -13, 3, 1, -37, -6, 4, -1}
}
, {{-10, 18, -3, -28, -8, -26, 13, -1}
, {-27, -26, 33, 1, 4, -10, 19, 8}
, {1, -19, 18, 20, -17, 6, 11, -7}
}
, {{-8, 7, -20, 8, 3, 2, -8, 14}
, {21, 6, -8, -36, -2, -10, -21, 14}
, {-5, -3, 19, -3, -10, -5, -9, 2}
}
}
, {{{12, -2, -14, -17, -13, -5, -7, -10}
, {-2, 19, -23, -2, -14, -7, 1, -19}
, {1, 16, -11, -3, -21, -4, 0, -15}
}
, {{-27, -1, -7, 25, -13, -17, -22, -14}
, {-19, -2, 2, 37, 3, -29, -6, -18}
, {-13, 12, 5, 13, -13, -17, 4, -29}
}
, {{-17, 3, 15, 19, 9, 5, 0, 9}
, {-13, -19, 12, 21, -3, -16, -7, 2}
, {-2, -10, 16, 15, 4, -8, -25, -10}
}
}
, {{{-20, -9, 12, -2, 0, -2, 2, -9}
, {-34, -10, 11, 16, -6, -18, -1, -12}
, {-27, -8, 15, 7, -17, -22, 3, -31}
}
, {{13, -24, 3, -17, 6, -15, -21, 13}
, {-6, -1, 24, -19, -1, -2, -9, 17}
, {-18, -5, 26, 8, 11, -16, -7, -19}
}
, {{28, -17, -13, -4, 11, -8, -13, 4}
, {24, -24, -14, -19, 6, -6, -5, -6}
, {22, 11, -5, -20, -5, 1, -12, 16}
}
}
, {{{-19, -8, -12, -10, 0, -6, 11, 7}
, {-32, -27, 14, 10, 3, -12, -18, 17}
, {-7, -1, 18, -17, 16, 12, -4, -16}
}
, {{-1, 7, -28, -10, -15, 3, -2, 11}
, {-10, 1, 9, -36, 11, 8, -13, 19}
, {3, 1, 7, -21, -9, 14, -35, -9}
}
, {{9, -1, 13, -3, -8, 1, 1, 1}
, {10, -7, -20, -7, -17, 6, -11, 2}
, {-13, -19, -13, -3, -26, 15, -4, 12}
}
}
, {{{8, -4, -11, -33, -12, 13, -19, 8}
, {-19, -1, -6, -34, 10, 19, -12, 14}
, {-2, 15, 7, -23, 10, 32, -20, 18}
}
, {{8, 5, 12, 8, 17, -2, 10, 1}
, {-19, -11, 19, 30, 7, 17, 15, 7}
, {-3, 0, -6, 7, 14, 10, -41, 22}
}
, {{-10, 9, -6, 0, 18, 17, 18, 4}
, {6, 1, -5, 10, 2, 14, -8, -2}
, {19, -4, 18, -5, 21, 14, 14, 2}
}
}
, {{{-15, 17, -19, 7, -22, 1, 10, 7}
, {-25, -9, -10, 0, -11, -19, 7, 6}
, {5, 10, 20, 3, 6, -8, 8, -18}
}
, {{14, -25, 13, -3, -5, -20, -20, 11}
, {-40, 2, 18, 6, -6, -2, -2, -16}
, {6, -3, 22, -9, 9, -13, -11, -23}
}
, {{4, -23, -1, -38, 9, -18, -14, -1}
, {-9, 8, -1, -26, -9, -21, 8, 14}
, {-14, -16, 10, -5, -8, -11, -1, -24}
}
}
, {{{-32, 17, 0, 23, -13, -8, 6, -10}
, {-4, -4, -3, 19, -11, -16, 16, -7}
, {11, -12, 8, -4, -12, -8, 8, 3}
}
, {{-21, -20, 35, 3, -5, 1, -4, -8}
, {14, 15, 24, 6, 23, -1, -3, -30}
, {10, 8, -29, 12, -15, -2, -17, 9}
}
, {{-9, -16, 0, -30, -3, -12, 6, 1}
, {4, 21, 14, 0, -8, -14, 11, -18}
, {14, 0, -18, 12, 2, -9, -4, 17}
}
}
, {{{5, -10, -15, 13, 7, -2, 0, -1}
, {-15, 0, 16, -8, -3, 6, -19, 5}
, {12, 3, 12, -5, -3, -13, -11, -11}
}
, {{-10, -14, -1, 19, -7, -11, -1, -1}
, {-6, -7, 4, -20, -7, 1, -2, 5}
, {-4, 21, 23, -36, 0, -8, -10, -40}
}
, {{-8, 8, 7, -16, -1, -19, 1, 20}
, {-15, -19, 32, -26, 5, -7, -8, -25}
, {-5, -10, -1, -5, -6, 11, -11, -27}
}
}
, {{{-7, -10, -2, -30, -2, -31, -1, 4}
, {-8, 8, 5, -1, -3, -28, 2, 14}
, {11, 16, 15, -1, 13, -5, 8, 22}
}
, {{7, -18, 3, -34, -1, -23, -9, -2}
, {-17, -3, -3, -43, 0, -8, -4, 21}
, {-1, 8, 8, -28, 5, -20, 9, 4}
}
, {{-21, 12, -14, 13, 4, -17, 6, -14}
, {-4, 0, -28, -25, 11, -7, 14, -8}
, {12, 2, 3, -33, -2, -20, -14, 10}
}
}
, {{{-8, 2, -10, 4, -1, -9, -16, -9}
, {5, -6, -20, -9, -20, -22, -13, -10}
, {2, -6, -3, 12, -35, -12, -7, 4}
}
, {{-5, 5, -3, 6, -5, -42, -9, 4}
, {-28, 0, -23, 6, -24, -7, -20, 3}
, {-13, -24, -7, 17, 4, -23, -21, 3}
}
, {{3, -28, -16, -14, 20, -22, -5, -12}
, {-26, -10, -9, 27, -17, -20, -9, 3}
, {-25, -15, 6, 14, 29, -8, -2, 13}
}
}
, {{{1, -18, -11, 20, 27, -22, -10, -5}
, {-27, -1, 4, -25, 16, -15, 23, -1}
, {5, 3, 16, -6, 9, -4, -14, 19}
}
, {{2, -10, 16, 13, 19, -7, -19, -6}
, {0, 7, 7, -27, 5, -6, -10, 14}
, {3, 9, -16, -25, -17, -23, 2, 7}
}
, {{10, -6, -1, -5, -1, -4, 5, 4}
, {29, -8, -3, -7, -33, -33, 8, 14}
, {7, -1, 5, -14, -28, -24, 4, 3}
}
}
, {{{-2, -9, -16, -2, 8, -3, -5, 10}
, {12, -15, -6, -24, -16, 15, 2, -6}
, {12, -1, 10, 10, -21, 19, -25, 0}
}
, {{3, -21, 3, -23, 19, 9, 18, -8}
, {19, -22, 18, -35, 6, -3, 16, -14}
, {18, -14, -6, -2, -6, -11, 9, 3}
}
, {{-8, -5, -27, 1, -12, -10, 17, -4}
, {-8, 11, -33, 14, -11, -15, 34, -11}
, {-5, 1, -15, 12, -31, 20, -3, -2}
}
}
, {{{7, 1, 11, -14, -6, -8, -14, 2}
, {7, 13, 0, -30, 14, -18, -1, -17}
, {-3, -4, 16, -2, 15, -19, 11, -5}
}
, {{-33, -3, 0, 23, -22, 3, -9, -9}
, {3, 1, -27, -7, 5, -23, 10, -36}
, {8, -9, -20, -19, 8, -9, 3, -3}
}
, {{-24, -19, 18, 16, 8, -5, -36, 6}
, {7, -9, 18, -4, -7, -27, -7, -18}
, {19, -17, 15, 14, -14, 4, -11, -8}
}
}
, {{{-24, 4, 9, 26, -8, 0, -28, 5}
, {17, -2, -2, 14, -8, 10, -9, 0}
, {10, 1, -11, -22, -28, -3, -1, -11}
}
, {{-34, 3, 31, 14, 6, -2, -12, 5}
, {-2, 5, 25, -1, 4, -12, -19, -16}
, {-23, -6, -10, -15, -31, -5, -8, -27}
}
, {{1, -2, -8, -11, 13, -21, -25, 0}
, {1, -21, 7, -2, -12, -20, -24, 4}
, {-2, 17, 8, 12, 10, -2, 5, 5}
}
}
, {{{5, 1, 5, 1, 23, 4, 4, -18}
, {-3, 8, 5, -26, -23, 11, -11, 2}
, {-11, 4, -9, -14, 12, -9, -3, -16}
}
, {{-35, 0, 9, -32, -10, -9, 13, -19}
, {-7, 3, -22, 18, -5, -8, 12, 3}
, {-2, -1, 20, -3, 22, -6, 0, -28}
}
, {{-2, 2, -22, 15, -34, -4, 17, 15}
, {-11, 1, 13, -4, -3, -12, -4, 23}
, {2, -12, 1, 5, 14, -9, -15, 0}
}
}
, {{{-12, 9, 19, -48, 4, -23, 4, 7}
, {-6, 32, 0, 3, 19, -20, -5, 10}
, {-11, 20, 2, 25, 11, -12, -18, 20}
}
, {{-10, 12, -43, 12, 4, 6, 1, -11}
, {-5, -1, -32, 3, -6, -5, -5, 19}
, {-12, 21, -3, -20, -2, -30, 3, 31}
}
, {{5, 5, -5, 0, -7, 5, 14, -23}
, {-2, -3, -4, 9, 1, -16, -12, -9}
, {3, 9, 13, 4, -2, -7, -9, -12}
}
}
, {{{-4, -13, 4, -13, 3, -10, 1, -12}
, {-18, 25, 6, 5, -5, 2, -12, -8}
, {-2, -5, -28, -1, 4, 14, -2, 2}
}
, {{0, 3, -16, 21, -1, -9, -7, -7}
, {6, -1, -2, -19, 8, -4, -14, 17}
, {-4, -18, 2, -21, 9, 3, -19, -12}
}
, {{-55, 4, -3, 23, 14, 9, 0, -10}
, {31, -18, 3, 11, 27, 1, -32, -25}
, {18, 8, -9, -26, -29, -5, -7, -6}
}
}
, {{{14, -2, -12, -2, 25, -16, 13, -3}
, {0, 20, -1, -3, 17, -9, -1, 1}
, {10, 3, -11, -7, -2, -3, 19, -10}
}
, {{1, 42, -13, 26, 8, -19, 36, -31}
, {13, 1, -21, 3, -8, -11, 21, -13}
, {-13, 15, -17, -12, -36, 1, -5, 14}
}
, {{-35, 30, 16, 6, 13, -7, -4, 3}
, {13, 14, 5, 3, -16, -12, -7, -13}
, {-4, 7, 6, 16, -9, -27, -5, 0}
}
}
, {{{-6, -8, 14, -23, -16, 26, 8, -11}
, {-49, 3, 14, -10, 13, 34, 10, -6}
, {-20, 2, -6, -7, 1, 17, -3, -3}
}
, {{-31, -11, -4, -17, 13, 11, 1, -21}
, {-28, 5, -1, -14, 2, 0, -2, -43}
, {-12, -7, -38, 7, 12, -18, -5, -8}
}
, {{-2, -15, -1, 13, 5, 21, -6, -18}
, {9, -9, -11, -13, 7, -10, 4, 5}
, {18, 8, -15, 15, -3, -3, -15, 18}
}
}
, {{{29, 10, 10, 24, 7, 2, 22, -10}
, {0, -4, -30, 0, 21, -23, -10, 9}
, {-8, -6, -26, -15, -5, -18, -10, 18}
}
, {{9, -13, -5, -36, -17, -17, 2, 13}
, {7, -10, -14, 1, -14, -10, 9, -9}
, {-5, 3, 12, 18, 20, -17, 21, -1}
}
, {{12, -3, -18, 27, -13, -22, -4, 16}
, {13, -1, -13, 5, -13, -7, -4, 25}
, {-3, 5, -1, -21, 6, -21, -10, 12}
}
}
, {{{-13, -24, 33, -3, 2, -18, -7, 7}
, {-31, 14, 5, -2, -1, 2, -1, -18}
, {-14, -7, -14, 12, -3, -7, 10, -1}
}
, {{-3, 10, -6, -7, -30, 0, 4, 24}
, {-19, -2, -3, -30, -7, 17, -8, 7}
, {6, 9, -9, 28, 21, 3, 11, 11}
}
, {{14, -7, -22, 14, -17, -14, 0, -12}
, {-3, 11, -22, 2, 18, -17, 23, -2}
, {8, 2, -12, -12, 17, -12, 7, 1}
}
}
, {{{4, -6, -5, -3, -5, -4, -2, 10}
, {6, -2, 0, -33, -10, 15, -8, 13}
, {6, -16, 19, -27, -3, 8, -9, 19}
}
, {{10, -9, -16, -17, -9, -10, 5, 0}
, {10, -10, -37, -2, -27, 11, -4, -5}
, {-11, -19, -40, 2, -27, 25, 9, -8}
}
, {{13, -7, 10, 0, 6, 8, 4, 9}
, {-13, -28, 28, 7, 23, -18, 8, -4}
, {-12, -43, 14, 3, 17, 8, 15, -16}
}
}
, {{{-25, 3, 13, -8, -8, -6, -13, -9}
, {2, -6, 3, 3, -11, 8, 17, -4}
, {-4, -1, -7, -24, 0, 7, 14, 3}
}
, {{-5, -9, -22, -3, -13, 16, -8, 26}
, {-32, -5, 33, -4, 7, 7, 33, -9}
, {-8, -27, -30, -6, 0, 3, 1, 12}
}
, {{-7, -10, -17, 11, -6, 3, -2, 22}
, {-19, -7, 19, -29, -4, 25, -2, -7}
, {-20, 9, -17, -6, 9, 13, -3, 1}
}
}
, {{{-19, -4, 5, -13, 3, -6, -13, 8}
, {-4, -21, 1, -28, -20, -22, -3, 15}
, {3, -9, 5, -1, -26, -13, -14, 1}
}
, {{-15, -6, 7, -7, 5, -11, -27, 3}
, {-5, 11, -9, -25, -16, -3, 0, 14}
, {-18, -20, 12, -4, -28, -12, 0, 10}
}
, {{-29, 1, -12, 10, 13, 0, 9, 5}
, {-4, -14, -29, -16, -12, -23, -14, 17}
, {-19, 3, 15, 8, 5, -21, -1, 15}
}
}
, {{{1, 15, -24, 4, 20, -3, 10, -12}
, {16, 11, -4, -17, 12, 20, 10, 18}
, {14, 21, 16, 19, -12, 11, 6, -7}
}
, {{-6, 20, 16, 3, 6, 1, -8, 14}
, {19, 22, -10, -3, 22, 4, 17, -19}
, {11, 6, -2, 13, -25, -3, -2, 14}
}
, {{-7, 20, -11, -8, -7, -12, -27, 6}
, {13, 2, -16, -9, 17, -15, 5, -28}
, {9, 0, -4, -10, -39, -17, 8, 1}
}
}
, {{{9, -4, 21, 6, 1, 7, 1, -11}
, {21, -12, 18, 37, 2, 3, 1, -39}
, {9, 5, 0, 5, -23, -20, -2, 6}
}
, {{-15, 2, -23, -7, 1, -4, 19, 21}
, {-23, 6, 3, 15, 9, -12, 3, -1}
, {-19, -31, 1, 8, -17, -9, 0, -27}
}
, {{-21, 8, -6, 7, -20, 0, 2, 13}
, {-8, 13, -6, 7, 19, 6, 3, -6}
, {-5, 15, 13, -2, -2, 3, -16, -35}
}
}
, {{{15, 20, 19, 4, 0, 0, 3, -8}
, {-7, -7, -19, -5, -11, -7, -8, -14}
, {9, 3, 6, 8, 13, 12, -16, 5}
}
, {{3, 10, -4, -3, 6, -6, -12, -16}
, {31, 0, -31, -24, -17, -5, 10, 26}
, {6, -5, -9, 25, -8, 3, 11, -25}
}
, {{-15, 3, 7, 4, -16, 6, -13, -5}
, {11, -7, -25, -36, 6, -12, 5, -3}
, {-25, 5, 6, 4, 16, -6, -18, -3}
}
}
, {{{-2, -3, -9, 4, -19, -7, -20, 7}
, {-23, 19, -36, 17, -2, -20, 19, -16}
, {-1, 8, -13, 19, -5, -11, -4, 14}
}
, {{-10, -2, -15, 8, -15, -7, -31, 12}
, {0, -5, 10, 16, 13, 3, -39, 1}
, {-5, -22, 17, 7, 18, -25, 0, -15}
}
, {{-9, -16, 10, -19, 0, 0, 7, -4}
, {-4, 2, 10, -44, -1, 2, 5, 15}
, {11, -6, 1, -13, -22, 10, -13, 4}
}
}
, {{{-1, 0, -12, -1, -8, 12, 20, -17}
, {-10, 21, 3, -4, 9, 28, 10, -6}
, {-4, 13, -19, -4, 0, 8, -24, -13}
}
, {{8, 13, 5, -28, 9, 21, 5, -6}
, {16, 10, -4, -38, 0, 3, 12, 18}
, {-4, 8, 10, -26, 24, 11, -20, 13}
}
, {{2, -25, 10, -18, 9, -2, -4, 2}
, {2, 0, -4, 17, -29, -12, -27, 7}
, {3, 0, 10, 20, -25, 20, -12, -11}
}
}
, {{{-9, -22, 12, -2, 12, -32, -22, 23}
, {-9, 1, 19, -18, 30, -26, -45, -5}
, {11, -4, 0, -14, 8, -43, 9, -22}
}
, {{-23, -1, 13, -31, 1, -17, -2, 14}
, {3, 6, 2, -55, -5, -21, 0, -2}
, {23, 14, -2, 3, -12, -27, 9, -7}
}
, {{6, 2, 5, -11, -1, -7, -5, 12}
, {-4, -2, -10, 4, -22, -20, -6, 1}
, {3, 4, 1, -10, -4, -28, 22, 14}
}
}
}
;

#undef INPUT_CHANNELS
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_GROUPS
/**
  ******************************************************************************
  * @file    batchnorm2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _BATCH_NORMALIZATION_1_H_
#define _BATCH_NORMALIZATION_1_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      32
#define INPUT_HEIGHT        7
#define INPUT_WIDTH         7

typedef int16_t batch_normalization_1_output_type[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS];

#if 0
void batch_normalization_1(
  const number_t input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],  // IN
  const number_t kernel[INPUT_CHANNELS],                // IN
  const number_t bias[INPUT_CHANNELS],                  // IN
  batch_normalization_1_output_type output);                // OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_HEIGHT
#undef INPUT_WIDTH

#endif//_BATCH_NORMALIZATION_1_H_
/**
  ******************************************************************************
  * @file    batchnorm2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "batch_normalization_1.h"
#include "number.h"
#endif

#define INPUT_CHANNELS      32
#define INPUT_HEIGHT        7
#define INPUT_WIDTH         7
#define ACTIVATION_LINEAR

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void batch_normalization_1(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],  // IN
  const NUMBER_T kernel[INPUT_CHANNELS],                // IN
  const NUMBER_T bias[INPUT_CHANNELS],                  // IN
  batch_normalization_1_output_type output) {                // OUT

  LONG_NUMBER_T tmp;

  for (size_t y = 0; y < INPUT_HEIGHT; y++) {
    for (size_t x = 0; x < INPUT_WIDTH; x++) {
      for (size_t z = 0; z < INPUT_CHANNELS; z++) {
        tmp = (LONG_NUMBER_T)input[y][x][z] * (LONG_NUMBER_T)kernel[z];

        // Scale for possible additional precision of bias
        tmp = scale(NUMBER_T, tmp, WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        // Scale bias to match accumulator
        tmp += scale(NUMBER_T, (LONG_NUMBER_T)bias[z], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Activation function
#ifdef ACTIVATION_LINEAR
        // Linear (MEANS NONE)
        output[y][x][z] = scale_and_clamp_to(NUMBER_T, tmp, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // ReLU
        if (tmp < 0) {
          output[y][x][z] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
          if (tmp > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
            tmp = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
          }
#endif
          output[y][x][z] = scale_and_clamp_to(NUMBER_T, tmp, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
}

#undef INPUT_CHANNELS
#undef INPUT_HEIGHT
#undef INPUT_WIDTH
#undef ACTIVATION_LINEAR
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
/**
  ******************************************************************************
  * @file    weights/batchnorm2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

const int16_t batch_normalization_1_bias[32] = {-45, -56, -64, -66, -54, -86, -12, -105, -51, -67, -90, -92, -69, -75, -80, -75, -64, -70, -73, -73, -75, -50, -62, -71, -54, -102, -50, -52, -61, -71, -67, -109}
;
const int16_t batch_normalization_1_kernel[32] = {286, 177, 198, 187, 173, 304, 161, 238, 209, 246, 193, 266, 233, 256, 244, 252, 281, 229, 320, 228, 268, 223, 266, 313, 337, 276, 280, 242, 287, 238, 275, 233}
;
/**
  ******************************************************************************
  * @file    conv2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    14 december 2022
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _CONV2D_2_H_
#define _CONV2D_2_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      32
#define INPUT_HEIGHT        7
#define INPUT_WIDTH         7
#define CONV_FILTERS        64
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )


typedef int16_t conv2d_2_output_type[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS];

#if 0
void conv2d_2(
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

#endif//_CONV2D_2_H_
/**
  ******************************************************************************
  * @file    conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "conv2d_2.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_CHANNELS      32
#define INPUT_HEIGHT        7
#define INPUT_WIDTH         7
#define CONV_FILTERS        64
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2
#define CONV_GROUPS         1
#define CHANNELS_PER_GROUP  (INPUT_CHANNELS / CONV_GROUPS)
#define FILTERS_PER_GROUP   (CONV_FILTERS / CONV_GROUPS)

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )

#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void conv2d_2(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const NUMBER_T kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS / CONV_GROUPS], // IN

  const NUMBER_T bias[CONV_FILTERS],						                // IN

  NUMBER_T output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]) {               // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short pos_x, pos_y, z, k; 	// loop indexes for output volume
  unsigned short x, y;
  int input_x, input_y;
  LONG_NUMBER_T	kernel_mac;
  LONG_NUMBER_T tmp;
  static LONG_NUMBER_T	output_acc[CONV_OUTHEIGHT][CONV_OUTWIDTH];

  for (k = 0; k < CONV_FILTERS; k++) { 
    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        output_acc[pos_y][pos_x] = 0;

        for (z = 0; z < INPUT_CHANNELS / CONV_GROUPS; z++) {
          kernel_mac = 0; 
            
          for (y = 0; y < CONV_KERNEL_SIZE_Y; y++) {
            input_y = pos_y * CONV_STRIDE_Y - ZEROPADDING_TOP + y;

            for (x = 0; x < CONV_KERNEL_SIZE_X; x++) {
              input_x = pos_x * CONV_STRIDE_X - ZEROPADDING_LEFT + x;

              if (input_x < 0 || input_x >= INPUT_WIDTH || input_y < 0 || input_y >= INPUT_HEIGHT) // ZeroPadding2D
                tmp = 0;
              else
                tmp = (LONG_NUMBER_T)input[input_y][input_x][z + (k / FILTERS_PER_GROUP) * CHANNELS_PER_GROUP] * (LONG_NUMBER_T)kernel[k][y][x][z];
              kernel_mac = kernel_mac + tmp;
            }
          }

          output_acc[pos_y][pos_x] = output_acc[pos_y][pos_x] + kernel_mac;

        }
      }
    }

    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        // Scale for possible additional precision of bias
        output_acc[pos_y][pos_x] = scale(NUMBER_T, output_acc[pos_y][pos_x],  WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Scale bias to match accumulator
        output_acc[pos_y][pos_x] += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


#ifdef ACTIVATION_LINEAR
        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // Activation function: ReLU
        if (output_acc[pos_y][pos_x] < 0) {
          output[pos_y][pos_x][k] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
        if (output_acc[pos_y][pos_x] > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
          output_acc[pos_y][pos_x] = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
        }
#endif
          output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_HEIGHT*INPUT_WIDTH*INPUT_CHANNELS];
#ifdef WITH_CMSIS_NN
  arm_convolve_HWC_q15_basic_nonsquare(
#elif defined(WITH_NMSIS_NN)
  riscv_convolve_HWC_q15_basic_nonsquare(
#endif
                                      (q15_t*)input, //Im_in
                                      INPUT_WIDTH, //dim_im_in_x
                                      INPUT_HEIGHT, //dim_im_in_y
                                      INPUT_CHANNELS, //ch_im_in
                                      (q15_t*)kernel, //wt
                                      CONV_FILTERS, //ch_im_out
                                      CONV_KERNEL_SIZE_X, //dim_kernel_x
                                      CONV_KERNEL_SIZE_Y, //dim_kernel_y
                                      ZEROPADDING_LEFT, //padding_x, left and right must be equal
                                      ZEROPADDING_TOP, //padding_y, top and bottom must be equal
                                      CONV_STRIDE_X, //stride_x
                                      CONV_STRIDE_Y, //stride_y
                                      (q15_t*)bias, //bias
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR, //bias_shift
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, //out_shift
                                      (q15_t*)output, //Im_out
                                      CONV_OUTWIDTH, //dim_im_out_x
                                      CONV_OUTHEIGHT, //dim_im_out_y
                                      bufferA, //bufferA
                                      NULL //bufferB, unused
                                      );
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef CONV_GROUPS
#undef CHANNELS_PER_GROUP
#undef FILTERS_PER_GROUP
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_CHANNELS     32
#define CONV_FILTERS       64
#define CONV_KERNEL_SIZE_Y 3
#define CONV_KERNEL_SIZE_X 3
#define CONV_GROUPS        1


const int16_t conv2d_2_bias[CONV_FILTERS] = {-38, -25, -27, -52, -28, 10, -3, 7, -16, -1, -33, -8, -8, -5, 5, -42, 32, -3, 7, 1, -26, -26, -6, -7, -5, 0, -35, 1, 3, 7, -22, 36, -12, -14, -6, 14, -16, -16, -14, -27, -16, -5, -49, -14, -13, -16, -20, -13, 8, -27, 4, 18, -13, 0, -7, -10, 13, 0, -6, -9, -18, -21, -3, 10}
;


const int16_t conv2d_2_kernel[CONV_FILTERS][CONV_KERNEL_SIZE_Y][CONV_KERNEL_SIZE_X][INPUT_CHANNELS / CONV_GROUPS] = {{{{-3, -3, -7, -21, -1, 0, -4, -4, -11, -6, 17, -14, -7, 3, 4, -6, 11, -6, 3, 7, -1, -2, 13, 13, 8, -8, 9, -4, -14, 8, 3, 2}
, {4, 9, 15, -7, 9, 2, 9, 3, -31, -8, 37, -16, 0, 18, -17, -12, -10, -2, 18, -14, -17, 6, 18, -6, -19, 1, 9, 7, 21, 8, -16, -8}
, {3, 12, 7, -4, 9, -3, -6, 0, 0, 2, -2, -15, 1, 7, -9, 7, -18, -28, -15, -12, -20, -6, -2, -9, -25, -6, 22, 1, -2, 3, 10, 4}
}
, {{-27, -28, 8, 19, 2, -16, 4, 20, -3, 13, -20, -17, -21, -37, -13, 12, -11, -28, 9, -5, 4, 5, -12, -14, -8, -13, -3, -6, -13, 27, -18, -15}
, {-35, -26, -39, -6, -29, -48, 23, -5, -2, -3, 1, -16, -10, -11, 11, -8, -7, -9, -17, 12, 11, -7, -8, 19, -20, -8, 19, -7, -8, -21, -27, -14}
, {-2, -7, -7, 13, -5, 2, 16, 6, 3, -20, -8, 14, -26, -1, -3, -4, -9, 20, -19, -8, 1, -10, -18, 4, 5, 9, 5, -9, 0, 10, -5, -12}
}
, {{-1, -9, -3, 2, -6, 0, 6, -4, -27, -8, -10, -6, -6, -3, -3, -2, 9, 1, 0, -17, 6, -1, -21, -10, 5, 14, -12, -7, -2, -22, 5, 7}
, {-19, -36, -14, 22, -20, -14, 0, -7, -15, 5, -33, 9, 7, 2, 8, -16, -3, -10, -3, -4, 6, -1, -24, -11, -18, -5, -11, 1, -1, -4, 3, -11}
, {9, -5, 1, -10, -4, -2, 10, -7, -5, 8, -22, -14, -1, 4, 2, -17, 1, 6, 10, 14, 6, 0, -20, -7, -31, 3, -7, -3, -8, -3, 10, -13}
}
}
, {{{-11, -12, -6, 8, -5, -1, -9, -5, -1, 9, -16, 7, -4, -10, -16, 9, 0, -21, -25, -7, 0, -12, -10, -5, 8, -8, -25, -11, -9, 4, -26, 5}
, {-6, -30, -16, 12, -45, -10, -9, -13, -7, 2, -26, -7, -21, -16, 7, 14, 6, 1, 4, -13, -7, -14, -32, 13, 13, 0, -17, 11, -23, -16, -11, -21}
, {-4, 1, -1, -20, 1, 6, -11, -7, -7, -14, 9, -21, -8, -4, -9, -17, -4, -7, 1, -7, 10, 1, 4, -20, 24, 6, 9, -15, 2, -10, -5, 4}
}
, {{13, 11, 12, 2, 5, 18, -14, -3, -3, -18, 0, -9, 0, -4, -24, -19, 5, 12, 6, 6, 2, 8, 14, 1, -7, 7, -19, -10, -9, 2, -2, -5}
, {2, 19, 7, -4, 0, 7, -15, -13, -3, -4, -26, -8, -2, -7, -18, 10, -10, -18, 3, -11, -25, -32, -5, -34, -15, -21, -2, 2, -19, -2, 1, 14}
, {5, -7, -2, -2, -5, -26, 5, 4, 8, -5, -19, -18, 4, -12, -5, 5, -5, -3, -12, -4, -9, 16, 5, -11, -4, -19, -25, -18, -13, -2, 3, -14}
}
, {{-19, 5, -12, -6, -3, -7, 0, -23, -9, -19, -15, 17, 12, -6, -2, -9, 2, -14, 2, -6, -8, 28, -10, -7, -34, 2, 6, -16, -6, 2, -7, -14}
, {-5, -13, 18, 8, 6, -6, 7, 6, 10, -8, -38, -11, 4, -17, -1, 15, -11, -12, -21, -1, -23, -5, -20, 1, -8, -40, -28, 5, -57, 11, -10, -22}
, {16, 10, 1, -4, 10, 6, 6, 13, -9, -10, -4, -10, 2, -7, -1, 0, -5, -5, -2, 13, -7, 7, -12, 7, -8, -3, -9, -16, -1, 5, 2, 8}
}
}
, {{{0, -10, 6, -5, -5, 0, -16, 5, -6, -8, 13, 6, -15, -2, -2, -14, 9, -9, -2, -19, -15, 1, -5, -24, -2, 18, -25, -5, 17, -11, -12, 1}
, {-5, 2, 3, 6, 13, 4, -8, 33, 5, 7, 0, -12, -18, -28, 6, 17, -8, -23, 3, -14, -24, -33, -24, -20, 5, -33, -35, 13, 0, 15, -2, -11}
, {-11, -18, -12, 1, -3, -7, -11, -17, 22, -9, -15, 15, -1, 17, 10, 12, 12, -10, -11, 5, 18, -5, -17, -7, 13, 5, -4, 2, -4, -4, 22, 1}
}
, {{9, 20, 0, -31, 3, 34, -2, -5, -7, 0, 13, -30, 24, -5, -29, -7, -18, 10, 13, 8, -9, -11, -5, 9, 12, 14, 17, -33, 4, 5, -5, -4}
, {14, 6, 16, -20, 9, 5, -6, -1, -16, 11, 0, -7, -10, 18, -7, -9, 5, -5, -16, 3, -19, 1, 11, 6, -9, -1, 13, 25, -15, -28, 0, 18}
, {1, 1, 12, 26, 17, -17, -2, 3, -2, -4, 0, 22, -35, -29, -7, -10, 1, 4, 12, -19, -6, 3, 11, -18, -10, -1, -7, 12, 12, 14, -16, -8}
}
, {{-17, -11, -2, 9, -25, -6, -12, -22, 4, -35, 4, -8, 7, 9, -6, 0, 3, 4, -5, 2, 2, -4, 7, -8, -4, 15, 5, -5, -1, -11, 2, 4}
, {10, 7, -16, -2, -11, 24, -14, 9, -5, 7, -4, 4, -18, -9, 5, -16, -12, 17, -7, -9, -14, -2, -2, 1, 5, 0, 0, 15, -13, -2, 4, -10}
, {3, 0, -11, -23, -3, -4, -8, 8, -7, 12, 11, -9, 8, 7, 8, -2, 17, -14, 10, 4, 7, -5, -5, 6, 29, -13, -15, 3, 10, -13, 7, 7}
}
}
, {{{-16, -3, -6, 14, -21, 2, 15, 6, 8, 15, -4, 4, -13, -22, 18, 4, 4, -18, 3, 9, 13, -20, 14, -11, 21, -3, 2, 10, 2, -3, -8, -1}
, {-11, -20, -6, 10, 6, 5, 6, 0, -9, -17, -7, 1, -23, -1, 8, -6, 18, 15, -4, 10, 12, 8, -10, -7, 3, -20, -5, -16, -2, 0, 22, -8}
, {14, -5, -15, 9, 6, 21, -1, 11, 5, 5, 4, -8, 4, -1, -8, -3, 6, 8, -6, 3, -1, -12, 6, 10, -3, 1, 0, 0, 4, -5, 8, 5}
}
, {{-23, 0, 9, -15, -1, -5, -7, 10, 10, 20, 3, -13, -8, -4, 2, 8, -3, -14, 2, -10, 1, -5, 20, -10, 33, 0, -11, 5, -17, 2, 7, 11}
, {-15, 10, 1, -36, 11, 10, -5, -3, -32, -20, 20, 3, 9, 20, -25, -19, -20, 0, 14, 3, -18, -5, -6, -16, -33, 23, 14, -37, 0, 3, 2, 16}
, {-16, 4, 6, -23, 12, 1, -12, 15, 0, 5, -5, -14, 13, -6, -29, 10, 0, -29, -30, -32, -19, 2, 12, -31, 16, -19, -10, -18, -40, 4, -10, 1}
}
, {{9, -18, 3, -2, 8, -1, -22, 1, 0, 4, -8, 3, -3, 15, 8, 1, -2, 2, -2, 4, 12, -14, -14, -1, -6, 3, 4, -3, 11, -4, 7, 1}
, {-5, -33, -3, 10, -37, -22, -1, -21, 6, -9, -4, 3, -31, 4, 0, 6, 4, -1, -18, 7, 1, -1, -12, 11, -2, -5, 2, 16, -10, -11, -11, -29}
, {14, -6, -7, 12, -6, 3, -8, -4, -12, -4, -11, 5, -32, 4, 17, 3, -2, 5, 5, 20, 0, -4, 6, 20, 5, -6, -1, 2, -10, 2, 12, -1}
}
}
, {{{-7, 7, 7, -11, -9, 17, -23, -22, -14, -7, -1, 5, 4, 5, -18, 3, -8, -2, 2, -4, -12, -9, -3, 0, -23, 5, 6, 2, 6, -19, 2, 11}
, {-3, -6, 8, -13, -15, -4, -32, -22, -4, -22, -9, -6, 14, 18, 1, 0, -21, -22, -18, -9, -29, -21, -23, 13, -18, -3, -3, -19, -26, -1, 5, 13}
, {-13, 0, -1, 12, -4, -8, -18, -7, -12, -18, -4, 10, 0, 6, -9, -4, -28, -20, -18, -27, -27, 6, -3, 14, -18, -7, -12, 7, 6, 1, 9, 0}
}
, {{-9, 5, 7, -14, -8, 11, -6, 3, -8, 14, 7, 14, -2, -10, -6, 4, 2, -5, -11, 0, -16, -19, -14, -5, 3, -1, -9, 9, 6, 8, -4, -2}
, {-33, 2, 7, 16, 0, -14, -25, -4, 2, -23, -14, 20, -10, 7, -20, 4, -13, -26, -32, -14, -20, -8, -15, -3, 6, 8, -17, 4, 1, -4, -8, -3}
, {-18, 7, -17, 6, -13, -13, -22, 0, -13, 1, -7, 7, -8, 5, 6, 5, -17, -22, -18, -11, -21, -14, -34, 2, -6, 10, -7, -5, -2, 11, -13, -2}
}
, {{0, 4, 0, -2, -10, 10, -14, -2, -8, -8, -7, 4, -15, -31, -13, 5, -9, -13, -1, 0, -15, -32, -1, -8, -2, 6, -14, 2, 1, -14, -14, 0}
, {-14, -8, 11, 7, 8, 6, -16, 19, -19, -3, -4, 5, -6, -6, -28, 9, -27, -11, 17, -5, -26, -44, -18, -24, -2, 33, -10, -5, 5, -18, -16, -6}
, {2, 4, -4, -8, 5, -1, -19, -19, 12, -2, -12, 14, -5, -1, 7, 7, -20, -21, 1, -11, -20, -24, -20, -6, -4, 6, -18, -8, -5, -13, -11, 12}
}
}
, {{{-15, -3, 12, 3, 22, -7, -21, 24, 23, 11, 8, -10, 8, 3, 2, 3, 6, 6, 3, -15, 11, -16, -5, 3, -2, -9, 20, 11, -7, 15, 1, -7}
, {10, -1, -3, -3, 11, 11, 2, 3, 8, 5, 15, -27, 20, 14, 15, 8, -32, -13, 25, 13, 16, -9, -16, -4, 10, -17, 4, 0, 8, 14, 8, 19}
, {-9, -1, -21, -18, -14, -5, -15, 0, -6, 12, 3, 4, 6, 5, 4, -9, 2, -1, -6, -6, 9, 15, 2, 9, 4, -1, -2, 11, 0, 7, 4, 5}
}
, {{-8, -18, 2, -21, 3, 7, 1, -1, -10, 19, 16, -28, 27, 15, 20, -5, 2, 0, 19, 16, 10, -16, 1, -8, 8, 12, 7, -7, -3, -3, 19, 14}
, {8, -11, -6, -34, -17, 9, -25, -27, 2, -14, -11, 2, 19, 10, -1, 22, 25, -9, 7, -2, -10, 16, -7, 0, -11, -8, 6, -11, 8, -38, 7, 12}
, {2, -18, -3, -5, -6, -10, 3, 4, -20, -14, -11, 15, 7, 8, -13, 7, 4, 7, -12, -13, -13, 15, -13, -4, -4, 1, -8, -8, 7, -12, 12, -15}
}
, {{-27, -28, 17, -8, -13, -14, 6, -3, 19, 3, -19, 19, 15, 16, 5, 15, -5, 6, 4, 14, 2, -9, -19, -10, -9, 11, 19, -8, -12, -16, -11, -21}
, {-9, 7, -11, -11, -22, 4, -13, -20, -11, -17, -14, 11, -18, 15, -21, 8, -21, -9, 2, -14, -14, 18, -12, -6, -5, 5, 5, -19, 10, -20, 0, -11}
, {6, 10, -28, -7, -19, 4, -24, 9, -16, 20, -5, 2, -10, 10, 12, 4, 0, -22, -6, 8, -17, -2, 2, 18, 12, 3, 2, -11, 6, -12, -8, -11}
}
}
, {{{-3, -8, 4, -3, -5, 3, -1, -9, -3, -2, 6, -1, 6, 5, -17, -5, -2, 4, -6, 11, 2, 10, 6, -9, 18, 26, 10, -7, 14, -8, 9, -6}
, {15, 5, -9, 16, -9, 15, 1, 5, -24, 12, 14, 3, 10, 2, 9, -20, 1, -2, 21, 1, 1, -1, -9, 10, -13, 13, 5, -21, -10, 1, 7, -6}
, {-6, 12, 2, 13, 14, 9, 0, 3, 9, -11, 7, 7, -3, -6, -1, 10, -7, -4, 3, -8, 14, -12, 8, -16, 15, -10, -20, -5, -18, 3, 8, 13}
}
, {{-5, -1, -4, -15, 1, 4, -9, 12, -12, -2, 13, -1, 4, 3, -1, -30, 9, 5, 15, -1, 1, 12, -21, -4, -2, 17, 0, -14, 21, 18, -2, -18}
, {-37, -12, 11, 5, 13, -2, -3, 30, 15, -14, -11, 8, -5, 4, -17, 16, 1, -26, -35, -18, -19, -20, -19, -12, 12, 9, -25, -7, -5, 10, 8, 2}
, {-12, -30, -18, 11, -33, -37, 9, -15, 3, 1, -30, 10, -13, 8, 15, 0, -2, -15, 12, 12, 8, -22, -28, 20, -6, -15, -10, -16, -13, -10, 3, -32}
}
, {{-8, 17, -7, -24, 8, 11, 15, -4, 9, 1, 16, -22, 9, 4, 10, 3, 7, -10, 10, 8, 3, -11, -17, -1, 7, 9, -14, -23, 8, -12, -3, 12}
, {-4, 9, 11, -50, 11, 10, 11, 3, -2, -3, 23, -2, 0, 6, -2, -8, -9, 23, 8, -2, -15, -16, -6, -4, 19, 16, 3, -12, 12, -12, 1, -4}
, {14, 12, 24, -29, 14, 2, 5, 7, 9, -4, 7, -13, -7, 4, -9, -2, -18, -4, -4, -27, -1, -4, 13, 2, -1, -11, 1, 9, 8, 0, 10, 5}
}
}
, {{{5, -10, -20, -12, -2, 16, -10, -2, -9, 4, 1, 4, -6, -17, -7, -9, 1, 8, 1, 2, 9, -3, 11, -4, 1, 6, 1, -5, -8, -18, -13, 6}
, {-2, -10, 11, 14, 7, 13, -13, 15, -12, 4, 2, 6, -6, -21, -32, 0, -2, 8, -13, -23, -5, -16, -18, -31, -8, 7, -36, -1, -10, 9, -5, -3}
, {8, 3, -4, -10, 0, 5, -18, -6, 3, -9, 1, -20, -7, -2, -2, 3, -19, 0, -9, -6, -14, -16, -7, -21, -17, -2, -9, 2, -17, 0, 16, -8}
}
, {{-4, 3, -30, -11, -13, 8, -14, -26, 3, -19, -2, 8, 14, 6, 5, -16, -7, 10, 13, 4, 7, 1, 4, -11, -1, 11, -25, -6, 3, -40, 12, 14}
, {-2, -5, 20, -10, 19, 24, -1, -7, -32, -23, 12, -2, -11, 4, -33, -28, -11, -2, 21, -12, 3, -2, -16, -15, -14, 10, -1, -21, 3, -11, 0, -7}
, {-14, -17, -7, 0, -2, -31, -7, 16, -4, 16, -21, -12, -6, -22, -4, 25, -12, -26, 1, -15, -8, -40, -17, -28, -7, 3, -28, -4, -18, 1, -20, -13}
}
, {{10, 1, 2, 2, 5, -9, 1, -1, -15, -4, -18, 6, -14, -7, -18, -4, -6, 3, 17, -2, -18, -3, -2, -24, -20, -3, -19, 10, -5, 11, -1, -32}
, {5, 0, 22, -12, 15, 1, 11, 6, -7, -2, 2, -1, -3, 10, -10, 20, -12, -1, 11, -12, -10, -3, -9, -11, -25, -6, -15, 23, 0, -1, -8, -16}
, {2, 9, -11, -1, -12, 6, -15, -20, -6, -6, -14, -5, -7, 20, 7, -5, -16, 4, 1, 7, -3, -26, -7, 5, -15, 1, -7, -29, -12, -18, 8, 19}
}
}
, {{{-24, -10, -13, 3, -2, 3, 7, -1, 6, 9, -15, 1, 1, -6, -6, 1, 5, -8, -11, 1, 1, -3, 2, -12, 0, 0, 7, 4, -13, 0, -11, 11}
, {-22, -14, 4, 13, -13, -3, -2, 1, 0, 17, -9, 21, -11, -17, -8, -11, 8, -17, -16, -8, -14, 0, -14, -12, -4, 7, -8, 5, 1, 1, -18, -13}
, {-1, -2, 6, 24, -14, -14, 5, 8, -5, 2, -16, 6, -13, -8, 0, 12, 4, -12, -13, 5, -12, 1, -6, 2, -17, -20, 1, 18, 1, 2, -4, 1}
}
, {{2, 5, -38, -41, -11, 9, -14, -3, -9, -9, 13, -20, -12, 12, 4, -26, -8, 4, -10, -18, -12, -32, -8, -6, -7, 15, -6, -44, 1, 8, 9, 0}
, {-12, 11, 19, -20, 12, -13, 6, 20, -11, -24, 19, -25, 5, 25, -63, -24, 21, -22, -30, -53, -10, 5, 8, -17, 14, 18, -1, -20, -7, 12, -12, -3}
, {3, -21, 5, -1, -11, 1, -1, 5, 0, 5, -17, 2, 2, -5, 2, 2, -1, 1, 4, -2, -25, -24, -24, 0, -17, 7, -10, -10, 10, -3, 12, -5}
}
, {{-1, -5, -6, 2, 0, -8, -9, -14, -13, -19, 0, -12, -14, 15, 6, -1, 24, 0, 4, 10, 9, 23, 14, -9, 7, -4, 7, 2, 7, 1, 11, -21}
, {15, -10, -13, -6, -23, 3, 11, 9, -11, -27, 10, -5, 1, 23, 14, 7, -1, -7, -4, 0, 18, 2, -13, -7, -2, 12, 10, -14, -16, -12, 3, -12}
, {13, 9, 12, -16, -12, 3, 7, -27, -5, -16, 10, -1, -6, 19, -4, -30, 3, 14, 2, 4, 4, -1, 6, -4, 0, 28, 6, -5, 12, -25, 15, -4}
}
}
, {{{-9, 2, -14, 16, -11, -3, 3, -2, 4, -1, -14, 12, -7, -9, -12, 8, 1, 4, -6, -16, -13, -10, -5, -6, -13, -6, -13, -6, -22, 7, 3, -13}
, {-9, -11, -2, -12, -1, -5, 1, 14, 1, 8, -14, 11, -10, -10, -4, 5, -2, -18, -26, -4, 2, -20, -6, 8, 11, -9, -55, -10, -22, -6, 6, 11}
, {5, -10, -16, -9, 4, -1, -4, -6, -16, -31, 8, 2, -11, -12, 0, -19, 2, 13, 4, -13, 12, 12, 15, 3, -7, 7, 9, -4, 30, -13, 19, -1}
}
, {{-12, 6, -10, 1, 0, 6, 18, 18, -5, 7, -1, -10, -16, -11, -21, 22, -9, -21, -16, -27, -11, -24, -26, -19, -3, -6, -48, 3, -40, -4, -13, 1}
, {2, 5, 21, -5, 31, -2, 3, 21, -24, 2, 24, 6, 8, -12, -19, -16, 0, -6, -1, -27, 1, 0, 4, -13, 9, 19, -33, -4, 19, 6, 10, -3}
, {-16, 7, -29, -2, -1, -6, -18, -5, -8, -9, 10, 15, 5, -20, 3, 1, -2, -21, 2, 1, -7, -25, -23, -17, 11, 3, -8, -17, 13, 5, -8, -19}
}
, {{2, 2, 1, -11, 9, 6, -2, 12, -20, 16, 20, -13, 0, 3, -4, -13, -1, 5, -21, -11, -5, -7, -10, 7, 4, 5, 3, -4, -3, -9, 3, 19}
, {-14, -27, -4, 5, 6, -8, 12, 0, -23, -8, -5, 15, 17, -10, -7, -12, 18, -7, -23, -11, -1, 13, -7, -5, -1, 25, 7, -8, 27, 13, 5, -8}
, {6, -13, -13, 0, -7, -6, 4, -7, 5, 11, 14, -12, 18, -3, 7, 13, -5, -6, 18, 0, -9, -4, 4, -5, 1, 10, -6, -21, 4, 2, 2, -10}
}
}
, {{{-22, -13, -6, -15, -11, -22, -16, 4, 2, -2, -40, 2, 8, -23, -10, -12, 7, -8, 0, -3, -5, -24, -12, -38, -2, -24, -18, 12, -8, -1, -3, 3}
, {-8, 3, -1, -12, 3, 1, 1, -1, -8, 12, 5, -5, 10, -29, -18, -2, -21, 4, 6, -7, -14, 6, -10, -44, 6, -1, -13, 1, -1, -3, -5, -22}
, {-6, -3, 9, -5, 5, -1, 14, -7, 11, -8, -8, -25, 8, -17, -28, 7, -17, -23, 0, -21, -26, -19, -7, -27, -17, -11, 3, 0, -4, 12, -3, -5}
}
, {{8, -3, 2, -39, 3, 4, 14, 4, -1, 4, 22, -20, 4, 6, -19, -20, -6, 1, 12, -19, -12, -27, -1, -16, -3, 20, 0, -18, 7, -7, 16, 4}
, {7, 4, 3, -53, 1, 3, -12, -22, -12, 16, -4, -24, -8, -9, -9, 7, -12, -21, 19, -11, -16, -4, -18, 0, 7, 0, -1, 0, 14, -25, 11, 3}
, {-13, -4, 1, -21, -3, -1, 3, -9, -1, -1, 4, -9, -2, 7, -14, 4, 14, 3, -8, -7, -16, -6, -2, -8, -5, -1, 23, -9, -6, 9, -17, 12}
}
, {{21, -5, -10, -15, -21, -3, 8, -5, 2, -1, 5, -3, 11, 6, -7, 1, -11, 16, -7, 8, 4, -7, 4, 12, 1, 7, 17, -8, 4, -23, 5, 5}
, {6, 4, 10, -6, -9, -21, -1, -4, -7, 4, 0, 10, -26, -20, -8, -12, -12, 17, 5, -10, -6, 25, 20, 14, 4, -14, -24, 32, 18, 7, 2, -9}
, {2, 2, 13, 4, -31, -13, 12, -11, 6, -1, -22, 13, -11, 0, 0, 19, -12, 5, -25, 23, -4, -14, 2, 3, 8, -10, 3, 12, -18, -6, -6, -6}
}
}
, {{{1, 7, -4, -17, 10, 15, -23, -5, -26, 3, 6, 2, -5, -15, -3, -9, -23, 3, 10, 5, 8, 1, -7, 11, -18, 11, -2, -10, -4, -7, 10, 11}
, {7, 7, -1, 13, -10, -4, -10, -9, 14, -8, -9, 0, 8, 3, -17, -9, 12, -11, -23, -23, -12, 11, 4, 15, 5, 0, 4, 14, -7, 10, -8, -1}
, {13, -3, -11, 12, -14, -9, 8, 1, 5, 8, -6, -10, -19, 2, -1, -3, -15, 7, 1, 17, 13, -21, -3, 5, 0, -9, -2, 1, -17, -19, -13, -6}
}
, {{-13, -11, -6, 3, -13, -42, -36, -28, -2, -13, -9, 8, -17, -15, -3, 4, 12, -16, -2, -9, 7, 6, 7, 15, 17, -22, -3, -2, -18, -2, -29, -8}
, {3, -8, 3, -16, -2, -9, -40, 12, -7, 4, -4, -1, -33, 10, -14, -1, -20, -14, -19, -13, -3, -20, -5, 17, 6, -20, 3, -1, -31, 4, 9, -20}
, {14, -3, -5, -8, 12, 0, -32, -16, -6, 12, 6, -18, 3, 5, 6, -7, 6, -4, 7, 9, 29, 11, 13, -8, 5, -1, -10, 9, -14, -15, 13, -2}
}
, {{-22, 0, 9, 4, -5, -6, -32, 10, -23, 1, -21, -6, -15, -1, -11, 8, -11, -16, -17, -11, -7, -22, -14, 2, -2, 0, -33, 5, -14, -1, -2, -2}
, {5, 2, -37, 5, -22, 4, -32, -19, -5, 15, -14, 19, -15, -9, 4, -14, -5, 6, 11, 2, 4, 2, -4, 7, -11, 2, -12, -9, -6, -8, -6, 1}
, {-26, -20, 1, 15, 2, -18, -22, -4, -10, 8, -26, 7, -32, -1, 14, 12, 7, -21, -15, 8, -1, -8, -11, 8, -2, -6, -5, 10, -33, -2, -27, -20}
}
}
, {{{-9, 16, 6, 2, -13, -14, -16, 2, 1, 2, 12, 0, 12, 7, -4, -7, 24, -1, -22, -7, 3, 6, 8, 6, 13, -5, -11, 2, -5, -4, -2, 8}
, {8, 18, 14, -3, 10, -1, -20, 5, 1, -11, 0, -3, -20, 8, -22, 11, -6, -10, 2, -10, 22, -13, 10, -14, 6, -11, -18, 22, 10, 22, 1, -9}
, {-11, -12, 5, 14, -3, -24, -4, 4, 0, 22, -15, 6, 11, -18, 14, -8, 17, 0, 11, 28, 29, -14, 4, -6, -17, -7, 20, -8, 9, 5, 9, -23}
}
, {{-2, -5, -9, -3, -13, -20, 2, -17, 11, -12, 8, -5, -9, 14, -17, 9, 2, 14, -20, 14, 7, -6, 6, 10, 16, 8, 6, 2, -6, -28, 2, -2}
, {8, 1, -13, -12, -31, 16, 4, -16, 14, -9, 3, -4, -18, 16, -4, 8, 4, -1, -1, 2, 9, -33, 1, 21, 19, -3, 5, 2, -14, -8, -3, 9}
, {9, -4, -22, 10, -11, 22, -16, -31, -11, 13, 7, 5, 2, -5, 8, -16, -6, 8, 13, 9, -11, -22, -19, 19, 12, 9, 10, 17, -14, -20, -7, 4}
}
, {{-10, -3, 7, 2, 9, 2, 5, -4, -24, -4, -12, 5, 0, -6, -15, -11, 9, 6, -13, 6, 5, 15, -15, -41, -15, 17, 0, -15, 4, -19, 11, -13}
, {7, 8, -11, -29, -1, 11, -10, -30, 5, 5, -8, -9, 0, -3, 0, -11, -5, -13, 8, 1, 1, -6, 9, -26, 5, -12, -2, -21, 3, -4, 7, 20}
, {-20, -1, 5, -10, 3, 4, -1, 9, 5, 7, -16, 1, -6, 11, -22, 22, -16, -42, -19, -14, -21, -28, -17, -14, 36, -18, -6, 5, -15, 13, 3, 5}
}
}
, {{{3, -7, -13, -29, -14, 3, -1, -7, -2, 19, -1, -2, -3, 15, 5, -15, 12, -6, 2, 14, 8, 13, 1, 8, 11, -13, 3, 1, 4, -10, 11, 9}
, {-1, -9, 2, 0, -5, 7, -17, -30, -14, -29, -5, -5, 12, 20, -10, -15, 2, 13, 10, 9, 4, 20, 2, 4, -2, -2, 4, 3, 6, -10, 21, -4}
, {2, -4, -4, 3, -6, 11, -5, -4, 6, -5, 1, -16, 4, -6, -3, -1, 6, 6, -7, -6, -13, 2, -13, 18, 4, 3, 3, 13, 8, 14, -2, -8}
}
, {{-21, -14, 3, 7, 10, -5, -8, -7, 11, -17, -12, -3, -10, 4, -17, -12, -3, -12, 17, -5, 12, -1, -2, -10, 8, -20, 6, -2, 4, -7, -4, 1}
, {-10, 1, -30, -16, -12, 13, -16, -30, 11, 0, -29, 0, 18, 10, -18, 22, 19, -33, 13, -4, -47, -1, -30, 2, 6, -1, 13, -3, 1, -18, 11, -2}
, {4, 14, 8, 4, 5, 18, -13, -5, -4, -6, -16, 1, 6, -10, -1, 3, 29, 0, -21, -25, -28, -2, -3, 17, 8, -9, 0, 8, 1, 0, 11, 2}
}
, {{-7, -11, 5, -6, -3, 21, -11, 12, 7, 19, 4, -14, 17, 4, 1, 6, -1, 3, -6, -3, 6, -6, -12, 6, -3, -5, -5, 20, 1, 1, -9, -6}
, {-6, -8, 8, -7, -8, 0, -17, -32, -10, -10, -24, 18, -10, -5, -31, 12, -11, 11, -26, 9, -18, 21, -14, -10, 11, 25, -30, 3, 5, -17, 17, -13}
, {3, 5, 2, 9, 3, 6, -3, 12, -28, -4, -26, -5, -12, -21, -9, -1, -20, -12, -14, -29, -16, 4, -4, 2, 13, -24, -31, 18, -6, 23, -1, -14}
}
}
, {{{-11, -6, -9, -27, -4, 4, -11, 4, -20, 4, -4, 14, -19, 7, -16, -6, 2, -13, -1, -19, -4, -27, 3, -17, 9, 2, -18, -23, -16, -27, -1, 19}
, {22, 5, -24, -13, -16, 1, -17, -12, 5, -4, -2, 7, -29, 23, -7, -18, -10, 4, -13, 4, 18, -17, 19, 10, -2, -7, 0, -37, -32, -14, 19, 4}
, {-17, 0, 5, 13, -10, -11, 0, 2, -1, -20, 4, 8, -9, 15, -22, 1, 14, 11, -8, -21, -12, 4, -3, 1, 3, -4, -1, 11, -26, 15, 14, -17}
}
, {{-14, -14, 3, 9, -6, -18, -21, 1, -19, 8, 14, -4, -24, 1, 5, -5, 21, 10, -10, -7, 23, 2, 5, -5, 14, -13, -15, 5, 6, 12, -3, -7}
, {-6, -3, 4, 16, 5, -4, -4, -19, 6, -12, 5, 5, 2, 21, 12, -24, -8, 11, -20, 27, 23, 18, 14, 13, -24, -36, -20, -18, -1, 16, 10, -14}
, {-2, 2, 14, 17, 7, 5, -15, 25, -9, 25, -3, 5, 13, 7, -12, 12, -2, -19, -13, 10, 10, -7, -28, 14, -22, -4, -11, 2, -27, 11, 2, -13}
}
, {{-1, 8, 6, 0, 19, 5, -3, 2, 8, 7, 22, -3, 4, 5, -13, -1, -14, -15, -15, -4, 9, -9, -2, 10, 2, 5, 3, 3, 4, 0, -14, 7}
, {2, -1, 2, 9, 20, 13, -9, -8, -2, -13, 14, -5, 4, -13, 1, 1, -6, 16, -13, 2, 2, 3, 19, 6, -11, -11, 0, -8, 6, 15, 10, -12}
, {8, -10, 7, 8, 0, 2, -4, 3, -9, -8, -8, 10, 26, -21, -11, 5, 8, 3, -7, 10, -20, 7, -8, 3, -16, 1, -4, 4, -16, 3, -1, 13}
}
}
, {{{5, -19, -33, 13, -26, 0, -22, -14, -12, -10, 10, -3, -42, -5, 8, -20, 8, 22, 26, 6, 5, -8, -3, 4, 3, 4, -7, 2, 15, -28, 13, -6}
, {6, -16, 2, 0, -7, -4, -3, -20, -16, -22, 2, 2, -20, -11, 3, -2, -5, 24, 6, 17, -1, 3, -4, 1, 16, 14, 15, -8, 8, -26, -2, 5}
, {5, 12, 14, 0, 13, -2, -5, -3, 9, -16, -7, -12, -11, -2, -19, -5, -27, 4, 3, -12, -16, 11, 17, -11, -3, -24, -18, 25, -4, 15, -14, -12}
}
, {{-14, -18, 6, -24, -5, -6, -24, 6, 5, 14, 4, 6, -13, -8, -9, 9, -13, -11, -26, -14, 12, -17, 1, -35, 3, 0, -13, 6, -8, 15, 8, -10}
, {-24, 5, 2, -18, -6, -5, -10, 6, 11, 20, 8, -32, 14, -10, 4, 18, -22, -19, -1, -13, -5, -30, -20, -43, 4, -19, 9, -3, -5, -5, -11, 4}
, {-13, -6, 26, -12, -6, 16, -6, -6, 23, -6, 15, -21, 0, 10, 9, 25, 0, 2, -10, 3, -14, 10, 10, 23, 20, -10, 9, -5, -8, 7, 16, 17}
}
, {{6, -13, -2, 2, -16, 4, -2, -5, -16, -25, -1, 7, -2, 6, 5, 2, 12, 24, -14, -1, 7, 9, 26, -3, -4, 11, 4, -6, -16, -17, 17, -4}
, {-9, -17, 2, 5, 5, -3, 3, -19, -9, -3, -5, -4, -5, 14, -1, 8, 0, 0, -2, -7, 9, -2, 6, 8, -2, -7, 0, -11, -5, 2, 3, 6}
, {-6, -24, -16, 8, -16, -17, 10, -11, -13, -31, -14, 0, 8, 20, -12, -6, 16, 2, -18, 12, -9, -9, 13, -5, -2, 12, -4, 4, -7, -6, 26, 14}
}
}
, {{{5, 9, -18, -12, 0, 8, -2, 3, -3, 4, 14, -17, -1, 4, -10, -5, -12, 4, -4, 3, 0, -17, -18, -4, 11, 0, 1, 3, -12, -1, -6, -5}
, {-16, 3, -20, -16, -7, -12, -8, -17, -14, -22, 17, -3, 0, 4, 8, -28, 8, 5, -8, -1, 12, 21, 11, 1, -2, -1, 0, -14, -5, -12, -11, 15}
, {-10, -10, -6, -9, -18, 19, 3, 1, -2, 14, 10, -14, 6, 6, -5, -5, 10, 13, 3, 6, 8, 2, 15, 2, -3, -8, 16, -18, -10, -10, 7, 12}
}
, {{-10, -7, -13, -30, -8, -5, -34, -10, -14, -2, 9, -12, -34, 1, -3, -10, 4, 4, 1, -8, -3, -7, 18, -6, -3, 15, -3, -16, -22, -1, -16, 20}
, {-7, -7, -18, -30, -8, 12, -19, 0, -7, 1, 6, -26, -3, -5, -24, -11, -5, -14, -11, -8, 0, -21, -16, -10, 9, -9, 2, -30, -9, -1, 12, -13}
, {-9, -2, -8, -33, 2, -5, -6, 6, -20, -6, -1, 0, 10, -8, -11, -5, -7, -1, 6, -14, -16, -3, -8, -5, -3, -2, 1, -38, -3, 11, 9, 11}
}
, {{-11, -14, -1, -8, -11, 9, -14, 7, -12, -8, 8, 0, -22, 14, -8, -10, -1, -18, -12, -18, 6, -6, 14, -22, 14, 12, -16, -9, 3, 1, -16, 11}
, {-40, -28, 10, -18, 1, -8, -22, -6, -26, -18, 2, 3, -19, -1, -25, -11, -17, -12, -19, -3, 0, -27, 9, -25, 9, 9, 3, -5, 7, -21, -19, 7}
, {-5, -18, -3, 14, -36, -23, -3, 10, -20, -24, -17, 18, -27, 15, -15, 3, 5, 3, -31, -1, -13, 3, 2, -3, 10, 13, -27, 5, -24, -9, 5, -6}
}
}
, {{{5, -14, -18, 7, -7, -4, -6, 2, 10, -2, -12, 10, 0, 7, 9, 14, 23, 1, 9, 6, 15, -2, -4, 3, 10, 0, -3, -4, -9, 3, 7, 3}
, {-19, -16, 1, 11, 8, -10, -17, 1, -2, -11, -18, 21, -8, 0, -6, 2, 20, 9, -10, -9, 18, 12, 7, -17, 2, -11, 4, 7, 3, 20, 0, -16}
, {5, 0, 2, 11, 18, 7, 2, 6, 11, -1, -5, -5, 3, 2, -9, -9, -6, 0, 0, 2, 18, 16, -3, 16, -18, -2, 19, -1, 1, 8, 14, -14}
}
, {{-8, 9, -9, -31, -12, -5, -25, -10, -24, -1, 7, -22, 0, 6, 9, -27, 3, 5, 28, -12, -1, 3, 7, -5, -12, -14, -5, -23, -23, 11, 12, -2}
, {-10, 11, -3, -47, 9, 25, -17, -1, -17, -8, 5, -2, 30, 23, -36, -19, 2, -12, -16, -26, -7, 22, -10, -11, -12, 7, 1, -35, -11, -12, 23, 17}
, {-4, 13, -3, -21, 2, 14, -6, 0, -17, -9, 4, -9, -6, -3, 3, -1, -13, -12, -1, -6, -24, 1, 6, 4, 8, 15, -2, 11, 4, 4, 5, 17}
}
, {{-29, -11, -7, -13, -15, -1, -20, -25, 1, -23, -20, 1, -7, 21, -15, 7, 6, -29, -10, 9, -13, 3, -10, -10, -20, 0, -3, -7, 21, -23, 1, 8}
, {-12, -9, -17, -10, -17, -15, -16, -20, -11, -3, -14, 16, -26, 9, -5, -5, -4, -15, 1, -21, -15, 8, -25, 7, -13, 3, -11, -15, -19, -4, -15, -42}
, {-8, 12, -9, -3, 11, 5, -4, 4, -9, -3, -3, -6, -13, 3, -16, 5, -12, -8, 14, -16, -15, 0, -8, -2, -5, -11, -15, -14, -7, 19, 2, 5}
}
}
, {{{8, 8, 5, 3, 4, -2, 10, -12, 4, -5, -17, 2, 7, -2, 5, 21, -7, -7, 8, 2, 0, 3, 9, 8, 6, -10, 10, 14, 15, 2, 7, -2}
, {19, -6, 18, 13, -2, 4, -1, 6, -1, 14, -15, 6, 0, -7, -25, 0, -13, 9, -5, 5, -5, 8, 2, 2, -9, -10, 2, 20, 1, 2, 15, -10}
, {1, -5, -2, 1, 8, 7, 1, -13, 0, -9, -9, -1, -11, -1, -2, 13, -14, -7, 6, 14, 7, 0, -2, 9, -9, -2, 7, 0, 2, -6, 0, -3}
}
, {{2, 15, 1, -16, -6, -23, 3, -11, -13, -22, -10, -7, -9, -23, -12, -9, 1, -22, -20, -18, -7, 7, 8, 2, -10, -7, -22, 4, 24, 6, -2, 11}
, {11, -4, -33, 0, 1, -14, 2, 20, -14, 6, -3, -5, -3, 7, 1, -14, 12, -18, -4, -9, -12, -19, -1, 21, 6, -13, 9, -4, 13, 5, -5, -6}
, {-16, -14, -14, -27, -5, -38, 2, -7, 0, 8, 10, -6, -18, -8, -12, -14, -4, 6, 20, -8, 16, -10, -10, -3, 9, -7, -1, 7, 21, -4, 7, -25}
}
, {{-1, 16, -6, -33, -5, -6, 8, -1, -15, -15, 4, -4, -10, 22, -13, -9, 11, -19, -5, -15, 0, -7, 21, -7, 33, 7, -22, -26, 1, -15, -20, 17}
, {-5, -4, -9, -6, -9, -19, 15, 11, -15, 13, 13, -13, -37, 29, -22, -34, 35, -9, 1, -12, 11, -8, 14, -15, 23, 1, -8, -5, -10, -15, -2, -1}
, {-2, -5, -24, -2, -15, 3, 19, -12, -17, 12, 15, -6, 5, 14, 5, -10, 26, -9, 12, 8, 22, -5, -12, -5, 7, -12, -26, 4, 6, -26, 20, -9}
}
}
, {{{-16, -35, 0, 16, -32, -29, -7, 1, 5, -5, -11, 9, -22, 4, 8, -11, 18, 3, 3, 10, 18, -5, 13, -12, 26, -9, -9, 14, 2, 7, 13, -29}
, {16, -9, 3, -11, -24, -16, -2, 10, -14, 2, 7, -1, 14, -4, -10, -24, 14, 14, 2, 16, 2, 4, -10, -20, 2, -8, -5, -14, 16, 0, 7, -16}
, {12, 8, 1, -32, 17, 9, -2, 1, 14, 2, -1, -18, -1, 18, -21, 11, -31, -8, 1, -3, -10, -5, -12, 16, -7, -14, 26, -5, -18, -2, 15, 13}
}
, {{6, -2, -3, -30, 19, 18, -15, -7, 8, 1, 1, -15, 17, 5, -8, 7, -3, 3, 13, 11, 2, -7, 7, 5, 18, 7, 9, -14, 12, -5, 25, 5}
, {5, 13, -15, -16, -10, 8, -10, -30, -15, -13, -2, 6, -19, -2, 5, -12, 11, 15, 2, 2, 6, 0, 7, 34, 8, 13, 2, -6, 6, -17, 13, 16}
, {-30, -43, 13, 2, -6, -9, 2, -7, -14, -29, -14, 0, -37, -10, -3, -10, 17, 0, 3, 1, 18, -6, -9, 4, 13, -3, -20, 8, 15, -2, -1, -23}
}
, {{-26, -17, -10, 1, 4, -25, -5, -1, -19, 0, -2, 20, -15, -7, -11, -7, 8, 13, 0, -3, -9, -5, 12, -17, -8, 7, -10, -6, 1, 5, -1, 4}
, {0, -1, -4, -17, 3, 0, -2, -2, -2, 28, -7, 11, 4, -20, -10, -8, 7, -5, 5, -15, 10, 0, -2, -23, 14, 2, -23, 1, -3, 9, -12, 1}
, {-12, -14, 6, 3, 5, 8, -3, 15, -1, 4, 3, -18, 13, 1, -4, 7, 12, -6, -18, -15, -14, -9, -19, -10, -21, 15, -12, -13, -21, -1, -4, -1}
}
}
, {{{6, -17, -17, -5, -14, -4, -3, -7, -16, -2, 3, -13, -27, 9, 8, -10, 10, 8, 2, 0, 18, -1, 11, -15, 9, -12, -6, -13, -16, -9, 0, 7}
, {-7, 2, 5, -8, 2, 6, -3, 2, -2, -4, 22, -37, 2, 2, -6, -13, -13, -7, 11, -4, 14, -5, 5, -14, 13, -6, 3, 5, 1, -9, -4, 10}
, {-4, -8, -25, -7, -1, 8, -13, 1, 7, 5, -5, -10, 4, 12, -10, -1, -6, -5, -22, -6, -9, -2, -19, -1, -6, -4, -11, -15, 7, -2, 14, 7}
}
, {{-10, -14, -1, 12, 12, 0, -1, 12, -19, 10, -16, -9, -24, -15, -11, -1, -2, -16, 0, -1, 0, 5, -17, -13, -26, -1, -17, -3, -17, 17, -5, -14}
, {-42, -48, 21, -13, -11, -32, 8, -7, 22, -8, -15, -12, 0, 5, 14, 35, -7, -29, -8, 4, 3, -15, -15, -5, -22, -5, -4, 18, -23, -11, 5, -17}
, {8, -5, -13, -1, -14, -1, -1, -21, -26, -17, 6, 11, -2, 12, -12, -13, -16, 18, 11, -8, -23, -8, -11, 14, 1, 0, -5, -23, -12, 16, -3, -25}
}
, {{0, 18, -1, -27, 5, 3, -1, -2, -25, 2, 13, -42, 2, 26, 5, -15, -16, -6, 4, 2, 7, -4, -4, 9, 1, -14, 3, -13, -12, -12, 15, 19}
, {-1, 12, 7, -21, 7, 3, -18, 6, -35, -20, 28, 6, -16, 22, -17, -11, 2, 17, 9, -18, -10, 2, 10, -6, 0, 1, -17, 6, 13, 8, 1, 17}
, {11, -13, -12, -9, -29, -6, -1, -28, 0, 0, 0, -3, -6, 19, -2, 6, 7, -12, 7, 5, -13, -18, 7, 8, 23, -3, -4, 0, -1, -12, -1, 1}
}
}
, {{{5, -12, 9, 15, 1, 11, 5, 6, 10, -6, -4, -4, 0, 10, 3, -9, -1, 10, 11, 12, 1, -6, 3, -5, -16, -15, 10, 8, -2, 6, 9, -4}
, {2, -6, -4, 16, 13, -12, 0, -14, 19, -6, -12, 8, 9, 1, 0, 21, -7, -7, 1, 6, 5, 9, 4, 5, -10, -26, 1, 15, -19, 20, -7, -12}
, {-11, -6, 23, 4, 9, -3, 4, -22, 21, -20, -16, -2, 21, -4, -6, 24, -16, -16, 3, -3, -25, 6, 2, -16, -21, -4, -6, -5, -12, 0, -10, -1}
}
, {{-21, -31, 6, 15, 9, -25, 3, -14, 16, -7, -29, 27, 6, -1, 9, 24, 1, -4, -6, 16, 0, 4, -4, 6, -16, -9, 10, -3, 4, -2, -10, -11}
, {12, 2, 9, 3, -6, 6, 7, 0, 15, 15, -17, -8, 7, -14, 2, 23, -37, 7, 3, 13, -28, 5, -14, -4, 1, 1, 2, 4, -9, -19, 2, -7}
, {-12, -1, 28, -9, 3, -5, -12, -13, -12, -15, -15, -15, 6, 17, 1, -4, -17, -18, -21, -4, -21, 2, -13, 1, -24, -11, 11, 9, 3, 5, 5, 3}
}
, {{9, 20, -4, -24, 8, 1, 9, 11, 23, 17, 12, -22, 1, -10, -3, 8, 5, -4, 6, -1, -40, -3, -14, 4, 19, 2, 4, 2, -3, 6, -4, 8}
, {-9, 4, 11, -41, 10, -18, 2, 9, 2, 8, -13, -6, 7, -12, -13, 5, -1, -26, -13, -30, -33, 3, -9, -5, 20, 3, 18, 10, 6, -25, 2, 18}
, {-4, 3, 3, -29, -22, -13, -9, -17, -7, 9, -17, 1, 18, 6, -12, 13, -9, -9, 10, 1, -12, 1, -2, 4, -15, 16, 9, -8, 9, -33, -16, -3}
}
}
, {{{-9, -10, 3, 14, -20, -16, 7, -6, -5, -7, -25, 6, -28, -5, -3, -12, -3, -1, -5, 5, 5, 10, 14, -3, 0, -2, -7, 17, -2, -5, -10, 16}
, {-11, 0, 2, 11, -5, -6, 20, 5, 11, 10, 4, 6, -12, -4, -11, 12, -14, 10, 5, -3, -4, -13, -4, -13, 0, 0, 14, 15, -16, 8, -23, -15}
, {16, 9, -11, 3, 4, 0, -8, -1, 0, 7, 0, 2, 7, -7, 6, 6, 0, -9, -9, 9, -12, -14, -6, -11, 4, 6, -4, -3, -5, -7, -4, 19}
}
, {{2, 7, 9, 1, 9, 5, 11, 6, -39, 14, -17, 3, 1, -10, -19, -27, -10, 9, 6, -13, -7, -3, 0, -9, 3, -5, -43, -3, -33, 15, -7, -17}
, {-12, -4, -9, -16, -8, 6, 2, 13, -2, -5, -23, 41, -1, -3, 1, 3, -26, -10, -34, -18, 13, -38, -3, -6, 15, 4, -35, -10, -28, -3, 25, 26}
, {-10, -14, -12, 2, -14, -7, 0, -14, -9, -8, -3, 23, -7, 9, -16, -3, -12, 3, -1, -20, -12, -18, -1, 3, 2, 6, -13, -12, -4, -7, 17, -1}
}
, {{12, -3, 1, 1, 5, 4, 16, 7, 9, 2, -4, -2, 8, -20, -2, 13, 2, 0, -1, 4, -12, 8, -8, 12, 21, -5, -30, -11, 9, -1, -19, 3}
, {0, -20, -15, 3, 1, -30, 12, 0, -36, 1, 3, -3, -36, -36, 1, -30, 21, 11, 28, -1, -1, -6, 3, -11, 2, 5, -25, 14, 9, -5, -5, -34}
, {-1, -12, 1, -3, 20, 8, -19, -4, -14, 1, -4, 1, 13, 3, -3, -9, 10, 0, 0, 2, 7, -32, -19, -15, -12, -10, -1, 0, 8, 3, 18, -3}
}
}
, {{{-27, -2, -14, -1, -16, 19, -18, -14, 5, -15, -24, 4, 14, -14, -2, -2, -21, -4, -1, -12, -1, 12, -5, 0, -5, 5, -3, 9, -21, 16, -2, 6}
, {-5, -8, 17, -3, -5, 16, -33, 9, -19, 14, -2, -2, 7, -26, -23, 2, 7, 9, 2, -19, -9, 7, -4, -18, -4, 4, -25, -2, -6, -4, -17, 8}
, {4, 4, -9, -2, 3, -17, -5, 4, 4, -2, 0, 7, -7, 4, 9, -3, -3, 4, -2, -1, 3, 3, 8, 7, -13, -2, 2, 3, 3, -1, 8, 7}
}
, {{-20, 12, 4, -34, 7, 29, -47, 13, -3, 4, 2, -23, 14, -1, -46, 4, 3, -17, -15, -40, -10, 9, -4, -23, 0, -5, -16, -12, -13, 27, 0, 9}
, {1, 4, 3, -14, -3, 15, -25, 20, 0, -6, -10, 17, 12, 5, -50, 14, 11, -28, -33, -43, -15, 17, 2, 5, 1, 10, -24, -24, -5, 0, 11, 11}
, {2, -10, 4, -15, 3, -12, -9, 4, 17, -9, -3, 6, -8, -8, 5, 6, 8, 3, 14, -2, 10, -4, 4, 13, 15, -5, 6, 0, -8, -5, 13, -12}
}
, {{-4, 3, -21, -22, -17, -5, 0, -7, -8, -4, -11, 10, -2, 19, -8, -5, -10, -18, -9, 3, 5, 5, -9, 18, -17, 17, 0, -33, -13, -35, -6, 3}
, {3, 8, -17, -2, -11, 8, -13, -3, -21, -11, -10, 13, -7, 3, -12, -16, 1, -17, -20, -13, 10, 14, 3, 10, -13, -4, -6, -3, -5, -15, 12, -17}
, {13, 3, -8, -10, -7, -3, 8, -11, -1, -6, -16, -2, -14, 4, 11, -21, -1, 4, 16, 3, 5, 26, 11, 0, -6, -9, -2, 10, 22, -12, 10, -4}
}
}
, {{{-25, -17, 18, -7, 6, 3, -9, -6, 16, 2, 4, -4, 8, 7, 8, 10, -2, -23, -15, -3, 2, -15, -10, -12, 0, -10, 4, -7, -3, 0, 1, 13}
, {-11, -4, 11, 22, 18, 8, -9, 4, 3, -4, 2, 4, 5, 15, -13, 2, -3, -14, -21, -15, -17, 1, -17, 1, -23, 8, 6, 20, 12, 6, -12, 6}
, {-8, -13, -16, 7, -1, -15, -3, -3, 6, 21, -8, 10, -6, -11, 19, -7, -7, 5, 2, 9, -17, -18, -12, 6, -40, -14, 8, -7, 14, -15, -9, -19}
}
, {{-4, -5, -2, 6, -9, 2, 7, -9, 0, 15, -4, 14, -8, 6, -8, 10, -10, 14, -15, -4, 1, 11, -14, 19, -5, 16, 7, -4, -4, -21, -15, 2}
, {8, -23, -20, 11, -15, -5, -16, 2, -3, -5, -35, 8, -29, -26, 10, 1, -23, 4, -29, 6, -26, -21, -5, 27, -15, 0, -10, -7, -38, 7, -23, -27}
, {8, 0, -7, 19, -5, 18, -6, -5, -4, -2, 2, -1, 1, -1, 5, 4, -17, 12, -4, -5, 9, 2, 13, -14, -6, 1, -8, 12, 6, 12, -5, 18}
}
, {{-11, 17, 15, -27, 12, 11, -7, 6, -4, 1, 13, 2, 6, 16, -1, -8, 7, 15, 6, -23, -3, 17, -15, 0, -11, 8, 11, -16, 8, 6, 3, 12}
, {6, 7, 4, -36, 20, 7, -14, 6, -5, -7, 7, -8, 20, 20, 2, -28, 0, -6, 11, -27, -3, 5, 3, -19, -19, -13, -2, -20, -6, 15, 11, 19}
, {8, 0, 2, -17, 21, 7, -20, -1, 5, -9, -4, -4, 27, 18, -8, 13, 5, -29, -18, -10, -13, 13, -6, 7, -4, 3, -4, -17, -4, 2, 19, 8}
}
}
, {{{3, -1, -25, 21, 0, -3, -9, -3, -4, -4, -4, -3, 1, -3, 9, 10, -2, 12, -8, -1, -10, 3, -16, 12, -25, 0, -12, -10, -8, -6, 7, 7}
, {9, 10, 20, 15, 14, 17, 5, -7, 1, -6, 1, -12, 6, -16, -8, 2, -6, -14, -2, -9, 4, -4, 11, -12, 9, -8, 5, -1, 4, 9, -7, 16}
, {-12, -7, -13, -2, -11, -3, 10, -2, -1, 10, 3, 11, 10, 12, 0, 11, -7, -4, 11, 4, -4, -4, -15, -8, 11, 19, 11, -17, -1, -6, 16, 2}
}
, {{-3, 4, -8, -37, -13, 5, -21, -6, -13, -1, -6, -18, 15, -2, -6, 2, -2, -2, -11, -17, -12, -5, -20, -12, -2, 1, -12, -10, 14, -13, 4, 8}
, {-27, -7, -9, -29, -18, -7, -2, -10, -17, -23, 0, 4, 3, 13, -10, -9, 7, -26, -37, -26, -13, 4, -16, 25, -3, 29, 16, -14, 8, -25, -3, 17}
, {10, 9, -2, 12, -2, 12, -4, -8, -13, -2, -5, 14, -8, 1, -15, -11, -12, 12, -2, -29, 1, 4, -1, 6, 4, 9, -12, 12, -20, 1, -17, 5}
}
, {{-14, -6, -27, -9, -21, -8, -24, -16, -31, 21, 0, 5, -9, -1, -13, -18, 1, -14, 7, -1, 1, -11, 1, 10, -9, 10, -10, 8, -2, -38, -1, -9}
, {-21, -21, -11, 8, -18, -13, 1, -13, -8, -21, 8, 28, -26, -9, -22, -13, 13, -1, -14, -14, 18, 16, 7, 6, -17, 22, -32, -8, 9, 0, -7, -19}
, {7, -4, 15, 17, 24, 9, -2, 11, 2, 21, -13, 21, -20, 4, -4, 19, -24, 10, -3, 4, 5, -9, -1, -11, 21, 3, -18, -1, -14, 20, -6, -16}
}
}
, {{{16, -9, -15, -20, -10, 1, -26, -12, -2, 4, 2, -15, -16, 20, -1, -26, 3, 12, 0, 1, 12, -8, 23, -6, 11, -8, -5, 3, 17, -2, 3, -13}
, {11, 3, 1, -2, 3, 10, -3, -3, -8, -2, -19, -2, -12, 6, -2, 1, 0, -3, -3, 10, 5, -1, 5, -4, -19, -34, -6, 7, -10, 4, 5, -15}
, {0, 20, 1, 13, 5, -5, -12, 0, -12, 6, -9, 7, -12, 8, 2, -1, -8, -11, 4, 7, -3, -7, 4, 11, -16, 1, 11, 1, 2, 9, 1, 9}
}
, {{-9, -28, -11, 17, -14, -13, -7, -12, -6, -17, -18, 1, -8, 18, -3, -7, 2, 9, -6, 22, 8, -17, -9, -21, -14, -16, 0, 1, 19, -11, 30, -31}
, {-5, -4, 5, -2, 19, -6, 0, -18, -10, 12, -4, -11, -1, 1, -2, -16, 1, -15, 26, -7, -10, 8, -1, 3, -18, -7, 7, 8, 8, -4, 3, -15}
, {-14, -16, -26, 9, -11, -22, -9, 11, 12, 4, -16, 11, 4, -6, -6, 4, 9, -19, -6, -11, -13, -15, -29, -13, -6, 2, -1, -17, -4, 6, 17, -21}
}
, {{25, 17, 1, -10, 7, 11, -16, 0, -6, 5, 19, -31, 0, 25, 13, -11, -22, 9, -2, 6, -30, -13, 4, 9, -20, -11, 15, -1, -2, 14, 13, -7}
, {3, -1, -5, -11, -9, -20, -13, -19, 13, -14, -11, 6, -11, 29, 16, 12, 8, -12, -15, 8, 14, 13, 32, 19, -4, -8, 16, 13, 13, -15, 21, 10}
, {10, -1, 12, 6, 2, 10, -2, -3, -21, -4, 10, 2, -31, 6, -3, -22, -7, 17, -2, -5, 7, 24, 29, 2, -10, 9, -10, 11, 9, 16, -9, -23}
}
}
, {{{-7, -14, 4, 2, 1, -3, -19, -1, -2, -1, -29, 1, 7, -3, 3, 3, 26, -11, -12, 1, 11, 8, 9, -4, -3, -11, -8, -11, -20, 17, 1, 6}
, {9, 12, -12, -3, -9, 1, -12, -1, 4, -14, 9, -22, 4, 2, -1, -19, -7, -11, 20, 16, 15, 3, 11, -16, -17, -21, 17, -28, -3, -3, 22, -2}
, {3, 5, -12, 8, 8, 5, -19, -16, 4, -11, 9, -15, 10, 0, -7, -7, -6, 3, -16, 14, -8, -2, 2, 15, 6, -12, 10, 5, -5, 7, 2, -14}
}
, {{-3, 3, -20, -7, -18, -12, -20, -20, 5, -12, -33, 7, -1, 10, 4, 5, -11, -8, 12, -5, -2, -2, -12, -1, -1, 22, 4, -11, -1, -13, 3, 2}
, {-19, 0, 3, -8, -2, -37, -12, -26, 20, -8, -20, -33, 13, 2, 5, -1, -19, -32, 7, 8, -4, 3, 14, -9, 25, -15, 23, 12, -12, -4, -17, -22}
, {-21, 1, 5, -15, -10, -2, -6, -13, 10, -2, -5, -14, 1, 0, -12, -8, -14, -11, -9, -13, -4, 19, 5, -5, 40, -23, 21, 1, 10, -1, -14, 2}
}
, {{-7, 14, 2, 8, -20, -25, -23, -10, 9, -3, -16, 5, -6, -30, 1, 19, -18, 2, 18, -5, -2, -6, 9, -3, 13, 3, -18, 4, -10, 8, -12, -6}
, {-7, 2, -9, 2, -28, -17, -15, -2, 10, 7, -9, -24, -12, 7, -3, 7, 0, -28, -2, 4, 0, -8, -10, -12, 29, -14, 1, 13, 3, -8, -10, -1}
, {-10, 3, 2, 1, -24, -16, 4, 7, -10, -7, -10, 4, -6, 2, -3, -15, -22, -3, -20, 9, -8, 12, 2, -8, 18, -8, 17, -10, -17, -2, -6, -13}
}
}
, {{{3, -3, -22, -17, -14, 6, -2, -17, -19, -10, 8, 7, 18, 3, 0, -3, -1, 3, 8, 0, -13, 30, -10, 15, -13, 10, 5, -17, -10, -13, -3, -18}
, {2, 4, 9, -9, 16, -3, -2, -5, -53, -8, 14, 1, 4, 5, -17, -21, -23, 7, -9, -22, -4, 7, -8, 9, -17, 36, -12, -6, -4, 2, 0, 4}
, {-3, 14, 2, 0, -8, 12, 9, 10, 3, 17, -2, 11, 5, -8, 8, 11, -13, -7, -18, 4, 6, 6, 17, 10, 2, 8, -18, 8, -2, -6, -4, 12}
}
, {{-38, -35, 5, 19, -21, -33, -6, -9, -8, -11, -20, 3, -19, 0, -16, 7, 13, -9, -12, -7, 1, 9, 1, 6, -2, -6, -1, 28, 1, -6, -11, -36}
, {8, 8, -30, -11, -17, -12, 6, -38, -16, 18, 5, -3, 5, 3, 19, 6, -17, 1, 8, 12, -4, -12, -28, 19, -28, 2, 5, 2, -3, -39, -9, -24}
, {-4, 13, 12, -15, 8, 6, -6, 17, -10, 2, 17, 13, 8, 2, -21, -2, 8, 10, 10, -35, 6, 15, 7, -6, 3, 12, -2, -7, -2, 21, 7, 10}
}
, {{5, 14, 9, -8, 16, 8, -5, -8, -17, 8, 3, -1, -15, 13, -12, -3, -17, 6, -5, -3, 14, -7, -6, 10, -9, 0, 3, 8, -9, 1, 3, 8}
, {-36, -1, -6, 5, 4, -27, 12, -8, -1, -4, -18, 10, -10, -9, 5, -3, 10, -14, -14, 1, 25, 6, 36, 14, -6, -7, -3, 4, -2, 17, -1, 0}
, {-6, -8, -16, 9, -6, 7, 11, -4, 11, 13, -2, 4, -3, -9, -10, -6, 4, 7, 20, 4, 7, -2, 4, 14, 1, -13, -11, -17, -17, 5, 9, 9}
}
}
, {{{2, 1, 20, 12, 5, 5, -32, 2, -5, -4, 19, 11, 1, 21, -11, -20, 11, 3, -19, -6, -2, 14, 4, -6, -12, 22, 4, -4, 28, 5, 14, -6}
, {-6, -10, 14, 1, -13, -25, -25, -13, 13, 1, -8, 0, -7, 3, -11, 18, -18, -25, -14, 21, -4, -8, -14, -3, 7, -17, 9, 12, 3, -5, 0, 1}
, {15, -7, -8, 7, -10, -8, -28, 10, -1, 37, 4, 2, -28, 9, -7, -14, -12, -23, -11, -22, -18, -27, 7, -19, 4, 1, -9, 2, -11, 7, -3, 1}
}
, {{-4, -6, -31, -10, 0, 0, -2, -1, -13, 18, 5, 5, 11, 15, 0, -3, 0, -6, -13, -10, -9, 4, -13, 1, -13, 24, 12, -28, 17, -16, 5, -2}
, {-30, -8, -10, -25, -7, -11, -9, -14, 19, -9, -18, -16, -2, -4, 14, 0, 0, -14, 4, -13, 21, -19, 12, 4, 17, -16, 0, 12, -4, -9, -5, -1}
, {2, -8, 3, 29, 7, 14, -13, 8, -27, 4, 3, 17, -22, 0, -2, -17, -7, 3, -16, 5, 10, -17, 2, -1, 3, 6, -29, 3, -11, -10, 7, 0}
}
, {{-16, -10, -2, -1, -9, -1, -17, -3, -16, 10, -6, 10, -26, 6, -19, -5, 1, 7, -1, -17, -8, 2, -7, -1, -13, 15, -19, 3, -5, 2, -11, -7}
, {-7, 8, -20, 8, -3, -16, -5, -1, 13, 3, -9, 6, -18, 1, 17, -7, -14, -5, 6, -12, 18, -21, 11, 10, -4, -9, -19, -3, -18, 8, -5, -3}
, {22, -20, -9, 7, 8, 8, 2, -8, -7, -8, -14, 21, -5, 7, -10, -14, -8, 9, 20, -2, -14, -15, 2, 3, -18, -14, -18, 3, -25, -4, 12, -37}
}
}
, {{{-5, 1, 0, 0, 2, 3, -10, 7, 12, 10, -20, -10, -8, -12, -12, 5, 14, -15, -11, -24, -11, 9, 12, -23, 5, -23, -12, 13, -9, 23, -2, 1}
, {3, 8, -16, -7, 19, 4, -22, 17, -1, 10, -7, 7, 16, -9, -16, 7, 4, -12, -15, -16, -5, -7, -13, -17, -9, -1, -5, 16, 10, 9, 4, 7}
, {1, 7, -4, -4, -4, 13, -5, -11, 10, -6, 5, 6, -6, 0, -2, 4, -22, 6, 6, -1, -11, 1, 7, -4, 1, 6, -8, 15, 5, -11, 3, 7}
}
, {{0, 1, -38, -66, -10, 9, -16, -4, -15, 12, 3, -1, 10, 21, -6, -4, -9, -17, -11, -4, 2, -27, -21, 1, -1, 19, -4, -32, -18, -35, 4, 23}
, {4, 12, -14, -17, 8, 13, -13, -1, -15, 4, 8, -1, -7, 13, -14, -30, -6, 4, 13, -41, -2, -4, 13, -4, -8, 7, -8, -28, -4, 16, 29, -3}
, {-12, 0, -6, -10, 13, -3, -18, 0, 1, -17, 14, 23, -7, 11, -29, -1, -4, -1, 0, -8, 9, 16, 17, -6, -6, 8, -7, 4, 6, 5, 5, -9}
}
, {{-13, -46, -16, 14, 6, -36, -7, -12, -15, -9, -14, 2, -38, -1, -1, -8, -4, -1, 16, -16, 11, -8, -5, 5, 9, -9, -7, -1, -4, 19, 7, -43}
, {-10, -8, -8, 12, 11, -23, 4, -3, -5, -13, -14, 14, -38, -3, -10, 5, -4, -28, 1, -9, 5, -1, 2, -1, -5, 3, -5, -8, 19, 17, -12, -10}
, {-6, -26, -14, 7, 2, -11, 3, 8, -2, 20, -11, -3, -20, -6, -9, -6, -10, 3, 1, -3, 10, -15, -14, -5, -13, -4, 2, -7, -12, 14, -12, -29}
}
}
, {{{-11, -13, -5, -15, -29, -8, -17, -15, -11, -27, 14, -11, -7, -1, -20, -12, 11, -10, -12, 2, 5, -12, -11, -5, 2, 7, 8, -25, 7, -28, -5, 3}
, {5, 3, 3, -8, 7, -11, -5, -7, -13, -7, 10, 10, -6, 13, -12, -30, -1, -1, -13, -32, -13, -3, -25, -11, -9, 12, 7, -1, 9, -2, 8, 6}
, {10, -1, -1, -3, 13, 1, -9, 6, 11, 0, 2, -20, -13, -2, 6, 19, -17, -5, 0, -4, -16, -12, 4, -9, -22, -5, 0, 5, 19, 0, 0, -2}
}
, {{16, -17, -12, -36, -34, -6, -37, 10, -1, -4, 11, 4, -8, 17, -9, -3, -12, -11, -1, 9, -5, 3, -2, -31, 9, -6, 19, -21, 20, -3, 6, -7}
, {8, 11, -27, -30, -6, 10, -11, -39, -19, -27, 21, -5, -5, 21, -20, -7, -17, -14, -7, 15, -11, 1, -6, 14, -10, 13, -1, -6, 1, -20, 11, 1}
, {6, 8, 19, -2, 0, -11, -3, -21, -10, -25, 14, -7, -29, 1, -9, -2, 0, 6, -32, 3, -15, -2, 1, 19, 21, 12, -11, 21, -4, -19, -15, 0}
}
, {{-7, 2, 7, 5, 8, 5, -4, 14, -8, -24, 10, 3, -11, 5, -2, -11, 19, 5, -5, 4, -13, 12, 3, -16, 7, 2, 10, -14, 12, 2, 3, -2}
, {-20, -28, 1, 7, -18, -28, -6, -22, -1, 0, -23, 6, -22, 3, 9, 3, -5, 3, -9, 6, 2, -11, -22, -14, -15, -16, -1, 0, -7, -14, -8, -49}
, {2, -6, 7, 2, 16, 0, -9, 3, -5, 7, -5, -6, 1, 9, 7, 9, -10, -12, 6, 6, 15, -20, -1, -3, -17, -7, -6, -11, 1, -2, 2, 8}
}
}
, {{{9, -1, -7, -14, 11, 10, -16, 12, -5, 7, 19, -8, -2, 4, -35, 1, 9, 3, 8, -17, -9, 5, 21, 10, 16, 2, -3, 2, 7, 9, 1, 9}
, {-22, -15, 19, -11, 12, -8, -11, -1, 5, -4, 1, -5, -14, -11, -21, -16, 14, 14, -2, -4, -10, 16, 9, -19, 5, -8, 15, 13, 9, 0, 13, -4}
, {-1, 7, 26, -12, 8, -3, -10, -2, -5, -2, -4, 18, -5, -23, -6, 0, -14, -10, 7, -4, -15, -2, -5, 4, -3, 5, -8, 7, -2, 14, -9, -23}
}
, {{-17, -10, -34, -4, -18, 8, 5, 9, -2, -6, -7, 2, 2, 1, 15, 4, 6, 1, 4, 1, 3, 12, -14, -12, -7, -15, 9, 5, -7, -15, 9, 1}
, {-1, -10, -9, -7, -9, 11, 9, -7, -44, -19, -12, 0, -8, -12, -29, -31, -12, 18, -18, -18, -5, -2, 1, -12, -21, 15, -8, -34, -14, -3, -9, -7}
, {-14, -1, -29, -9, 5, -2, 11, -13, 1, 17, 0, -34, -6, -30, 2, 10, -29, -15, 12, -20, -15, -36, 1, -18, 5, -26, -3, 5, -33, -6, -33, -4}
}
, {{-20, -13, -5, 4, -14, 1, -11, -19, -5, -12, 1, 6, 22, -27, -5, -4, 7, -3, 18, 14, -10, 19, -12, -3, -8, 0, -8, -9, 5, 3, -17, -6}
, {-2, -5, -21, -24, -16, 6, -7, 3, -13, -8, 2, 3, 7, -15, -21, -17, -27, 3, -1, -16, -6, -5, -26, -5, -4, 23, -17, -35, 3, -1, 0, -7}
, {-13, -13, 1, -21, 2, -1, 6, 19, 9, 24, -13, -11, 2, -14, -4, 21, -4, -34, 8, -12, 4, -14, -13, -23, 3, -14, -17, 3, -13, -4, 0, -3}
}
}
, {{{7, -11, -9, -6, 6, -4, 1, -5, -21, -6, -21, -17, -5, -1, -21, -7, 7, 6, -8, -5, -19, -13, -10, 7, -18, -5, 0, 7, -1, 6, 16, -2}
, {12, -12, 4, 10, -12, -25, -2, 2, -9, -3, -4, -8, -25, -5, -13, 2, -9, -4, 4, -2, 6, -5, 13, 21, -4, -15, 2, 18, -11, 1, -1, -12}
, {2, -5, 0, 3, 0, -19, 1, 5, 12, 3, 1, -5, -13, -9, 11, -6, -15, -6, -7, 19, 5, -14, 0, 6, -18, -15, -1, 6, -10, 8, -16, 13}
}
, {{10, 3, -21, -18, -3, -13, -19, -22, -11, -13, -6, -4, -11, 5, 21, -13, -5, 16, -10, 16, -22, -6, 9, 5, 0, -10, 2, -1, -10, -22, 32, 1}
, {15, 14, -3, -22, 20, -1, -12, -11, -14, -6, -1, -22, -1, 17, 1, -31, -4, -7, 2, 3, 9, 10, 8, 9, -10, -16, 3, 13, -12, 6, 24, 1}
, {-3, -8, -10, -15, 11, 16, -18, -11, -4, -10, -8, -15, 16, 0, 20, -8, 18, -2, -13, 14, 5, 25, 13, 13, 9, -20, -10, 9, -15, -7, 27, 7}
}
, {{1, 13, 10, 1, 14, 23, -8, 12, -7, 1, -13, -4, -4, -24, -21, 2, -11, -6, -20, -27, -27, -7, -2, -24, 6, 1, -22, 6, 2, 16, 4, -6}
, {-19, -8, 20, 12, 12, -10, 2, 14, 11, 10, -23, 16, -9, -23, -2, 3, -1, -29, -1, -5, -32, -6, -13, -10, -29, 4, -6, 20, -15, 24, -3, 15}
, {-8, 0, -19, 13, 3, -13, 7, -10, 4, 5, -34, 14, 16, -10, 18, -5, -14, -16, 7, 19, -8, -14, -2, 8, -9, -7, 4, 0, -6, -5, 12, 7}
}
}
, {{{-2, -10, 10, -26, 3, 0, -30, 2, -2, -10, -7, -11, 0, -10, -10, 12, -18, -3, 13, 7, -7, 4, -20, -13, -19, -1, 12, -15, -5, 3, 4, 10}
, {19, 13, -13, -3, 9, 9, -16, -6, -12, -14, -13, -6, 10, -4, -15, -27, -7, -3, -2, -20, -11, 22, 14, 6, -21, -21, -6, -11, -1, -1, -1, 2}
, {-23, -12, 8, -5, 3, 11, -18, 7, 6, 3, -4, -1, -2, -4, 0, 5, -6, -18, -27, -21, -17, -11, 3, -19, 6, -20, 5, 15, -10, 0, -1, 3}
}
, {{-9, -37, -5, 4, -40, -34, -12, -35, -7, -23, -27, 14, -8, 15, 0, 18, -24, -13, 3, 7, -27, -19, -26, 20, -3, 14, 1, 2, -9, -40, -4, -17}
, {-1, 3, -15, -10, -10, 7, -5, -20, -30, -18, -11, 17, -45, 8, -13, -16, -12, -14, 12, -21, -20, 12, -12, 18, -7, 6, -37, 1, -13, 4, -2, -28}
, {-14, -20, -12, 3, -14, -8, 3, 5, -17, -11, -18, -8, -24, -15, -6, -5, 4, -7, -17, -22, -3, -6, -15, 3, 0, 11, -12, -12, -5, -1, -5, -13}
}
, {{-7, 13, 20, 6, 14, -11, -10, 27, 8, 11, -4, -7, -1, -17, -4, 9, -12, -7, 0, -14, 7, -22, -2, -1, 1, -4, -21, 10, 6, 10, -16, 0}
, {9, 11, -8, -15, 8, -2, -6, -1, 2, 2, 4, 3, -7, -3, -27, 2, -13, -15, -2, -21, -3, -3, 2, -15, -4, -24, -11, 11, -7, 7, 24, 3}
, {8, -1, -9, -17, -9, -15, 11, 6, -7, 13, 9, 4, -8, -3, -9, -8, 5, -4, 4, -10, -1, -5, 0, -4, 7, 9, -13, -9, -2, 7, 6, 4}
}
}
, {{{-4, -8, -13, 4, -17, -11, 9, -9, 13, 1, -5, 10, 4, -16, 3, 13, 13, 9, 9, 4, 6, 5, -1, -13, 22, -4, -6, 8, -10, -6, -14, 2}
, {0, 3, 8, -2, 1, 14, 0, -14, -18, -2, 0, 17, -28, 0, -8, -28, 9, 13, -9, 9, 10, -8, 12, -9, 5, 15, -7, 3, -8, 18, -4, -4}
, {-2, 4, -19, 1, 7, -6, -19, -13, 4, 5, -4, 7, 24, -6, -4, 14, -5, -2, -2, 8, -21, -15, -29, 9, -12, -2, 5, 0, -7, -6, -4, -10}
}
, {{-17, 7, 1, -12, 9, 10, -8, 18, 9, 7, 5, -9, 5, -8, -24, 10, -6, -8, -11, 0, -5, -5, 8, -6, 15, 13, -21, 4, 4, 9, -19, 17}
, {-10, -10, -8, -11, -5, 11, -2, -14, -11, 14, 11, -2, 8, -14, 4, -19, 18, 2, 7, 2, 11, 6, -12, -13, 6, -12, 6, -54, 1, 0, -10, -7}
, {-9, 7, 12, -18, 17, -3, -8, 3, 1, -8, -5, 1, -22, -14, -25, 21, -19, -12, -4, -31, -10, -23, -14, 0, -3, -2, -10, 1, -6, 2, -16, -3}
}
, {{0, -5, -31, -11, -9, 15, -7, -10, -12, 0, 6, -28, 21, -5, 3, -12, -12, 12, 13, -4, -18, -25, -8, 16, -6, -3, -11, -17, -16, -34, 13, -3}
, {-24, 7, -12, -41, 4, -6, -19, -34, -18, -13, 4, -21, 19, 15, -15, -20, -11, -7, 4, -20, 9, 17, 6, 1, -8, -2, 0, -24, 2, -15, 6, 33}
, {-2, 2, -25, 2, 0, 11, -7, -2, -46, -7, 0, -16, -5, 17, 9, -30, 6, 12, -2, -6, -11, -12, -20, 4, -12, -5, -4, -21, -4, -15, 16, -3}
}
}
, {{{-5, -19, -14, 12, 9, 5, 0, -4, -18, 8, -26, 5, -16, 0, 7, -6, 12, 25, 2, -15, 7, 18, 0, 6, 2, 11, -10, 9, 6, 7, 11, -21}
, {-9, -4, 9, 11, 14, 9, -9, 1, -10, -8, 13, -5, 11, -16, 9, -6, -3, -3, 14, 9, 8, 4, -14, -22, -10, 3, -8, 1, 9, 5, -10, -16}
, {-7, -10, -23, 7, -2, -4, -7, 4, -11, -12, -6, -7, 15, 2, 1, -9, -7, -9, 8, 1, -2, 1, -18, -14, -13, -2, 3, -10, 16, 2, 15, -7}
}
, {{-8, -13, 11, 22, 15, 1, 4, 6, -47, 7, 5, 18, -15, -22, -13, -18, -1, 5, 8, -32, -1, 18, -11, 16, -42, -4, -13, 11, 15, 0, -17, -22}
, {-2, -22, -16, -7, 4, -15, -22, 7, -13, 4, -4, 1, 9, -9, 34, 20, -10, -10, -11, 21, -12, -19, -6, 0, -24, 33, 19, -24, 47, -29, 4, 2}
, {-18, -2, -10, 3, -13, -12, -18, -11, -24, -15, -7, 1, 18, -4, -3, -1, 5, -20, 4, -13, 2, -15, -9, 6, -4, -1, -32, -18, -5, 1, 16, 7}
}
, {{2, 5, 10, -2, 13, 21, 2, 19, -16, -8, 0, 7, -3, 4, 5, -17, -10, -2, -8, 9, -8, -9, -40, 3, -18, 11, 12, 7, 12, -14, -11, -16}
, {-7, -23, -8, -3, 2, 1, -12, -3, -15, 0, -10, 1, 9, -6, 16, 1, -8, -6, 11, -12, -19, -9, -5, -4, -9, 15, 3, -21, 31, -4, -7, -1}
, {-6, 0, -10, -6, 3, -2, -8, -11, -11, -6, 0, -3, 6, 7, -5, 1, 9, -19, -12, -1, 7, -2, -9, -8, 3, 17, -24, -4, -2, -17, -8, 6}
}
}
, {{{-6, -20, 2, 7, -29, -15, -2, -9, -27, 10, -15, 20, -7, 5, 6, 5, 0, 23, -4, 9, -6, -14, -16, 6, 9, 1, -24, 25, -21, -21, 9, -6}
, {28, 8, -28, -5, -8, 8, 0, -40, -11, 1, 10, 0, -19, 29, 15, -1, -7, 10, 19, 12, 2, -20, 8, 20, -11, 26, -9, 2, -3, -24, 27, 13}
, {-11, 3, 3, -14, -19, -3, -11, -36, -12, -14, 16, -1, -22, -3, -10, -25, 10, 5, 0, -2, 13, 15, 11, 14, 20, 2, -13, 0, 5, -22, -7, 9}
}
, {{1, 3, -8, 4, 9, 23, -19, 4, -10, 3, -3, 9, 6, -2, -5, -3, -13, 15, -12, -1, -6, -12, 0, 2, -16, -18, -6, -10, -26, 1, 4, 1}
, {8, 15, 15, -3, 18, -3, -13, -2, 8, -14, 1, 9, -24, -8, -22, -1, -13, -5, -6, -23, -10, -1, 22, -11, 12, -14, -29, 29, -14, 25, -5, -16}
, {2, 7, 5, -7, -4, 14, -11, -4, 4, 14, 4, -6, -2, -21, -37, 16, -4, -10, 6, -23, -5, -2, -4, -12, 9, -2, -10, -3, 7, 7, -14, 3}
}
, {{11, 0, -4, 1, 1, 1, -14, 7, -7, 7, -1, -3, -9, -9, -19, 7, 11, 4, -7, -10, -13, 1, -10, 1, -5, 8, -15, -1, 2, 8, -22, -7}
, {-20, -11, -18, -4, -5, -14, 3, 3, -12, 2, -4, -2, -2, -1, 0, -21, -5, -8, -10, -7, 9, -1, 9, -6, 0, -5, -3, -30, -5, 4, -2, -1}
, {-18, -16, -10, 5, -8, -9, 4, 8, -3, -6, 8, -5, -2, 9, -6, -23, 13, -5, -14, -1, 24, -10, -2, 4, 2, -5, 12, -10, 0, 5, 8, -13}
}
}
, {{{-20, -18, -5, 4, -8, -24, 9, -13, 2, -4, -2, 4, -21, 21, -2, -2, 4, 6, -1, 8, 7, 6, 21, -9, 0, 1, 2, -12, 10, -5, 16, -4}
, {11, 3, 3, 11, 0, 3, 14, -2, 7, -22, -24, 9, -17, 19, -24, -6, -6, 2, -23, -5, -12, -5, -1, -1, -5, -31, 6, 4, -9, 9, 1, -21}
, {22, -6, 0, -1, 14, 4, 5, 5, 1, 7, 7, 2, 9, 5, -9, 1, 1, 5, 4, 3, -5, -2, -12, 5, 17, 3, 21, 13, 4, 3, 11, -25}
}
, {{-9, -11, -10, 3, -18, -18, -3, -19, -3, 1, -13, 4, -10, -31, -9, 6, -6, -11, 15, 12, -7, -2, -4, -3, 14, -9, -4, -1, -16, 12, -18, -15}
, {-5, -7, -16, -12, 0, -7, -15, 26, -33, 7, -17, 1, 5, -11, -18, -20, -20, -28, -35, -20, -16, -28, -31, -6, 4, -11, -7, -16, -35, 20, -3, -17}
, {12, 2, -18, -12, -5, 16, -12, -6, 1, 15, -7, -19, 8, 3, 8, 7, -23, 7, -2, -7, -6, -2, -2, 25, -5, -3, 3, -14, 1, -18, 3, 34}
}
, {{-5, -10, 9, -3, 4, 20, -14, -4, 7, -7, 19, -2, -1, 8, -12, 7, 5, -22, 11, -7, 8, -9, 2, -15, 2, 1, 2, 11, 9, -26, -10, 10}
, {6, -13, -21, 5, -20, 23, 9, -3, 14, 9, -10, 7, -3, -3, 13, -1, -9, 3, 11, 8, 12, -33, -24, 15, 1, -4, -5, -14, -31, 0, 1, -3}
, {-19, -18, -4, 13, -15, -7, -4, -5, 10, 9, -9, 12, -12, 17, -5, -1, -5, 3, -6, -22, 4, -13, 4, -2, 23, -8, -6, 24, -13, 7, -9, 11}
}
}
, {{{6, 13, -14, -17, 24, -5, -33, 3, -6, -3, 12, -14, -16, 10, -8, -9, 7, -13, 6, -14, 1, 0, 3, 9, -3, 10, 3, -16, -5, -7, -6, 2}
, {-25, -8, 8, 2, -20, -10, 0, 10, 1, 3, -13, -9, -1, -7, 10, 10, 7, -14, -44, 7, -7, 3, 1, 19, -2, -5, 8, 15, 5, 11, -7, -6}
, {1, 9, -7, 10, -2, 8, 15, -29, -12, -5, 32, -11, -8, 0, 16, -4, -2, 21, 12, 29, 11, 1, -1, 9, -23, 1, 8, 6, 7, -27, 3, -15}
}
, {{-16, -1, -9, -12, -2, -4, -16, -9, 8, 14, 3, 3, -6, 10, -4, 11, -7, -17, 12, 26, 7, -8, -7, -12, 8, 2, -7, -6, 7, -9, 10, 5}
, {2, 8, 12, 8, 12, -6, -17, 5, -20, 0, 22, 2, -8, -6, -31, -42, -15, 10, -4, -20, 8, -14, 9, -30, -9, -1, 12, -2, -6, 21, -1, -14}
, {-23, 2, 7, 14, 0, -20, 6, 4, 17, 4, -11, -8, -8, -13, -11, 26, -8, -31, -16, -3, -5, -4, 0, -14, 4, -25, 12, 10, -19, 25, -7, -26}
}
, {{-30, -16, -12, -6, 5, 13, -11, -16, 6, -7, -18, -7, 14, 8, -8, 18, 8, 5, -10, -2, -1, 13, 0, 5, 4, -5, -4, 6, -18, -18, -2, 6}
, {7, -14, -8, 15, -24, 3, 0, -1, -16, 13, -4, 1, 14, -9, 5, 6, 23, 16, 2, 1, 2, -9, -19, 14, -1, 0, -29, -17, -19, 9, 5, -8}
, {-6, -2, -1, 0, 4, 11, -6, -8, -32, 3, -5, -12, -5, -7, 17, -6, 13, 10, 10, 0, 22, -5, 4, 8, -4, 3, -18, -11, -5, -4, 21, 12}
}
}
, {{{13, 9, 9, -29, 3, 31, 1, -1, -1, 3, 15, -21, -20, 2, 6, -4, -11, 6, 13, 7, 0, -3, 10, -13, -6, -13, 0, 2, -3, 2, 11, 13}
, {4, 2, -4, -1, -2, -2, 3, -9, -4, -7, 6, 15, -6, -19, -17, -12, -5, -3, -10, -18, -15, 12, 14, 5, 6, 0, 4, -9, 2, 12, 0, -8}
, {13, 4, 5, 3, -9, 1, -3, 12, 9, -8, 7, -4, -21, -2, -4, -3, -2, -13, 3, 1, -13, -8, -1, -8, 23, -7, -6, 6, -5, 9, -2, -7}
}
, {{-18, -27, -22, -3, -38, 2, 0, 5, 6, 14, -25, 4, 17, -4, 8, 24, 1, -5, -5, -7, -10, -27, -24, 1, 4, -1, -3, 7, -9, -19, 19, -12}
, {-2, 17, -2, -31, -15, 11, 4, -14, -62, -6, 38, -17, 7, -5, -14, -45, 12, -2, 13, -9, 6, 19, -3, 1, -2, 31, -17, -27, 28, -24, -15, 15}
, {-12, -25, -13, -6, -4, -5, 5, 15, -30, -4, 6, 7, -3, -6, 7, -40, 5, 11, -24, -11, 9, -11, 7, 10, 7, -3, -6, -11, 15, -3, 3, 3}
}
, {{3, -10, 3, 17, 17, 8, -13, -15, -5, -4, -7, 3, -41, 9, 8, 7, 1, 5, -6, 0, -4, -23, -1, -25, -12, 5, -9, 10, 1, 9, -10, 2}
, {-3, -20, -24, -7, -22, -19, -10, -5, -4, -1, -10, 14, -2, 0, 6, 11, 4, -31, 6, 0, -1, 4, -3, -8, -23, 5, -8, -17, 0, 0, -11, -20}
, {9, 3, -28, -5, -1, 12, 1, 5, -24, 8, 16, -14, -23, 11, 15, -19, 8, 12, -1, -2, 1, -21, -1, 5, -13, -11, -5, -19, -10, -6, 8, 8}
}
}
, {{{-8, -18, -2, 15, -18, -6, 6, -12, 8, 17, -21, 7, -16, 7, -10, 18, 23, -3, -2, -3, 3, -9, 2, -1, 0, -2, -9, 20, -11, -12, -2, -9}
, {-1, -9, -18, 7, -2, -4, 12, -24, -2, -7, -1, -4, -3, 7, 7, -7, 3, -1, 7, -3, 4, 9, 4, 3, 2, 5, 20, -2, 13, 4, -9, -1}
, {-3, 0, -17, -2, -2, -12, -7, 12, 14, 12, 2, -8, 10, 2, -2, 8, 10, -4, 7, 2, -5, -3, -2, -4, 0, -12, 13, -1, 19, 8, 21, 6}
}
, {{-2, 1, 5, 4, 10, 14, -1, 13, 3, 14, -1, 6, -9, -2, -15, -18, -7, -2, -22, -25, -4, 2, 6, -29, 8, -5, -17, -20, -8, 2, -2, 10}
, {-32, -42, 24, 11, 6, -19, 6, 23, 7, -9, -41, 1, -25, -20, -13, 30, -7, -49, -41, -23, -6, -24, -11, -24, 16, -3, -9, 10, -14, 21, -13, -10}
, {-15, -41, -7, 19, -32, -26, 5, -3, 8, -1, -31, -1, -29, -17, 13, 4, 0, -9, 1, 4, -9, -34, -34, 18, 6, -3, 15, 3, 5, 2, -1, -40}
}
, {{-5, -14, 0, 3, -11, 7, 8, -10, -30, -17, 1, -5, -6, 13, -7, 3, 1, 16, -17, 10, -6, -9, 1, 10, 0, 15, -7, -20, 1, -12, 5, -2}
, {16, 10, -5, -20, 16, -6, 8, -4, -27, 7, 4, 9, 4, 8, 3, -30, -15, 9, -2, 7, -10, 0, 0, 5, -5, 11, 1, -2, 11, 5, 2, 0}
, {3, -5, 3, -9, 9, 9, 1, 11, -19, -2, 15, -1, 0, -7, -4, -5, -5, 4, -3, 3, -1, -19, 0, 6, -15, 3, -3, 2, 4, 4, -4, 9}
}
}
, {{{-5, -13, -8, -8, 1, -16, -11, 0, 8, -9, -8, -11, -9, -1, 15, 0, 19, 7, -10, 8, 6, 3, 25, -21, 9, -19, 16, 2, 6, -7, -3, 7}
, {-13, -8, 18, -5, 7, 1, -19, 6, -12, -8, 10, -14, -14, -30, -24, -3, 4, 3, 8, 6, -3, 3, 14, -25, -8, 1, -20, 15, 6, 18, 9, 0}
, {2, -27, 2, 9, -2, 3, -11, -2, 20, 9, -9, 11, -8, 0, 9, 16, 4, -4, 0, 19, 1, -8, -11, 6, -22, -30, 28, 12, -10, -8, -6, -32}
}
, {{-17, -12, 6, 1, -6, -5, -19, 7, 0, 0, -2, 4, 5, -8, -4, -9, 11, -3, -14, -4, -1, -8, -6, -14, 5, 6, 12, -10, 4, 2, 34, 14}
, {5, -12, -27, -27, -14, -1, -14, -21, -21, 9, -2, -13, 4, 11, 16, -4, 4, -1, 4, 7, -33, -7, -22, 5, -8, -3, -2, -20, 5, -12, 1, 1}
, {-10, -6, 7, -33, 10, 0, -25, -4, -27, 1, 11, -5, -3, 9, -5, -21, -12, -20, -3, -15, -8, 12, 22, 6, -1, 5, -17, 4, 5, 2, -12, 30}
}
, {{-13, 1, 1, 5, 8, 5, 2, -15, -13, -24, 2, 4, -5, 7, -3, -13, 1, -12, 1, -11, -18, 9, 16, 21, 1, 4, -2, 5, 4, 10, -4, 2}
, {-2, -9, 5, 8, 4, 6, 6, 19, 14, -1, -7, -10, -18, 3, -15, 19, -25, -15, -11, 7, -16, -8, -20, 8, -10, -28, -15, 2, -31, 1, 22, -18}
, {-16, -30, -2, 11, -16, 1, 16, 15, 6, -13, 7, -13, -28, -15, -10, 13, 10, -11, -3, -16, -9, 6, -9, -9, 14, 12, -6, 6, 6, 13, -16, -19}
}
}
, {{{18, -13, -5, 3, -29, -4, -23, -6, -1, -2, -19, 8, -4, 3, 18, 2, 10, 4, 9, 1, -34, -9, -3, 16, 1, 6, 1, -3, 2, -4, -23, -27}
, {18, 13, -4, -9, 13, 4, -13, -5, -1, -26, 3, -4, -11, 28, 13, 1, -22, -10, -14, 1, -24, 7, 7, 6, -20, -22, 3, 14, 0, -10, 37, 5}
, {29, 12, -4, 12, -1, 17, 4, 13, -3, -21, 11, 7, -10, 23, -5, 6, -9, -15, -23, -1, -11, 15, 2, 14, -18, -18, 0, 12, -5, -11, 16, -10}
}
, {{13, -6, 8, -4, 12, 14, -13, -16, 11, -5, -10, -7, 12, 7, 6, -6, 9, 5, -10, -1, -1, 7, 18, 10, -1, -1, -11, 25, 12, 4, 19, 3}
, {20, -4, -9, -4, 3, -12, -4, -11, 2, -7, -8, 2, -2, 12, 12, 15, 7, -4, 8, 12, -10, 5, 15, 27, 0, -29, -6, 6, -6, -5, -3, 0}
, {17, -2, 3, -15, -9, -9, 1, -11, -3, -21, -10, -2, -11, 7, -10, -14, 2, -5, -24, 28, 2, -6, 24, 4, 10, -11, 4, 3, 21, -6, 14, -13}
}
, {{-6, 12, 7, 14, 7, 1, -13, 12, 15, 8, -20, -9, 3, -14, 4, 26, 2, -24, -30, 2, -15, -14, 11, -22, -10, -9, 18, -2, -11, 6, -15, -16}
, {3, 25, 3, -6, 16, 9, -2, 5, 12, -25, 4, -20, 1, 2, -4, 1, -7, -8, -14, 14, -6, 14, 20, -26, -8, -3, -1, 9, 20, -1, 17, 4}
, {-1, 23, 0, 2, 6, 5, -3, 6, -7, -13, 0, -9, -8, 0, 1, -12, -4, -13, -16, 3, 12, 13, 27, 3, -4, -16, 10, 3, 14, 6, 36, 1}
}
}
, {{{-21, -16, -4, -6, -5, -3, -31, -15, 0, -12, -6, 1, 2, 8, -4, 13, 6, -5, 1, 5, -10, 11, -18, -15, 11, 11, 15, -4, -1, 2, -14, 4}
, {-16, 7, -16, -14, -11, 4, -16, -11, -11, 5, 1, -7, 6, 1, -4, -11, 9, -18, -12, -5, -14, 23, 6, 19, 5, -14, -3, 5, -2, -2, -7, 10}
, {-3, -3, -4, -5, -6, -1, -1, 4, 0, 3, -13, 3, -2, -16, -2, 2, -8, -3, -7, -12, -9, 8, 4, -3, -5, -18, 1, -4, -2, 3, -6, 2}
}
, {{-6, 10, -11, -16, -21, -18, -17, -44, 8, -37, -30, 9, 16, 0, -18, -7, -34, -19, -10, -14, -29, 10, -27, -2, -8, 3, 10, -28, -18, -11, 3, -6}
, {-2, 21, -2, 0, 6, 7, -3, 27, 9, 0, -30, -26, -13, -10, -2, 17, -26, -43, -8, -22, -27, 2, -6, 9, 26, -10, -4, 2, -18, 7, 0, -19}
, {10, -18, -3, -13, -20, -9, -24, -32, -15, 0, 13, -17, -16, -3, 13, -10, -19, 8, 12, 6, -6, -13, -5, 2, 1, 20, 11, 1, 19, -15, -10, -9}
}
, {{-14, 14, 1, 7, -4, -8, -11, -1, -8, 0, -16, 18, -12, -1, 3, 2, -12, -1, 10, -9, -24, -7, -15, 14, -12, 14, -7, 3, -1, -2, 3, -10}
, {1, 10, -11, 2, -17, 6, -3, 8, -22, -12, 13, 0, -1, 13, 3, -29, 10, 3, -7, -5, -25, 14, -9, -2, -23, 18, -5, -11, 19, -6, 15, -1}
, {-7, -3, 5, -6, -4, -21, -10, 12, 10, -5, 2, 5, -4, -3, -7, 15, 1, -25, -21, 1, -11, 4, 4, -6, 23, -5, -16, 3, 0, 3, -17, -7}
}
}
, {{{-6, 16, 0, 2, 2, -10, -11, 5, 7, -10, 10, -16, -10, -13, -1, -5, -7, -2, 17, -6, 7, -15, 11, -13, 8, -6, -3, -10, 11, 16, -5, -7}
, {-16, -9, 10, -12, 8, -11, 12, 20, 3, -14, 17, -28, -6, 7, -4, -4, 0, -20, -15, 2, 5, 0, 0, -8, -8, -16, 22, -8, -9, 14, 3, 10}
, {-5, -12, 10, -3, 23, -4, 5, 30, -2, 3, 8, -16, -11, -4, -14, 4, -2, -9, -8, -5, 9, -6, 5, -15, 4, -7, 21, -7, 17, 10, -4, -9}
}
, {{-2, -15, 4, 6, -12, -13, -5, -1, -2, -9, -6, -1, -26, -16, 12, -12, 22, -22, -21, 8, 10, -5, -1, 13, -7, -16, 2, -3, 1, -2, -27, -1}
, {-9, -45, -31, 11, -33, -22, 7, -3, -9, -18, -7, -8, -8, 10, 32, -11, -22, 10, -11, 30, -2, -10, -12, 20, -13, 2, 18, -21, 1, 12, 0, -12}
, {13, -32, -47, 18, -48, -11, 9, -35, -12, -5, 1, 1, -20, 6, 23, -21, 8, 25, -6, 16, 7, -1, -16, 21, 7, -9, 4, -17, -5, -14, 5, -13}
}
, {{-14, -8, -15, -4, -13, 14, 2, -1, -8, 1, -1, 3, 0, -3, 6, -8, 1, 15, 7, 1, 9, -4, -4, -4, 3, 4, -17, -12, -17, -19, -2, 5}
, {-9, 9, -32, 1, -11, -21, -10, -18, -14, -13, -10, -8, -17, 0, -8, -29, -5, 2, 5, -11, 3, -12, -10, -3, -3, 14, -1, -16, 4, -2, 0, -9}
, {6, 6, 4, -9, 16, -6, -3, 9, -24, -3, 2, -25, 2, -5, -2, -14, -7, -23, -18, -18, 0, -18, -8, -3, -6, 4, -4, 23, -4, 11, -2, -2}
}
}
, {{{-3, 16, -11, -20, -1, 3, -12, 4, 1, 7, 0, -2, 19, 8, 5, -4, -11, -2, 2, 4, -10, -4, 0, 8, -15, -4, 0, -15, -6, 5, 28, 3}
, {13, 7, 27, -24, 13, 23, -22, 12, -12, -3, 21, -44, 5, -4, -7, 2, 1, -2, 24, -30, -8, -8, -6, -9, -21, 0, 0, 10, 21, 15, 4, 6}
, {-7, -5, -3, -5, -5, -3, -19, 5, 0, 5, -2, 14, 5, -4, -2, -2, 4, 9, -13, -7, -4, -1, 10, 8, 11, 7, -10, 11, 10, -10, 18, 17}
}
, {{-7, -4, -9, -11, -8, 15, -23, -11, 4, 13, -6, -15, -7, 3, -8, 14, 2, -6, 11, 0, 1, 2, -2, -11, 7, 14, -9, -1, -4, 9, 1, 1}
, {-58, -19, -11, 4, -38, -44, 17, -22, -3, -31, -16, 23, -30, -7, -6, 1, 36, -27, -24, -44, -3, 22, 0, 19, 9, 6, 12, 13, -17, 3, -43, -19}
, {-4, -21, -15, 0, 2, 6, -7, 14, 3, 18, -8, -8, -15, 4, -4, 0, -15, 5, 13, -10, -18, -35, -22, -17, -5, -7, -5, -9, -1, 25, -5, -31}
}
, {{-10, -6, -4, -10, -15, -16, -8, -10, -5, -14, -3, 25, 4, 12, -10, -5, -2, 15, -8, 0, -1, 7, 14, -1, -16, 10, -8, -7, -6, -4, 3, -3}
, {8, 0, -13, -3, 0, -8, 0, -5, -9, 9, -5, 2, -5, -4, 4, -5, -12, -25, -4, -10, -1, 8, -14, 15, -5, -15, 0, -4, -11, 4, -11, -1}
, {3, -24, -13, 5, -18, -5, 0, 8, -6, -2, -5, 3, -2, 9, 7, -6, -2, 8, -12, 6, -4, -4, -17, 14, -22, 2, -14, -7, 4, -6, 9, 6}
}
}
, {{{-34, -29, -26, 7, -5, -3, -7, 6, 5, 8, -13, -1, -13, -18, 0, -10, 25, -2, -3, 12, 16, -6, 2, -9, 1, -12, 5, -4, -4, -9, 13, 0}
, {-11, -39, -22, 5, -49, -4, -19, -19, -7, -21, 7, 23, -14, -18, -1, -15, 5, 8, 11, 18, 23, 10, 3, -3, -3, 14, 4, -17, -2, -27, -2, -27}
, {5, 3, 0, -5, 8, 14, 10, 12, -8, -1, 2, -2, -14, -14, -8, -5, -10, 1, 0, -8, -25, -6, -25, 12, -7, -3, 5, -26, 7, 26, -20, -9}
}
, {{14, 6, -9, -5, 6, 17, -9, -17, -23, -17, 2, 2, -16, -1, -11, -22, -5, 22, 10, -15, 5, 1, 23, 13, 14, -28, -2, 7, 4, -5, 11, 1}
, {4, 5, 2, 13, 6, 3, -16, 5, -8, -2, 2, -4, -41, -10, -1, -3, 13, 15, 28, 0, 6, 7, 10, 2, 12, -5, -26, 10, -3, 14, -1, -14}
, {-3, -1, -5, -11, -4, 1, -7, -22, 3, -10, 8, -20, 15, -4, 18, 16, -13, 2, 24, 17, 14, -22, 5, 21, -9, 0, 5, -23, 0, -31, -6, 17}
}
, {{-13, -7, -6, 5, 1, 4, 1, 2, -22, -12, -12, -8, 8, -5, 6, -9, -1, -20, 4, -2, 9, 4, -7, 13, -25, -24, -8, -6, -6, 9, 7, -9}
, {2, -24, 0, 1, -2, -11, -11, 20, 4, 24, -15, -5, -10, -12, 14, -14, 15, -5, 8, -3, 4, -9, -6, -2, 11, -11, -13, 1, -8, 0, -5, -12}
, {-23, -16, -8, 3, -2, -3, -22, -6, -6, 2, -4, -6, 8, 0, 8, 12, -1, -3, 17, 17, 2, -13, -10, -7, 5, -7, 5, 2, 2, -2, 3, -11}
}
}
, {{{-13, 8, -13, -15, 0, 16, -10, -5, 7, 1, 2, -19, 5, 6, 2, -7, 0, 1, 14, -4, 16, -16, -2, -4, 27, 4, -7, 9, -14, -17, 6, 20}
, {-18, -3, -3, -1, -13, 5, -16, 13, -18, -13, -8, -3, -21, -8, -12, -16, 2, -8, -3, -17, -7, -3, -4, -4, 8, 12, 8, 4, 17, -1, 5, -10}
, {-1, -19, 0, -18, -4, -8, -24, 9, 5, -10, 13, -1, -10, -21, -5, -5, 10, -10, -7, 4, 18, -22, -13, -14, 14, 11, 11, 1, 2, -15, -1, -9}
}
, {{-8, -10, 10, 8, -8, -1, -21, -16, -12, 15, 13, 5, -6, -5, -1, -7, 3, 0, 10, 3, 13, -8, -7, -22, 10, 1, -13, 6, 8, -4, -5, -22}
, {-9, 9, -7, -16, -11, -3, -17, 9, -9, -1, 10, -7, -10, -27, -1, 4, -13, -38, -19, -24, 7, -35, -21, 10, 21, 5, -8, -7, 4, 8, -18, -1}
, {-25, -18, 4, 0, -8, -11, 1, 18, -10, -8, -19, 1, -19, -2, -2, -8, -3, -7, -24, -7, 6, -15, 9, -18, 14, 9, -8, 7, 24, -7, 10, -1}
}
, {{-4, 0, 4, -7, -4, 4, -3, -21, 1, 8, 4, 7, -12, 6, -1, -2, 4, -9, -5, 8, 11, -2, -3, -3, 13, 22, 12, -3, -3, -17, -9, 11}
, {-17, -15, -13, -14, -19, 1, 2, -8, 28, 15, 6, -26, 7, -18, 16, 9, -9, -10, 9, -5, 3, -13, 1, -2, 25, -14, -6, -8, -26, -2, -20, 4}
, {-30, -26, 11, -9, 5, 6, -15, 0, -33, -1, 15, 14, -6, -12, -46, -12, 7, 6, -21, -24, -2, 5, -5, -25, 17, 27, -21, -12, 16, -10, 5, 4}
}
}
, {{{-6, 5, 5, 15, -8, -7, -12, 2, -2, -3, -6, 5, -16, -31, -3, 3, 0, 12, -21, -2, -16, -24, -6, 7, -16, 2, -7, 7, -17, 9, 3, -13}
, {-2, 9, -17, -10, -3, -11, -7, -12, -2, -16, -6, -6, -6, 25, 8, -7, -3, 6, 10, 3, 9, -9, 19, 6, -9, -9, 7, -3, -4, -12, 26, 7}
, {17, 11, 3, -7, 4, 6, -23, -11, -4, -26, 3, 0, -12, 9, -2, -2, -19, -6, 3, -8, 1, 4, -1, 6, -12, 6, -13, 19, 4, -21, -1, -1}
}
, {{23, 13, -4, -26, 7, 5, 6, 16, -20, 19, 5, -17, -6, -4, -18, -17, -25, -14, -14, -5, -11, -30, 7, -13, 8, 6, -4, -20, -14, 7, -20, 15}
, {-22, -19, 11, 0, 1, -19, 6, -8, 9, -51, -13, 25, 5, 8, -41, -10, 22, -12, -26, -25, -12, 29, -3, -1, -15, -1, -5, -1, -12, 2, 11, -15}
, {2, 25, 9, 17, 0, -6, -6, 27, 11, 14, -11, -4, 15, -16, -21, 27, -20, -13, -10, -20, -35, -27, -10, -2, -4, -10, -21, 12, -13, 13, -12, -27}
}
, {{-6, -7, -1, -13, -11, -3, 5, -2, 2, -9, 6, -1, -14, 6, 14, 14, 17, 7, -17, 11, 21, -11, 22, -10, 30, 7, -11, -21, 5, -13, -8, 13}
, {-5, -1, -2, -12, -20, -1, -10, -14, -14, -6, -9, -19, -9, -3, -12, -12, -3, 14, 4, 7, -6, -1, -25, 9, 10, 5, 7, -7, -3, -16, -1, -23}
, {15, -3, 4, 8, 0, 7, 2, 5, -25, 25, -2, -11, -1, -4, -8, -6, 6, 5, 8, 1, -8, -6, -17, 5, -7, 11, -18, 1, -5, -11, -6, -4}
}
}
, {{{-11, -18, 5, 15, 0, -3, 1, -3, 3, -16, -13, -4, 3, -7, -2, 5, 23, -1, 0, -1, -11, 22, -20, 6, -6, -1, 13, 9, -13, 8, 1, -16}
, {0, -13, -14, 0, -5, -6, 7, -38, 10, -27, -16, 8, -26, -4, 7, 26, -16, 10, -13, 13, -10, 12, -10, 13, 25, 15, -2, 10, 13, 9, -6, -28}
, {10, 11, -6, 17, 0, 2, 12, 5, -8, 10, -7, 11, -31, -5, 5, -7, -1, 4, 8, 14, -16, -2, 11, -3, 16, -10, -13, 4, 5, 12, -15, 11}
}
, {{-9, 1, 15, 5, 8, 6, 4, 15, -16, 11, 12, 0, -19, 6, -27, 12, -3, -25, -20, -19, -16, -25, -20, 11, -13, 29, -12, 16, 2, 4, 7, -21}
, {-16, 1, -27, -1, -25, -19, 6, -18, 2, -37, -8, 33, -22, 4, -1, -2, 3, 3, -4, -4, 8, -9, 13, -8, 3, -5, -13, -19, -12, -14, 13, -7}
, {5, 13, 7, 3, 18, 3, 9, 15, 4, 13, 1, -6, -5, -1, 6, 4, -20, 9, 5, -7, -15, -20, 17, 5, 33, -44, -25, -1, -18, 25, 5, -6}
}
, {{-6, -9, -11, 2, 8, 3, 0, 8, -10, -7, -4, 12, 0, 1, -2, 5, -11, 2, 8, -6, -12, -7, -8, -2, 4, 14, -15, -5, 5, 7, -4, -12}
, {-17, -17, -4, -7, -1, -8, -6, -12, -3, -15, -2, 1, -18, -17, -9, -24, -1, 3, 19, -14, -2, -20, -6, -36, -1, 8, -27, 4, 2, -4, -14, -8}
, {-20, -17, -15, -4, 12, 7, 2, 23, -18, 24, 4, -4, -11, -3, -2, -2, 2, -20, 3, 0, -8, -12, -9, -1, -14, -12, -17, 9, -5, -9, -7, 10}
}
}
, {{{-17, -4, -23, 5, -18, -20, 10, 11, -6, 3, -9, 9, -1, 3, 2, -3, 6, 4, -18, 8, -8, -19, -13, -2, -5, -14, 6, -15, -8, -19, 13, -3}
, {-7, -2, -24, 1, -18, -10, 6, -31, -13, -14, 6, -8, 6, 12, -4, 3, -3, 5, -4, 9, 6, 21, -8, 2, -5, 4, 10, -7, -6, -6, 16, 24}
, {10, 8, -2, 8, -9, 4, 2, 7, 10, 4, 3, -9, -10, 22, -12, -17, -5, 6, 4, 8, 8, 8, -3, 5, -2, 10, 7, 1, 10, 0, 9, 0}
}
, {{-33, -21, 15, -3, 13, -14, -7, 4, -25, 1, -23, 24, -19, 6, -25, 5, 3, 4, -20, -26, 21, -20, -7, -17, 9, 12, -22, 11, 6, -7, -3, 12}
, {-31, -35, -13, 18, -16, -51, 0, -5, 2, 17, -36, -2, -12, -7, 4, 11, -4, -19, -3, 6, 1, -11, -20, -13, 7, 3, -2, 16, 7, -26, -13, -29}
, {-6, 1, -11, -15, -23, -3, 1, -2, 0, -20, 1, 4, -1, 12, -12, -10, 10, 0, -12, -3, 0, 1, -9, -2, 4, 10, 2, -8, 20, -12, 8, -12}
}
, {{-8, -11, -8, -7, 8, -9, 8, -3, -10, 9, 3, 10, 5, -6, -10, -16, -19, 3, 7, -11, 2, 9, -3, 0, -12, 9, -14, -10, -4, -2, -12, -23}
, {-33, -5, 18, -10, 14, -10, -4, 15, 3, 18, -11, 9, -8, -20, -29, 29, -9, -44, -21, -33, -22, -8, 5, -45, 9, 5, -16, 18, -6, -1, -17, 14}
, {-4, -10, -7, 5, 13, -9, -1, 17, -5, 13, 2, 12, -7, -6, -3, 4, -11, -11, -1, -13, -7, -12, -18, 0, -2, -5, 3, -10, 6, -4, -11, 9}
}
}
, {{{-15, -6, -3, 10, -13, -7, -5, -1, -16, 0, 2, 13, -14, -3, 7, -4, 36, 9, -5, -3, 12, 6, 25, -16, 16, 0, -19, -3, 4, 5, 4, -6}
, {16, 1, 5, 8, -1, -10, -5, -1, -2, 3, 7, 4, -13, -12, 14, 2, -9, 17, 6, 10, -8, -15, -5, -25, -22, -10, 3, 12, -5, 2, 16, -11}
, {8, 3, 6, 1, 7, 5, -6, -1, 13, 7, 6, -8, -2, -23, -12, 22, -16, -10, 3, 4, 7, 0, 6, 0, 5, 0, 10, 16, 19, -10, 4, 0}
}
, {{-6, -14, -1, -7, 12, 19, 7, -6, -24, -9, 20, 13, 14, -16, 1, -22, 6, 3, -8, 7, 14, 5, 16, -4, 7, 21, 6, -33, 7, 17, 5, -1}
, {8, -6, 8, 5, 9, 15, -21, 6, 6, 19, -14, -17, 16, -8, 0, -3, 0, 0, 3, -6, -6, -10, -10, -22, -17, -21, 3, 12, -10, 22, 7, 8}
, {-8, -2, 21, -29, 19, -15, -31, 2, 6, -12, -3, -29, 17, -12, -3, 2, 2, -8, -11, -14, 1, 17, 5, -32, 17, -11, 11, -3, 15, 10, 19, 4}
}
, {{-25, -24, -19, -13, -34, -30, -17, -10, -20, -27, -9, -13, 13, -14, -1, -4, -15, -1, -20, -13, -1, 9, -3, 11, 14, 11, -15, -12, -9, -16, -7, -14}
, {-1, -2, -6, -9, 0, 10, -28, -2, -30, -12, -23, 13, 0, 0, -16, -31, -18, 10, -25, 7, -3, -10, -5, 11, -13, 11, -10, 10, 2, -3, 1, 20}
, {-18, -15, -6, 9, -1, -16, -5, 14, 11, 6, -31, 7, -9, -8, -3, 3, 16, -1, -3, -1, -18, -3, -6, -9, -5, 14, 4, 9, 1, -10, -13, -16}
}
}
, {{{-25, 1, 4, -24, 4, 6, -22, -9, -5, -10, -1, -5, -2, -1, -29, 7, -5, 2, 6, -25, -10, 4, 8, -15, 1, 0, -9, 12, 2, 11, -4, -5}
, {14, 3, -7, -16, 2, -11, -26, -4, 8, 19, -17, -16, 5, 2, -8, -3, 3, -21, -1, -4, -18, -21, 5, 3, -3, -8, -4, -6, -26, -9, 21, 0}
, {-7, 5, 10, -28, 0, 10, -20, 11, 2, -2, 15, -17, -6, 6, -20, 10, 9, -16, -19, -28, -5, -3, 7, 2, 5, 8, 4, 7, 10, -5, -2, 9}
}
, {{0, -5, -14, -9, -21, -2, 1, 1, 0, 0, 2, 0, 19, 10, 1, -3, -28, -18, -24, -5, -8, -37, -17, 14, 2, 10, 3, -14, 2, -31, -6, 8}
, {29, 20, 20, -24, 14, 2, -32, -11, -44, 7, 6, -19, -27, -8, -15, -9, -26, 6, 12, -22, -5, -16, 11, -2, 13, -25, -31, -2, 1, -7, 13, 18}
, {0, -2, -5, -11, 3, 2, -19, 26, -4, 12, -2, 9, 1, -9, -16, -2, 15, -19, -22, -4, -4, 11, 0, -3, -14, -6, 2, -26, 4, 0, 1, -7}
}
, {{-5, 2, -10, -8, -5, 12, -23, 5, -16, 0, 3, -17, 5, -12, 7, -2, -3, -3, 16, -9, 22, -20, 12, -16, -3, 6, -12, -1, 4, -13, 3, -11}
, {-14, -11, 1, -9, -17, 1, -28, 3, 2, -15, 8, 15, -1, 10, -5, 1, 4, -14, -7, 10, 13, -19, -4, -15, 7, -14, -5, 5, 4, -13, 10, 3}
, {4, -16, -14, 4, 8, 14, -2, -5, -12, 23, -2, 6, -16, 10, 23, -6, 13, 1, 16, 12, 10, 12, -4, 10, 0, -3, 7, -9, -2, -37, 17, -7}
}
}
, {{{-13, -30, -12, 11, -36, -2, -15, -24, -3, -1, -4, 3, 5, -4, 11, 12, -6, 10, 16, 20, 3, -7, -2, 4, 16, -5, 11, -7, 16, 7, -9, 2}
, {8, 0, -14, -4, -7, 3, -5, 7, -33, 6, 15, -5, -7, -5, 16, -7, -5, 13, -10, -10, -18, -5, -9, 20, -1, -15, -9, -3, -44, 7, 1, -7}
, {6, 1, -4, -20, 4, 13, -9, 6, -5, -26, 9, 3, 9, 5, -21, -2, -7, 11, -3, -29, -11, -2, -3, -10, 7, 13, -4, -1, 8, -14, 12, 5}
}
, {{-15, -21, -2, 8, -39, -10, 7, -22, -3, -25, -5, 10, 2, 13, 17, -2, -6, 5, -24, 15, -3, 5, -12, 1, 17, -5, 4, -12, 6, -44, -2, -2}
, {14, 5, -6, 16, 0, 24, -9, -2, -13, 25, -5, -13, -8, -13, 17, -15, -40, -3, 16, 12, -11, -16, -1, 23, 10, -26, -36, -12, -9, -9, 13, 5}
, {3, 11, 8, -8, 10, 6, 1, -4, -5, 1, 19, -14, -3, 6, -2, 1, 4, 10, -20, -11, -25, -3, -22, -26, 22, -5, -5, -13, 17, 4, 1, -26}
}
, {{-19, -12, -1, 10, -11, -5, -6, -18, -3, -9, 1, -4, -8, -7, 5, 7, -14, -8, -10, -2, 6, -20, -22, 7, 4, 16, -11, 19, 14, -29, 3, -10}
, {-4, -7, 0, 2, -7, 10, -14, -12, -33, -13, -3, -2, -4, -8, -8, -17, 9, -6, -2, -9, 5, 0, 8, 6, -8, 16, -15, -10, 10, -6, -2, 12}
, {0, -11, -13, -8, -19, 2, -4, 20, -13, 21, 5, -8, -6, 6, -1, 3, -10, -1, -8, -6, -5, -25, -19, -1, 0, -6, -10, -20, -21, -13, 15, 5}
}
}
, {{{-42, -26, 18, 16, 3, -17, -11, -12, 9, -31, 7, 21, -2, 4, 2, 9, -14, -24, -3, 21, 4, -12, -17, -4, 6, 8, -8, 4, 1, 2, -8, -13}
, {-25, -5, -2, 20, 6, -3, 3, -9, 12, -6, 0, 2, -6, -19, 13, 7, 14, -5, 10, -4, 11, 15, 4, 7, -3, -23, -9, 2, -13, 8, 6, -8}
, {-1, -19, -11, -1, -1, -6, -14, -8, -20, -12, -2, 3, -4, -6, 8, -13, 12, 6, 9, -1, 14, 5, 12, -24, -21, -22, -5, -6, -5, -13, 13, -6}
}
, {{4, -3, 5, 6, -2, -3, -14, -2, -9, 2, 6, 13, 10, 2, -3, 1, -10, -10, -4, -22, -16, -13, -7, 6, 5, 2, 7, -10, -5, 0, -18, -1}
, {0, 12, -5, -12, 7, 6, 7, 7, 13, -17, 14, -8, 9, 17, -13, 12, -28, -7, -20, -17, -16, 12, 9, 6, -12, 6, -2, -4, -13, 3, -21, 18}
, {13, 19, -10, -15, 7, 4, 3, 6, -2, 7, 11, -39, 1, 19, -28, -15, -10, 0, -10, -14, 2, 11, 7, 14, -10, -13, 8, -10, -8, 12, -4, 6}
}
, {{-3, -6, -1, -1, -4, 7, 4, 13, -7, 5, -1, -8, 5, -3, -2, 15, -18, -25, -9, 3, 5, -8, -5, 3, -4, 10, -11, 4, 9, -1, -24, -1}
, {-33, 0, -7, 3, -29, -17, 1, 0, 8, -1, 7, 2, 7, 7, 16, 10, -24, -17, -8, 30, 16, -14, -21, 23, 3, -4, 14, -9, -25, -7, -7, -2}
, {-15, -20, -34, 12, -45, -15, -1, 6, -28, -16, -12, -3, -18, 20, -11, -8, -14, -4, -17, 12, 22, 11, -1, 15, -6, 21, 2, -24, 4, -11, 4, -2}
}
}
, {{{-28, -15, -7, 1, 5, -24, -9, 7, -9, 5, -14, 5, -16, -13, 2, -14, 13, -9, -23, -2, -14, -7, -10, -6, -6, 0, 2, -5, 6, -1, -15, -3}
, {-19, -31, -18, 1, -3, -11, -9, 9, 4, -1, -12, 2, 2, -13, 18, -13, -20, 5, 6, 1, 19, -18, -3, 6, 1, -20, 7, 4, -28, 21, -11, -6}
, {-14, -32, 2, 3, -8, -4, -15, -6, -1, 4, -8, 7, -24, -7, 9, -8, 7, -9, -12, 16, 11, 5, -13, 1, 9, 2, 12, 9, 2, 3, -5, -5}
}
, {{0, -2, -34, -16, -8, 9, -1, -1, -18, 5, 2, -7, -10, -4, -8, -9, -1, -9, 2, -1, -29, -18, -11, 2, 2, 10, -12, -21, -2, 7, -16, 3}
, {-5, 3, -8, -18, -3, 15, -12, 10, -12, 25, 7, -16, -1, -3, 8, -10, -4, -17, -7, -19, -13, -24, -5, 5, -6, -3, -15, 1, -14, 5, -5, 10}
, {3, 4, -22, -41, -5, -3, 0, 11, 8, 29, 15, -22, -19, 19, -3, -10, 31, 0, 16, -5, 16, 1, -3, 7, 22, -9, 20, -25, 20, 16, 21, 8}
}
, {{12, 1, -31, -2, -23, 5, 3, -5, -4, 7, 6, -32, -2, 10, 8, -13, -14, 11, -7, 7, -5, 0, 11, 9, 6, -25, 4, -7, -11, -18, -7, -2}
, {7, 13, -8, -23, 12, 8, -9, 14, -16, -2, 21, -8, -18, 9, -15, -26, -11, -2, -6, -10, 8, -23, 5, 17, 10, 20, -19, -10, 0, -16, -11, 9}
, {-7, 6, -6, 4, -16, -1, -19, 3, 11, 15, 17, 1, -12, 28, -3, -2, 16, 0, -6, -7, 28, -19, 10, -31, 19, 9, -14, 10, 7, -8, 22, 13}
}
}
, {{{12, 2, -14, 2, 0, -5, 0, -10, 1, 0, -8, 13, -2, -13, -9, 3, 14, 14, 10, -14, 18, -1, 16, -14, 0, -9, 5, 4, 25, -3, 9, -10}
, {5, 7, -3, 19, 22, 2, 8, -8, 18, -18, -9, 7, -6, 8, -24, -2, -5, 8, 11, -32, -1, 12, 15, -5, -13, -5, 18, 7, 7, 16, 11, -11}
, {-1, 0, -4, 6, -6, -6, 0, -6, 7, 3, -14, -6, 16, 10, 5, 10, -10, 9, -10, 11, -7, 13, -8, -10, -21, -10, 25, -1, -2, -1, -11, -6}
}
, {{-16, -17, 3, -9, 2, -14, -12, -9, 3, 7, -3, 6, 9, 9, 5, 7, -3, -16, -6, 10, 2, -19, 18, 2, 27, -14, 1, -13, 8, -15, 1, 12}
, {-18, -9, -11, 17, -16, 2, 6, -26, -21, -9, 14, 19, 1, -8, -18, -27, -4, -2, -17, -12, -13, 13, -9, 24, -29, -5, -7, 4, -10, 4, -11, -23}
, {3, -7, -27, 6, -15, 4, 6, 7, -10, -15, -2, 19, 15, 24, 9, -5, -26, 3, -28, 0, -24, -6, -19, 27, -20, 5, -1, -15, -17, 4, 23, 0}
}
, {{-17, -19, 0, -17, 7, -2, 5, -5, 5, -23, 4, -28, -7, 12, -9, -3, 18, 1, -2, 6, 10, 1, 10, -20, 14, -6, 6, 7, 0, 15, -1, -20}
, {-13, -28, -13, 7, 20, 18, 15, 8, -1, -10, -13, 17, -24, 6, -7, 2, 13, -29, -11, 12, 15, 7, -5, -7, -2, 6, -6, -11, -3, -14, 9, -15}
, {-9, -19, -21, 13, -20, -11, 12, -3, -7, -9, -9, 0, -6, 5, 2, -7, 29, 14, 5, 6, -12, 3, -7, -10, -13, 2, -15, -11, -14, -10, 19, -8}
}
}
, {{{11, -8, -2, 6, 6, 15, -6, 19, -17, 1, 12, 8, -14, -4, 3, 2, 8, -11, -17, -1, -4, 13, -8, 7, -4, 11, -2, -9, 10, -5, -14, -4}
, {-27, -26, -15, 3, -22, -10, 2, -21, 22, -12, -13, -4, 23, -2, 15, 12, 2, 0, -8, 29, 31, -23, -14, -4, -26, -14, 0, 15, -14, -33, 1, 1}
, {-13, -25, -19, 4, 2, -5, -10, 12, -4, -14, -8, 3, -18, -14, 7, -1, 7, 2, 0, 12, 2, 6, -25, 12, -1, -9, 4, -10, -3, 12, -16, -14}
}
, {{8, 7, -13, -26, 17, 7, -5, -2, -6, -1, -6, 0, 14, -13, -16, -3, -2, -20, -4, -25, -22, 4, -17, -6, -25, -11, 0, -16, -3, 11, 1, 0}
, {-12, 2, 21, -21, 5, 2, -36, 12, 9, -2, -24, -24, -2, 8, -15, 0, -15, -44, -10, -13, -15, -11, -4, -19, -14, -29, -8, 9, -9, 14, -9, 1}
, {-15, -13, -21, -22, -12, -11, -22, 5, -5, 7, -34, 6, -4, 5, -3, -4, 20, -23, -12, -6, -2, 3, -13, -1, -23, 14, -6, 7, 10, -34, 9, 8}
}
, {{1, 9, -7, 10, -18, 7, -3, -5, 2, -3, 5, -14, 18, 1, -9, -2, -8, 3, 0, -7, -12, -1, -5, 11, -2, 2, -4, 0, -22, -6, 2, 21}
, {-15, -5, -5, 12, -14, 0, 10, -22, -12, -26, 0, 21, 12, -2, -10, -6, -1, -3, 3, -7, -5, 18, -17, 4, -16, 4, -13, -10, 1, -15, -4, 8}
, {-8, 7, -1, 8, 9, 6, -2, 17, -5, 7, -1, 12, -11, -8, -8, -21, -7, 9, -1, -12, -5, -1, 6, -14, 4, -9, -7, 3, -8, 17, -1, -5}
}
}
, {{{13, -5, -6, -13, -5, 4, -2, 4, -7, 6, 6, -2, -27, 10, -26, -17, -5, -6, -3, -19, -20, -16, -31, 3, -8, 10, 0, 11, -5, -8, 0, -9}
, {-5, -24, -17, 3, 2, -19, 8, 11, 1, 16, -18, -14, -16, -3, 20, -12, 21, 13, 16, -14, 11, -9, -6, -18, -8, -2, -5, 18, 5, -14, 16, 5}
, {15, -2, -19, -7, -4, 0, -19, -20, 5, 2, -3, 16, 3, -7, -6, 11, -5, -3, 21, 24, -12, -8, 2, -1, -4, 14, 1, 27, 19, -15, 2, 9}
}
, {{-11, 4, -3, -8, 11, 3, 19, -1, -10, -12, -11, -1, -9, 8, 5, -22, 3, -17, -11, -6, 0, -19, -3, 8, -9, -10, -18, -32, 0, -5, 19, -9}
, {6, 9, 4, -8, 27, 17, -10, 23, -13, 19, 10, -17, 19, 11, 25, -9, -35, -11, 19, 8, 6, 4, -1, -11, 19, -14, 11, -2, -9, 4, -14, 20}
, {-9, 3, 15, -14, 12, 2, -10, 8, 8, 6, 1, -11, -6, -6, -18, 21, 19, -4, 0, -14, 1, 11, 16, -12, 7, 6, -3, 25, 2, 9, 15, -5}
}
, {{-8, -25, 9, -2, 4, -8, 0, -2, 10, 7, -22, -2, -10, 6, -20, -1, 3, -1, -17, -2, -10, 10, -24, 8, 16, -1, -6, 23, -6, 14, -13, -1}
, {3, -8, -7, -2, -54, -8, 8, -9, 7, 12, 0, -5, 2, -17, 3, 11, 9, 0, -15, 3, -21, -2, 1, 10, 0, 22, -5, -19, 1, -25, -27, -7}
, {3, -13, -21, -5, -4, -5, -6, -7, -14, -8, -11, -18, 14, 12, 10, -29, 9, 0, -16, 11, -32, -10, -7, -2, -3, 10, 9, -33, 21, 0, 5, 1}
}
}
, {{{-4, 21, 17, -5, -1, 6, -13, -15, -18, 0, 7, 4, -7, 3, 9, 6, 1, -7, -6, 6, -20, -4, -8, 20, 17, 26, 7, 7, 12, 11, -1, 3}
, {25, 15, -8, -1, 2, 12, 16, -17, 8, -9, -2, -29, 14, 13, -12, 11, -10, -22, 0, 2, -39, 11, 9, 25, -10, -8, 10, -1, 1, -15, 5, 6}
, {9, 1, -6, -27, 2, -6, -5, 15, -14, 10, 7, -17, -28, 7, -24, -1, -7, -10, -29, -22, -25, 1, 3, 18, 4, -1, -17, -2, -1, 2, -3, -4}
}
, {{-6, 1, 4, -11, -12, 2, -1, -14, 6, -2, -4, -19, 6, -8, 12, 12, -13, -15, -14, 11, -3, -20, 4, 1, 3, 17, 30, -19, 13, -4, 0, 9}
, {-9, -16, 13, -2, -21, -11, -13, 10, -5, 1, 3, -7, -31, -5, 3, 9, 22, -32, -10, 12, -5, 0, -20, -2, -2, 11, 13, -2, 11, 3, -9, -7}
, {-12, -26, -30, 12, -17, -31, 6, 19, -25, 7, -10, -15, 2, 5, 5, -22, 19, -17, -20, 10, 24, 4, 1, 15, 8, -25, 7, -10, 22, -8, 26, -11}
}
, {{3, 2, -4, 6, 9, 10, -2, -17, 11, -17, 13, 6, -13, 2, 8, 2, 7, 19, 4, 3, 16, -2, 12, -11, 5, -5, 10, 3, 1, -9, 10, 8}
, {-3, -15, 5, 5, -1, 5, -2, 13, 16, -4, -21, 6, -14, -11, 5, 26, 14, -8, -4, -3, -28, 0, -13, 2, -3, 9, 0, -8, -21, 1, 9, -8}
, {-12, -9, -16, 2, -8, -17, -1, 6, 1, 3, 4, -3, 5, 4, 14, 0, -10, -19, 0, 2, 9, 0, -11, 2, 11, -26, 1, -8, 12, 6, 9, -1}
}
}
, {{{16, 10, 9, -17, 6, 28, 0, -9, -19, -20, 19, -9, 0, 8, -10, 8, -12, -9, 2, 5, -8, 7, 0, -3, 3, -1, -3, -18, 4, -20, 17, -6}
, {-8, 1, -9, -6, 10, 12, 6, -10, -5, -17, 1, -10, 13, 7, -10, -24, -16, 7, -12, -2, -7, -6, -13, 7, -20, 12, -8, -28, -38, -5, 28, 13}
, {-6, 2, 4, -18, -3, 4, -18, 1, -16, -18, 0, -5, 9, 1, -11, -5, -4, -2, -4, 0, -3, 11, 3, 4, 14, 8, -17, -6, 3, -3, 8, 9}
}
, {{-13, -12, 16, 16, 3, -17, 19, 2, 8, -11, -22, -6, 2, -25, 1, 8, 4, -27, -11, -4, -16, 11, -23, 10, -8, -13, 17, 0, 0, 23, -18, -44}
, {-7, -31, -4, 17, 9, -18, -4, 11, -18, -6, -31, 25, -19, -30, -2, 8, -5, -23, -7, -4, -32, -20, -16, -9, -24, -6, -20, -2, -7, 21, -22, -31}
, {-21, 2, 2, 5, 14, -23, -6, 10, -17, -3, -5, 7, 2, 1, -1, -12, -5, -21, -11, -11, -14, -11, -28, -5, -9, 4, -18, -7, -8, 1, -4, -8}
}
, {{2, -2, -10, -5, 3, 8, -5, -20, -4, -13, 16, -13, -12, 13, 1, 0, 5, -7, 0, 3, -13, 2, -1, 2, 17, 13, 2, -7, 10, -25, 9, 5}
, {8, 1, 6, -10, -19, 6, 13, -6, -2, -15, 7, -2, -27, -10, 5, -13, 8, 6, 8, 11, 4, -22, -14, 16, 2, 19, -25, -7, -15, -24, -12, -9}
, {9, 1, -17, 6, -16, 2, -11, -8, -19, 5, -4, -3, -26, -1, 8, -19, 10, -6, -3, -11, 20, -14, -19, 14, 6, -1, -16, -1, -8, 8, 10, -9}
}
}
, {{{-10, -36, 3, 7, -6, -19, -7, 18, -6, 22, -9, 9, -11, -2, 8, -4, 18, -2, 9, 8, 7, 10, 0, -23, 10, -4, -9, 6, 7, 3, -23, 5}
, {-1, -26, -23, 14, -27, -15, 16, -19, -7, -19, -14, 27, -30, 18, 24, -5, -14, 1, 2, 19, -8, -2, -25, 23, -18, 5, -3, -4, 18, -4, 16, -33}
, {3, -5, -16, 9, -3, -7, -10, -2, 5, 0, -21, 6, -27, 2, 14, 2, 4, 5, 15, 3, -11, -7, -1, -5, -11, -6, 16, 23, -15, -16, 22, -30}
}
, {{-4, -21, -35, -11, -15, 9, -6, -13, 5, -3, -8, -5, 4, 14, 2, -14, -9, -12, 3, 10, 12, -11, -18, 1, 6, -6, 18, -8, 5, -14, 16, 2}
, {11, 3, -9, -16, -9, -9, 2, -23, -9, -4, -2, -14, -13, -14, 3, -18, -18, 13, 22, 7, -5, -12, -3, 18, -1, -12, 14, 14, 17, -21, 7, 15}
, {22, 6, -9, -3, 0, -10, -11, 7, 6, 16, 8, 3, 4, -5, -4, -4, -11, 11, -1, -8, -28, -7, 10, -11, -10, -11, 5, 3, 11, -1, 20, 0}
}
, {{-4, -5, 11, 0, -4, -11, 4, 8, -5, -2, -10, -6, -7, -15, 7, 1, -1, -10, 6, -14, -7, -3, -11, -22, -14, 3, -16, -2, -11, 1, 6, -24}
, {-3, 3, -8, -1, -3, 5, -6, -10, 5, -4, -9, -4, 25, 19, 6, 17, -30, -24, 15, -6, -5, -15, -22, -26, 16, 0, -1, 2, -10, 0, 11, 4}
, {6, 9, -1, -13, -4, 9, -5, 12, -22, 11, -5, -13, -5, 17, -1, -12, 12, -12, -6, -3, -12, -14, -2, -6, 2, 3, 0, 0, -10, 5, 14, 24}
}
}
, {{{11, 19, 8, -7, 0, 7, 16, -6, -8, -1, 4, -7, 9, -15, -9, 2, -6, 0, 15, -14, -3, 13, 9, 5, -1, -15, -10, 4, -13, -1, -2, 5}
, {1, 8, -8, -11, -4, -1, -11, 16, 2, 8, -4, -31, 15, -8, -22, 16, -6, -16, -21, -22, -6, -16, -30, -12, 3, -1, 4, -7, -20, 1, 2, 21}
, {0, -8, -3, 5, 7, -4, -4, 6, 3, -10, 2, 0, -10, 18, 2, 0, -3, -1, -4, 12, 12, 0, 13, 19, -2, -6, 10, 3, 6, 3, 14, 14}
}
, {{1, 13, 13, -24, -12, -6, -4, 5, 3, 17, -11, -29, 4, -10, -5, 9, -1, -21, -13, -5, -9, -17, -20, 16, 0, -6, -13, 15, -8, -10, -4, 12}
, {-26, 11, -26, -41, -12, -12, 2, -14, -35, -6, 24, 0, 17, 13, -3, -25, -20, -1, -12, -4, 6, -12, 2, -7, -4, 28, -8, -30, 7, -52, -8, 28}
, {-15, -4, -16, -1, -2, -11, -5, -14, -10, 1, -7, -11, -8, -2, -21, -7, -4, -6, -21, -14, 10, 0, -13, -1, 4, -16, -4, -4, 9, -10, -3, 2}
}
, {{-4, -9, 16, -10, 14, 15, -6, -5, -18, -17, 0, -5, -33, 10, -10, 2, -13, -5, 5, 7, 15, -2, 3, -24, 11, 4, -2, 6, 2, -13, 8, -4}
, {-30, -29, 14, 14, -45, -26, 4, -6, -14, 13, -2, 20, 0, 4, -2, 2, 19, -15, 6, 10, 0, -12, -6, -6, -10, 2, -4, -3, -17, -16, 8, -33}
, {8, 11, -18, 5, 2, 16, -17, -16, -3, 4, -12, 6, 11, 28, 10, -13, -2, -1, 8, -6, 6, -15, -20, -3, -11, 0, 6, -19, -4, 1, 27, 6}
}
}
}
;

#undef INPUT_CHANNELS
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_GROUPS
/**
  ******************************************************************************
  * @file    batchnorm2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _BATCH_NORMALIZATION_2_H_
#define _BATCH_NORMALIZATION_2_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      64
#define INPUT_HEIGHT        3
#define INPUT_WIDTH         3

typedef int16_t batch_normalization_2_output_type[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS];

#if 0
void batch_normalization_2(
  const number_t input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],  // IN
  const number_t kernel[INPUT_CHANNELS],                // IN
  const number_t bias[INPUT_CHANNELS],                  // IN
  batch_normalization_2_output_type output);                // OUT
#endif

#undef INPUT_CHANNELS
#undef INPUT_HEIGHT
#undef INPUT_WIDTH

#endif//_BATCH_NORMALIZATION_2_H_
/**
  ******************************************************************************
  * @file    batchnorm2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "batch_normalization_2.h"
#include "number.h"
#endif

#define INPUT_CHANNELS      64
#define INPUT_HEIGHT        3
#define INPUT_WIDTH         3
#define ACTIVATION_LINEAR

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void batch_normalization_2(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],  // IN
  const NUMBER_T kernel[INPUT_CHANNELS],                // IN
  const NUMBER_T bias[INPUT_CHANNELS],                  // IN
  batch_normalization_2_output_type output) {                // OUT

  LONG_NUMBER_T tmp;

  for (size_t y = 0; y < INPUT_HEIGHT; y++) {
    for (size_t x = 0; x < INPUT_WIDTH; x++) {
      for (size_t z = 0; z < INPUT_CHANNELS; z++) {
        tmp = (LONG_NUMBER_T)input[y][x][z] * (LONG_NUMBER_T)kernel[z];

        // Scale for possible additional precision of bias
        tmp = scale(NUMBER_T, tmp, WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        // Scale bias to match accumulator
        tmp += scale(NUMBER_T, (LONG_NUMBER_T)bias[z], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Activation function
#ifdef ACTIVATION_LINEAR
        // Linear (MEANS NONE)
        output[y][x][z] = scale_and_clamp_to(NUMBER_T, tmp, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // ReLU
        if (tmp < 0) {
          output[y][x][z] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
          if (tmp > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
            tmp = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
          }
#endif
          output[y][x][z] = scale_and_clamp_to(NUMBER_T, tmp, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
}

#undef INPUT_CHANNELS
#undef INPUT_HEIGHT
#undef INPUT_WIDTH
#undef ACTIVATION_LINEAR
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
/**
  ******************************************************************************
  * @file    weights/batchnorm2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0
  * @date    26 june 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

const int16_t batch_normalization_2_bias[64] = {-65, -42, -46, -63, -51, -68, -76, -75, -70, -80, -34, -79, -46, -35, -88, -41, -100, -63, -80, -64, -57, -39, -72, -80, -39, -104, -66, -40, -83, -89, -90, -72, -64, -58, -62, -103, -53, -62, -59, -65, -109, -76, -29, -38, -23, -71, -61, -67, -83, -51, -68, -99, -33, -70, -68, -72, -64, -79, -58, -50, -28, -52, -62, -102}
;
const int16_t batch_normalization_2_kernel[64] = {107, 123, 105, 101, 169, 100, 100, 137, 119, 113, 102, 133, 99, 128, 102, 113, 143, 125, 96, 126, 118, 75, 116, 122, 104, 120, 99, 115, 91, 135, 123, 111, 135, 87, 147, 107, 127, 97, 139, 92, 111, 109, 138, 83, 127, 108, 129, 98, 136, 113, 115, 110, 116, 118, 148, 96, 114, 96, 118, 91, 104, 131, 96, 106}
;
/**
  ******************************************************************************
  * @file    conv2d.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    14 december 2022
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _CONV2D_3_H_
#define _CONV2D_3_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define INPUT_CHANNELS      64
#define INPUT_HEIGHT        3
#define INPUT_WIDTH         3
#define CONV_FILTERS        128
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )


typedef int16_t conv2d_3_output_type[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS];

#if 0
void conv2d_3(
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

#endif//_CONV2D_3_H_
/**
  ******************************************************************************
  * @file    conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "conv2d_3.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_CHANNELS      64
#define INPUT_HEIGHT        3
#define INPUT_WIDTH         3
#define CONV_FILTERS        128
#define CONV_KERNEL_SIZE_Y  3
#define CONV_KERNEL_SIZE_X  3
#define CONV_STRIDE_Y       2
#define CONV_STRIDE_X       2
#define CONV_GROUPS         1
#define CHANNELS_PER_GROUP  (INPUT_CHANNELS / CONV_GROUPS)
#define FILTERS_PER_GROUP   (CONV_FILTERS / CONV_GROUPS)

#define ZEROPADDING_TOP    0
#define ZEROPADDING_BOTTOM 0
#define ZEROPADDING_LEFT   0
#define ZEROPADDING_RIGHT  0

#define CONV_OUTHEIGHT     ( ( (INPUT_HEIGHT - CONV_KERNEL_SIZE_Y + ZEROPADDING_TOP + ZEROPADDING_BOTTOM) / CONV_STRIDE_Y ) + 1 )
#define CONV_OUTWIDTH      ( ( (INPUT_WIDTH - CONV_KERNEL_SIZE_X + ZEROPADDING_LEFT + ZEROPADDING_RIGHT) / CONV_STRIDE_X ) + 1 )

#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void conv2d_3(
  const NUMBER_T input[INPUT_HEIGHT][INPUT_WIDTH][INPUT_CHANNELS],               // IN
  const NUMBER_T kernel[CONV_FILTERS][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y][INPUT_CHANNELS / CONV_GROUPS], // IN

  const NUMBER_T bias[CONV_FILTERS],						                // IN

  NUMBER_T output[CONV_OUTHEIGHT][CONV_OUTWIDTH][CONV_FILTERS]) {               // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short pos_x, pos_y, z, k; 	// loop indexes for output volume
  unsigned short x, y;
  int input_x, input_y;
  LONG_NUMBER_T	kernel_mac;
  LONG_NUMBER_T tmp;
  static LONG_NUMBER_T	output_acc[CONV_OUTHEIGHT][CONV_OUTWIDTH];

  for (k = 0; k < CONV_FILTERS; k++) { 
    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        output_acc[pos_y][pos_x] = 0;

        for (z = 0; z < INPUT_CHANNELS / CONV_GROUPS; z++) {
          kernel_mac = 0; 
            
          for (y = 0; y < CONV_KERNEL_SIZE_Y; y++) {
            input_y = pos_y * CONV_STRIDE_Y - ZEROPADDING_TOP + y;

            for (x = 0; x < CONV_KERNEL_SIZE_X; x++) {
              input_x = pos_x * CONV_STRIDE_X - ZEROPADDING_LEFT + x;

              if (input_x < 0 || input_x >= INPUT_WIDTH || input_y < 0 || input_y >= INPUT_HEIGHT) // ZeroPadding2D
                tmp = 0;
              else
                tmp = (LONG_NUMBER_T)input[input_y][input_x][z + (k / FILTERS_PER_GROUP) * CHANNELS_PER_GROUP] * (LONG_NUMBER_T)kernel[k][y][x][z];
              kernel_mac = kernel_mac + tmp;
            }
          }

          output_acc[pos_y][pos_x] = output_acc[pos_y][pos_x] + kernel_mac;

        }
      }
    }

    for (pos_y = 0; pos_y < CONV_OUTHEIGHT; pos_y++) { 
      for (pos_x = 0; pos_x < CONV_OUTWIDTH; pos_x++) { 
        // Scale for possible additional precision of bias
        output_acc[pos_y][pos_x] = scale(NUMBER_T, output_acc[pos_y][pos_x],  WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

        // Scale bias to match accumulator
        output_acc[pos_y][pos_x] += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


#ifdef ACTIVATION_LINEAR
        output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
        // Activation function: ReLU
        if (output_acc[pos_y][pos_x] < 0) {
          output[pos_y][pos_x][k] = 0;
        } else {
#if defined(ACTIVATION_RELU6)
        if (output_acc[pos_y][pos_x] > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
          output_acc[pos_y][pos_x] = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
        }
#endif
          output[pos_y][pos_x][k] = scale_and_clamp_to(NUMBER_T, output_acc[pos_y][pos_x], INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
        }
#else
#error "Unsupported activation function"
#endif
      }
    }
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_HEIGHT*INPUT_WIDTH*INPUT_CHANNELS];
#ifdef WITH_CMSIS_NN
  arm_convolve_HWC_q15_basic_nonsquare(
#elif defined(WITH_NMSIS_NN)
  riscv_convolve_HWC_q15_basic_nonsquare(
#endif
                                      (q15_t*)input, //Im_in
                                      INPUT_WIDTH, //dim_im_in_x
                                      INPUT_HEIGHT, //dim_im_in_y
                                      INPUT_CHANNELS, //ch_im_in
                                      (q15_t*)kernel, //wt
                                      CONV_FILTERS, //ch_im_out
                                      CONV_KERNEL_SIZE_X, //dim_kernel_x
                                      CONV_KERNEL_SIZE_Y, //dim_kernel_y
                                      ZEROPADDING_LEFT, //padding_x, left and right must be equal
                                      ZEROPADDING_TOP, //padding_y, top and bottom must be equal
                                      CONV_STRIDE_X, //stride_x
                                      CONV_STRIDE_Y, //stride_y
                                      (q15_t*)bias, //bias
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR, //bias_shift
                                      INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, //out_shift
                                      (q15_t*)output, //Im_out
                                      CONV_OUTWIDTH, //dim_im_out_x
                                      CONV_OUTHEIGHT, //dim_im_out_y
                                      bufferA, //bufferA
                                      NULL //bufferB, unused
                                      );
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, CONV_FILTERS * CONV_OUTHEIGHT * CONV_OUTWIDTH);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_CHANNELS
#undef INPUT_WIDTH
#undef INPUT_HEIGHT
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_STRIDE_X
#undef CONV_STRIDE_Y
#undef CONV_GROUPS
#undef CHANNELS_PER_GROUP
#undef FILTERS_PER_GROUP
#undef ZEROPADDING_TOP
#undef ZEROPADDING_BOTTOM
#undef ZEROPADDING_LEFT
#undef ZEROPADDING_RIGHT
#undef CONV_OUTWIDTH
#undef CONV_OUTHEIGHT
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef TMP_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/conv2d.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_CHANNELS     64
#define CONV_FILTERS       128
#define CONV_KERNEL_SIZE_Y 3
#define CONV_KERNEL_SIZE_X 3
#define CONV_GROUPS        1


const int16_t conv2d_3_bias[CONV_FILTERS] = {-5, -12, -7, -19, -10, -10, -9, -16, -10, -4, -11, -10, -6, -6, -17, 5, -18, -10, -9, -9, -14, -6, -9, -20, -6, -9, -6, -15, -4, -6, -8, -15, -18, -24, -17, -9, 1, -12, -8, -7, 6, -9, -12, -1, -19, -16, -18, -2, -1, -17, -11, -11, -10, -7, -3, -18, -9, -13, -18, -16, 1, -9, -17, -16, -10, -8, -9, -11, -16, -1, -16, -10, -14, -6, -7, -15, -11, -11, -10, 6, -6, -12, -8, -9, -9, -17, -14, -12, -1, -23, -18, -13, -7, -21, 6, -3, -10, -12, -1, -6, -15, -9, -7, -12, -11, -1, -10, -4, -8, -7, -17, -10, -8, -8, -16, -10, -7, -18, -9, -9, -8, -11, -18, -10, -18, -7, -13, -11}
;


const int16_t conv2d_3_kernel[CONV_FILTERS][CONV_KERNEL_SIZE_Y][CONV_KERNEL_SIZE_X][INPUT_CHANNELS / CONV_GROUPS] = {{{{1, -18, -13, 8, -1, -12, 7, -11, -3, 2, -3, -1, -10, -5, 7, -5, 0, -12, -8, 0, 3, -7, -9, -14, -21, -19, -12, -13, 4, -3, 3, -16, -8, 5, -2, 16, 4, 16, -1, -6, 21, -10, -29, -7, -26, -6, -7, 15, 4, -14, 12, 2, -10, -2, -15, -9, 7, -14, 11, -17, -5, 17, -5, -9}
, {-7, -3, 1, 1, -17, -2, 3, -2, -1, 10, 0, -12, -6, 3, -3, -4, -11, 8, -5, 12, 7, 0, 16, 13, -8, -3, 7, -11, 9, -13, 9, 4, -1, 18, 7, 4, 9, -8, 17, 2, -8, 7, 1, -4, -3, -6, -5, 15, -9, -4, -8, -8, 2, -19, 11, -1, 9, -10, 3, 14, -4, 1, -4, -11}
, {6, -3, 5, 8, -21, 1, -15, -10, 8, -1, -5, -3, 13, -7, 3, -12, 1, 8, 17, 7, -2, -2, -10, -21, 1, 8, -19, 0, 1, 3, 7, -4, 12, -2, 6, 2, 10, 4, 9, -10, -2, -15, 2, 8, 5, -4, 15, 11, 0, -14, 3, -7, -1, 0, 9, 12, 11, 9, 3, 0, 1, 12, -4, -4}
}
, {{20, -17, 0, 4, -1, 9, -1, 0, -6, -10, 0, 3, -5, -9, 8, 4, 0, -23, 14, 0, -2, 13, -6, -9, -11, -12, 1, -10, -3, 6, -15, 2, 14, 2, 13, 17, 23, 7, 3, -3, 14, 6, -3, 3, 7, 9, 18, -6, 6, -6, 0, 3, 3, -14, 3, -8, -10, -10, -4, -3, -1, 7, 2, 9}
, {7, -7, -8, -4, -28, 5, -24, -24, -28, 18, 16, -3, 2, 5, -18, 4, 12, -19, 6, -13, 14, -7, -4, -11, -11, 23, 8, -2, 16, -9, -14, -13, -12, -24, -17, 10, 11, -17, -5, 12, 14, -21, 10, -14, 0, -11, -3, -20, 10, -23, -14, -2, 28, -7, 13, -7, 19, -1, -4, 6, -9, 8, 0, 4}
, {-10, -12, 16, -7, -24, -10, -3, -7, 3, 4, -16, 4, 5, -9, -1, -8, -5, -23, 7, 9, -20, -20, -5, -1, -5, -7, -19, -13, -16, -1, -4, 3, 10, 16, -2, -22, 11, 6, -10, -14, -5, -9, 7, -21, 18, 2, 7, -2, 3, -19, -4, -17, -3, -11, -9, -31, 7, 15, -10, -14, -2, -19, 5, -30}
}
, {{16, 7, 5, -5, 1, -10, -3, 6, -22, 8, 0, 2, -7, 11, 23, 3, 5, -6, 5, -1, -6, 11, -5, -1, 5, 14, -3, -12, 5, 4, -7, -19, -3, 5, 3, -2, 7, 10, 0, -12, 9, 0, -13, -8, 0, -28, 18, 1, 15, 5, 4, 4, 2, 9, -1, 15, -5, 9, -10, 4, 18, 5, 17, 3}
, {1, 14, -11, -11, 3, -13, 19, 8, 6, 13, -23, -3, -22, -14, -17, -24, -1, 5, 4, -4, -14, -16, -28, 4, -16, -10, 9, -44, -5, -1, 17, -12, 3, 9, -22, 1, -11, 6, -31, 20, -9, 17, -15, -16, -15, -4, 16, 26, 19, -1, -8, -25, -8, 4, -16, -24, -12, 21, 12, -12, 4, 23, -11, -1}
, {14, 1, 4, 4, -27, 11, 11, -12, -8, 11, -3, -3, 2, -6, -8, 4, -20, -9, -11, -5, 0, -4, 9, -8, 1, -25, -3, 6, 2, 8, -1, -14, 5, -1, -14, -9, 18, 1, 6, -22, 4, 1, -1, 0, 12, 9, -4, -7, 10, -5, 20, -10, 9, 11, -12, 12, 0, -5, 19, 8, 9, -2, -8, -17}
}
}
, {{{-1, -5, 5, 5, -10, 3, 7, 11, -9, 4, -7, -20, -4, -1, 13, -8, -1, -3, 11, 2, -3, -3, 10, -2, -5, 24, 8, 4, 6, -7, 4, -11, 0, -6, -4, 10, 8, -5, 7, 11, 17, 1, 9, 9, -17, -5, -4, 8, 8, 10, 4, 7, -8, 14, 10, 2, -3, 13, -4, -3, -7, 4, 3, 15}
, {5, -19, -5, -6, -19, 6, -14, 3, 6, 1, -26, -1, 10, -12, -2, 4, 13, -19, 17, 1, 12, 6, -3, -9, -23, -10, -5, -14, 11, -13, -24, -1, 4, -25, -11, 22, 4, 16, -13, -19, 11, -20, 1, -3, 5, -12, 4, -9, 12, -8, 9, 5, -12, 4, 11, -2, 10, 6, -12, -9, 0, -18, 3, 8}
, {-18, 1, 0, 3, 7, 4, -17, -6, -5, 19, 10, 21, -13, -17, -3, 10, 16, -1, 2, 0, 6, -14, -1, -3, -16, 5, -22, 4, -11, 1, -5, 3, 7, 11, -14, -6, -2, 3, 3, 5, 9, -27, 2, 0, -1, -11, -7, -14, 9, -4, 12, -22, 14, 21, 27, 5, 8, 7, -12, -3, -12, -8, -1, 19}
}
, {{-15, -3, 14, 2, -2, -1, 17, 21, -6, 1, -17, 6, -8, -11, 14, -7, 2, -10, -17, 6, 3, -5, -18, -3, 12, 19, 20, -17, 12, 14, 10, 3, 6, 7, 4, -8, 6, 14, -10, 6, 15, 6, -10, 2, -12, 9, -1, 6, -2, -7, 4, -14, 5, 5, 3, -1, -4, -5, -3, -7, -2, 12, -1, -3}
, {2, 13, -24, -25, -1, 12, -23, -4, -30, 14, -34, 0, 8, 23, 6, 8, 9, -37, -5, 10, -24, -12, -19, -8, -9, -13, -16, 25, 2, 5, -12, -31, 12, -35, 0, -5, 35, -29, -32, -27, -24, 8, 2, 3, -5, 13, 17, -10, 32, 9, 18, 47, -13, -22, -17, -15, -16, -8, -27, -11, 16, -27, 20, -16}
, {14, 3, 8, 4, -3, -4, 13, 17, 7, 9, -25, 23, 9, -2, 17, -14, -11, 17, -14, 0, -5, -11, 1, -19, -17, -9, 10, -25, 11, 16, -3, 3, 19, 10, 2, -2, 7, 5, -6, -1, 4, 9, -1, -1, -12, -5, 0, 0, -2, -8, 15, 12, -6, -17, -11, 10, -29, 6, 2, 15, -4, -5, 6, 7}
}
, {{8, 1, -6, 4, -2, 8, 15, 11, -12, -13, -5, -3, 2, 11, 13, -10, 8, 12, -7, 0, -2, -25, -2, 9, 2, 11, -4, -17, 0, 3, 25, 11, 5, 13, 14, -14, 1, 10, -25, -10, 2, 0, -14, 4, -2, 1, 10, 4, 0, -7, -7, -8, -15, 3, -28, 10, -17, 4, -4, -19, -2, 9, 5, 2}
, {6, 1, 0, -10, 22, -9, -15, -19, 3, 4, 8, -16, 15, -15, -7, -7, -13, 7, -5, 4, -9, -13, 27, -3, -17, -5, -6, -2, -1, 27, -8, -7, -15, 15, 27, -12, 14, -22, -5, -27, 1, -32, -11, 12, 5, 8, 2, -19, -11, -10, 22, -3, -21, 4, 25, 2, -1, -15, 8, -6, -2, 0, 12, -3}
, {-7, 3, 20, -10, -17, -3, -5, -5, 11, -1, 10, -2, 20, -16, -19, 5, 0, -22, 7, -7, -3, -6, -1, 1, -4, -5, -9, 22, -3, 17, -4, -6, -24, 10, -7, -13, 6, 17, 5, 3, -2, 18, -7, 6, -10, -9, -10, 11, -2, -7, -1, -26, 7, 0, -7, 14, 10, -12, 10, 17, 0, -28, 20, -28}
}
}
, {{{-20, 7, 11, 2, -6, -3, -19, -6, 5, 16, -2, -9, -8, -10, -7, -1, 6, -5, 9, -16, 0, 9, 9, 9, -8, -3, 0, -3, -2, 10, 17, -5, 4, 9, -2, -1, -14, -3, 8, -5, 24, 2, 1, -12, -17, -18, -16, -5, 6, -1, -5, -16, 1, -6, -16, -2, 0, 2, 24, 2, 1, -18, 1, 11}
, {1, 6, 11, 8, -2, -17, -12, 32, 2, 15, 7, 12, -3, -28, -5, 5, -3, -10, -11, 14, -1, 2, 13, 5, -15, 3, -9, 3, 9, 12, 0, -2, 27, -5, 4, -9, -9, 3, 8, 7, -12, -7, 7, -25, -2, -12, -5, 29, 4, -4, -5, 8, 2, -2, -13, 4, 4, 5, 9, -15, -21, -15, -10, 11}
, {12, 18, 4, 17, 4, -10, -1, 6, 5, 5, -7, 12, 3, -4, 4, -1, 23, -4, 29, -6, -3, -6, 5, 2, -6, 9, -8, -14, -12, -1, 1, 12, 4, -3, 3, 11, -2, -3, -2, 3, 5, 9, 0, -7, 6, 1, 13, 6, -5, 13, -17, -13, -9, 17, -17, 22, 0, -1, 11, -9, -4, -2, -16, 18}
}
, {{-4, -3, -4, -3, -13, 11, -20, -3, -24, 8, -12, 8, 9, 3, 6, -8, -7, -9, 9, 5, -14, 21, 0, -15, -7, 1, -1, -14, 7, -15, -3, 1, 13, 10, 18, -11, 10, 1, -6, -2, 23, -13, -4, -11, -1, -2, 16, -7, 1, 22, 19, 4, -14, -1, 27, 14, -15, -11, 1, 18, 3, 4, -6, 20}
, {1, -19, 13, -3, -5, -18, 18, 4, 0, 32, 26, 2, -24, -5, -22, 0, -8, 0, 12, 2, 2, 10, 1, 29, -9, -7, 2, -5, -6, -1, 13, -16, -2, -41, -28, 13, 10, -4, 3, 8, 0, 3, 2, -30, -5, -12, -14, 17, 8, -16, -11, 16, 22, -13, -15, -10, 14, -31, -2, -22, -12, -13, -22, 4}
, {-4, -14, 10, 10, 7, 22, 26, 1, -6, -16, -6, 22, -4, -3, 10, -3, 3, 10, 15, 2, 6, -1, 15, 19, 0, -5, 3, -3, -14, -9, -8, -12, 1, -6, -7, 17, -6, -11, 2, -9, -15, 6, -17, 10, 5, 14, -13, -19, -9, 11, 0, -8, -1, 17, -4, 16, 27, 12, 25, -12, -4, 12, 8, -9}
}
, {{-1, 7, -4, 3, -7, 3, -7, 1, 3, -1, 2, -3, -4, -1, -12, -5, -10, 5, 9, 8, 3, 5, 14, -5, -17, 4, 16, -4, 24, 1, -8, -14, -16, 10, -5, 3, 21, 2, 2, 9, -7, 2, 0, -20, -6, -3, -3, 2, 0, 3, -10, 12, -10, 1, -19, 1, 16, -2, 1, 11, -5, 2, 1, -3}
, {-6, -12, 3, -6, -32, 6, 22, 11, -15, -22, -3, -2, 10, -9, 0, -12, 9, -13, 2, -19, 19, 12, -2, -5, 4, 8, -2, -3, 2, -18, -12, -19, 26, -1, -14, -20, 7, 2, -18, 14, -13, -8, -16, 7, -2, -4, -1, -5, 4, 10, -3, -14, 22, -5, 7, -9, 7, -11, -24, 1, 2, 17, -13, 6}
, {16, -26, -18, 4, -15, 22, -11, 3, -21, -18, 0, 1, -6, 0, 0, -10, -4, -7, 24, 15, 13, -11, -7, 13, 12, -8, 6, 22, 9, -24, -30, 7, 0, -3, -9, -13, -11, 2, -15, -18, 8, 5, 5, 4, 32, 8, 8, -7, 30, 5, -14, -11, -48, -4, -12, 12, 4, -8, -23, -14, 12, 19, 3, -1}
}
}
, {{{-2, -9, -2, -2, -4, -5, -5, -4, -12, 1, 4, 12, -2, 2, 6, 6, -3, -22, 21, -2, -1, -9, -7, 7, 7, -7, 1, -21, 4, -2, 0, 4, 4, 7, -11, 3, -10, -5, 15, 0, 0, 2, 0, 11, 2, 13, -8, 8, -22, 8, -9, -8, -16, 24, 10, 8, -2, 14, -19, 5, 4, 9, 17, 10}
, {9, 3, -3, 6, -2, -8, -5, -18, 22, -8, -6, 1, -8, 3, 17, -8, -2, 7, -14, -1, 19, -14, 11, 12, -10, 7, 26, 5, 7, 29, -2, -5, -23, 19, -14, -9, 7, 1, -12, 2, 14, 4, 8, 13, -22, 2, -11, 9, -19, 13, -1, 1, -14, 4, -2, 8, 0, 5, -20, -5, 5, 11, 7, 16}
, {8, 5, 4, 2, 9, 7, -6, 0, 6, 1, -3, -11, 3, -4, -3, 5, -8, -10, -16, 7, -3, -15, -15, 10, -11, -1, -2, -18, 12, 10, 2, -9, -2, 8, -1, 5, 1, 1, -8, 15, 23, 9, 1, -8, -15, -4, -1, -8, -21, 11, -6, -8, 7, -9, 2, -9, -2, 3, 2, 2, -7, -4, -4, 9}
}
, {{8, -9, -2, 10, 7, -14, -2, 24, -1, 8, -15, 7, -8, -11, 6, 0, -6, -15, 2, 9, -6, -34, -8, -2, -14, -1, 16, -22, 6, -8, -4, -21, 10, 11, -15, 17, -7, -6, 3, 14, 14, 2, 4, -17, -19, 4, -16, 0, 11, 5, -9, 6, 3, -15, -10, -18, -6, 3, -5, 0, -12, 17, -1, 9}
, {2, -20, 0, -34, 24, -11, 14, -21, -31, -5, -19, -8, 18, -9, -7, 4, -7, -36, 30, -5, 9, 25, 29, 2, 14, -24, -31, 6, -11, -12, -31, -12, -9, 21, 4, -10, -6, 22, -29, -19, -39, 9, -10, 6, -18, 15, 2, -7, 16, 18, 31, 24, -8, -7, 13, -15, -7, 4, -13, 2, -7, 3, 29, -5}
, {1, -11, -22, -5, 1, 1, -1, -23, -18, 0, -11, -8, -4, 14, -3, 8, 8, -3, 1, -1, -2, 9, -11, -4, -15, 20, 4, 14, 13, -2, -4, 2, -6, -9, -1, 14, -4, 13, 23, 12, -1, -6, 15, 2, -16, -14, -4, 5, 7, 8, 4, -3, -2, 1, 18, 13, -5, 9, -5, -7, -9, 0, 13, -4}
}
, {{19, -3, 8, 17, -13, -9, 10, 13, 23, -1, -4, 4, -10, -14, -1, 0, 4, 13, 9, -11, 9, -8, -2, 3, -13, 5, 9, 5, 11, 8, -7, 5, 12, 6, 12, 9, -3, -11, 19, -2, 11, -10, 19, -13, 9, 3, 0, -7, -11, -29, -7, -6, -19, -10, 0, -6, -3, -14, 25, -2, 0, 5, -6, -4}
, {2, 7, 20, 8, -10, -8, -20, 4, 19, 9, 30, 10, -9, -16, 18, -15, -2, -8, 11, -12, 12, -8, 12, -3, -8, 3, -13, 12, 19, -12, 5, 15, 22, -8, -10, 11, -11, -33, -5, -17, 4, 16, 17, 5, -10, 0, -29, -5, 14, -18, -16, 33, -4, 5, -9, 2, -3, 10, -28, -4, -2, -43, -19, 1}
, {-10, -13, -5, -16, -6, 2, -15, 4, 0, -8, 7, 9, -7, 5, 21, -12, 2, 2, 2, -10, -7, -19, -4, -13, -31, 1, 18, -6, -7, -2, 13, 3, -9, -3, -1, -4, -8, -1, -2, 13, 9, 1, -6, 4, -11, -9, 12, -10, -2, -11, -5, 10, -11, -5, -14, -4, -14, 5, -4, 9, 9, -16, -5, -1}
}
}
, {{{16, -3, 12, 1, -3, -4, 4, -4, 6, -5, -3, 3, 11, 5, -16, 11, -4, 12, 9, 4, 13, 3, -3, -1, 26, -3, 10, -18, 3, 5, -3, 2, -8, -7, 1, -4, 21, -12, 14, 12, 0, 12, 4, -7, -1, 2, 2, -3, 1, -11, 17, -10, -5, -8, -14, 16, 10, 2, -7, 1, 2, 1, 3, 0}
, {-9, -4, 12, 2, 1, 9, -6, -4, 4, 20, -18, 11, 2, -9, 7, 15, 15, -2, 8, -2, -7, -11, -4, -3, -2, 11, 1, -9, -7, -7, 12, -7, -7, -7, 1, 3, -8, 13, 0, 14, 6, -12, 16, -29, -8, -11, 7, -3, 8, 5, -2, 1, 0, 7, 15, 5, 6, 10, -11, 9, -10, 2, 7, -4}
, {-4, 1, 8, 8, -7, -16, -2, -17, 7, 3, -2, -6, -15, 8, -18, 5, -6, 5, 2, -15, 3, -6, -16, -16, -3, 4, -16, -16, -6, 0, 1, 2, -3, -10, -2, 11, -1, 4, 23, 9, 12, 0, -5, -17, 9, -7, 2, -16, -8, -12, 8, 4, 9, -6, -7, 14, 16, 5, -15, 4, 2, 5, 3, 12}
}
, {{-1, 9, -8, 0, -15, -2, 8, -3, -1, -7, -19, 2, -6, 7, -6, -2, -12, 1, 4, -15, -9, -6, 3, 12, 1, 2, 34, -17, 9, -5, 7, 1, 3, -4, -3, 7, 6, -4, 9, 5, -6, -20, 6, -23, -5, -8, 11, 7, -14, -22, -1, 0, -7, 10, 10, -8, 16, -13, 27, 2, -15, 11, 8, -10}
, {-10, -1, -5, 6, -5, 11, -15, -19, 1, 5, 1, 5, 2, -6, 7, 17, 6, 6, -21, 12, -6, -18, 22, 0, -24, 23, 7, -8, -18, 8, 42, 6, -19, 9, -17, -30, -8, -15, 7, -10, 5, -14, 6, -30, -11, 18, -16, -14, 11, -10, -12, -12, 1, -11, -11, 12, -16, 8, -1, 17, -33, -5, 1, 13}
, {7, 0, 3, -5, 5, 4, -22, 10, -19, -11, -16, 7, 0, 5, 11, 13, 12, -7, -14, 17, -12, -14, -16, -21, -5, 4, -4, -15, -2, -12, -15, 14, 10, 8, -1, -12, 1, 11, 2, 0, -1, -9, 0, -30, -19, -15, 9, 32, -6, 5, 6, 10, -1, -9, -3, -10, -44, 3, -35, -22, 2, -27, 18, -12}
}
, {{9, -6, 16, 2, -22, -2, 4, -1, 21, -11, -1, -7, 3, -13, -4, 9, -13, -8, 2, -2, 22, -12, 7, 15, -4, -15, 14, -14, 7, 15, -7, -4, 3, 13, 12, -16, -6, -14, 2, 3, 2, -7, -26, -7, 4, -36, 5, 6, -13, -4, -13, -5, 3, -5, 18, -6, 3, 2, -5, -9, 4, 3, -4, 4}
, {-11, -19, -25, 16, 16, 17, -16, 5, 2, 13, 10, 2, -22, -1, 23, -12, -19, -5, -10, 0, -10, -12, 11, -6, -2, 10, 17, 9, -28, 20, 4, -30, 35, -24, 20, 18, 13, -15, 0, -19, -6, -16, -28, -31, -15, -31, 11, -14, 10, 19, 41, 10, -19, 2, -9, 2, -14, 18, 0, -22, -21, 12, 9, 4}
, {-3, 0, 24, -7, -15, -10, 17, 22, -1, 25, 18, -16, -15, -13, -2, 16, -16, -5, -18, 19, 2, -18, 28, 24, -29, -8, 11, -15, -2, 11, 12, 1, -27, 0, 12, 3, 5, -1, 21, -7, -13, 18, 3, -25, -12, -23, -11, -1, -15, -14, 18, 23, -3, 13, 8, 0, 22, -9, 11, 18, -19, -34, 17, -7}
}
}
, {{{19, -6, 3, -4, 1, 27, -3, 0, -5, -13, -23, -2, -6, 7, -3, 5, 20, 6, -9, -1, 2, 15, 4, 10, 8, 5, 14, -13, 8, 3, -7, -1, -3, -9, -40, 3, 7, -21, -8, 24, -14, -16, -1, -6, -9, -3, 5, -10, 0, -13, -6, -3, 19, -7, 3, 0, -11, 14, 2, 10, 6, -17, 22, 7}
, {5, 6, -4, -7, -15, 28, -4, -1, -21, -22, -7, -1, -3, 14, -13, 0, 29, 16, 27, 3, 3, -3, -13, 7, 2, -3, 4, -5, 0, -2, 2, -11, 11, -16, 2, 30, 11, -3, 1, -5, -16, -6, -1, 1, 9, 2, 6, 4, 3, 1, 17, 4, 9, 4, 3, -1, 10, 1, 3, -4, -5, -3, -10, 0}
, {-5, -5, -19, -2, -42, -1, 6, -12, -1, 6, -3, -17, 2, 13, 13, -9, 9, 8, 6, -1, 10, -9, -7, -3, -6, -19, -1, -3, 2, 5, 7, -1, -10, 7, -33, 1, 0, -2, -2, -1, 12, 8, 5, 11, -3, -7, -4, 15, -3, -13, 1, -4, -14, -22, -7, -20, 8, -8, -5, 13, -5, -16, 5, -14}
}
, {{-15, 14, -7, -16, 28, 26, 10, -7, -5, -18, -8, 14, 1, 18, -5, 2, 8, 17, -15, 15, 4, 13, -12, -2, 14, 11, 2, 0, -13, -2, -7, 22, -1, 6, -1, 0, -2, -13, 5, -10, -7, 9, -13, 7, 0, 3, 5, 21, -13, 4, -28, -6, 0, -7, -15, 7, -22, 16, 17, -2, 5, 12, 9, 3}
, {-18, -6, 4, -28, 4, 13, -3, -7, 4, -9, -24, -12, 4, -2, 9, 7, -16, 15, -5, -7, -4, 21, 0, 8, 12, 10, 14, 4, 0, 1, 14, -5, -5, 14, 7, 5, 9, 4, -9, -8, -20, -8, -18, 14, 14, 18, -5, -8, -15, -28, 11, 3, -11, -21, -14, 30, -7, 4, -6, 0, 6, 2, 5, -24}
, {2, 9, 1, -9, -28, 9, 24, 8, -10, -3, -7, 0, -16, 18, 24, -13, -13, 33, 9, -3, 16, 29, 1, 19, 8, 17, -6, -20, 23, -21, 18, -11, -10, 21, 8, -7, -1, -13, 7, -22, -6, 8, 8, 7, -4, 0, 14, -10, -14, 3, 6, 0, 19, 4, -11, 4, -5, 16, 1, -4, 1, 13, -15, -7}
}
, {{0, -1, -3, 0, 36, 17, 5, 25, 4, 5, 0, 9, -18, 6, -8, 3, -16, -3, -9, 22, -2, 16, 19, 2, 0, -1, -5, 11, 14, 8, 8, 11, 23, -5, 10, -6, -8, -24, 9, 9, 24, -7, -28, -19, -2, -25, -22, 12, -5, -6, -21, 10, -19, 2, -12, -15, -16, 16, 17, 2, -1, -18, -13, 4}
, {-15, -4, -6, -7, 40, 39, -1, -2, -31, 22, 16, 1, -15, -12, -7, 2, 16, 5, -30, 8, -13, 23, -7, 15, -2, 26, -8, -12, 6, 15, 12, 6, 6, 0, 28, 0, -1, -24, 12, -1, -17, -27, -2, -10, 29, -32, 6, -38, -5, 4, 9, 22, -7, 0, 2, 8, -5, 7, 19, -14, 11, -10, -24, 12}
, {2, -5, 24, -6, 10, 7, -2, -1, -7, -7, 4, 29, -9, -1, -28, -6, -15, -10, 10, -6, 2, 11, 9, 20, -8, 15, -13, 4, 18, 0, 16, 16, 24, -11, 15, -20, -10, 5, 10, 10, 31, -19, -9, -3, 9, -32, 11, -16, 15, -26, 11, 13, -8, 12, 12, 9, 7, 13, -3, -2, 12, 5, -4, 1}
}
}
, {{{-8, 12, -3, 10, 0, 12, -15, 8, -6, -12, 9, -5, 5, 1, -2, 10, 2, -10, 5, -4, 3, 14, -12, 1, -14, 12, 4, 16, 3, -6, -15, -5, 15, 1, 5, 8, 18, -19, 2, -8, 1, -10, 1, -3, 6, -13, -11, 1, 13, 12, 5, 3, 5, -9, 12, -6, -20, 8, -13, 10, -4, -3, -1, 9}
, {-6, 10, -25, 3, -2, 13, -12, -12, -11, 12, 6, 2, 3, 16, -7, -7, 11, 10, -3, 4, 6, -8, 0, 10, -21, 2, 9, -7, -6, -12, -15, -13, -6, 22, -3, 12, 7, 0, -12, 0, 1, 0, 5, -2, 19, 8, 7, -17, 6, 14, -8, -3, -4, -2, 19, 8, -6, 0, -13, 0, -1, 8, 20, -14}
, {7, -15, -7, -24, -9, -2, 16, -5, -4, -3, -2, 11, 0, -23, -4, 19, 7, -20, -7, -8, 15, 4, 3, -2, 0, -1, -8, 24, 3, 10, 1, -18, 0, -1, -6, 9, -4, 2, -14, -16, 5, -5, -2, -3, -1, -9, 9, 13, 12, 4, 8, -28, 14, -3, -7, -20, 1, 8, -11, -8, 17, -16, 15, -4}
}
, {{0, 2, 6, -4, -5, -4, 6, 1, -3, 1, 6, -7, -11, -4, 15, -3, -12, -12, 7, -4, 13, 5, -11, -12, 21, -2, 10, -6, -14, -1, -17, 2, 14, -20, 10, 10, 0, 6, 3, -7, 14, -8, -4, 2, -17, -7, -23, -10, 1, 4, -5, -1, 1, 9, 3, -5, 17, -9, -5, 1, 6, -1, -3, -2}
, {-11, 11, -14, -25, 23, -26, 22, -2, -7, 10, -12, -22, -1, 10, -6, -3, 6, -25, 10, -4, -14, 1, -4, 0, 14, -39, -23, -5, -7, -20, -10, -17, 17, -4, 8, -2, 3, 15, -2, -23, -36, 28, -22, 7, 8, -1, -11, -14, 20, 22, 21, 37, 7, -3, 8, -21, -7, -4, -2, -3, 1, 14, 15, -6}
, {17, -3, 0, -3, -8, 12, -14, -14, -5, 15, -19, 11, -4, 4, -5, 2, 2, -1, 2, -6, -6, 4, 4, -5, -32, 8, -8, 7, 24, 12, 4, 2, 2, -18, 1, -4, 2, -5, 3, 2, 23, -14, -8, -5, -10, -23, 8, -9, 4, 1, 22, 3, -22, 2, 0, 14, 4, 0, 16, 2, 0, 0, 12, 16}
}
, {{5, 14, 3, -4, 0, 5, 7, 13, -15, 5, -12, 5, 0, -6, -15, -9, 5, -15, 1, 9, -3, -7, 2, 16, -20, -3, -1, -19, -16, 0, 6, -4, 10, -18, -7, -4, 12, 8, -25, -4, 6, 7, -6, -8, 0, 14, 14, -7, 0, -1, -8, 1, 9, 0, -22, -13, -15, -13, 18, -2, -18, -2, -6, -12}
, {29, 17, 2, -16, 4, -6, -23, -1, 14, -11, 10, 2, 0, -4, -24, -15, 16, 2, -3, -4, -15, -6, 8, -16, 2, 10, -7, -4, 12, 14, 3, 12, 14, -11, 16, -18, 9, -34, -16, -20, 10, -39, 17, 3, -2, 1, -4, -21, -2, -19, 19, -1, -13, -6, -11, 2, -41, 5, -1, -1, 1, -29, -18, 6}
, {-33, 3, -10, -9, -1, -2, -8, -1, -4, -6, -4, -11, 0, -16, -5, -18, 9, -2, 17, 4, -28, -38, 6, 14, -14, 8, 7, -9, -37, 19, 12, -16, -9, 19, -3, -5, -3, 12, -4, 12, -11, 2, -23, -4, -7, -17, -7, 13, -6, 5, 4, -8, -5, 5, 11, 1, 8, -8, 5, 6, -27, -15, 3, 5}
}
}
, {{{-4, 7, -4, -6, -10, -13, 6, 9, -7, 8, 5, 13, 16, -2, 6, 9, -8, -2, 15, 20, -17, -6, 9, 5, -3, 11, 0, -16, 5, 16, 5, 16, 11, 7, -14, 12, -16, -9, 18, -2, -10, -11, -9, -9, -23, 6, 3, -13, -6, -7, -9, 10, 3, 12, -11, -9, -7, -20, -3, 8, -3, -19, -1, -10}
, {-12, -5, -5, -8, -3, -18, -3, 0, -14, -20, -2, 7, 6, -4, -1, -21, -16, -24, -9, -1, 8, 10, 20, -11, -1, -19, -10, 15, 3, 1, 2, -14, 15, 12, 13, -14, 0, -3, -4, -8, -4, 4, 8, 7, 4, -7, -17, 6, -5, -10, -3, 8, -2, -20, -11, 2, -10, -12, -3, -6, -9, -7, -22, -4}
, {2, 10, -17, 1, 7, 17, -14, -12, 0, 0, -5, 5, -14, 6, 9, -12, -18, -5, 13, 2, -18, 3, -5, 6, -8, 2, 4, 15, -8, -10, -22, 1, -6, 5, -6, -8, -4, 6, -2, 4, -4, -2, -1, -7, 17, -8, -10, 0, -5, 1, -2, 15, -5, -15, -18, -3, -10, 10, -6, -5, -18, 1, -8, -13}
}
, {{-21, -23, -9, -3, -12, 3, -25, 3, -11, -3, 12, -2, 7, -9, 8, 2, 0, 11, 15, 2, 6, 7, 5, 8, 6, 9, 3, -13, 4, 7, -2, 6, -6, 1, 9, 15, 9, -6, 10, -25, 5, -22, 16, 1, 11, -4, -11, 0, -2, -21, -9, -11, -8, 0, 9, -1, 1, -6, 0, -7, -29, -7, 8, -3}
, {5, -7, 12, -2, 0, 12, 26, 1, -13, 3, -1, 4, 5, -2, -26, -15, -23, 3, 0, 16, 0, -20, -27, 2, 5, -5, 11, -3, -11, 8, 18, -24, 0, -11, 0, 7, -17, 10, 3, -1, -8, 18, 6, -16, 13, 9, 31, 2, 16, -14, -5, -25, -15, -19, -8, -29, 9, -12, 11, -2, -8, -27, 3, -19}
, {-1, 24, 16, 3, 11, -7, 5, 15, 11, 9, -7, -5, 5, 2, 7, -20, 3, -3, -13, -13, 6, 0, 5, -2, -4, -9, -5, 5, 16, -8, 15, -8, -15, 15, 5, 3, -8, -5, 17, 0, 0, -12, 5, 12, 13, -13, -8, 0, 15, -11, -14, 6, 14, 5, -16, 1, -11, 3, -8, 0, -10, -1, 9, -3}
}
, {{6, 2, -20, -5, 4, -7, -2, -21, -10, -14, -5, 15, 14, -2, 30, -9, 17, 6, 8, -22, -17, -18, 1, -13, -11, 21, -6, 4, 10, 8, 5, -10, 0, 9, -6, 10, -24, 4, -16, -9, -6, 1, -9, 6, -22, -6, 12, -10, -28, -1, 7, 5, -8, 18, 0, 15, -3, 24, 8, -2, 12, -7, -8, 7}
, {2, 4, 7, 1, 30, 6, 1, 4, 0, 11, 8, -11, -12, -12, -5, -12, 2, 4, -6, -26, 12, 2, -19, 20, -32, -9, 2, -7, 1, -5, 14, -15, -5, 12, -2, -10, 1, 15, -26, 3, -1, 3, -16, -11, -18, -10, -1, 10, -8, 16, 0, -18, 18, 32, 11, 12, -5, -9, -6, 16, -5, 7, 10, -1}
, {-16, -16, 22, -4, 14, -1, -9, 1, 16, 8, 0, -3, 2, -21, -5, -20, -15, -21, 6, -10, -22, 5, -12, 12, 6, -10, -7, -22, 12, 3, -9, 2, 12, -48, -3, 15, 4, 0, -6, 8, 23, -17, -10, -9, 6, 11, 3, -16, 0, 3, -10, -3, -18, 12, -13, 6, 17, -15, 10, 7, -2, -13, 10, 10}
}
}
, {{{1, -17, -2, -4, -19, 10, 5, -4, -17, 9, -10, -10, -8, -8, 10, 0, 5, -2, 2, 2, 6, -11, -5, 8, -16, 7, -11, -31, -2, -9, -1, 6, -6, -20, -23, -1, 15, -16, -12, 6, -6, -17, -1, -5, -24, 14, -2, -16, 5, 14, -16, -1, -10, -3, 7, -7, 4, 3, 5, 5, -5, -12, -4, 12}
, {14, -4, -18, -4, -5, 5, -24, -1, -25, -20, -4, 15, 9, 10, 2, -16, 21, 20, -9, -1, -14, -17, -15, -10, -4, 15, 15, 9, 4, -2, -10, -13, 2, 10, 9, 12, -6, -3, -6, 0, 13, -6, 3, 11, 1, 9, 12, -11, -11, 3, -14, -4, -14, -5, 20, 7, -31, -2, 5, -6, -8, -10, -1, -3}
, {2, 5, 3, 1, -4, 2, -14, -3, -7, -7, -4, 15, 1, -7, 6, 8, -5, -5, -18, 9, -15, -4, 13, 0, -4, -2, -9, -13, 6, -7, -10, -2, 5, -11, -13, 6, 24, -15, 5, -9, 14, -18, 2, -15, -13, 10, 5, 21, 3, 6, -16, 0, 16, -5, 16, -8, -4, 16, -22, 7, -8, -13, -6, 5}
}
, {{-6, 9, -14, -15, 8, 2, -6, -3, 10, 2, 9, -17, -3, -12, -9, 1, 6, -12, 1, 4, -3, -24, -4, 1, -10, -3, 6, 7, -13, -2, 7, -2, -10, 5, 9, -4, -14, -7, 7, -20, -3, -21, -8, -9, -22, -25, -2, -1, 12, -27, -2, 13, -13, 17, -14, 4, -10, -1, -4, -17, -13, -15, 3, 1}
, {7, 15, -2, -12, 17, 1, 8, 3, 7, -43, -11, -32, -4, -7, 15, -15, 3, 3, 1, 6, 8, -5, -1, -31, -1, -16, -9, -12, -5, -21, -11, 12, -4, 13, 25, -4, -20, 13, -23, 10, 13, -5, -5, 13, -9, -5, 15, -15, -11, 2, 34, 10, 3, 10, 0, -18, -28, 31, -16, -20, -3, -4, 8, 8}
, {-1, 16, 17, -16, 5, 10, 8, 1, 2, 9, -2, -15, 13, -4, -11, -15, 13, 9, 3, -9, 5, -9, -16, 5, -15, 13, 8, 1, -7, -1, -3, 3, -12, 1, 3, 0, 4, 8, -12, -17, -15, 3, -15, -18, -5, -12, 3, -11, 4, -17, -3, -4, -3, -2, -2, 8, 3, 0, 2, -12, -12, -4, 13, -7}
}
, {{0, 2, -2, -2, -7, -2, -10, 19, 13, 9, 3, -1, 0, -11, 0, -9, -7, -3, -7, 7, -4, 17, -1, 13, -7, -7, -9, 2, -24, 14, 10, 2, -3, -9, -11, 9, 6, -17, 3, 18, 1, 13, 6, -13, -5, 8, -5, -1, 6, -6, -12, 4, 1, 6, -6, 13, -7, 7, 17, 1, -2, 6, 4, -7}
, {35, 15, 9, -2, -2, -24, 10, 14, 7, 6, 8, 3, 11, -3, 4, -24, 4, 14, -10, -19, 14, 5, -11, 5, 17, 1, -11, -17, 31, -17, 7, 6, 8, 0, -20, 6, 10, -19, -1, 8, 7, 11, 21, -4, -6, -12, -33, -24, -4, -9, -4, 8, -7, 16, -7, 7, -23, -3, -10, -4, -16, -22, 0, -5}
, {-9, -20, 2, -5, 10, 11, -13, 4, -18, 10, 2, 5, -5, 5, 0, -15, -15, 13, -3, -11, -31, 13, -15, -1, -30, 16, 1, 6, -11, 13, -1, 18, -5, 0, 29, 22, -12, 0, 12, 8, -12, -8, -2, 0, -6, -8, 7, -21, -9, 6, 10, 0, -6, -1, -15, -6, 2, 0, 8, -3, 12, 11, -23, -1}
}
}
, {{{26, 6, -5, -17, 7, 19, -2, -14, -11, -1, -29, -6, -16, 0, -21, 7, -11, -3, 15, 14, 9, 8, -11, -6, 0, -17, 18, 21, -9, -17, -6, -9, -7, -10, -20, 10, -5, 2, -14, -6, 23, -6, -2, 1, -15, 24, 8, 19, -9, 12, -9, 6, 16, -12, -17, 1, -4, 16, 6, 9, 19, -4, -3, 10}
, {2, -4, 13, -1, 8, -3, -3, 17, -8, 21, 8, -3, 4, -1, -13, -11, 0, 8, -7, -1, 5, -17, -13, -2, 9, 12, -4, -19, 9, -15, 1, 9, 16, -5, 0, 7, 11, 6, 0, 14, 0, -3, -6, -4, -2, -6, -1, 5, 14, -9, 15, 0, 2, 3, 7, -9, -1, 0, 2, 8, -3, 5, 10, -19}
, {-1, -8, -13, -4, -13, -4, 16, 1, -4, 11, -3, -7, 9, 8, 18, 10, 8, -3, -7, -7, 6, -10, 10, -12, -15, -14, -1, -12, -17, 14, 3, -20, -3, 1, -15, 4, -6, -10, 16, -2, -7, 32, -5, 6, -5, 3, 11, -5, 13, -3, 7, 0, 8, -1, 14, 1, 21, -6, 6, 14, -16, 9, 9, -8}
}
, {{-9, 3, -3, -20, -2, 11, 3, 18, -4, -3, -24, -10, 13, 6, 18, 0, -16, -10, -10, 21, -7, -10, 14, -16, 6, 15, 0, -11, -15, 2, -19, 8, -6, -21, 3, -6, 8, -19, -17, -1, -26, 1, 11, 9, -6, 2, -13, -2, 4, 12, -13, -11, 1, -15, -18, -11, -23, 0, 3, 5, 10, -1, 2, 13}
, {-7, -21, -24, -26, -18, -47, 2, 27, 7, -5, 13, -7, -12, 8, -7, -34, -8, -4, 10, -9, 6, 8, 11, 1, 7, -8, 11, -5, 9, -17, -21, -12, 7, 0, 31, 2, 4, -21, -29, -12, 25, -9, -2, 0, 7, -4, -15, 3, 12, -32, 1, -2, -19, 0, 20, -24, 4, -4, -28, -24, -41, -2, -3, -7}
, {-16, -24, -3, 11, -3, 31, -23, 3, -5, -6, 3, -12, 3, 12, -11, 6, 11, 18, 7, 10, -1, 5, -2, 13, -4, 17, -11, -3, -31, 5, -7, 15, -20, 1, -7, 16, 8, 3, -5, -18, -8, -14, 8, 8, -7, 3, 0, -9, 14, 4, -6, -8, -17, 24, 5, 16, -2, -6, 19, 3, 13, -11, -2, 2}
}
, {{-3, 0, -3, -6, 24, 5, 5, 19, -2, -1, -19, -8, 4, -3, -10, 5, -21, -1, -23, 19, -19, 13, 11, -17, -26, -13, 17, 1, -8, 4, 4, 14, 5, -6, -11, -4, -14, -1, 9, -23, 4, 7, -14, -13, -6, -6, -2, 12, 5, -5, -13, 4, 0, -8, -11, -26, -23, 15, 15, -23, -1, 16, -1, -5}
, {-14, 0, -10, -13, -1, -23, -23, 16, 3, 29, 2, 7, -11, 7, -25, -8, 2, -12, 13, -27, -19, 10, 17, 16, -7, 9, -21, 7, -4, 15, 20, 4, 19, 8, 9, -22, 1, -2, -2, 6, 3, -12, -14, -10, 4, -30, 4, -16, 23, 7, 1, -14, 5, 11, 4, -12, -7, 5, 1, -4, -12, -7, -28, 14}
, {-2, -14, -6, 14, 2, 8, -12, -17, -9, 14, 1, -22, -29, -1, -12, -11, 18, -21, -15, 7, 5, -1, -3, 9, -11, -3, 7, 2, -28, 7, -8, 5, -11, -38, -1, 7, 5, -21, -10, -16, 22, -12, -7, -9, 13, -6, -4, -7, 7, -6, 32, 8, -14, 13, 4, 0, -11, -5, -5, -6, -2, 2, 7, 11}
}
}
, {{{-1, 12, -20, -13, -24, -8, -5, 2, -9, 5, -19, 5, 12, 22, -7, -1, 6, 9, 13, 6, 5, 0, 19, -23, -7, 1, 9, 14, -24, -12, -6, -21, -12, 41, -23, -1, -22, 18, 16, -8, -22, 3, 15, 19, -5, 1, -7, 27, -11, 3, 5, 4, -6, -16, -13, 3, -10, 26, -15, 16, 9, -7, 11, -13}
, {2, 9, 12, -5, -25, -16, 9, 8, 15, -1, -6, -1, -17, -15, -5, 0, 14, 0, 25, -8, -19, -9, 6, -22, -16, -15, 1, 2, 10, -22, -6, 2, 3, 1, 14, 3, 21, 3, 3, 5, 14, 14, 23, 2, -15, -10, -9, -18, 7, 2, -23, -6, 9, 9, 6, -1, 26, -6, -14, 8, 13, 0, -5, -16}
, {2, 9, -19, -8, -5, 10, -7, -17, -6, -8, 6, 2, -1, 19, 15, -6, -1, 11, 11, -9, 0, 16, -1, 8, -20, -4, 6, 4, -14, -16, 0, -8, -8, -2, -11, 9, -10, -10, 0, -15, -15, 21, 14, -19, 13, 3, 11, -22, 8, 6, 19, -2, 14, 8, -7, -1, -1, 26, 6, 5, 9, -6, -12, -8}
}
, {{4, 16, -7, 4, -6, -3, 18, 18, 2, 4, -14, -12, -7, -4, -5, 9, -2, 18, -17, 7, 7, 5, 17, -6, -6, -16, 6, -1, -17, 0, -10, 7, 0, 3, 7, 14, 5, -21, 6, 9, -5, 15, 14, 4, 4, 1, -14, 2, 3, 0, -17, -28, 13, -4, 3, 10, -21, 17, 6, 4, -3, 10, -5, -22}
, {-11, -7, 5, -13, -9, -14, -33, 5, -2, 9, 5, -5, -8, 11, 2, 10, 4, -6, 10, -22, -7, 2, -5, -23, 1, 0, 10, 5, 13, -5, -32, -8, -29, -2, 20, 9, -5, 6, -29, -43, 10, -34, 11, 7, 23, -28, -45, 2, 0, -33, 9, 24, -17, 14, 33, -29, -12, 5, -8, 1, -23, 15, -28, -16}
, {-16, -8, -4, -11, 6, -38, -7, 18, 9, -9, -9, 10, 9, -17, 14, -15, 9, -21, -15, 3, -2, -4, 1, -3, 19, -5, -10, 3, 16, 23, 21, 4, -12, 17, 14, -3, -10, -26, -3, 4, 3, -13, 1, -8, 8, 2, 13, -22, 0, -20, 10, 4, 0, -4, -1, -14, -3, -3, -7, 10, -6, 8, 0, 13}
}
, {{-10, 4, -26, 4, 15, -12, 9, 26, -1, -4, -3, 3, -2, -3, -2, -17, 0, 6, -16, 12, -13, 3, 15, -1, -5, 13, 8, 8, 5, -7, 1, -4, 12, -9, -4, 7, 12, -3, -7, -12, -8, -21, 2, -7, -9, 1, 1, -6, -8, 9, 7, -7, -1, 2, 5, -15, -9, -1, -11, -12, 4, -1, -5, 0}
, {7, 3, 0, -15, 8, -11, -5, -6, -2, 11, -3, 9, 6, -20, 2, 9, -1, 17, 10, -9, -14, 5, 15, 12, -13, 25, -6, -5, -3, 1, 3, -4, 15, -2, 16, 0, -6, -3, 20, -9, 14, -20, -5, 4, 1, -5, -7, -4, 2, -2, -6, -23, -5, -6, 1, -2, 0, -3, -12, -4, -20, -1, -8, 4}
, {8, -4, 4, 8, 2, 2, 9, -8, 7, 10, 20, 1, 7, -9, -2, 7, 15, 9, -14, 20, 5, 11, 5, 14, -6, 4, 0, 1, -3, -6, 0, -21, -10, -26, 9, 25, -4, -15, -15, -12, -1, 1, 2, 4, -12, 2, 19, 5, -12, -2, 2, -12, 24, 14, 20, 8, -13, 24, 5, -20, -14, -11, 9, 1}
}
}
, {{{-10, -12, -17, -6, -20, 22, 14, -4, 17, 3, -4, -8, -7, 2, 8, 10, 9, 4, 3, -6, 3, 11, 1, 8, 1, -4, 6, -1, 5, -4, -3, -5, -7, 0, -8, 7, 6, -13, -12, -9, 6, -10, -11, 0, 6, -2, -3, -9, 4, -14, -5, -11, -11, -10, 4, 5, 1, -10, -6, 9, 2, -13, 11, -17}
, {-8, -9, 12, -13, -4, -8, 15, -7, -2, -14, 2, 2, 6, 11, 10, -5, 8, 22, 7, -10, 1, 7, 2, 3, 1, 15, 2, -8, -2, -30, 0, -1, 23, -12, -10, 13, 6, -3, -2, -10, 7, 7, 0, 8, -8, 6, -10, -20, -11, -10, -12, -1, 5, 3, 11, -4, 2, -6, 4, 0, 1, 7, 1, -4}
, {-3, -15, -2, 0, -5, 7, -4, -4, -10, 9, -22, 12, -4, -4, 4, -2, -15, -2, 3, -11, -7, -2, 1, 4, 3, 19, -19, -20, 12, -1, 9, -7, -19, -6, 6, -12, 2, -9, -2, -11, 7, 2, 9, 5, -15, 6, 3, 13, -5, -7, -12, 4, -10, 2, 15, -10, 5, 18, -19, 9, 0, -6, -15, -3}
}
, {{8, -7, -15, -21, 6, 3, 12, -5, 10, 1, -8, 7, 15, 0, 4, -4, 9, -10, -10, 7, -9, -24, -12, 4, 8, -5, 0, -7, 0, 6, -25, 6, -14, 14, 10, -20, 9, 13, 3, -22, 14, 22, -41, 21, -3, 6, 1, 13, -16, -16, -2, 2, -11, 1, 11, -1, 2, -3, -10, -39, -5, -9, -2, -3}
, {-1, 0, 21, -9, 3, 21, -51, 6, 5, -35, 6, 2, -10, -8, 14, -23, -3, -10, -6, 18, -14, -14, -17, -32, -17, 7, -5, -4, 10, 3, -7, 25, -4, 21, 29, -14, -28, -34, -20, 6, 19, -61, 3, 17, -26, -14, 13, 0, -8, -26, 8, 8, -19, 28, 16, 6, -1, 17, -2, -27, 27, -21, -23, 5}
, {-14, -5, 10, 3, 6, 3, -7, 10, 26, 8, -8, 4, -10, 7, 6, -15, 4, 18, -6, -6, 1, -31, 0, 10, -18, 9, 5, -3, -22, 1, 32, 0, -4, 1, 6, -10, -2, -5, 4, -2, -23, -18, -4, -16, 3, -1, 1, 2, 0, -2, 12, 6, -3, 8, 7, 17, 0, 9, 2, 3, 0, -2, 17, -8}
}
, {{2, -24, -3, -16, 13, -1, 16, -28, 8, -10, -12, 4, -6, -8, -28, 0, -3, 5, 15, -7, 4, 0, 8, 14, -4, 19, -14, 0, -8, -2, -5, -1, -7, -11, -2, -14, -11, -8, 7, -6, -7, 20, 7, -16, 4, 5, 2, 1, 12, -10, -8, -3, 10, 11, -2, 18, 25, 0, 11, -4, 6, 13, -20, -2}
, {-4, -6, 18, -6, 12, -7, -5, -2, 14, -15, -2, 8, 31, -11, 13, -23, -13, 0, 9, -5, -2, 1, -16, 4, 2, 18, -1, 5, -10, 4, -6, -7, -4, -14, -8, 5, 7, 4, -6, 3, 16, -22, 2, -22, -8, -2, 12, -3, -16, -4, -27, 10, -3, -5, -2, 16, 11, 6, 2, -12, -12, -31, 0, 14}
, {-8, 7, -7, 20, 9, 5, 8, 11, 6, -3, -11, 6, -12, -1, -4, 3, -6, 11, 5, -13, -5, -12, -10, 1, -18, 1, 2, 7, -14, -1, -9, -3, -2, -8, 29, 9, -3, 23, 13, 9, -18, 4, -7, 5, 5, -3, -10, 0, 0, 8, 6, 12, 3, -3, 3, 7, 8, 6, 2, -8, -6, -4, 3, -14}
}
}
, {{{12, -13, -2, -6, -5, 3, -10, -12, -13, -4, -12, 12, -6, 15, 7, -7, 8, 16, 1, -6, 6, 0, -10, -15, -9, 24, 17, 8, -4, 2, -5, 1, -9, 10, -22, -3, -1, 8, -3, 8, -3, 9, -4, -4, -24, 3, 4, 5, 8, -12, -4, -4, 4, 1, -6, -16, 8, -5, -9, 12, -17, -8, 6, 6}
, {-4, -15, -8, -12, -14, 25, 3, -9, -9, -11, -19, -6, -4, 19, -3, -1, 26, 12, 3, 13, 4, 3, -10, -21, 3, -8, -2, -11, 6, -5, -11, 5, -4, -18, -8, 5, 17, -15, 3, -12, 3, -8, 19, 3, 3, -10, 11, -9, 13, -13, -12, -8, 19, -5, -8, -1, -2, 4, -3, -10, 7, -23, 0, -3}
, {-1, 2, -1, 0, 0, 3, 1, 3, 18, 0, -2, -4, 0, 1, 7, -1, 8, 3, 2, -12, -18, 8, -6, 11, -7, -8, -14, 4, -21, 7, -14, -3, 4, 4, -18, -5, -8, 4, -7, -11, -8, -2, 0, -5, -6, 2, -6, 2, -15, -9, -4, -5, 5, 1, -6, -2, -13, 7, 5, 7, -1, 5, -6, -1}
}
, {{13, -6, -13, 7, -2, 0, -14, -16, -9, -15, -17, -7, -2, 2, 8, -2, 4, 2, 3, -1, -12, -3, -3, -15, -4, 12, 1, 0, 5, -5, -11, -8, -20, -5, 14, -3, 21, 21, 6, -6, -11, -3, -12, -1, -11, -5, 3, -2, 4, 10, 1, 7, 1, 0, -2, 8, -15, -6, 8, 10, -13, 23, 6, 3}
, {-4, -13, 11, -15, -11, -15, -9, -11, -16, 11, -9, -12, 16, 7, 1, -2, -4, -21, -12, -5, -19, -4, -6, 6, -2, 2, -19, 13, 12, 3, 2, -12, -5, -37, 7, -10, 5, -2, -9, -24, -6, 0, -10, -6, 6, 3, -4, -21, 20, -14, 7, 48, -38, -15, -27, -19, -23, -1, -21, -28, 8, -30, -18, -4}
, {8, 11, 4, 5, 11, -19, 12, 3, -10, -2, -17, 4, -2, -5, 7, -25, 8, -5, -24, -12, -13, -9, -7, -29, 17, -5, -2, -3, -4, 5, 0, -1, 2, -16, 23, 0, 2, -4, 5, -2, -9, 10, 5, -13, 2, -10, -28, -16, -5, 10, 10, 22, -3, -38, -2, -7, -35, 16, 2, -2, -9, -7, -2, 9}
}
, {{-1, -20, -23, 6, -3, -5, 0, -1, 9, -14, 0, 2, -6, -8, -6, 5, 8, 13, -4, 19, 9, 1, -19, -6, 3, -5, 2, 15, -4, 12, 5, 14, -20, 14, -4, -8, 11, 7, -22, 3, -4, -10, -9, -16, -12, 1, 1, 12, 5, 9, -6, -4, 3, 12, -26, 7, -7, 4, -6, -5, -24, 11, 9, 10}
, {-14, 2, 5, -18, -1, 36, 7, -13, 3, -33, 16, -7, -5, -16, -13, 21, -24, 5, -3, 10, -1, 21, 20, -1, -6, 11, -17, 7, -42, 14, 8, -16, -4, -15, 2, -2, -14, -19, 8, -41, 1, -13, -7, -1, -9, -6, 8, -18, -5, -25, 15, 8, -11, -23, -7, 2, -2, -6, -2, -9, -14, -17, 5, 11}
, {-10, 27, 14, 2, -17, 5, 29, -11, -4, -2, 27, -4, 2, 23, 5, 15, 1, 24, 8, 19, -3, 2, 20, 7, 19, 12, 24, -6, 3, -2, -10, -1, -31, -11, 8, 7, 5, 1, -1, 10, -2, 12, -5, 8, -6, -8, -6, 3, -5, 3, -3, 13, 12, -13, 7, 16, 13, 19, 1, 3, -13, 7, 20, -18}
}
}
, {{{-2, 6, 2, 1, -5, 4, -16, 6, -3, -1, 1, 20, -16, -24, -1, -3, -10, 6, -1, 9, -9, -20, -7, -18, -22, 7, 8, 10, 3, -10, 7, 12, 1, 8, -1, 6, -2, -9, 7, 0, 2, -7, -1, 1, -7, 0, -3, 16, -4, 2, 2, 8, -10, -5, 4, -13, 1, 7, 5, -2, -7, 3, 7, 2}
, {-12, -10, -5, 6, 11, 4, 6, -18, -6, -25, -5, 3, 12, 4, 7, 4, -6, -13, -10, -4, 11, -28, -23, -13, -3, 5, -9, 16, -4, 17, 12, 17, 3, -7, -15, -16, -2, 3, -2, 9, 12, -3, -5, 5, 4, 3, -7, -11, 4, 7, -3, 13, -3, 5, 13, -7, -11, -3, -15, -2, 17, 13, -3, 0}
, {18, 2, 4, 19, 3, -10, -3, -11, 5, 2, -12, -2, 5, -4, -6, 10, 1, 13, -9, 1, -10, -33, -5, 3, -1, -3, 5, -19, 3, 7, 14, -12, 3, -7, 10, -11, 2, 2, 9, 13, 5, -8, -9, 7, -25, 18, -1, 2, 13, -10, 9, 14, 3, -24, 10, -9, 6, 7, 5, 1, -7, 14, 3, -1}
}
, {{13, 2, -6, -18, -5, 14, -18, 9, -26, 13, -19, -1, 8, -6, -16, 2, -4, 7, -9, 10, 33, -2, -11, 1, -13, 30, -13, -11, 8, 8, 22, 1, 6, 10, 10, -11, 20, 1, -2, -2, 11, -10, 6, -18, -7, -3, 12, 0, 7, -9, 34, 5, -10, -10, -11, -18, -12, -4, 15, -8, -1, -11, -4, 11}
, {-4, 8, -4, -12, -19, -33, -4, 27, 11, -15, -22, 13, 4, -33, -11, 0, 16, -10, 7, 7, -1, -8, -13, 1, -10, -38, -14, 8, 8, 21, -13, -18, 10, -21, -4, 5, -11, 12, 7, 15, -8, -18, -3, -2, 25, -23, -8, 14, 22, -27, -31, 0, 5, 1, 5, -10, 19, -29, 7, -19, -2, -49, 9, -9}
, {7, 3, -10, -5, 2, -31, 4, -1, -6, -6, 15, -5, -1, 7, -20, -4, 4, -9, 4, -1, 5, -15, -6, -15, 7, -6, 1, -1, 0, -9, -6, -4, 25, -25, 10, 24, -16, -13, 22, -2, 0, 0, -12, -12, 15, 0, -3, 5, 1, -9, 17, 6, 3, -10, -6, -7, -7, -2, -12, -15, -7, -11, 15, 1}
}
, {{-9, 5, 3, 2, 2, 7, -5, 19, -4, 0, 13, -17, 16, 2, -1, 6, -5, 6, -2, 7, -7, 10, 26, 1, 4, 16, -7, -26, -16, 9, -5, -4, 11, -19, 4, 35, 16, -8, -7, -23, -4, -2, -10, -11, -1, 2, -4, 3, -1, -4, 17, -8, -10, -18, -13, -19, 5, -7, -6, 14, -10, 8, -9, 5}
, {21, 16, -11, -12, 12, 3, 3, 3, -14, -8, 12, -6, 4, -5, 20, -15, 1, -1, -11, 19, -1, 8, -11, 9, 25, -23, -18, 17, -11, -15, 7, -8, -41, 7, -1, -9, 5, -1, -23, -30, 4, -5, -5, 0, 10, 31, 0, 10, 8, -15, -11, 2, -21, 7, -6, -13, -18, 2, -15, -4, 2, 2, 14, 9}
, {5, -2, 5, -24, -15, -9, -8, -9, 6, -12, 8, -1, 18, -4, 6, -16, -9, 12, 6, 0, 6, -15, -14, -16, 1, 11, -20, 5, 0, -2, -14, 26, 3, 7, -11, -25, -4, -8, 6, 12, 15, -6, -2, -12, 16, 10, 4, 35, -2, -23, -26, -1, -15, 8, -2, -1, -28, 2, 2, 11, -6, -3, 5, -12}
}
}
, {{{6, -7, 6, -7, -9, -2, 23, 4, -6, -11, 1, 5, 10, -3, 13, 3, 8, 0, 3, -3, 6, -7, 9, 7, -17, 7, 4, -7, 2, 9, 13, -5, 0, -10, -6, -5, 11, -10, 12, -3, -2, 8, 2, -14, -15, -17, 4, -20, 24, -21, 6, -6, 5, 2, -7, -14, 9, -15, -1, -27, -15, -10, -13, -4}
, {-21, 2, -13, 8, -2, 15, -4, 1, -14, -7, -4, -3, -12, -6, 6, 12, 32, -1, -17, 8, -9, 8, -8, -7, -14, -7, 3, 32, 16, -15, -6, 8, 3, 13, 2, -11, -2, 7, 14, -3, 2, -12, 12, -15, 2, -9, -4, 5, -11, -10, 7, 15, -5, -12, 0, 23, 11, 7, 6, 9, 10, 8, -7, 5}
, {-1, -10, -6, 10, -15, 17, -1, -19, -7, 5, -15, 5, -3, -21, -8, 16, 14, 1, 9, 7, 8, 0, 7, 6, -21, 21, 4, 24, -12, 20, 1, 4, -8, 2, 4, -17, 2, 9, 5, 2, 13, -6, 2, 10, 11, -14, 8, 10, -10, -2, -6, -7, -16, -9, 6, 20, 5, -2, 10, 7, 7, 6, -6, 4}
}
, {{5, -1, 15, -20, -2, 6, 5, -9, 3, -6, 0, 10, 7, 3, 13, -3, -10, 4, 7, 9, -7, -9, 11, -11, 1, 5, -10, -5, -15, 11, -6, 10, -2, 26, 14, 2, 4, -2, 6, 2, 10, 4, -4, 12, -16, 10, -7, -16, -4, -11, -4, -10, -6, -10, -9, 0, 17, -4, 1, -13, -3, 1, 13, 8}
, {29, 4, 15, 0, -32, -13, -17, -30, -3, 7, 0, -3, -25, -15, -20, 0, 12, 22, 9, -14, 10, -4, 16, 16, 1, 23, -5, -21, 15, 32, 0, 1, -31, -18, -45, 17, 14, 2, 11, 2, 20, 6, -19, -25, -43, 6, 0, -14, 15, -6, -2, 3, 0, -13, -30, -6, 8, -21, 30, -10, -1, -35, 1, 11}
, {-4, 22, -10, -20, 22, 1, -14, 17, -9, 0, -8, -3, 16, 22, 10, -9, -10, -16, 3, -8, -4, 5, 3, -10, 6, 1, -8, -6, -7, -4, -3, -4, 6, -6, 3, 8, 12, -4, -4, 12, -20, 13, -12, -9, -9, -2, 0, -4, 1, 5, 21, 10, -13, -9, -6, 12, -8, 4, -17, -10, -9, -6, -9, -4}
}
, {{12, 18, 1, 8, 5, 0, 13, -1, 7, 1, 14, -4, -9, -12, -3, -5, -8, 4, -1, -6, -13, -4, 4, 1, 0, 22, -4, -9, 0, 10, -5, -7, -3, 3, -13, 7, -18, 0, -7, 6, -2, 5, -11, -2, -7, -12, 5, 8, -3, -4, 3, 17, 3, -8, -19, -8, -1, 8, -4, -4, 4, 1, -6, 8}
, {-3, 0, -23, -10, 6, 29, 7, 28, -6, 7, -10, 17, -23, 19, 18, -2, -14, -1, -17, -16, -21, 2, -7, -15, 15, -20, -13, 9, 28, -11, -21, -5, 2, 16, -10, -4, -12, 0, -4, 7, -16, 8, -8, -8, -6, -5, 13, 2, 21, 6, 15, 17, -11, -3, -16, -4, -15, 3, -7, 12, -4, 7, -13, 20}
, {5, 10, 0, -6, -2, -3, 14, 8, -41, 1, 3, -7, -34, -12, 0, 2, -19, -5, -6, 0, 6, 13, 16, 26, -7, -3, -7, -33, 5, -9, -2, -12, 11, -19, -10, -6, 3, 3, 16, -32, 5, 7, 13, -10, -3, -31, -5, -17, 8, -10, 5, -2, -9, -13, -12, -29, -10, -2, 10, -14, 2, -9, -22, 4}
}
}
, {{{5, 9, 11, -17, 8, -10, 8, 17, 4, -3, -7, -9, -13, 10, -6, 13, 5, 3, -13, -8, -3, 1, 15, -3, -7, 5, -5, -15, -11, -5, -14, -2, 3, 2, -7, 19, 1, -15, 0, 0, -4, -6, 6, 5, 10, -6, -6, 21, 13, -5, -1, 4, 7, 2, -7, -3, -1, -5, 0, -10, -12, -7, 8, -16}
, {-36, 9, 26, -11, 11, 5, 5, 4, -11, 14, -10, -6, 13, -10, -6, -10, 22, -2, 5, 9, -9, -7, 1, 16, 12, -1, -12, -3, -18, 0, -10, -13, 12, -8, -5, -9, -7, 23, -7, -2, 8, 7, 3, -3, -2, -23, 4, 7, 13, 15, -9, 2, 12, 20, 9, 2, 14, -9, -8, -11, 9, 1, -13, 8}
, {-25, -12, -1, -14, 14, 23, 2, -13, -11, 18, -12, 4, 0, -19, -9, 13, 24, 14, 0, -6, 17, 18, -10, 2, -24, 8, -9, -9, -4, 24, -19, -11, 0, -10, -1, -11, -4, -1, 4, 0, 5, -8, 7, -4, 26, -9, -29, 3, 8, 0, -6, -6, 9, -2, 13, -3, -2, -7, 19, 3, 11, 5, -11, 4}
}
, {{3, -2, -9, 0, 4, 10, -13, 6, -21, -5, 8, 7, -15, 16, -20, 11, 8, -1, -15, -8, 5, -11, -11, 6, -20, -2, -14, 19, 2, 2, -16, -12, 9, -4, -2, -1, 2, -20, 9, -27, -4, -12, -1, -8, 0, -12, -18, -11, 5, -18, -24, -17, 5, 18, 7, 14, 23, -7, -3, 5, -3, -16, -21, 13}
, {-23, 7, 17, -64, -13, -13, -8, -4, -1, 17, 16, -2, -2, -35, 8, -23, 16, -13, -3, 1, -16, -8, 19, 27, -13, -8, -12, 16, -41, 36, 9, -37, 6, -30, -15, -2, 3, -5, -27, -39, -8, 2, -27, -18, 3, 8, 9, 17, 29, -3, -6, 8, 0, -18, -6, -26, -10, -1, -3, -19, -14, -36, -7, -20}
, {22, 19, -6, -22, 13, 1, 9, 25, -16, -1, -24, 4, -21, 11, 16, -31, 6, -17, -15, -5, 16, 19, -9, -9, 13, -13, -17, -6, 9, -10, 6, -6, 16, 4, 8, -2, -2, -1, 5, -10, -13, 9, 15, 26, 11, -18, -1, -1, 10, 16, 16, 20, -7, 2, 8, 3, -27, -13, 0, -7, -7, 12, -6, -18}
}
, {{-4, -8, -4, 8, -18, -27, 0, -5, 19, -19, 9, -6, 8, 6, 3, 11, -9, 11, 9, -3, 3, -18, 6, -4, -13, 6, -4, -7, 21, 11, -1, 6, -18, 10, -9, -10, 7, -16, -21, 7, -14, 11, -15, -18, 1, -15, -3, 10, 11, 5, -7, 17, -10, -4, -5, -7, -6, 3, -14, -13, -30, -8, -8, 8}
, {-19, 7, 7, -1, 8, -22, 17, 26, 5, -2, 19, -8, -2, 8, 8, 14, -24, 2, 0, 0, 14, 20, 8, -14, -14, -9, -1, -12, -11, -1, 4, -8, 7, -1, 18, 18, 3, -30, 8, -31, 1, 0, 14, 9, 15, -13, -18, -12, 14, -12, 18, 13, 21, -20, 12, -14, -16, -20, -27, -10, 7, -12, -17, -15}
, {-3, 4, 24, 16, -7, -23, 17, 12, 12, 29, 18, -10, 3, -21, -24, -19, -22, 1, 9, 14, -4, 9, 29, 18, 16, -10, -5, -20, 0, -24, 10, 13, -3, 5, 12, 8, 2, -8, 15, 21, 6, 7, 12, -6, -2, 0, 4, -10, 10, -12, 21, 9, 1, -14, 9, -7, 17, -18, -2, 12, 2, -4, -2, -4}
}
}
, {{{18, 0, -1, -10, -12, -6, -7, 2, -8, 5, 9, 0, 1, -9, -24, 10, 12, -1, 10, 16, 0, -1, 14, 4, 12, -4, 8, 1, 4, 7, -5, -1, 7, 7, -5, 12, 1, 6, -6, -5, 20, 2, -5, -29, 1, 5, 12, 10, -4, -19, 12, -13, 5, -3, 17, 10, 13, -9, -3, 17, -16, 0, 2, -6}
, {-15, -5, 0, 11, -11, 9, -10, 8, -10, -13, -15, 5, 2, -8, -9, -11, 7, 1, -17, -10, -1, -28, 4, 2, -5, 8, -17, -8, 1, 12, -4, -10, 13, -8, -17, 0, -5, 5, -2, -17, 25, -5, 3, 0, -4, 0, 3, 5, -7, -16, 6, 4, 4, 3, 5, 0, 10, -8, -2, 1, -10, -14, -8, 0}
, {5, -20, -2, -4, -22, -1, 3, -10, -8, -10, -22, 4, 0, -5, 1, -23, 21, -4, -10, 12, -16, -3, -7, -6, -30, 9, 8, 1, -15, 13, 9, -9, -2, 0, -16, 1, -1, 7, -16, 16, 16, -6, 14, 7, -20, 2, -7, -2, -11, -12, -1, 1, -1, -4, 19, -6, -2, -14, 3, 12, -8, -9, 3, -1}
}
, {{-2, -10, 13, 18, -8, -9, -5, 22, 8, -20, -9, 10, -2, -3, 13, -15, -28, -14, 27, -1, -16, 1, 3, -13, -8, 1, -2, -1, 8, -5, -9, -4, 33, -3, 4, 24, -7, -3, -25, 22, -16, -21, -8, -4, -2, 0, 5, -9, -11, 14, 3, -10, -27, -10, -3, 11, -10, -7, -15, 28, -6, -4, 10, 19}
, {22, -19, -16, -24, 9, 0, 12, -46, -46, 17, 10, -24, 2, 5, -17, -9, 14, -26, 19, -18, 37, 13, 42, 27, -5, -7, -32, 2, 7, -6, -30, -20, -23, -21, -30, 12, 29, 6, -10, -11, -14, 30, -43, -15, -23, 2, 20, -2, -3, 16, 14, 15, 2, -46, -30, -20, -11, -9, -24, -28, -8, -3, -6, 12}
, {-13, -12, -25, -7, -5, -7, 17, -16, 10, 12, -5, -12, -10, -7, 1, -14, -2, 2, 17, 6, -13, -15, 8, 21, -16, 27, 5, -1, -23, -1, -2, -16, -9, -13, -3, 11, 3, -8, 5, -7, 5, 6, -13, 3, -11, 9, -5, 2, -5, -6, 18, 2, 6, -8, 10, -10, -2, 13, -15, -5, -2, 0, -3, -16}
}
, {{0, 5, -4, 25, -19, 11, 0, -1, 2, -2, 3, 4, 3, 7, 18, -9, -1, 13, 13, 21, -9, 5, 1, -9, -13, 5, 16, 19, -9, 4, -2, 1, -1, 3, 1, -5, -6, -18, 3, -5, 4, -4, -2, -1, -1, -7, -2, 2, -4, 0, 0, 6, 11, -17, -10, -25, -5, -4, 5, 6, 2, -5, 20, 12}
, {-16, 4, 3, 3, -11, 0, -23, 7, -13, 17, -9, -2, -5, 9, -13, -11, 12, -1, 3, -27, -14, -13, 14, -15, -4, 9, -12, -11, 20, -27, -9, 8, 16, 0, 3, -1, 1, -12, 28, -8, 6, -12, 1, -7, -7, -15, -12, 1, 32, -29, 4, -7, -8, -15, -8, -19, -1, 9, -21, -2, -24, -16, -13, 1}
, {9, 4, -12, 2, 1, -5, -10, 8, -1, 5, 6, -6, -21, 3, 15, -15, 18, 11, 7, -11, 4, -18, -11, -6, -16, 10, -20, 4, -3, -11, -8, 1, 11, -20, -14, 14, 0, -1, -1, -11, 2, -10, -4, 0, 8, -23, 6, -13, 5, 2, 12, 13, -5, -19, -4, -5, 0, 21, -8, -3, 7, -11, -15, -3}
}
}
, {{{1, -8, -5, -1, -13, -8, -3, -1, 3, 10, -11, 8, -12, -8, 18, -11, 0, -17, 0, -11, 5, -22, 0, -14, 1, -6, 5, -13, 5, -16, 2, -11, 3, 8, 5, -5, -17, 10, 3, -9, -3, 8, -21, 11, -7, -4, -3, 12, 4, -10, -5, 9, 0, -15, 4, -2, 22, -25, -21, -9, 9, 5, -9, 10}
, {4, -4, 9, -5, -7, 10, 1, -1, 27, 9, -5, 2, -9, -17, -2, -27, 1, -17, 2, 4, 7, 0, 1, -5, -10, -16, 22, 8, -5, 3, 1, -7, 7, 11, -14, 18, -11, -4, 0, 2, 5, 7, -3, 4, -34, -17, -4, -16, 2, -8, -15, 5, -12, 11, -11, 18, 21, -1, -15, 8, 5, -9, 12, 5}
, {-6, -13, -12, 16, 1, -10, 16, -1, 15, -8, -1, 0, 0, 12, -3, 19, -12, 21, 1, -2, 13, 12, -4, 5, 10, -4, 1, -9, -12, -2, -14, 14, -2, 10, 15, 0, 2, -12, 0, 0, 4, 7, -4, 1, 7, -15, 5, 8, -14, 10, 0, 24, 18, 2, -3, 18, 7, 4, -6, -4, 17, -16, -4, 14}
}
, {{13, 3, -19, 1, -21, -6, 13, -14, -8, -10, -3, 19, 9, 9, 11, 1, -22, 7, -8, 4, -12, -3, -19, -8, 8, 4, 0, 14, -2, 1, -7, 30, -1, 38, 0, -14, 10, 0, -7, 0, 2, 5, -1, 24, -2, 22, -6, 27, -16, -15, 1, 12, -2, -4, -1, -6, 5, 10, -5, -16, 13, -12, 10, -15}
, {-11, 11, 15, 1, -13, 13, 8, 7, 41, -24, -10, -5, -3, -16, 25, 6, -4, 22, 2, 2, 10, -5, -7, 3, -22, -2, -3, -25, -9, 9, 6, 4, -12, 28, 5, -12, -8, 22, -5, -1, -2, -3, 2, 6, -17, 24, -11, -1, -6, -8, -28, 14, 3, -22, -10, 5, 12, 9, 17, 16, 31, 2, -3, -11}
, {11, 6, -3, 21, 10, 13, 3, 7, 23, -12, 5, 2, 18, -5, -5, 2, 17, 0, 5, 9, -9, -6, -2, -26, -3, 13, -1, -4, 0, 12, 24, 12, 13, -3, 15, -1, -9, -11, -2, -2, -2, -3, -6, -19, -4, -18, -1, 5, -11, -10, -11, -12, -7, 16, 19, 4, -4, 3, 8, 17, -13, 10, 8, 3}
}
, {{-4, -15, -4, 3, -19, -8, 1, 11, 27, -14, 4, -13, 6, -16, -8, -7, -5, -12, 22, -10, 18, 1, -3, -4, 5, -6, 19, 5, 13, -10, -7, 1, 1, 17, -6, 4, -10, 0, -23, 9, -2, 4, 17, -6, 6, 2, 12, 7, -14, 2, -1, 0, -3, -9, -8, 8, 21, -6, 7, -12, 2, 6, -3, 0}
, {-2, -6, 3, -5, -4, 7, 0, -9, -1, 11, -3, 15, 22, -20, 16, 8, -1, 1, 21, 19, 16, -18, 22, -2, -12, 16, -2, 23, -24, 3, 17, 9, -4, 13, -21, 6, -5, -10, 4, -12, 7, 15, 7, -8, -11, 7, 4, 14, 1, 1, -2, -4, -13, -14, -10, -3, 2, 12, -10, -6, -13, -6, -12, -7}
, {9, 18, 0, 12, 7, 1, 0, -1, 12, -3, 0, 10, 3, 0, 5, 19, -15, 15, -6, -4, 2, 14, 5, -16, -15, -17, 1, -20, 3, 13, 15, 4, -13, 31, -16, 4, -8, -20, 1, 12, 0, -14, 5, -2, -20, -4, -1, 13, 15, -6, 4, -8, -3, -18, -4, 4, -20, 7, 2, 13, -1, -2, 4, 16}
}
}
, {{{12, -1, -17, -10, 6, 4, -17, -9, -2, 6, -9, -4, -9, 1, 13, -8, -23, 1, 5, -16, -5, 12, -2, -16, 5, 2, 1, 2, 3, -11, -4, -8, 0, -9, -6, -11, 9, 11, 0, -2, 3, 4, -5, -21, -15, 10, -4, 5, -11, -6, 12, -4, -16, -14, -10, 4, -2, 16, -2, 17, 0, -1, 5, 7}
, {-5, 4, 1, -4, -1, -5, 26, -3, -5, 7, -13, 8, -5, 10, -5, 2, -10, 6, -11, 8, 8, -15, -1, 10, -2, 5, 5, -10, 9, 9, 7, -10, 9, -13, 8, 9, 17, -11, 9, -3, -8, 3, 4, -13, -7, 8, 0, 6, 4, 1, 13, 1, -9, 12, -4, -10, 3, 0, -7, 17, 7, -11, 5, -7}
, {-11, -6, -6, 5, -9, 7, 10, -3, 2, 5, -15, 2, 10, -16, 10, -17, 7, -13, 1, 8, 5, 4, -4, -3, -3, 7, 17, -6, 5, 14, 6, -18, 5, -22, 2, 18, 5, 2, -26, -9, 10, 20, 10, -12, -14, -12, 24, 6, -6, 1, 6, -6, -9, 8, 5, 0, -8, 12, 0, 3, -11, -9, -2, 12}
}
, {{-5, 4, -3, 10, 2, -16, 12, 20, -5, 15, 1, -22, 12, -2, -2, 3, -22, -2, -1, 2, -2, 4, 2, -4, 5, -18, 1, -7, -5, -16, -8, 0, 12, -4, 22, 2, 10, -16, 4, 15, 2, 17, -10, 13, -14, -5, -24, 17, 5, -13, -10, -20, -4, -6, -20, -12, -22, -4, 5, -15, -21, 18, 7, -15}
, {-11, -8, -14, 13, -10, -22, -35, -1, 11, -25, -6, -5, -32, -12, 21, -6, -2, 9, -3, -24, -29, 8, -9, -48, -27, 2, 13, -35, 9, -10, -6, -1, -2, 16, 25, -5, -28, -7, -4, 8, 21, -54, 12, -2, -9, -7, -34, -8, -14, -28, 0, 17, -16, 1, 8, 6, -25, 25, 6, 16, 5, 33, -6, 14}
, {-11, -21, -1, 7, 0, 1, -19, -11, -2, -3, -12, 4, -12, -6, -1, 6, 10, -15, 6, 14, -4, -13, 7, -17, 0, -4, -6, 5, -20, 5, 20, -22, 26, 5, 4, 18, -14, 5, -5, 11, 7, -8, 16, -18, 12, 3, 0, -4, -9, -1, 7, 5, -9, 4, -19, 1, 0, -8, 4, -4, 13, -22, 18, -1}
}
, {{11, -9, 4, -15, 3, -8, -1, 15, -5, 26, -6, -4, -14, 2, -9, -1, -20, -17, -1, -5, 6, -3, 11, -3, 8, -12, -1, -4, 12, -14, -7, -1, -6, -2, 4, -1, -2, 10, 8, -5, 6, 8, 5, -9, 4, -1, -1, 7, 9, -4, -1, 5, -17, 6, 5, -13, 12, -15, -6, 5, -4, 0, 6, -1}
, {6, -7, 2, 7, -9, -10, -5, -14, 1, 17, -1, -4, -8, -8, 8, -1, -6, 3, 7, -4, -7, 5, -9, 0, -7, 20, 7, 2, 11, -2, -14, -30, 2, -1, -14, -4, -1, 5, -8, 2, 10, -15, -1, -6, -20, 0, -10, 7, 8, 8, 22, -13, -2, 7, 3, 13, -4, 4, -2, -5, -21, -6, -15, 15}
, {7, -3, 5, 16, 8, -1, -12, -12, 5, 12, -1, -18, -10, 2, 2, 13, 1, 2, -28, 1, 9, -36, 8, 5, -18, 1, 0, -22, -25, -2, -11, -24, 5, -15, -9, 7, -7, 14, -2, 11, 10, 3, 5, 7, -9, -11, -9, -1, -2, 6, 13, 7, 26, 26, 5, 0, 0, 3, 13, -2, -6, -19, 15, 8}
}
}
, {{{12, -17, -27, 1, -2, 18, -17, -21, 3, -8, -17, -9, -2, -2, 23, -4, 13, 21, -9, 18, -7, -4, -14, -9, 1, 7, 9, 8, -3, -12, -8, 8, -10, -6, 4, 9, -4, 28, -6, 10, 4, 6, -17, -8, 7, 14, 9, 6, -13, -9, -6, -2, -5, 10, 16, 3, 7, 3, -13, -11, 4, 1, -9, -13}
, {-5, -21, 5, -25, 13, 10, 5, -10, -4, -10, -17, -2, 7, 5, 1, -5, 2, -16, -2, -19, 18, 3, -11, 12, 2, 10, 10, -12, -1, 7, 13, -4, -14, -11, -10, -17, 0, 9, -7, 10, -7, 3, -17, -11, 10, -3, 24, -8, -15, -1, 2, -8, 6, -1, 2, -14, -4, 4, -17, 5, 2, -4, -13, -7}
, {5, -2, -10, -3, 7, -4, 4, -9, -2, -11, -24, 3, -1, 9, 10, 12, -7, 17, -3, -11, 0, 6, -4, 4, 6, 0, 4, -9, 3, 1, 1, -11, -4, 6, -7, 8, -2, 3, 7, 8, 11, -3, -6, 2, -8, -4, 14, -7, -1, -14, -8, 10, -4, -7, 12, 13, -13, -11, -8, 7, 7, 4, 6, -6}
}
, {{12, -18, -10, 17, -1, -9, -8, 8, 3, -2, -23, 21, -10, -5, 1, 11, 6, -4, 11, 1, 2, 7, -12, -3, -18, -13, 11, 11, -3, 17, -6, -8, 4, -7, 2, 12, 1, 9, -13, 8, -3, -16, -15, 6, 5, 3, 21, 7, -1, 1, -4, -9, -23, 17, 1, 6, -7, -7, 3, 16, -11, -2, 4, 3}
, {16, -9, 1, 12, -9, 18, -8, -2, -16, -2, 17, 4, -1, -23, -4, 2, -2, -8, -15, 3, 29, -36, 13, 23, -4, 9, 2, -33, 0, 0, -1, 7, 9, -24, -14, 11, 9, -3, 19, -2, 21, 0, -1, -17, -22, 13, 19, 5, 24, -22, -19, -29, 1, 14, -18, 5, 11, 0, -5, -7, 10, -8, -13, 22}
, {7, 16, 0, 10, 10, 18, -24, -5, 1, -2, -5, -1, -9, 8, -5, -8, 5, -10, -16, -5, -16, -18, -16, -10, -3, 20, -7, -5, 5, -10, 23, 7, 18, 7, 19, 4, -2, -7, 14, 7, -6, -6, -15, -12, 2, -8, 11, -5, 2, -5, -16, -8, -2, -9, 8, -13, -14, 10, 0, 1, 5, -8, -6, 7}
}
, {{-4, 10, -10, 1, -7, -12, 12, -12, 12, 1, 2, 5, 15, -21, -3, 4, -11, -4, 7, -7, -6, -17, -9, -6, 11, 3, 11, -9, -12, 5, 13, -14, -7, 2, 4, 5, -7, 2, -10, 2, -15, 23, 5, -21, -3, -1, 13, -4, -3, -4, 14, -2, -13, 1, 2, 4, 8, -5, -5, 4, -7, 15, -2, -6}
, {13, 2, 7, 0, 11, -15, 5, -4, -2, 11, -14, -2, 9, -7, -1, -11, -1, -14, -2, -32, -8, -4, -7, -4, 5, -21, 3, -3, -10, 2, -5, -15, -17, 10, 0, -6, -12, 19, 8, -10, 12, 10, -16, 1, -19, -1, 6, 16, -7, -8, -13, -10, -35, 16, 3, -9, 17, 9, 22, -13, -19, 8, 3, -18}
, {-16, -15, 18, 3, 2, -5, 9, -3, 19, 9, 16, -4, -22, -15, -4, -5, -6, 3, -13, 2, 5, 8, -17, 0, 3, 1, 2, -9, 6, -2, 15, 1, -28, 13, 8, 8, 13, 3, 21, -10, -8, -9, 5, 6, 24, -30, -26, 3, -12, -6, 0, -9, 2, 8, 7, 8, 14, 7, 2, 2, 3, -8, 5, -7}
}
}
, {{{11, 13, 23, 7, -1, 8, -17, 6, -4, -9, 5, 4, -9, 6, 4, -9, -1, 13, 5, 6, -17, -5, -19, 10, 8, 9, 12, 15, -5, -17, 9, -7, 26, 1, 1, 11, -7, 5, -3, 9, 13, 13, 4, 13, 17, -4, -2, 5, 6, 10, 0, -6, 15, 14, 10, 10, 4, -4, 25, 0, 3, 4, 6, 7}
, {3, 6, 3, -11, 12, 0, -14, 4, -2, -1, 0, 9, -2, -3, 3, 16, 0, 6, 15, 10, 9, 15, 9, -8, -21, 3, -13, -7, -9, -21, 2, -17, 24, -5, -3, 17, 8, -2, 3, -5, -6, -1, -4, -11, 13, 0, -6, -10, 12, 7, -4, -3, 6, 1, 3, 10, 8, 3, 10, -3, -1, 5, 11, 2}
, {2, -1, -3, 14, -10, -7, 9, -3, 8, 6, -8, 0, 15, -5, -1, -5, 12, 6, 10, 8, -5, 7, 16, -5, 6, 3, 2, -7, -3, -15, 3, 8, -5, 18, -10, 2, -7, 5, 5, 9, 2, 10, 5, 6, 8, 15, 7, -3, 14, 8, 11, -29, 19, 2, -4, 9, -13, 5, -3, 14, -6, 7, -10, 6}
}
, {{22, 2, 0, -12, -20, 12, -7, -16, -3, 0, 4, 11, 2, 1, -4, -5, 0, 7, 5, -29, 2, 17, -8, 13, -13, 17, -1, 0, 15, -5, 2, 7, -9, -2, 5, 9, 7, 6, 2, -16, 7, -25, -16, -2, 5, -8, 19, -5, -9, 17, 4, -1, 14, 8, 14, 4, -14, 2, -9, -4, 1, 1, 2, -6}
, {-16, 13, 4, 15, 0, 23, 17, 20, 33, 10, -2, -25, -7, -11, -21, 5, -7, 19, -19, -7, -22, -15, -15, 29, -54, -9, 4, 4, -34, 1, 3, -10, -16, -11, -7, -1, -7, -4, -12, -20, 5, 26, -8, -4, -9, -17, -27, -2, 15, -8, -21, 8, -27, 5, -12, -6, -9, 6, 1, -4, 5, -26, -2, 18}
, {22, 2, -7, -6, -8, -4, 10, -6, 0, 3, -1, -11, -8, -10, 1, -6, 4, -4, 3, -12, -4, 3, -7, 4, -3, -15, 8, -8, 16, 10, -2, -18, 5, 5, 0, -4, 13, -12, -11, -9, -10, -8, -1, -12, 8, 5, -9, 10, -19, -9, -6, 7, 5, 9, 1, 7, 7, 7, -12, 8, -5, -4, 0, 10}
}
, {{-12, 3, -2, -17, 1, 11, -9, -1, -3, 15, 12, 19, 9, -9, -10, 0, -9, 0, 4, 3, -13, -9, 13, 5, -11, -5, -1, 0, -8, -3, -15, -13, 11, -1, 7, -5, 1, 8, -25, 2, -10, -15, -12, 4, 8, -18, -15, 3, -12, -11, 18, -2, -14, 13, -8, -12, -11, 4, -4, -13, 6, -10, -11, 0}
, {-12, 12, -14, 22, -10, -13, -6, 13, -23, 4, 9, 4, 0, -13, 23, 22, 0, -5, -14, -10, -17, -7, 40, -20, -11, 19, 1, -11, -18, 20, -6, -13, -6, -4, -14, 0, 7, -1, 22, 17, -18, -14, 1, -3, -7, -33, -33, -15, -1, 11, 30, -9, 20, -1, 20, 8, -4, -9, -4, 13, -13, -1, 15, -23}
, {1, -2, 0, 9, -2, -6, -2, 4, 11, 5, -13, -1, -12, 2, -10, 12, 13, 7, 5, 11, -17, 1, 16, -4, -31, -4, -16, 2, -3, -5, -13, 12, -2, -2, -8, -14, -10, 12, -2, 16, 7, -2, 0, -10, -20, 1, -12, 3, 5, -17, 1, 5, -17, 3, 0, -3, -21, 8, -2, 32, -14, 15, 6, 12}
}
}
, {{{5, 7, -2, -15, -6, -2, 4, -5, -6, -9, -22, -8, 12, 6, 0, -16, 5, -1, 14, 1, 15, -10, 2, -4, -1, 4, -4, -14, -2, 9, 7, -4, -5, 4, 15, -9, 4, 12, -6, 2, 18, 12, -8, 3, -13, 24, 11, 2, -9, 2, 33, 18, -18, -3, 5, -15, -6, 7, 14, 8, 6, 15, -14, -10}
, {9, 4, 3, -17, -15, -3, 2, 20, -14, -16, -13, -3, -11, 7, -3, 0, 15, 0, -19, 19, 9, -2, 12, -6, -12, 7, 8, -8, 6, 3, -17, 2, 6, 0, 24, -4, 3, 12, -10, 1, 0, -2, -5, 7, -6, 1, 1, 4, -4, -13, 4, -4, 9, -15, -3, -5, 8, 5, -12, -3, -6, 4, 2, -2}
, {1, 17, -11, 8, -3, 9, -6, -7, -10, 6, -15, -3, 16, -6, 13, -10, 6, -10, -9, 14, 10, -6, -6, -19, -7, 4, 7, 5, 3, -4, 11, -12, -12, -9, -1, -5, 4, 13, -3, 11, 13, -4, 6, 4, 20, 22, 16, 7, -7, 2, -8, 0, -6, 5, -3, 5, -12, -1, 3, -3, -7, 16, -4, 6}
}
, {{13, 0, 9, 10, -16, -5, -3, -8, 1, -2, 3, 12, -3, -12, 14, -3, -9, -13, 16, -18, 2, 9, -10, 4, -13, -35, 0, -6, -3, 10, -12, 8, 9, 17, 13, 4, -12, -4, -9, -11, 15, 4, 14, 18, 30, 7, 0, -8, -6, -3, -5, -21, 12, -20, 12, 6, 8, -1, -16, 12, -9, 11, 6, 11}
, {-1, -18, -9, -7, -26, -16, -18, -19, -7, 25, 22, -12, -2, 5, -16, 9, 27, 6, -10, -7, 14, -2, -18, -5, -20, 16, 11, 4, 16, -14, -24, 0, -35, -8, -11, -2, 12, -16, -13, -5, -7, 13, 0, 2, 10, -31, -16, 7, -9, -36, -8, 5, 35, -4, 4, -15, -5, -15, -13, 8, -41, 3, 2, 7}
, {-25, 10, 4, -23, 8, 9, -22, -7, 4, -11, -10, 2, 3, -7, -14, -14, -3, 10, -2, -6, -22, -7, -7, 7, 0, 5, -8, 1, -31, 21, 0, -9, -5, 16, -12, -11, -14, 30, 1, -6, -15, -8, 2, 4, 13, 8, 3, -12, -3, -1, 1, -11, 3, 3, -4, -18, -2, -8, 2, 1, 21, -17, 7, -18}
}
, {{10, -3, -2, -14, 13, 1, -4, 22, 15, 13, 2, -7, -16, 9, -6, -8, -6, -2, -12, -4, -8, 11, 3, -12, 5, 7, 12, 21, 3, -11, -13, -9, 4, 0, 7, -10, 2, 20, 5, -16, 11, 0, -9, 14, 2, -7, 18, -9, 10, -5, -16, -6, -1, 0, -2, 11, -12, 4, 4, -7, -8, 4, 5, -4}
, {0, -3, 3, -21, -6, -14, 11, 1, -10, -3, -20, -9, -2, 13, -9, -10, 27, -4, 8, -7, -8, -1, -7, -3, 2, 10, 15, -5, 20, -3, -23, 2, 14, 4, -14, 7, 1, -1, -13, 3, -21, -9, 23, 13, 6, -1, -10, -4, 16, -11, 20, -1, -1, 4, -19, -5, 0, 4, -5, -14, 9, -8, -16, -3}
, {7, 4, -24, -6, -3, 13, -15, 1, 9, 1, 5, -4, -6, -1, -5, -22, 4, -5, -16, 1, -27, 0, 23, 4, 1, -10, 0, 10, 3, -11, -10, 9, 4, -14, -13, 2, -4, -3, -4, -6, 4, -21, -1, -4, -6, -1, 15, -2, 3, -2, 12, -12, -9, 16, 1, -12, -1, 5, -5, -1, 1, -3, -1, -5}
}
}
, {{{0, -20, -13, -7, 2, -6, 3, -17, 10, -11, -8, 31, 10, 6, 6, 1, 7, 2, -10, -9, -4, -4, -18, -14, -17, 7, 5, 3, 0, 2, -16, 11, -5, 10, -8, -10, -2, 21, -1, 0, 10, 8, -6, 14, -15, -5, 13, -24, 0, 1, -6, 1, -13, 2, 2, 1, -18, 7, 4, 5, 12, -5, -3, 18}
, {9, -6, 10, 14, 7, -6, -11, -16, 0, -8, -2, -1, -1, -2, -19, -7, 6, 10, 20, 24, -17, -25, 9, -16, 1, -13, -2, 0, 1, 10, 10, 14, -12, 5, 15, 10, -2, 1, -4, -6, 8, -3, -5, -12, 5, -1, -34, -7, 18, -5, 19, 10, 5, 5, 11, 5, -15, -6, 18, -3, -5, -3, 8, 11}
, {-2, -25, -14, 10, -9, -20, -10, -15, -4, -6, -16, -9, 6, 0, 6, 11, 9, -1, 31, -9, -19, -8, 13, -14, -23, -2, -6, -14, -10, 6, 7, 6, -23, -4, -13, -6, 7, 7, -8, -1, -15, 2, -9, -5, -13, 11, -3, -5, 23, -26, 1, 27, -2, 15, 3, -9, 10, -2, -17, 2, 0, 1, 11, -16}
}
, {{10, 9, 9, 7, 16, -9, -18, -12, 3, -2, 0, 1, -13, -15, -8, -21, 8, 16, -5, -16, 1, 1, -5, -8, -11, 26, -17, -31, -1, -3, 10, 24, -24, 2, -3, -12, -15, 5, -16, 14, -6, -7, -8, -11, 1, 17, -7, 24, -26, 0, 13, 6, -4, -12, 12, 18, -13, 11, 15, -19, -28, 1, 13, -18}
, {-24, 28, 21, 18, -2, -7, 24, 17, 25, -27, -8, 5, -13, -4, -17, -1, 2, 8, -1, -33, 13, -30, -49, -12, -6, -18, 10, -7, -18, 29, 3, -7, 2, -1, 2, -5, -23, 10, 8, 9, -48, 7, -1, -11, -4, 6, -7, -5, -10, 4, -9, 13, 2, -7, -7, 21, 27, -29, 7, 2, 6, 21, -28, -46}
, {-3, -10, -6, 25, -18, -10, 5, -10, -2, -2, -6, 7, 17, 6, 13, 5, -3, 3, -17, -2, 8, -28, 2, 1, -19, -2, -4, -15, -9, 3, 17, -16, 9, 1, -21, 2, -7, -4, 23, 2, 4, 5, -7, -28, -6, 12, -15, -2, -3, 19, -3, -18, -4, -1, -15, 18, -7, 12, -6, 1, -23, 5, -3, 10}
}
, {{-4, 0, -10, -5, 5, -5, -4, 21, -3, -13, -14, 6, 6, 2, -5, -6, 2, 13, -17, -17, 0, 1, 4, 3, 2, 19, 5, -6, -6, -14, -2, 7, 17, -6, 2, 9, -1, -17, -6, 9, -2, -2, 7, -16, -8, -7, 10, -11, -1, -15, 1, 5, -18, -6, -13, -19, -34, 7, 8, -1, -17, 11, 10, -5}
, {-9, -14, -4, 17, -15, 3, -11, -14, -4, -8, -21, -9, 9, -14, 10, 4, 1, 11, -12, -2, -4, -2, -3, 2, 24, 9, 6, -8, 3, 3, -5, 9, -14, -10, 4, -15, 1, 10, 10, -3, 0, 3, 4, -11, -6, -20, 4, 0, 4, 8, 25, 14, 18, 2, 19, 12, -12, -18, 2, 17, 15, 10, 7, 0}
, {10, 2, -14, -7, 18, -3, 25, 13, -4, -4, -37, -1, -8, 15, 1, -8, 2, 16, 1, 8, -14, -1, 29, -2, 8, -5, 9, -8, 3, 6, -3, -2, 13, -18, -11, -16, -3, 3, 2, -2, 5, 15, -15, -7, -1, 16, -1, 8, 16, -18, -8, -1, -13, -2, 21, -8, -9, -11, 14, 9, -9, 20, 24, -8}
}
}
, {{{21, -4, 2, -6, -12, 6, -3, 20, 10, -4, -15, 3, 2, 23, 2, 4, 6, 28, -11, 4, -23, -10, 3, -7, -9, 6, 19, -2, -2, 7, -1, 14, 18, 12, -12, 14, 5, 14, 7, -9, 12, 2, 18, 0, -1, -2, 2, 15, -8, -3, -7, -7, 14, -8, 11, 11, -7, 18, -3, 8, 1, -3, 3, -7}
, {9, 10, -2, -19, -6, 1, -11, -6, 0, -1, -15, -12, -2, -3, -23, -7, 12, 10, 11, 6, 1, 2, -5, 3, -6, -3, -2, -3, -5, 5, -8, -4, 0, 0, 17, -14, 18, -4, -3, 0, 15, 0, -3, 9, -9, -8, 10, -5, 8, -1, -2, -15, 15, 3, 17, 0, -1, 8, 0, -7, 7, -8, 10, 3}
, {11, 1, -33, 21, 5, 2, -13, -4, -2, -2, -10, -12, 8, 20, 10, 9, 3, 6, -6, 4, -5, 12, -3, -13, 2, 4, -9, 5, -14, -6, -7, -8, 15, -9, -3, 10, 1, -8, 7, 15, -6, -8, 13, 2, 2, 12, -9, -8, 0, -5, 14, -33, 21, -9, 15, 14, -18, 18, -20, 6, -16, -4, 13, -5}
}
, {{-6, 4, -12, 26, 12, -11, -7, 18, 13, -4, -19, 13, -16, 3, -1, 12, 0, 0, -21, -4, 0, -2, -29, -17, 15, -7, -15, 4, 9, -23, -6, -15, -12, 14, 19, 4, 9, 5, 17, -8, -1, 2, 10, -7, 5, -5, -2, 13, -6, 14, -6, 5, 0, -7, 0, 21, 7, 15, 4, -5, -1, 18, 11, -11}
, {7, -13, 29, -10, -17, -12, 2, -21, 12, -13, 4, -21, -1, -11, 4, 2, 4, -5, 4, -21, 2, 3, 23, 19, 11, 16, -7, -30, -16, 18, 30, -15, -27, 0, -9, 3, -6, 2, -11, -20, -23, 15, -16, -7, 23, 1, 4, 9, -2, 20, -7, -22, -14, -6, 3, 3, -27, 9, -4, 6, 2, 2, -10, -25}
, {-2, 0, -11, 2, 11, -19, 20, 12, 6, -1, -2, -12, -10, -6, 26, -5, -7, -2, -12, 27, 1, 8, -2, -2, 29, 15, 8, -2, 8, 8, 15, 7, -25, 0, 14, -16, 5, -4, -9, 0, -9, 24, -17, -2, 1, -9, 6, -3, -14, -4, -1, 23, 0, -12, -6, -18, -21, -6, 8, 11, 10, 30, 8, 13}
}
, {{-12, -11, 2, -5, 3, 8, 14, -5, -11, 8, 5, -4, 10, -9, -33, -18, -25, -2, -13, 7, -10, -2, 17, -2, -7, 21, 7, 4, 2, -4, -19, 1, -8, -10, -7, 12, 5, 10, -16, -11, -4, 2, -3, 0, -5, -11, -6, -8, 7, -1, 21, 2, -10, -14, -3, -11, -3, -1, -9, -11, -14, 2, 13, 8}
, {-29, -6, -3, -5, 18, 0, 19, -1, -5, 10, -20, -5, 11, -11, -5, 5, 6, 2, 3, 24, 10, 11, 8, -9, 19, 15, 0, -3, -14, -5, -3, -17, 13, -18, -5, 21, 5, -13, 18, 8, 13, 31, -3, -6, 14, -2, 2, -6, 3, -8, -3, 18, 3, -16, -4, -3, 7, 13, 11, -1, -14, 9, 9, 6}
, {20, -15, 11, 14, 6, 2, 17, -10, 6, 10, 10, 0, -13, -1, -3, 21, -6, 28, -14, 4, 7, 44, -11, -14, -12, 2, -5, -3, 11, 2, 10, 13, -20, 7, 19, 2, 0, -10, -12, 9, 25, -20, 6, 5, -2, -28, -15, -8, -10, 5, -11, 7, 1, 10, 15, 18, -4, 7, 3, -10, 17, 9, 7, 22}
}
}
, {{{4, -4, -6, 11, 9, -1, -12, 1, 20, -12, 6, -13, -7, -7, -8, -8, 7, 28, -9, -12, -3, -8, -10, 0, 5, -7, -13, 10, -15, -11, -3, -3, -10, 3, -9, 8, 4, -6, -19, -4, 17, 14, 5, -10, 3, -3, 4, 3, 0, -1, -2, 21, -6, -7, 22, 10, -9, 13, 4, 11, -27, -11, 13, 9}
, {2, -11, -2, -8, 6, 16, 8, -15, -11, 0, -7, 2, -5, -5, -18, 9, 2, -6, 5, 5, 20, -3, -15, 4, -13, -23, -24, 5, -2, -6, -29, 11, -14, -17, -21, 14, 27, -11, -7, -22, 1, -27, -4, -8, -6, 2, -9, -14, 21, -4, -5, -8, 1, -1, 21, 6, 13, 3, 3, 4, 5, -33, -6, 7}
, {2, -1, -1, 4, 1, 16, -5, -15, -6, 6, -3, -14, 7, -8, -18, 3, -12, -7, 0, 3, -12, -7, 0, -2, 10, 8, -30, -11, -16, 5, -11, 10, 14, -18, -5, 17, 5, -3, -11, -9, 10, -8, -20, -19, -7, 16, -4, -14, 7, -17, 3, 3, -8, 12, 7, -15, -7, 16, -9, -18, 12, -7, -7, 10}
}
, {{2, -1, -17, 2, 20, -8, -8, 17, 4, -11, -16, 9, -9, 4, -19, -1, -10, -18, -10, -10, -4, -12, 0, 7, 16, 18, -2, 3, 22, 19, 1, 9, -2, 2, 4, -27, -15, -14, -4, -9, -6, -4, -13, -10, -5, -8, 13, -6, 11, 2, 5, 12, -11, -2, 15, 6, -9, -8, 17, -13, -11, -13, -3, -2}
, {-22, 13, -32, -8, -21, 13, -9, 4, -27, 8, -8, 21, -5, 11, 7, 1, -4, 5, -14, 0, 7, -12, 4, 13, -8, -16, 10, -10, 8, -15, 21, 5, -10, 3, 21, -8, -4, -17, -16, -7, -16, 1, 10, -9, 0, -28, -11, 11, -10, -7, -9, 23, 15, -7, 0, 2, -26, 8, 6, -14, -12, -18, -14, -5}
, {9, 10, 7, 9, 8, 3, -16, 9, 7, -23, 14, 2, 12, 0, -6, -15, -17, 11, -34, 3, -22, -29, -18, -2, -17, 7, 2, -25, 1, 3, -7, 34, 2, -3, 2, -35, 4, -28, -15, 1, 10, -31, 3, -3, 6, -9, -19, 11, 5, -16, -18, 5, 3, -11, -4, 3, -30, -1, -8, 7, -7, -15, 21, 12}
}
, {{-22, 1, 15, -3, 3, -16, -13, -7, 1, 4, 20, -13, 4, -4, 8, -13, -8, -3, 8, 1, -15, 8, 2, 29, -9, -11, 4, -17, -23, 2, 8, -1, -23, 10, 2, 13, -13, 9, 15, 4, -17, -3, -9, 6, -2, -24, 2, -3, 2, 11, -4, -25, -14, 20, -1, -14, 2, -2, -2, 8, -9, -25, 9, 6}
, {-5, -24, -23, 32, 4, 1, -7, -16, -3, 2, -11, -1, 9, 9, 5, -16, -3, 14, 6, 18, -16, -8, 16, -5, 11, 25, 18, 6, -26, -5, 18, 2, 19, -16, 17, 27, 27, 14, -2, 5, 2, -32, 9, -2, 4, 18, 13, -22, -33, -5, 21, -10, -2, -21, -1, -14, -15, 31, -27, -2, -33, 10, 15, 11}
, {6, 1, 3, 7, 2, -14, -9, 20, -1, -1, -6, 10, -15, -4, -33, 34, -12, 3, -5, 6, 2, -3, 3, -4, 4, -19, 20, 0, -14, 5, 9, 6, -19, 15, 8, -8, -7, -6, 4, -7, -9, 7, 3, 1, -11, -24, -21, 24, -30, 5, -8, -21, -5, -5, -10, 16, 4, 1, -1, 5, -13, 2, 9, -16}
}
}
, {{{-8, -24, 4, 6, -16, 11, -7, 6, -5, 17, -9, -13, -4, 8, -12, -4, 7, -1, 3, 11, 17, 1, -6, -9, -5, 18, 10, 20, -5, -5, 10, -12, 6, 14, 1, -8, -6, 3, 4, 10, 2, -15, 7, 14, 3, 2, 0, -11, -13, -3, -1, -2, 3, 4, 16, -16, -7, -2, 2, 2, 0, -8, 11, -11}
, {20, -6, -14, -16, -5, 7, 12, -4, 0, -8, -18, 14, -7, 7, -3, -12, 3, -7, 1, -5, 2, -17, -8, -27, 0, -15, -3, -1, 12, -22, -20, -8, -6, -9, -5, 8, 2, -17, 0, 1, 8, 12, -19, -2, -13, 16, -3, -20, 0, -5, -12, -1, -22, 0, 11, -11, 3, 2, -25, -8, -8, -5, 16, -4}
, {-13, -4, -7, -14, -1, -3, -8, -7, -4, 10, -8, 7, -11, 17, 15, -14, 7, -1, -6, -5, 9, -1, -17, -14, -27, -10, 12, 8, 9, -1, -6, 0, 9, 3, -1, 6, -14, 0, -7, -5, 0, -16, 2, -3, -16, -16, 10, 2, 19, -8, -2, 3, -9, -3, -1, -18, -3, -5, -8, -11, -7, 3, 1, 1}
}
, {{-17, 0, -2, 3, -3, 8, -1, -1, 1, 10, 8, -7, -20, -2, -6, -14, 0, -7, -10, -2, -12, -17, -14, -9, 7, -9, -17, -7, -7, -2, -20, 9, 4, 10, -1, 8, 17, -4, -11, 22, 20, -1, 0, 4, -15, -23, -16, -5, 5, -10, -13, -19, -11, 9, 6, 12, -5, -6, -16, 20, -4, 11, 14, -19}
, {-10, 0, -18, -17, -3, -13, -4, -45, -28, -10, -13, -7, -9, 20, 21, 13, 0, -9, 28, -11, 6, 5, 11, -11, -22, -3, -5, 4, 24, -6, -38, -4, -13, 0, -11, -1, -4, -11, -17, -55, -29, -15, 8, 9, -3, -14, -34, -25, 10, -1, 8, 18, 7, -3, 45, -3, -13, -32, 10, 23, 7, 16, -18, -27}
, {-1, 2, -12, -2, 8, -16, -3, 13, 7, -1, 5, 14, 4, -10, 20, -18, 8, 18, -2, -17, -28, -27, 4, -19, -18, 5, 0, -20, -6, 1, 3, 2, 4, -2, -25, -7, 5, -21, -2, 10, 15, -13, -10, -31, -21, 9, 3, -1, 4, -16, 13, 3, -17, -7, -23, -10, -8, 5, -6, -8, -13, -27, -9, -5}
}
, {{8, -9, -3, 15, -9, -2, 8, 2, -18, -1, -6, -12, -19, -2, 7, -12, 11, -4, 12, -2, 6, -16, -5, -8, -30, -2, -4, -14, 10, 7, -16, 3, -13, -16, -25, 7, -1, 5, -4, 7, -9, -12, 2, -3, 0, 15, 2, -10, -8, 6, 10, -13, -8, 9, -1, 9, -10, -14, 5, 2, -13, -7, -10, -5}
, {-6, 5, -5, 2, 16, -14, -13, 0, 0, 14, -4, 2, -15, -6, 18, 16, 5, 6, 17, 7, -13, -9, 13, 8, -1, 6, -3, -1, -14, 13, -14, 1, -1, -9, 16, -12, 10, 13, 6, -9, -20, -10, -13, 9, 5, 3, -9, 13, 14, -4, -8, 12, 10, -14, -22, -11, -13, -8, -4, -7, 3, -21, -1, -1}
, {-3, 11, -13, -8, 1, 2, 3, -9, 0, -9, -20, -7, -4, -2, 11, 8, 4, 5, 8, 5, -12, 8, -8, 18, -5, 3, 13, -6, -4, 7, 12, -12, 5, -1, 14, -4, -4, -1, -2, -9, 3, 6, 6, 5, -14, 6, 6, 3, -13, -11, -3, 6, 17, 3, 4, -8, -9, 6, 11, -17, -9, -5, -9, 1}
}
}
, {{{3, 1, -9, 12, 3, -16, -13, 4, 23, 4, 4, 8, 2, -8, 11, -7, -18, -1, 17, 11, -9, 1, -16, -13, -9, -8, 4, 1, 3, -1, -4, -1, -12, 0, -3, -9, 11, 2, -2, 10, -6, 8, -13, -1, -19, 1, -16, 1, -7, -12, 11, -6, -13, 10, -4, 2, 10, 2, 11, -17, -14, 3, -6, -13}
, {-6, -3, -17, 16, 2, -5, 12, -3, -3, -5, 6, 0, 0, -2, -9, 21, 8, -2, -7, 16, 11, -2, 11, 9, -3, -15, 1, -4, 8, -6, -5, 2, -1, -13, 7, 10, -16, 5, 11, 11, -31, 9, -2, 0, -4, -2, -6, 4, -8, 18, 8, 0, 5, -9, -1, -11, 1, -7, -10, 0, 0, 13, 12, -11}
, {10, 1, 8, 12, -17, -3, 15, 4, -1, -15, -12, 2, 3, -2, 12, 13, 15, 6, 18, -2, 30, 0, -6, 1, 7, -6, -1, 4, -1, 3, 16, -22, -1, -6, 5, -1, -28, 14, -2, -10, 0, 29, -9, 5, -17, 7, -9, -13, -10, 1, 12, 6, 4, 19, -6, 1, -13, -2, 1, -1, 8, 5, -14, -4}
}
, {{22, 8, 8, 4, 1, -8, 4, 12, 4, 24, 3, -11, 5, -12, 4, -7, 1, -2, 6, -7, 0, -8, -7, -9, -12, -12, 13, -3, -3, 6, -6, 5, 21, 1, -7, 12, 1, 10, 2, -2, -8, 16, 3, -25, 6, 15, -11, 12, -1, 2, -1, -2, 20, 8, -13, 0, -12, 0, -5, -1, -6, 7, 8, -4}
, {20, -3, -16, -3, 5, -10, -42, -32, -25, 0, 4, 15, 1, -9, 9, 14, -5, -4, -17, -30, -7, -3, 8, -29, -3, 24, -7, -31, -2, 11, -9, -3, -6, -19, -6, -19, -4, 0, 24, -16, 31, -42, -4, -12, -17, 2, -3, -19, 15, -34, -14, 21, -17, 0, -15, 4, -5, 22, -19, -32, 2, 23, -11, 26}
, {-12, -14, -4, -13, -18, -3, -28, -8, 3, -5, -12, -1, 3, -10, 8, -9, 13, -23, 8, -6, -7, -36, 1, -24, 5, 29, 1, 0, -13, -10, 26, -9, 10, -5, 13, 17, -14, -4, -1, 0, 19, -17, -1, -20, 9, 8, 5, -15, -13, -7, 10, -4, -8, -11, -2, -3, 5, 10, -2, 11, 16, 1, -2, -11}
}
, {{26, 10, -5, -16, -12, -7, -11, -1, 3, 7, 16, -4, -14, 5, 8, 5, -10, 1, 4, -2, 5, 4, -1, -18, 7, -2, -8, -2, 6, 6, -21, 7, -1, -2, 8, 16, 1, -5, 2, 0, 5, 1, -6, -8, -6, -6, 9, -2, -6, 9, 0, 3, 1, -4, -4, -10, -8, 0, 2, 2, -3, 5, 5, 6}
, {-3, 21, -5, -9, -5, -7, 12, 7, -4, -21, -1, 1, 14, 3, 14, 6, 0, -4, -20, -12, 6, -1, -13, 1, 8, 3, -18, -5, -2, -4, -12, -8, 8, 10, -14, 13, -3, 5, 5, -13, 1, 20, 6, -7, -22, 0, -5, -7, 6, 1, 15, -13, -22, -14, -25, 19, 10, 8, 13, 3, -14, 7, 2, 5}
, {-2, 15, -12, -6, -18, -8, 5, -1, -9, 17, -22, -5, -9, -1, 12, 2, 4, 2, -3, -3, 0, -19, 22, -15, -5, -18, -5, -2, -13, -19, -19, -10, 29, 5, -1, -12, -12, 15, -3, 3, -8, 11, 3, 19, -4, 2, -13, 3, -10, 12, 8, 10, -4, -1, -6, 12, -2, -1, 2, 18, 13, -6, -8, 6}
}
}
, {{{-11, 3, -11, 31, 4, -10, 8, 14, 1, -6, -1, -8, 13, 2, 8, -14, -5, 7, 5, 21, 1, -17, -8, -5, -5, 8, 1, -2, -7, -12, 1, -4, -4, -8, 20, 9, -13, -7, -30, 8, 9, -8, 19, -8, 10, 4, 10, 4, -4, 0, -22, -12, 11, -4, 3, -12, -8, 15, -4, -11, -11, -15, -17, 12}
, {-17, 1, -22, 2, 4, 6, -23, -12, 9, -1, 1, 4, -18, 1, -14, -4, 15, 0, 5, -16, 17, 7, -2, 9, 4, -9, 2, 21, -9, -6, -15, -6, 5, 2, 24, 2, -2, 13, -1, 1, 0, -8, 12, -15, 22, -22, -10, 2, -6, 11, -21, -24, 10, -2, -5, 13, -9, -15, 6, 16, -9, 10, -9, -8}
, {8, 6, -11, -2, 10, -7, -14, -24, 1, 18, -1, -2, -3, -5, 15, 21, 27, -10, 5, -16, 0, -11, 10, 12, 23, 20, -24, 8, -1, -8, -2, -5, -5, 0, -5, -12, -13, -6, -3, -16, -7, -4, -5, -4, 11, -4, -15, -4, 23, 15, 1, -21, -4, -1, 1, 8, 19, 7, 7, -10, -18, -14, -11, 13}
}
, {{7, -11, -13, 9, -3, -27, -3, -24, 10, -9, 14, 2, 18, 4, 8, 6, 5, -9, 28, 7, -4, -12, 4, 4, -6, -28, 8, 4, -10, 0, 2, 13, -12, 7, -7, -7, 19, 12, -10, 6, 19, -9, 17, 3, 5, 12, 22, 0, -18, 5, -18, -1, 18, 18, -4, -12, -4, 21, -12, -4, 17, -6, 0, -12}
, {-5, 14, 16, 12, -7, 2, 2, -14, -13, -9, -21, 20, 4, 11, 6, 5, -8, -16, -7, -10, 2, -5, -3, -13, -9, -7, 21, 17, -1, -4, 16, -2, -28, 4, -12, -22, -5, 4, 2, 11, -23, -7, 10, 10, 5, 6, -11, 2, -19, 13, -2, -8, 1, 10, 3, 5, 3, -6, 12, 27, 15, 12, 17, -17}
, {8, -8, 12, 22, 21, -12, -19, 3, 20, -13, -3, 5, 8, -29, 13, -14, 10, -1, -15, -3, 8, -11, 5, 2, 14, 2, 6, -12, 3, 8, -6, 31, -7, 4, 1, -6, -10, 6, -13, 5, 16, -4, 2, 12, -18, 12, -1, 17, -14, -5, -15, -23, 1, -10, 0, 10, -1, 15, 3, -3, 9, -3, 14, 5}
}
, {{11, -10, 6, 1, -16, -8, -5, -5, 5, -2, -14, 9, -8, 4, -6, -2, 0, -2, -2, -3, -17, -29, -29, 0, -10, -5, 5, 4, 3, -3, -12, -3, -22, 0, -16, -4, 25, 6, 7, 17, 17, -5, 11, 11, 13, 22, -7, -18, -19, 27, -9, -12, 11, 0, 6, 6, -17, 5, -13, 15, 25, -4, 2, -20}
, {25, 0, 6, -11, -27, 6, -4, -22, -1, 5, -18, 1, 0, -5, -6, -4, -3, -2, 11, 3, -12, -11, -2, -1, -29, -26, 3, 9, -4, 4, 0, -5, -17, -1, -16, -24, 5, -2, 2, -4, -17, 0, 12, 5, -14, 14, -17, 11, -9, -36, -12, -15, 14, 2, -1, -13, -8, -13, 12, 1, -5, 7, 17, 2}
, {-17, -2, -15, 6, -3, 21, 16, 34, -3, -7, -13, -11, 13, 7, 1, 6, 7, -2, 22, 18, -5, 0, -8, -11, -4, 4, 21, 6, 31, 24, -2, 6, 8, 8, -2, 6, 34, 2, -6, -1, -6, -14, 14, -11, -7, 1, 5, 6, 3, 8, 5, 15, 19, 0, -5, -13, 1, 16, 6, 2, -17, 1, 0, -4}
}
}
, {{{-5, -4, -19, 2, 15, 8, -6, -7, -11, -2, -10, 8, -5, -5, 12, -10, -11, -4, -9, -5, 5, 6, -16, -2, 19, -2, -4, 0, -2, -2, 4, -26, 8, -7, 2, -3, 19, -9, -5, -6, 3, -19, -9, -17, -3, -12, 7, -4, 19, 1, -5, 5, -13, 5, 9, -9, -3, 10, -3, -12, -10, -10, -2, 13}
, {-1, 0, -1, -10, -3, 4, 4, 16, -18, 3, 1, 0, -15, -1, -13, 10, 10, 19, 2, -4, 1, 7, -8, -3, 5, -5, 4, 17, -2, -2, -8, 2, 12, 7, -20, 19, 2, -7, 14, -4, 2, -5, -1, -10, 22, -26, -13, -1, 0, -10, -8, 0, 5, 1, 2, 5, 4, 10, 8, 10, -13, 2, 6, 14}
, {9, 1, -5, -17, 7, 11, -4, 0, -11, 5, -3, -12, 12, 5, -11, 8, -1, 23, -7, -2, 17, 20, -1, 12, 2, 4, -10, 20, -10, -11, 1, 9, 20, -20, 6, -5, 3, 1, -2, -10, 7, -18, 11, -1, 26, 11, 1, 9, -5, 6, 13, -5, -6, -2, 7, -9, -18, 0, 9, -14, -17, -1, -13, -11}
}
, {{-16, -1, -6, -12, 8, 6, 8, -3, 6, -6, -8, -4, -9, 2, -5, -21, -16, 21, -11, -10, 10, -2, 0, -12, 23, -9, -25, -15, -27, 0, -9, 11, 11, 13, -19, -3, 22, -28, -9, -17, 3, -5, 11, -4, -3, 7, -6, 10, -15, 1, -8, 1, 9, -6, -9, -4, -18, 8, -1, -13, 18, -22, 21, 7}
, {-8, 12, 20, -23, 9, 8, -10, -3, 8, -1, -2, 7, -3, -4, 8, -16, 5, 13, -12, -15, 13, 3, -1, 3, 12, 11, -9, -5, 15, 0, -1, 7, -2, 1, 19, 11, -6, 4, -8, -8, -23, -17, -15, 25, 4, -2, -7, -13, -13, 15, -2, -23, -1, -4, 0, 20, -1, -16, 32, 5, -14, 8, 0, -25}
, {-5, 13, 6, -5, 8, 0, 6, -3, 7, 1, 16, -2, -2, 14, -11, -15, -5, 8, -3, 3, 1, 12, -10, 22, 5, 26, -16, -18, 24, -25, 13, 11, 8, -4, 13, 4, -4, -16, -14, -20, 19, -15, 20, 14, 1, -11, -17, -9, -33, -5, 5, 9, 4, 5, 2, 1, 10, 3, -9, -16, -8, -16, -10, 8}
}
, {{13, -22, -2, -10, 20, 14, 1, -1, -3, -5, -6, -15, -10, 4, -16, -13, -6, -12, 13, -9, 9, 11, -9, -12, 13, -2, 10, -13, 7, 1, -18, 4, 7, -12, 13, 13, 5, -6, 1, 8, -2, -3, -1, -10, 14, 1, -12, 15, -3, 0, -24, 9, -20, -11, 1, -13, 8, 3, -4, 5, -11, -12, -10, -7}
, {10, -14, 5, -8, 23, 32, -5, -4, -13, 0, -4, -23, -22, 14, 0, 6, -5, 11, 9, 9, 8, -7, -25, 4, 6, 6, 0, 1, -9, 13, 6, 7, -4, -3, 15, 11, 11, -20, 0, -11, 17, -15, 11, 2, 0, 21, 6, -10, 4, -11, 6, 16, -5, -6, -5, 2, 7, 7, -12, -9, 7, 9, 2, -6}
, {-4, -1, 10, -13, 10, 7, -17, 11, 0, 18, -5, 13, -6, 5, -1, -4, 18, 13, 31, 19, 5, -14, 2, 21, 8, 7, -11, -2, 26, 14, 11, -11, 14, 1, 16, 31, -7, 26, 17, -1, -5, -14, -4, -15, 0, 0, 15, 6, 1, 11, 10, 8, 17, 2, -6, -19, 13, 26, -5, -8, -19, 24, -12, -5}
}
}
, {{{-15, 3, 0, -7, -2, -6, -4, 6, -1, 20, -6, -6, -11, -26, -4, -17, 14, -5, 6, -6, 7, -8, -15, -23, -5, -3, -8, 16, -16, -2, -2, -1, 4, -1, -7, 4, -3, 3, -3, -8, -4, 0, -17, -20, -15, -21, -20, 16, 0, 9, 2, 10, -16, -9, 5, 9, -1, -7, -14, -9, -13, -4, 2, 17}
, {13, -28, -3, 0, -1, -1, -7, -4, 7, 15, 13, 6, -8, 13, -12, 8, 16, 0, -6, -20, -2, -8, -23, 2, -10, 5, 27, -9, 24, 8, -12, -3, 7, -15, -14, -1, 6, -19, 23, 14, 23, -8, -7, -35, -5, -1, 6, -8, 2, 2, -7, -15, -4, 4, 12, -11, 19, 1, 1, -3, -7, 7, -14, -9}
, {-13, 3, 0, -3, 15, 8, -1, -7, -16, 7, 5, -16, -8, -10, 10, -11, -14, -11, -7, 2, -4, -21, 12, 0, 12, -1, -18, -15, -6, -2, 18, -5, -7, 4, -16, -5, 0, 4, -17, 0, 3, -12, 2, 4, 1, -2, -9, -8, 0, 6, 17, 3, -24, 22, 7, -9, -11, -2, 5, -5, -6, -17, -5, 1}
}
, {{0, -8, 5, 2, 6, 2, -6, 9, -20, 4, -15, -3, 6, -17, -13, -1, -4, -19, 6, -2, -7, -34, 9, -6, -15, 3, -9, -3, 3, 1, 4, -4, 1, -2, -3, 7, 2, -5, -6, -17, 6, -18, 2, -29, -1, -25, 8, -2, -2, -1, 3, -12, -5, -3, 22, -17, -13, 6, 7, 18, -7, 7, 3, 10}
, {-27, -28, -39, 17, -18, -1, -20, -10, -12, 2, -7, 40, -12, 24, 33, 1, 22, 14, -6, -18, 2, -15, 10, -4, -2, 0, 5, -9, -11, -26, 13, 6, -9, -17, -5, 3, -18, -3, -7, 23, 0, -8, -17, -17, -7, -40, 8, -11, -16, -1, 3, -11, 22, 2, 6, 2, 1, -4, 19, 4, -22, -25, -4, -19}
, {13, 10, 2, 10, 0, 12, -22, -5, -6, -19, 12, -12, 3, 4, -9, 13, -12, 27, -7, 8, 1, 1, -5, 7, -29, 14, -18, -10, -6, -9, 8, 27, -10, -12, -10, -24, 29, 6, -2, -10, 3, 1, 34, 4, -12, 14, 15, 15, -19, -11, -8, -12, 8, -23, -11, 8, -8, 1, -3, 1, 18, -23, 10, 2}
}
, {{-1, 11, -3, -7, 0, -13, -15, -4, -13, 6, 9, -10, -16, -6, -6, -2, -15, -28, -9, 9, -13, 14, -13, 11, -1, -8, 6, 0, 3, 3, -1, 4, -4, -9, -7, -7, -1, -1, 7, -3, 3, 2, -13, -18, -8, -3, 4, 10, 33, -19, 11, 14, 9, 0, 2, -1, 11, -9, 2, -10, 8, -25, 11, -11}
, {-17, -11, -27, -5, 3, 2, 1, -4, -1, -9, -21, -10, -17, 20, -6, -14, 3, -14, 0, -1, -17, 16, -19, -5, 5, -2, 18, 7, 14, -20, -16, -11, 12, -13, 7, 2, -3, -1, -20, 3, 5, -13, 4, -14, 8, -7, 16, 22, -15, 5, 21, 9, -28, -2, -27, -12, -30, 22, -24, -21, -14, -2, -12, 8}
, {-3, 15, 11, 5, 3, -11, 1, -2, -4, -1, 2, -7, -23, -12, 4, 15, -31, -6, 9, 3, 18, 7, 7, 3, -6, -11, 30, -5, 2, 15, 9, 1, -19, -11, 0, -8, 4, -5, 15, -13, -18, 21, -5, 4, -3, -8, -18, -5, -19, 1, -3, -3, 22, 18, -13, 22, 18, -13, 13, -7, -9, 13, 14, -20}
}
}
, {{{-10, 3, -7, 8, -12, -18, -5, -2, -17, 11, -1, 0, 9, 4, -1, 0, 4, -8, -1, -3, -13, -8, -8, 9, -31, 4, -3, -24, -9, 4, 5, 0, -18, 5, -7, -3, 7, 7, -12, 0, -14, -3, -6, -12, -13, -3, 12, -7, -13, 8, -6, -7, -14, -10, 3, 9, 16, 3, -21, 9, -11, 10, 4, 7}
, {12, -7, -17, 5, -15, -24, 7, -6, 22, 8, -11, -8, -27, -8, 5, -1, -7, -25, -5, 10, 9, -1, 1, 15, 1, -2, 22, -5, 1, 6, 2, -3, -8, 1, 6, -4, -5, -1, 16, 14, 15, 8, -20, -8, -4, -10, 1, -6, 4, -17, -18, -10, -16, 8, 4, 6, 7, 13, -22, 6, -15, 5, 8, 9}
, {4, -20, -20, 6, 7, -4, -9, -1, 9, 2, -2, 8, -9, -9, -7, -4, 8, -3, 0, -20, 16, -15, 12, -2, -5, 8, -14, -17, 8, 2, 2, -19, -12, -1, 1, 3, -7, -13, -10, -7, 3, -2, -12, -4, -5, -9, -9, 3, 4, -6, -5, -3, -26, 8, -9, 17, 7, 5, 8, -3, -11, 4, -10, 5}
}
, {{10, -15, -1, -8, -9, -1, -10, -6, 12, 15, 0, 4, 1, -19, 2, -10, 8, 4, 2, -16, 2, 5, 14, -3, 4, -17, -12, -5, 6, 6, -20, -18, -13, 19, -6, -3, 17, -4, 20, -12, 8, -5, -7, 2, -1, -3, 5, -13, -7, 5, 12, -2, -2, -12, 4, 9, 4, 8, -1, -18, 3, -7, -5, 11}
, {-17, -5, 9, -14, 13, 3, -13, -10, -16, 4, -20, 17, -4, 2, -9, 3, 12, -2, 1, -2, -16, 18, -2, 10, 0, -12, -17, 24, -21, 4, 8, -17, -6, 14, 0, -15, 8, 34, 16, -1, -22, 8, -6, 4, 2, 9, 4, 4, -15, 14, 1, -6, 10, 10, 15, 12, 24, 6, 3, -6, 3, -9, 15, -13}
, {13, -2, -4, 6, 7, 10, -4, -19, -18, -17, -24, -14, 2, 1, 0, 5, 17, -3, 9, 22, 10, -16, -7, -1, -17, 13, -13, 1, -5, 12, -1, 1, -2, -29, -12, 3, 5, 21, -7, -1, -15, 0, -15, -10, -1, -8, 24, 8, -15, -3, -2, 5, -25, -6, 2, -17, -1, 12, -1, -19, -2, -9, 13, 18}
}
, {{0, -5, -6, 10, -11, -2, 8, 7, 2, 4, -9, 6, -13, -12, -2, -7, -7, -2, -22, 10, -10, -4, -19, -11, 17, 6, 22, 0, -8, -4, -4, 11, -12, -4, -1, 1, 3, 11, 2, -2, 5, -4, 0, 1, -12, 14, 5, -6, 13, -12, 5, -7, 16, 7, -6, -1, 7, -20, 3, 29, -9, 20, 5, -9}
, {-3, 6, -8, -33, -13, -15, 9, -4, -4, 15, -3, 2, 2, -8, -7, -2, -1, -18, -6, -15, 10, 6, 8, 4, 6, 1, -4, 0, -3, -2, -8, -7, -14, -11, 6, -6, 10, -14, 11, -23, -1, 9, -9, -6, 0, -19, -12, -23, 17, -26, -20, 11, 9, 2, -2, -14, 0, -23, -1, 1, -29, -3, -18, 1}
, {-22, 20, 4, 17, 11, -11, -6, 11, -13, 2, -12, -5, 2, 27, 25, -22, 16, 9, 20, 2, 5, 21, 10, -16, 13, 1, 5, -4, -2, 5, 1, 0, -7, 2, 15, 29, 5, -11, -1, 5, 1, -22, 11, 7, -13, 3, -14, -14, 17, 9, -9, 4, 16, -6, 7, -5, 3, 5, 0, 4, -18, -2, -18, 6}
}
}
, {{{7, 11, -4, 1, 18, -13, 10, -5, 0, -12, 11, -6, 10, 10, 1, -4, -22, -24, -9, -11, 3, 3, -21, 3, -2, -26, 24, 19, -8, 4, -10, 5, -20, -11, 13, 2, -7, 4, -8, 2, 3, 10, 13, 11, 33, -4, 10, -14, -8, 18, -10, 0, -13, 1, -5, 6, -21, -10, 9, 15, 9, 20, 20, -7}
, {3, 7, -22, -1, 10, -5, -12, -6, -1, 2, 10, -8, -27, -8, -14, 13, 4, -20, 7, -16, 6, -2, -8, 4, 1, -16, 0, 12, -10, 3, 0, 7, -1, 1, -2, 12, 7, 2, -11, -10, -7, 10, 0, -19, -1, 9, -5, -15, 15, 7, 8, -4, -19, 1, -5, 10, -1, 2, 20, -12, 14, 20, -1, 1}
, {4, 3, -20, -10, 2, -25, -12, -6, -3, 13, 0, 18, -21, 15, 12, -2, 2, -12, 17, -15, -16, -17, 7, 7, -8, 6, 2, 8, -16, 19, 21, 13, -28, 28, -3, -15, -37, 3, 27, 3, -16, -2, -28, 17, -1, 13, -10, -9, 1, -5, 10, 3, -2, -4, -7, -18, 8, -3, 10, -20, 17, 17, -2, -24}
}
, {{27, 11, 11, 16, 13, -2, -21, 3, 5, -1, 1, 13, -16, -8, -16, 10, -4, 9, -13, 0, -3, -3, -14, 0, -1, 13, 7, 15, 11, 8, -9, 15, 0, 15, 15, 3, -19, 3, 6, 9, 23, 13, 3, 17, 1, 16, -18, -6, -24, 0, 3, -17, 5, 2, 3, -5, 14, -17, 3, 4, 21, -3, 14, 5}
, {7, -3, 22, -12, 8, -24, -2, 6, 26, -1, 12, -2, -18, -23, 8, -18, -20, -30, 2, -24, -9, 7, 17, -9, 6, -31, -26, -6, -12, -3, 0, -8, 28, 1, -24, -12, -20, 20, 3, -12, -11, -19, -45, 3, -14, -15, -15, -6, -6, 26, 8, -7, -12, -7, 15, -7, 25, -37, 7, -7, -1, 3, -2, -6}
, {-19, -21, -5, 12, 0, -13, 4, 8, 6, 23, 1, -26, 11, -1, 32, 4, -23, 2, -21, 14, -22, 0, 13, -8, 6, 13, -10, -33, -37, -5, 10, -16, 5, -7, -25, 12, 3, 16, 5, 2, 5, -17, -8, -14, -5, 11, -2, -3, 10, 24, 11, -20, -18, -14, -6, 4, -12, -5, -15, 5, -22, -10, 7, 27}
}
, {{7, 5, 2, -30, -4, -5, -13, 21, -19, -11, 13, 5, 7, 20, -6, 17, -2, -15, 9, -5, 12, 13, 7, 18, 6, 23, -13, -7, 10, -2, -12, -4, 4, -6, -11, -5, 0, -2, 6, -19, -18, -6, -8, 5, 6, -3, -8, -10, -4, 19, 20, 8, -1, 2, -3, -45, -9, -21, 10, -3, -27, -12, 17, -1}
, {-22, 1, 3, 12, -12, 6, -4, 22, 5, 20, 22, -10, -11, -2, 1, 1, 0, 2, 26, 10, 0, 8, -8, -10, 2, -9, 6, -8, 11, -9, -11, -7, 5, -29, -9, 16, -5, -32, 9, -39, 9, -3, -5, -14, 0, -35, -6, -1, 16, -20, 5, 13, 3, -6, 7, -21, 14, -5, 5, -13, -6, -26, -9, 29}
, {-13, 2, 12, 6, -5, 12, 33, 3, -5, -10, -4, -9, -5, 5, 5, 6, -13, 13, -14, -7, 20, 2, 7, -18, 15, -5, 10, -15, 12, -8, -9, -3, 3, -13, -6, -6, 10, -9, 14, -3, 3, 26, 1, -16, -3, 11, 10, -19, 8, 1, -2, 12, 0, -14, 12, -13, 3, 8, 8, -6, 11, -7, -17, 11}
}
}
, {{{3, -3, 4, -9, -14, 4, 7, -5, 4, -18, -9, 16, -5, -6, -13, 5, 9, -22, -1, 2, 1, -21, -7, 3, -3, 9, -19, 2, 8, 5, 12, -5, -2, -5, -6, 4, -3, -2, -13, 1, -3, -3, -18, 5, -15, -23, -1, 11, 11, 11, 3, 16, 1, -12, 1, -5, 14, -23, 5, 11, -1, -5, 9, -17}
, {1, -14, -8, 8, -19, 0, -2, -5, -22, -7, -8, 8, 19, 3, 12, 0, -8, -3, 2, -4, 16, -3, 0, -15, -21, 4, 25, -2, 7, 0, -9, -5, -16, -6, -14, 4, -3, 6, -10, 8, -1, -5, 1, -15, -2, -3, -1, 3, -3, 15, 18, 2, 17, 0, 18, -14, -16, 7, 0, 10, -2, 9, 6, -12}
, {12, 2, -1, 4, -38, -3, 3, -18, -10, 5, -10, -4, 1, -7, -11, 8, -13, -14, 21, 9, -10, -3, -9, -14, -28, -6, 5, 1, 3, -19, 2, -23, 9, 1, -8, 2, -11, -5, 14, 7, 14, 16, -11, -3, -1, 4, -10, 4, -11, -27, -6, 0, -16, -17, 2, 9, 5, 5, -3, 3, 8, 15, 10, 4}
}
, {{1, -13, 3, -7, -11, -12, -14, -13, 12, 13, -4, 0, -21, -9, -5, 1, -17, 9, 16, -14, 18, 11, 5, 11, -10, 4, -6, 5, -5, 3, 11, -4, 3, 14, -5, 13, 4, -6, 14, 0, 17, -12, 6, 7, -6, 9, 1, -7, -3, -30, -2, 9, -9, 0, 10, 4, 9, 17, -22, -14, 9, 3, -26, 15}
, {-48, 36, -2, -17, -23, 9, 8, 21, 31, 4, 22, -11, -4, -10, 11, 14, -22, 4, -5, -8, 3, -6, -6, 13, 21, -8, 15, -8, -44, 4, 4, -25, 19, -4, -23, 29, -51, -10, -12, 25, -13, 20, -7, -1, 13, -14, -25, 0, 9, 7, -18, 7, 12, 15, 20, 12, 6, -14, 0, -24, -9, -38, 2, -12}
, {-4, -5, 5, -1, 9, -5, -17, -17, 15, 28, 11, 9, 0, 3, 8, -1, -2, -3, -10, -9, 8, -15, -20, 23, -9, 17, -18, -19, 6, -3, -2, 9, -11, -11, -16, -9, -1, -6, -8, -12, 16, 12, -19, 12, 14, -14, 0, -9, -3, -12, 6, -8, 3, -2, -2, -2, 28, -9, 16, -6, 13, 8, -6, 23}
}
, {{-2, 10, -31, -24, -2, 3, 4, 2, -20, 13, 8, 11, 12, 5, 24, 4, 7, -13, -29, -5, 0, 16, 7, 2, 20, 5, -5, 16, -21, 18, 17, -12, -3, 15, 8, -16, 0, 2, 24, -1, -17, 0, -6, -5, 14, -18, 2, 5, 5, -18, 2, 10, 1, 3, 2, 0, 2, -11, -8, 0, 0, -2, 12, -15}
, {23, -28, -14, -2, 21, 6, -17, -14, -22, 2, -4, 16, -8, 26, 11, -48, 18, -5, -3, 22, -24, -12, -30, -17, -7, 0, 3, 12, 20, -15, 6, 25, 27, 2, -13, 5, -19, -1, -5, -6, 7, -35, 10, 23, -7, 21, 12, -20, -6, -3, 4, 22, -9, 16, 9, -15, -36, 31, -28, -20, 5, 11, 11, 24}
, {3, 1, 3, -40, -11, -8, 10, -1, 1, 4, 3, 17, 9, -4, -30, -23, 17, -19, -2, -12, -1, -14, -1, -17, -1, -2, 6, 6, 4, 19, 9, -4, 8, 2, -7, -28, 10, -12, 22, -4, 18, -14, -19, -20, -1, 3, -6, 1, -2, -4, -6, -21, -12, 29, 22, -12, -3, -8, 4, 10, 0, -17, -17, -7}
}
}
, {{{20, -12, -3, 9, -3, -4, 7, -7, 9, -7, -25, 3, -2, -6, -21, 11, -25, -15, -1, -7, 0, 21, 5, -6, 11, -15, -28, 5, 6, -3, -14, -24, -14, -2, 10, -6, 15, -5, -20, -9, 16, 2, -10, 0, -5, 25, 9, 6, -8, -33, 2, -4, -2, -14, 16, 12, -13, -23, -2, 9, -19, 13, -6, -12}
, {-23, 10, -4, -6, -5, -2, -5, -1, -8, -16, -15, -3, -7, -4, 2, 14, 0, 1, 1, -7, 11, 5, -6, -8, -10, -25, -18, 12, -15, -21, 15, 1, -20, 1, 11, 11, 9, -15, -10, -14, -16, -10, 0, -15, -14, -21, 9, 2, 26, 1, -10, 2, 7, -3, -4, 17, -19, 5, 9, -9, 4, -4, -7, -1}
, {-14, 10, -17, 32, 0, 21, -11, 22, -1, 5, -30, 10, -10, -19, 8, -13, -12, 11, -13, 8, -5, -6, -32, 2, 5, 25, -14, 2, -10, -1, 22, 2, 17, 5, 19, -3, -9, -21, 0, 3, -6, -16, -10, 3, -3, -4, 15, -18, -1, -17, -5, 28, -21, -9, 13, 4, -18, 17, -9, -10, -6, 16, -14, -1}
}
, {{-8, 0, 7, 15, -8, -11, -11, 26, 28, 21, 20, 7, -6, -3, -1, -30, -3, -19, 16, -24, 0, -3, 15, 7, 5, 6, -21, -28, -3, 5, 8, -8, 7, -3, -8, 17, -21, 0, -7, 11, 12, 22, -20, -5, -13, -15, -18, -22, -16, -8, -4, 16, -8, -4, 16, 13, 17, 4, -9, -10, -15, -27, 2, 11}
, {1, 14, -2, 1, -7, 18, 6, -1, 6, 25, 4, -3, -17, 5, -19, -20, -17, 7, 5, -20, 24, -7, -4, 39, -11, 1, 4, -1, 1, 7, -7, -11, -22, -26, -9, 12, -6, -9, -30, 5, -6, 20, -8, 11, -13, -24, -4, -13, -18, 14, -12, -2, -11, 19, -8, 2, -10, -10, 17, -3, -14, 2, -22, 14}
, {-3, -2, 5, -10, -5, -11, 5, -3, 21, 0, 25, -3, 13, 2, -15, -7, -34, 0, -8, 0, 7, 4, -10, 11, 14, 12, 14, 11, 7, 9, 1, 11, 2, 0, -6, -15, 22, -10, -6, 12, -4, -6, 8, 8, 11, -12, 7, 24, -1, -4, -19, 0, 13, -12, -23, 9, -22, 3, -18, 3, 13, -6, -6, 12}
}
, {{2, -10, -4, -4, 2, -16, -14, 12, 8, -4, -12, -1, -18, 0, 16, 6, 3, -1, 15, -6, 2, -8, -2, 13, 5, -3, -6, -6, 3, 8, 2, -4, 2, 8, -9, 26, -3, 13, 4, 4, -21, 2, 8, 8, -18, 4, -12, 0, -5, 12, -4, -1, 13, -8, -3, -23, -2, 15, 12, -4, 14, 4, 26, -10}
, {-12, -11, -11, 3, -3, 4, 18, 23, 11, -3, -4, -10, -17, 25, 4, 1, 0, 17, 21, -2, 18, -3, -18, 28, 23, -17, -4, 9, 4, -24, 13, -8, -27, 10, -15, 19, -4, -13, 2, 2, -12, 2, 5, 20, -5, -7, -13, 3, 0, -10, -9, 29, -8, -9, -2, -18, 11, -5, -21, 10, -1, 6, 6, 0}
, {8, -8, 15, 12, 13, -29, 8, 17, 13, 6, -5, -7, -6, 9, 9, -35, -8, 12, 17, -33, 8, 20, -4, -17, -11, -6, 23, -2, 16, 0, 6, 10, 16, 3, -9, -17, 2, -14, 0, 24, 26, -14, 19, 7, -9, -21, -15, -10, -27, -17, 1, -16, -16, 8, -2, -2, -18, 17, 21, 14, -2, 0, 2, -3}
}
}
, {{{6, -10, 9, -3, -4, 14, -6, -10, -5, -16, -15, 6, 1, -4, 24, 10, -11, 15, 9, 18, 2, -2, -17, -5, -9, 12, 9, 13, 7, -1, -23, 6, 6, -4, -15, -10, 0, 6, -4, 14, 6, 16, 13, 2, -9, -3, 2, 1, 6, 8, 2, 4, 7, -18, 3, 11, -7, -9, -7, -3, -16, 10, 5, 11}
, {4, -2, -4, -6, -15, 3, 17, -2, -22, -8, -4, 4, 9, -9, 6, 8, 5, 14, -19, 9, 17, -9, -8, 8, 10, 9, -3, -16, 12, -4, -4, 12, -2, 7, 5, 7, -4, -16, 9, -9, 9, -15, 2, 5, 7, 7, -5, -15, 11, -1, 1, -9, -2, -14, 20, 0, -10, -25, -11, -10, -7, 3, 5, 5}
, {-2, -10, 14, 0, -4, 7, 13, -1, -1, -6, -13, 7, 8, -6, 1, -13, 16, 5, 7, 2, 10, 9, 17, 0, 0, -3, -7, 9, 4, 15, 16, -11, 4, 2, -7, 11, 3, 6, -8, -10, 6, 13, -18, 6, -5, 0, 11, -6, -14, 3, 0, -16, -4, 4, 9, 9, 8, 6, 0, -3, 6, -12, 0, 13}
}
, {{15, -11, 23, -7, 13, -12, 13, -15, -1, 17, -1, 3, 15, 17, 16, -16, -16, -14, 5, 19, -6, -9, 4, -8, -7, 29, 5, -35, -1, 5, -2, 10, 13, -27, -12, -12, 6, 3, -8, 25, -16, 9, -2, -4, 0, 9, -5, -14, 15, 8, 13, -5, 7, 7, -9, -5, -16, -12, -6, -3, 8, 9, -6, -7}
, {3, -18, -23, -20, 17, -16, 13, -34, -56, -3, -35, 22, 8, 3, -3, -12, -5, -33, 5, -41, -5, 10, 20, 0, 22, -13, -13, -6, 2, 5, -19, 1, 1, -6, 16, -37, 22, 0, 17, -39, -1, 2, -7, -7, -16, 26, 1, -4, -3, -6, 0, 18, -39, -11, -45, -5, -10, 9, -6, -3, -5, 32, 3, 6}
, {-9, -25, -5, -6, -11, 19, -6, -17, 5, 6, -6, -6, -10, 9, 1, -15, 14, -13, 13, 5, -7, -13, 4, -13, -15, 16, -2, 4, -1, -11, 27, 3, 9, -6, 6, 22, -21, 11, -10, 19, 22, -6, -8, 9, -1, 7, 4, 1, -10, 23, -8, -28, -21, 3, 8, -1, 2, 23, -7, -1, 8, -10, -5, 28}
}
, {{11, 11, 4, 10, -5, 3, 15, -12, 8, -3, 9, -8, -7, -4, 6, -2, -7, 3, -13, -6, -12, -19, -6, 0, -8, 3, 19, -18, -13, 1, 2, -3, 6, 7, 0, 5, 6, -6, 3, 13, 10, -8, 11, -7, -11, 14, -7, -7, 11, -8, 0, -2, 1, -1, 0, -12, -1, -2, 12, -7, 5, 18, -22, -12}
, {-25, 8, 1, -8, -31, 25, -10, 2, -1, -8, 13, 2, -21, -12, -2, 10, -20, -10, -24, -24, -9, 1, 1, -6, -17, -4, 0, 10, -18, 3, 4, -8, 14, -10, 3, -7, 5, 7, -2, -5, -1, 12, -19, -23, 4, -15, -5, -8, 23, -2, -5, -19, -16, 7, -2, 10, -11, -12, 11, 16, -13, -16, -3, 4}
, {-9, 14, -21, 3, 7, -10, 3, 7, -17, 3, -2, -27, -9, -19, -19, -5, 18, -24, 10, 11, 3, -13, -19, 1, 1, -3, 8, -4, -17, -3, -14, -11, 3, -4, 1, 12, 7, -7, -12, -6, 5, -1, -2, -19, -4, -4, -12, 0, 8, 8, -4, 18, -16, -5, 5, 9, -11, 4, 3, 4, -10, 0, -3, -3}
}
}
, {{{1, -4, 1, -11, -8, 4, 2, -7, -7, 2, -7, 6, -11, -3, 13, -14, 9, -3, -5, -25, 0, -6, 0, 1, -3, -5, 0, -14, 5, 17, -1, 2, 0, 8, -17, -10, 3, 8, 8, -1, -4, -15, 0, -14, -4, -23, -8, -9, 0, 4, -1, 3, -22, 1, 7, 14, -7, 0, -5, 0, -9, -13, 3, 18}
, {-7, 2, -6, -9, 10, 9, 6, -6, -5, 11, 9, 2, 1, 1, -5, -14, 4, 7, -6, -2, -19, -17, 20, -1, -13, 9, -2, -14, 9, 0, 1, -5, -17, -11, -5, 13, 7, 9, 6, 8, 5, 9, -4, -7, -2, 13, 7, -14, 4, -14, 3, -7, -11, -7, 4, -4, -1, 9, 16, -5, -7, 3, 10, -5}
, {11, 16, -7, -9, -8, -14, 1, 3, -12, -7, -4, 16, -10, -10, 2, 10, -2, -10, 10, 3, -6, -10, 14, -20, -4, 8, -12, -1, 14, 0, -1, 1, -3, 8, -2, -4, -3, 10, 14, -3, -2, -5, -1, -2, -15, 3, -7, 17, -5, 15, 0, 8, -10, -6, 7, 1, -4, 7, -5, -2, -7, 7, 10, -5}
}
, {{-6, -7, -8, -19, -13, 9, -3, -22, -8, -9, 2, 1, -8, -19, -22, -27, 3, -3, 0, -21, -6, -8, 3, 7, -11, 9, -3, -8, 9, -8, 27, 1, -49, -12, -7, -26, -3, -1, 3, -34, -5, -9, -15, -3, -16, -26, 2, 1, -3, -30, -12, -5, -5, 2, 16, -6, 20, -6, 4, -6, -13, 3, 7, -7}
, {-6, -5, 1, 22, 2, 24, -2, 33, 29, -15, 0, -2, -18, 14, -9, -28, 10, 21, -9, 7, -24, -37, -30, -8, -45, 11, 10, 8, -19, -5, 14, 22, -1, -7, 12, -10, -16, -6, 5, 9, 14, -10, -10, 2, 21, -13, 17, -11, -17, -12, -6, -44, -13, 0, -4, -11, -21, 27, 1, -9, 7, -28, -27, -1}
, {-4, -1, 19, -5, -4, -19, -2, 2, -7, 7, -8, -3, 7, -4, -4, -8, 8, 7, -10, -14, 7, -1, 1, 15, -3, -9, -12, 4, -4, -3, -12, -10, 1, 6, 7, 3, 6, -9, 24, -1, 1, 1, 6, -9, 5, 7, 3, -7, 3, -11, -9, 1, -3, 7, 10, 7, 3, -4, -2, -6, -9, -17, -4, 10}
}
, {{1, -1, -7, -8, -4, -8, 5, -9, -24, 5, -1, -12, 9, -8, -5, 5, 8, 14, 0, -14, 7, 19, -1, 9, 9, -5, -15, -12, -10, -18, -2, 8, -6, -21, 13, -11, 4, 0, 6, -7, 8, 18, -11, -1, 0, 2, -7, -7, 13, -10, 6, 4, -9, 12, 0, -9, -2, 13, -12, -23, -4, 9, -10, 16}
, {-5, -6, -20, 21, -11, -4, -4, 2, -2, 2, -21, -6, 8, 3, -8, -2, 0, -7, 11, -6, -23, 7, 10, -8, 9, 6, 15, 3, 3, 10, 9, 18, 10, 16, 8, -12, -5, 7, -1, 12, 1, -14, 2, -10, -19, -22, -2, -5, -27, 20, 15, -5, 3, -2, 4, 7, -25, 0, -15, -5, -8, 6, 19, -2}
, {7, -3, -2, -3, 6, -5, -18, 19, -18, 14, -15, 15, 0, -16, -11, 11, -12, -22, 1, 0, -7, -20, 24, 17, 18, 6, 8, -8, 7, 27, 7, 5, -21, -8, -4, -11, 9, 6, 6, -13, -8, -10, -15, -18, 1, -6, 4, 1, -8, -23, -9, 3, 4, -5, 0, -8, -19, -13, -3, -7, -11, 29, 3, 20}
}
}
, {{{-19, 9, 4, -3, 15, 13, -9, -7, 1, 22, 6, -6, -13, -9, -26, -4, 8, 5, 2, 11, -10, -21, -17, -4, -19, 6, -12, 22, -2, -17, -3, 7, 6, -9, 9, 6, -8, -26, 3, -2, 4, -8, -17, -14, 15, -13, -6, 5, 14, 7, -1, 2, -16, -12, 8, -8, -1, -21, -5, -4, -5, -11, -9, 16}
, {-7, -31, -5, 2, 22, -4, 8, 7, 22, 1, -9, 8, 2, 6, 8, 7, 9, -8, 9, -24, 0, 12, -18, -2, 3, -18, 18, 20, 1, 2, -13, -3, 15, -2, -2, 5, 11, 4, -8, 0, 19, -11, 4, -10, 7, -14, -20, 9, 20, 2, -11, 4, -2, 5, -4, -6, 10, -25, 2, 25, 11, -13, -6, 14}
, {0, -6, -3, 0, 21, 13, -6, -18, -8, 8, -2, -4, 13, 8, -9, 3, -2, -4, 23, -7, -7, -12, 7, 5, 13, -14, -13, -14, -5, 14, -16, 12, 5, -14, 18, 19, -6, -11, 12, 15, 13, -11, -4, -3, -26, 3, -4, -7, 3, -4, 5, 4, -2, 0, 5, -10, 8, 2, -11, -17, -8, 19, 0, 23}
}
, {{-7, -5, -19, 3, 17, -16, -11, 0, -30, 11, -7, 2, -21, 9, -12, -11, -11, 0, 8, -1, 19, -1, 8, 10, -13, 19, -5, 4, 29, 5, 0, 1, 7, -12, 5, -21, 10, -10, -7, -18, -5, -12, -11, 8, 7, -21, -5, 17, -9, -15, 14, 11, -25, 6, -3, -14, -14, -8, 14, 5, -15, -7, -22, 4}
, {-27, -23, -2, 8, 1, 1, 16, -1, 8, 9, 16, 0, -28, -7, 1, 27, 14, 7, -4, -13, -2, -3, 20, 1, -11, -2, -1, 3, -12, -14, 19, 1, 4, -12, 12, 21, -16, -13, -1, 12, 21, -5, 9, -30, -4, -1, -42, -25, 10, 5, -11, -8, 22, -19, 16, -6, 14, 1, -3, 15, -8, -24, -16, 0}
, {13, 31, 1, 2, 28, 0, 8, 6, -10, -4, 11, -16, -12, -12, -2, -6, 4, 11, -16, 16, -12, 3, -27, 4, 2, -13, -8, -17, -12, -12, -10, 1, 18, 3, 1, -8, -7, -8, -15, -26, 1, -14, 5, -9, 1, 2, -13, 7, -21, -3, -13, 12, -7, -17, -26, -4, -40, -16, -16, -24, -5, -14, 1, -6}
}
, {{-2, -1, -5, 9, -11, 5, -2, -17, -15, 3, 13, -5, 33, 7, 19, 1, 10, 5, 33, 3, -4, -10, -3, 12, -5, -2, 10, 3, 4, 14, 10, -11, -1, 1, 0, 10, 17, 7, -12, -10, -7, 3, 1, -10, 1, 3, 5, -21, -1, -2, 8, -24, -31, -2, -6, -5, -12, -18, -8, -13, -8, 1, -18, 5}
, {5, 17, -20, 12, -3, -9, -4, -29, -7, 21, -18, 5, 2, -10, -1, -6, -18, -18, -15, 15, 6, -19, -21, 14, 1, -9, -2, -3, -34, -11, 9, 2, -13, 16, 9, 8, -14, 15, -19, 2, -5, -15, -4, -12, 4, 12, 18, 35, -21, 5, -6, -31, -12, -18, -7, -6, -33, -1, -17, -25, -17, 21, 11, -34}
, {13, 6, 28, -2, 7, -13, 0, -6, 6, -3, 15, -3, 9, -33, -1, 16, -7, -10, -36, -1, -3, 17, -15, -19, 3, -10, -11, -8, 2, -5, -5, 28, -14, 11, -5, -22, -19, 2, -1, 15, 3, 14, 8, 11, 1, 18, -23, -15, -1, 8, -10, -23, -9, -11, -28, 0, 11, 26, 13, 22, 1, 12, 21, -3}
}
}
, {{{15, -6, -22, 8, 0, 6, -16, -5, -6, 6, -4, -8, -1, -6, 5, 12, -18, 2, 10, -2, -9, -21, 2, 11, -3, -11, 21, -10, 4, 9, 7, 11, -8, 17, -11, -14, 12, 9, 13, 18, -6, -9, 19, 20, -10, 9, -12, -9, -18, 11, 8, -10, -11, -1, 1, 13, 9, 16, 12, 19, 16, 14, 5, -1}
, {9, -20, 1, -1, 9, 1, -16, -8, 0, -23, -15, 1, 14, -13, -7, 5, -12, -6, -10, -2, -2, -7, 0, -14, -7, 1, 13, -1, -8, 24, -7, -4, -10, 0, 2, -24, -3, -20, 13, 13, -2, 16, -18, -11, -11, -12, -5, 3, 10, 10, 6, -12, -16, -21, -24, -5, -14, 20, -12, -4, -18, -9, -7, -2}
, {19, 2, 1, 10, 3, -13, -14, 4, -13, -15, -3, 2, 12, 3, -8, 5, -2, 12, 7, -22, -5, 3, -7, 8, -2, -6, -21, -9, 4, 9, 10, 3, 1, -21, -3, 7, 5, -7, 21, 10, -4, -7, -3, 8, -6, 22, -9, -17, 4, -23, 1, 11, -12, -1, 5, 5, 2, 16, -23, 10, 0, 13, 4, -18}
}
, {{-5, 3, 3, 13, 7, -4, -9, -14, -12, 5, -27, -5, -7, -15, -23, 0, -15, 10, -1, -8, -5, -9, -21, -8, 0, -5, 14, -24, -2, -8, 5, -5, -9, 26, 2, -1, -8, 20, 13, 4, -4, 12, -3, -33, 0, 10, 10, 25, 3, -10, 7, 1, 4, 2, 8, 10, 4, 0, -3, 9, -19, 25, -2, -32}
, {38, -14, -19, -59, -8, -40, -13, -19, -9, -22, -22, 8, 30, -44, -21, -8, -33, -33, -2, 28, -23, -32, -1, -26, 8, -5, -17, -5, -49, 14, -15, -42, 17, -18, -12, 10, -27, 33, -4, -14, 1, 10, -45, -16, -13, 29, 24, 8, 41, -45, 25, -9, -26, 9, 16, -9, 5, 13, -24, -34, -23, -6, 15, -24}
, {-9, 8, 6, -9, 14, 7, 5, -6, -19, 12, -9, -13, 1, 0, 15, -8, -23, -3, -15, -5, -1, -9, 5, 2, -1, 1, -4, -18, 3, -15, -12, 17, -14, -16, 8, 17, 4, 19, 5, -9, 5, -6, 2, -9, 4, 5, -4, -2, 4, 7, 10, 3, -18, -6, -6, 2, -13, -2, -4, -2, -4, -14, 20, 12}
}
, {{5, 8, -7, -12, 6, 11, -17, -10, 18, -8, 3, -10, 0, 2, -6, 5, -19, 12, -4, -15, 11, 25, -5, -13, 24, 0, 0, -12, -14, -1, 13, 2, -1, -9, 9, -1, -17, -1, -1, -10, 3, 22, -7, -16, 6, -17, -12, -10, 6, -14, 1, 9, -7, 2, 0, 9, 5, -6, 12, 6, -8, 8, 12, 3}
, {15, -49, 8, -4, -30, 14, 4, -19, -15, -16, 11, -20, -19, -2, -17, -1, 10, -5, -18, -3, 38, 7, -6, -2, -34, 1, -14, -4, 5, 4, -9, -20, -10, -18, -53, 9, 21, -27, -11, -57, 12, 5, -8, 4, -12, -10, -2, -18, 9, -7, 1, 22, -27, 16, 22, 19, 14, 0, -14, -7, -17, -24, -13, 43}
, {-7, -5, 13, -32, 5, 4, 8, -8, 16, -3, 4, -12, 16, 8, -9, -21, 17, 17, -2, -11, 0, -7, 4, -26, 6, -6, -16, -16, -11, 13, 10, 5, 10, -14, 3, -1, -4, 9, -2, -11, 12, 15, -2, 13, 3, 3, -26, -13, -2, -7, 25, 5, 2, 30, 1, -5, 28, -10, 8, 6, 14, -24, -18, -12}
}
}
, {{{-1, -13, -6, 13, -18, -1, -2, -9, 7, -2, -14, -5, 4, 3, 31, 6, -2, 3, -2, -4, 7, 2, 1, -2, -6, 3, 28, -7, -4, 10, -12, -6, -17, -17, 13, -9, 14, 11, 2, 15, -13, 19, -6, -3, -8, -1, 13, -16, 3, 6, -3, 13, 1, -14, -4, 4, -14, 8, -6, -2, -13, 7, 5, 7}
, {0, -2, 6, -2, -3, -11, 3, -11, -7, 11, -11, 8, -8, 4, 7, 15, -13, 10, 4, -6, 13, 9, 7, 11, 5, 7, 22, -8, -10, 4, -2, -8, -4, -20, 19, -12, 9, 2, -3, 1, -10, -11, -3, -9, -2, -3, 3, -14, -8, 6, 10, -2, 6, -2, 0, -4, 12, -6, 9, 6, -16, 13, 26, -15}
, {2, -5, -1, 16, -20, -10, -10, -2, -8, -4, -13, -3, -12, 7, -11, 7, -4, 6, 9, 11, -5, -2, 9, -2, -11, 4, 1, 7, -8, 1, -3, 7, -6, -7, -19, 6, -5, 20, 11, 4, -1, -15, -3, 13, -17, -8, -1, 1, -5, 3, 14, 9, -8, -12, -18, 5, -25, 0, 3, -7, -8, -10, 2, -1}
}
, {{-7, -27, -5, 1, 2, -2, -16, -30, 0, 6, 9, -10, 3, 2, -27, 2, 12, 2, -9, -14, 11, 2, 5, 2, -4, 0, 0, -10, -7, 0, 0, -7, -3, -17, -22, -5, 7, 2, 1, -34, 4, -15, -2, -34, -11, -28, 8, 5, -6, -13, 19, -2, -7, -13, 18, 10, 10, -1, -6, -9, -10, -17, -7, -10}
, {-19, 7, 8, 12, 6, 19, -3, 6, 8, 8, -3, 16, -13, -7, -6, -24, -8, 7, -18, 18, -28, -41, -9, 22, -28, 8, -8, -13, -26, 13, 40, 12, -18, -6, -24, 13, 1, -21, 9, -19, 0, 7, -9, -3, 10, -14, 1, 5, -8, -6, -32, -33, 3, 18, 4, -5, -3, 14, 15, -7, -3, -25, -23, 6}
, {-5, -16, -10, -19, -22, -6, -10, -14, -1, -1, -15, 19, -8, 5, 9, -4, 3, -9, -2, -8, -2, -31, 5, 20, -13, 8, 14, 7, 11, 9, 11, 3, -11, -2, -27, 12, -5, -5, 14, 2, 8, -4, -13, -9, 11, 11, -9, -16, -5, -9, 8, 9, -5, 1, 18, -13, 13, 2, -10, -3, -7, -9, -8, -3}
}
, {{-10, -3, 5, -17, -13, -7, -6, -3, 0, -15, -13, 4, 8, -7, -2, -3, -1, -9, 4, 12, 6, -5, 8, -2, -2, 3, 2, -8, -1, 2, -7, 1, 7, 17, -3, -1, -6, 1, -6, 18, -13, 6, 2, 21, -18, 5, -3, 0, -10, -9, 8, -10, 4, 9, 3, -4, 0, -2, -2, 22, -17, -4, 15, -6}
, {-2, -3, -13, 7, -22, -5, 1, -4, -4, -7, -12, 2, -16, -3, 17, 2, 1, -5, -12, -13, -7, -5, -23, -22, -5, -5, 21, -10, -3, -21, -16, -4, -5, -2, -15, 15, 34, 14, -6, -3, 7, -12, -1, -3, -18, -23, 15, -3, -22, -8, 8, 0, -8, -12, 8, 8, -18, -2, -12, 3, -6, 21, 20, -2}
, {-1, -16, -3, 5, -6, -2, 2, -8, -5, -8, -8, -9, -24, -6, 12, 15, 3, -1, 5, 2, 3, -18, -3, 10, 1, -2, 7, 1, -7, -14, -33, -1, -11, -8, -2, -12, -14, 10, -18, -7, -14, 3, -20, 22, -1, 1, -7, 3, 7, -2, -8, 0, -19, 2, -9, 9, 0, 13, -8, -17, -14, -4, 18, -5}
}
}
, {{{-8, 3, -23, -2, -2, -6, 3, 4, -7, -1, -7, -1, -13, 6, 3, 1, 7, -6, -6, 2, 13, -14, 1, -2, -5, -3, -23, 3, -4, -3, 13, 2, 10, -5, -22, 5, 16, 6, -8, 2, -6, -8, -16, -6, -8, -26, -4, 5, -4, -15, 17, 1, -19, -7, 18, -16, 14, -5, 13, 1, -12, 4, -5, 6}
, {8, -1, 2, 21, -11, 12, 0, -10, -7, 5, 6, -2, 16, 9, 10, -5, 19, 11, -18, 15, -16, -15, 10, 1, 6, -6, -14, -21, 12, -7, -10, -10, -20, -1, -6, 12, 0, 13, -3, 6, 15, 5, 4, 6, -13, 12, -9, 10, 3, 0, -6, 0, 6, -2, 11, -1, 9, -7, -3, 1, 15, 0, -7, -16}
, {7, -15, 7, -17, -17, 1, -5, -9, 5, -9, -12, 10, 2, -18, 7, -11, 7, -5, 5, -9, 4, -25, -4, -17, -9, -2, -5, -13, 10, 4, 13, 7, -1, -3, -7, 4, -2, -6, 8, -2, -2, 16, 5, -8, -19, 10, 14, 13, -3, 13, -15, -1, 3, -10, 13, 1, 5, 4, -4, 14, 12, 6, -15, 11}
}
, {{10, -4, 5, -2, -1, -6, 11, -10, 0, -3, 6, 4, 4, 9, -7, 2, 14, -3, -1, -6, -5, -7, -1, -10, -10, 10, -13, 17, -8, 16, -7, -11, 8, 11, -2, 11, 15, 4, -3, -14, 11, 10, 5, 16, 2, -2, -16, -4, 0, -27, -6, -9, -4, 4, 7, 17, 18, -3, -9, -8, -9, -9, 3, 9}
, {-3, -9, -2, 7, 16, 11, -32, -15, 8, -11, 7, -19, -10, 15, -7, -14, 18, 5, 2, -6, -9, -3, -10, -26, -22, 22, 5, 2, 15, 7, -6, 8, 7, -5, 34, -7, -6, -6, -6, 6, 28, -26, -1, -24, -12, -2, 12, -7, -7, -27, 20, 37, -4, 12, -1, -12, -5, 8, -7, -17, -5, -23, -10, 10}
, {-9, -1, -5, -9, -3, -11, 0, 1, -3, 6, -20, -5, -20, -17, -13, -8, 14, 0, 1, 5, -22, -1, 7, -8, -11, 1, -21, 1, -22, 12, 7, -26, 0, -4, -1, -21, -4, -5, -24, -4, -13, 4, -20, -17, -3, -9, 13, -10, 8, -20, -1, 3, -7, 13, -7, 23, 6, -8, 15, 11, 23, -5, -2, -17}
}
, {{19, 9, -3, -5, -4, 15, -1, 14, -9, 3, 16, 11, -7, -6, -8, 0, -7, -4, 17, -15, 5, 21, 19, 12, 11, 6, -10, -6, 16, -7, 6, 0, -8, 8, 1, 2, 2, 3, 11, -6, 3, -6, -14, 1, -6, -16, 0, 12, 18, -12, -6, 6, -2, 10, -19, 2, 7, 5, 14, -1, 4, 3, -11, 6}
, {15, -3, -14, -1, -11, -7, 9, 3, -3, -9, -19, 10, -4, -15, -8, -13, -7, 9, -12, 4, -27, 4, -10, -6, 23, 14, -5, -14, -3, 6, 0, 3, -17, 12, 6, 7, 4, 2, 1, 9, 14, -12, 0, -8, -3, 6, -17, -3, -3, 14, -8, -12, -21, -3, -11, -1, -10, 1, -1, -7, 2, 13, 3, 7}
, {2, 15, 12, 10, -4, -15, 3, -8, 4, 6, 2, 2, -2, -30, 8, 8, -21, -16, 8, -5, -5, -14, 3, 19, 5, -13, 6, -7, -11, 12, 4, -2, -4, 11, -1, -16, -8, 22, 2, 4, -15, 10, 14, 5, -12, -9, -7, 12, 7, -2, 3, -7, 15, 3, -12, 16, 5, 2, 2, -25, -1, 8, -3, 5}
}
}
, {{{-1, -6, 9, 8, -4, 7, -11, 1, -4, -16, -8, -1, -5, 14, 1, -15, 6, 9, 8, 2, 7, -4, 0, -1, -7, -10, -8, -2, -7, 12, 2, 2, -4, 1, 13, 17, -4, -5, 0, -6, 18, -17, -7, -14, -12, 7, -1, -9, -7, -15, 14, -6, 6, -2, -6, -8, 10, 9, 17, -8, 11, 0, -18, 18}
, {-2, 8, 12, 18, -7, 1, 0, 1, 5, 2, -4, 8, -1, -4, 2, -1, -2, 2, -21, 9, -4, -13, -3, 2, 2, 5, 12, -12, 20, 2, 6, 3, -13, 13, 1, -7, -10, 8, -5, 7, 4, 4, -7, 3, -13, 5, -7, 7, -12, 8, -3, -2, 4, 2, 6, 3, 8, 16, 17, 8, -3, 7, 12, -15}
, {11, -8, -3, 9, -8, -14, -3, 2, -2, 0, -14, -12, -7, -6, -1, -12, -9, 11, 0, 2, 15, 0, 6, -22, 3, -12, 1, 7, 3, -6, 0, 1, -5, 14, -13, -1, -7, 5, -15, 9, 13, -1, -2, 18, 7, 18, 5, 7, -14, -2, -3, -2, -9, 8, 3, 9, -12, -7, -1, 5, 4, 16, 7, -5}
}
, {{6, -3, -1, -4, -2, 5, 5, -11, 11, 1, 4, 5, 9, -12, 10, -1, -6, -14, 14, -3, -6, -3, 7, -8, -25, -21, -14, 2, -39, -6, 8, -14, 2, -18, -10, 13, 3, -2, 2, 6, -1, 0, -5, 3, 8, -7, 1, -25, 1, 8, -7, 13, 14, 1, -4, -5, -11, 5, -3, 1, 15, -12, 3, 19}
, {18, -6, -13, -5, -1, -8, 1, -13, -5, 9, 5, -28, 3, 15, -23, -14, 18, 0, -2, 4, 2, -5, -9, 2, -1, 14, 7, 9, 9, -13, -5, 2, -25, -4, 10, -15, 21, 14, 9, 13, 13, -10, -12, -12, 16, -10, 3, -4, -1, -14, -1, -1, 20, -8, 2, -21, 0, -8, 1, -2, -3, 4, 15, 8}
, {2, -15, -1, -39, 2, -19, -28, -9, -5, 8, -25, -2, 14, -2, -17, 4, 10, -17, 13, 17, -14, 0, -3, -2, -7, 19, -6, 14, -6, 10, -6, -20, 1, -21, -6, -13, 17, 12, -1, -13, -7, -15, 8, 3, 1, -17, 21, -7, 11, -21, -8, -6, 3, 0, 7, -9, 13, 1, -1, 2, 5, -5, 11, -25}
}
, {{8, 13, -6, 0, -2, -9, 4, 0, -7, 8, 9, 1, -10, -2, -5, 9, 3, 8, -14, -6, -19, 6, -15, 4, -4, 12, 3, 9, 0, 9, 11, -3, -15, 2, -17, -9, 7, 5, 4, -27, 23, 6, 4, 17, 9, 0, -9, -8, 2, -11, 16, 15, -1, 11, -9, -9, -7, -8, -10, -6, -11, -9, 1, 10}
, {-14, 13, -13, -9, -7, -18, 5, 10, 11, 4, -23, -1, -12, -11, -3, -4, -5, -13, 7, -3, -9, 3, -12, -14, -4, 0, 7, -21, 9, -2, -8, 10, 2, 22, -3, -11, -6, 2, -22, 12, -10, 4, 13, 15, -10, -1, 2, -6, 14, 4, -11, -25, -8, 0, -13, -4, 11, -1, -12, 8, 1, 13, -16, 8}
, {15, -1, -17, 13, -17, 14, -10, -7, -6, -4, -11, -4, 4, -4, 23, -5, 14, -9, 2, 2, 2, -29, 8, -11, 3, -14, 3, -30, -13, -14, 5, -6, 10, -13, -20, -11, -4, -16, 0, -21, -24, -11, -13, -6, -9, -3, -3, -14, 11, 22, 0, -16, -14, 0, -8, -18, -25, 14, -27, -5, -13, -2, -20, 14}
}
}
, {{{4, -14, 6, 3, -3, -7, -5, 4, 4, -1, -10, -5, 8, -11, 8, -12, -13, 0, -11, 1, -11, -4, -12, 0, -1, 9, 12, -7, 21, 5, 12, 9, 3, -10, 16, -11, 2, -20, 6, 2, -1, 2, -2, 7, 11, 15, 3, -7, 1, -27, -17, 5, -13, -5, -4, -6, -10, -3, -14, -16, 12, 9, -11, 0}
, {-19, 1, -11, 3, 6, -13, -4, 3, -18, 2, -8, 3, 11, -11, 3, 4, 14, -11, -17, -5, -6, 2, -10, -2, 20, -9, -11, -5, 6, -14, 8, -14, 9, -4, 9, -8, -2, -19, -1, -13, -2, 2, -1, 0, -8, -5, 9, -14, 1, -6, -9, 11, -5, 3, -13, -26, -11, -15, -6, -2, -10, -3, 0, -6}
, {-8, 1, -2, -4, 4, -7, 12, -2, -4, 10, -30, 9, -25, 15, 17, -15, 3, -5, -6, -7, 1, -9, 15, -6, -15, -11, 7, 5, 4, -3, -4, 18, -25, 7, -1, -10, -4, 20, -14, 8, -6, 4, -22, -2, -9, -20, 11, 5, -16, -7, -14, 11, -7, -1, -1, -7, -17, 10, -1, -10, 3, -11, -9, 3}
}
, {{-7, 4, -10, -7, -4, -11, -22, -10, -8, 14, 6, -11, 5, 6, 0, 14, 9, 22, -5, -18, 3, -5, 2, 6, -21, -14, -15, 17, 10, 5, -11, -1, -12, -1, -3, -8, -4, 9, 11, -18, -12, -16, -4, -4, -14, -16, -4, 6, -4, -8, -1, 18, -1, 17, 17, 15, 8, -1, -9, 0, 3, -14, 9, -9}
, {-7, -16, 17, 7, 10, 29, -37, 2, 10, -37, -1, 19, -24, -20, 8, -15, 2, 3, 0, 15, -36, -25, -13, -22, -27, 20, -14, -19, -5, 14, 29, 22, -12, 6, -2, -11, -8, -23, 14, -21, 29, -66, 12, -3, -26, -8, 0, -4, -15, -14, -17, -5, -27, 28, 1, 15, 15, 22, -18, -29, 27, -1, -25, 16}
, {-16, -19, -21, 3, -9, -5, -7, 9, 30, 1, 8, 6, 6, -13, -6, -6, 16, -19, 2, -7, -10, -13, -4, -1, 1, -4, 5, 19, -5, 4, 34, -18, 1, -11, -11, 17, 10, -25, -8, 6, 2, 5, -18, -14, 10, 13, -3, 0, 4, 11, 3, -13, -2, 5, -6, 0, 8, 6, 0, 8, -13, -11, -7, -14}
}
, {{4, 6, 5, 4, -15, -7, -17, 9, 15, -9, -8, 8, -12, -12, -17, -9, -2, -19, 19, -1, -5, -13, 8, -5, 12, -2, -11, -8, 9, -4, 1, 22, -3, 4, 7, -9, -1, -16, -8, 14, 0, -12, 19, -34, -9, 12, -19, 11, -3, -15, 5, 14, 6, 4, 20, -12, 6, -1, 0, 10, 2, -20, -5, 5}
, {-6, -16, 4, 0, -7, -13, 0, 1, -5, -21, -4, 6, 16, 0, 35, 3, -22, 2, 0, 13, 5, 5, -15, -25, -6, -8, 20, 14, -18, 6, -19, -14, -9, -5, -11, 13, 8, -3, 13, 2, 7, 4, 3, -24, -13, 18, 6, 3, -11, 0, -6, 4, -25, -6, 7, 4, 5, 1, -10, 6, -27, 7, 32, -18}
, {10, -7, -11, 5, -6, 0, 2, -12, -23, 9, -4, 4, -14, -12, 13, 13, -4, -6, 4, 4, 18, 4, -9, 1, -17, 8, 10, -10, -3, -27, -4, -1, -6, 8, -2, 16, 13, 21, 1, 17, -6, 8, 6, 11, -13, 1, -15, -9, 1, -1, 5, 26, 2, -22, 2, -10, -15, 17, -6, -18, 0, -6, -10, 5}
}
}
, {{{2, 11, -20, -9, 12, 5, 5, 6, -7, -15, 7, -2, 5, -18, 6, 0, -1, -5, -16, -24, -3, -6, -5, 10, 6, -6, -12, -5, -3, 7, 5, 5, -3, -9, 1, -1, 14, -3, -2, -9, -3, -14, -10, -11, 7, 10, -11, -21, 9, 0, -7, -2, -1, -5, -3, 10, -5, -12, 4, -8, -7, -17, 7, -3}
, {-10, 5, -12, -12, 13, -12, 4, 7, -24, 4, 2, 18, -16, -4, 8, -1, -1, 13, -10, -7, -14, -2, -3, 11, 15, 9, 2, -17, 3, -1, 2, -10, -18, 2, 1, 10, -9, -3, 10, -14, -6, 0, -8, -18, -6, 14, 0, 3, 12, 5, 6, -2, -6, -7, 3, -10, -12, 4, 1, -19, -13, -9, -15, 8}
, {5, 24, -3, -2, 15, -24, -4, 15, -6, 9, 7, -6, -24, -10, 6, -21, -20, -4, -1, -11, -21, 1, 11, 4, 14, 8, -3, -13, 9, 0, -13, 3, 4, -11, 2, 5, 3, -3, 2, -8, -1, -25, -20, -4, -2, -20, -25, -6, 4, -8, -1, 5, -8, 8, 21, -7, -18, -7, 0, 5, -25, -3, 9, -5}
}
, {{10, 3, -25, -13, 7, -3, -8, -12, 7, -2, -2, 10, -9, -13, -4, 1, 11, 9, 0, -23, 8, -27, 6, 14, 7, -7, 0, 0, -7, 2, 7, -7, -20, -11, 14, -9, 2, 19, 6, -23, 3, -11, -19, -10, 10, -24, 12, 6, -13, -5, 7, -14, -6, -2, 7, -6, 15, 5, 5, -27, -10, 13, 2, -13}
, {-7, 3, 16, 7, 16, 5, -4, 5, -6, -7, -8, -1, 2, 6, 7, -33, -1, 12, -5, 16, -2, -16, -9, -5, -1, 9, 8, -24, -15, -16, 16, 27, 7, 14, 2, -8, -27, 7, -1, 21, 0, -15, -2, -11, -6, -12, 7, 9, -37, -10, 8, -28, 6, 4, 5, 1, -27, 7, 23, 9, 0, -20, -11, 8}
, {4, 4, -4, -6, -10, -4, -8, -7, -2, -7, 4, 3, -3, -16, 13, -4, -12, 13, -7, -12, -5, 7, 18, -2, -9, 4, 0, -13, 9, 8, 0, 15, -7, -4, -12, 12, 4, 10, -4, 3, -1, 0, -18, 11, -13, 22, -4, 7, -1, -22, -2, 9, 2, 13, 13, 11, -3, -3, -5, -21, -10, 2, 8, -7}
}
, {{0, 0, 2, 0, -2, -12, -14, -18, -9, 0, 7, -8, -18, -3, 1, 2, -4, -15, 0, -15, -8, 18, -13, 10, 3, -3, 3, -6, -2, -9, 4, 4, -7, -9, -10, -11, -24, 1, 9, 15, -5, -9, -1, -8, -17, -13, -10, -12, 7, -3, 3, -13, 5, 11, 1, 2, -7, 8, 11, 5, 1, -8, 16, 0}
, {-8, -6, -10, 6, -18, 7, 10, 13, 1, -9, -8, 14, -11, -11, -5, 10, -11, -21, 1, 20, -16, -2, 5, -1, 7, 11, 13, -6, 14, -3, -1, -5, -9, -10, -6, -4, 8, -5, 16, 16, 1, 20, 15, -10, 14, -8, 9, 13, -14, 2, -5, 16, 9, -4, -13, 1, 1, -15, 6, 9, -2, 19, -17, -13}
, {-3, -8, -15, 7, 10, 18, -14, 4, -6, 16, -20, 9, -19, -4, 11, -5, -3, 4, -7, -19, 10, -2, -2, 10, -6, 17, 11, -2, -1, -3, 23, 3, -2, 5, 19, 12, 3, -8, 12, -9, 21, -17, -8, 12, -8, 6, -3, -20, 1, -2, -16, 2, -8, -7, -17, 0, -5, 15, 2, -22, -11, 6, 7, 19}
}
}
, {{{-11, -9, -4, -16, -15, -20, 6, 12, -4, -3, -4, -4, 17, -5, 8, 13, 23, -4, -14, 14, 11, -3, -18, -8, 8, -6, 20, -9, -21, 0, -6, 17, -1, 12, -8, -1, 2, -1, 8, 21, 10, 2, 1, 24, 8, -13, 12, -9, 6, -12, -5, -5, -11, -30, -9, -14, -8, -17, -12, 3, -12, -16, 13, -22}
, {-12, -4, -12, 12, -7, -8, 23, 9, 16, -17, -13, -16, 9, 0, -5, 17, -2, 1, -17, -2, 4, 22, 23, 13, -7, -18, 12, -12, 4, 21, -14, -2, 2, 15, -3, 15, 16, 15, 2, -5, 10, 10, 2, 2, -2, -6, 3, 12, 12, 1, 0, -16, 3, -23, -18, -7, -19, 2, 4, 4, -19, 5, 9, 4}
, {-22, 3, -5, -14, -19, 10, -9, 1, -1, 2, -15, -3, -3, -12, 10, 7, -25, 13, 5, -5, 9, 7, -5, -6, 5, -9, -3, -14, -7, -5, 20, 9, -2, 9, -7, 0, -3, -10, 0, 4, 12, 10, 14, 17, 8, 7, -5, -15, -17, -7, -7, 14, 10, -10, -3, -16, 9, -9, 11, 8, -28, -26, -6, 10}
}
, {{-2, 3, 4, -16, -8, 3, -2, -16, -16, -4, 0, 5, 11, 5, -26, -2, 5, 23, 0, 9, -2, 8, 0, 0, 10, 19, 19, -20, 26, 4, 5, 11, -11, -6, -10, 13, -6, -13, -10, 0, -5, 4, -1, 8, 3, 1, -4, -19, -11, -14, 4, -9, -3, -9, -3, 8, 0, -3, -3, 20, 2, -13, -2, -4}
, {-1, -6, 9, -30, -12, -13, 20, -19, 7, 12, 13, 25, -22, -23, 10, -16, -3, 14, 7, -20, 6, 21, 14, 9, 3, 4, 16, 13, 0, -2, 2, 6, -5, -19, -14, 14, 7, 13, -8, 9, 9, 18, 0, 2, 14, -9, 3, 22, -8, 9, 5, 4, 19, -22, -12, -3, 9, -19, 14, 13, -8, -10, -13, 5}
, {12, 6, 3, 4, 33, 18, -6, 13, -9, -14, -13, -25, -2, 8, -21, 3, 1, -3, 8, 8, 1, 17, -6, -7, 3, 3, -17, -1, -11, -7, -31, -7, 13, 20, 1, 10, -5, -10, 19, -2, -11, -10, 18, 19, -5, 9, -6, -12, -10, 17, -14, -5, -18, 21, 6, -3, -23, -19, -18, -24, 13, 2, 2, 3}
}
, {{-3, -30, -4, -16, 17, 15, -3, 11, 7, -30, -1, 0, -2, -10, 8, -13, -4, 6, -1, 0, 1, 6, -5, -16, 2, 3, 2, 3, 14, 4, -8, -5, 3, -5, 21, 12, 12, 0, 4, 3, -3, -18, -1, 21, -7, 13, 14, -13, -3, -5, -10, 4, -13, 14, 24, -1, 0, -3, -13, -22, -1, 9, -18, 0}
, {-20, 5, -4, -13, -3, 6, 15, 6, -4, 8, -18, 7, 4, 6, 8, -10, 6, -1, -6, -6, -1, 8, 3, 4, -5, -1, -15, 17, 19, -7, -12, 3, 16, -7, 11, -11, -3, 2, -7, -17, -16, 9, -9, -12, 16, 5, 7, -21, 14, 8, -1, 3, -20, 28, 6, 15, -35, -23, 5, -22, 1, -1, -5, 1}
, {0, 9, 14, 8, 22, -11, 5, -9, -9, -2, -14, -6, 8, -8, -3, -18, -18, -7, -4, 31, 3, 14, -9, 6, 9, -2, -19, -7, -12, 21, -13, 27, -21, 14, 5, -30, -16, -1, -4, -8, 7, 9, -5, 8, 9, 15, -3, -21, 21, 11, -2, 2, -2, 7, -8, 4, -7, -16, 8, 0, 11, -7, -14, -5}
}
}
, {{{14, 1, -2, 8, -3, 21, -2, -3, -15, -1, -8, 1, 6, 6, -14, -11, -4, 12, -16, 12, 6, 10, -5, -17, 6, -13, -13, 1, -16, 9, -8, -6, 7, 1, 8, -2, 13, -4, 3, 2, 1, -1, 15, -8, -16, 23, 7, 15, -5, 15, 10, 9, -3, -4, -1, -6, -5, 15, -3, -15, 7, -4, 9, -7}
, {-4, 4, 6, -5, 1, 0, -18, 6, -1, -3, -9, 6, -25, 0, -8, -11, -9, -24, -21, -5, 4, -10, 2, -15, 14, -14, -4, 6, -18, -18, -7, -22, 15, -4, 16, 16, 5, -22, -5, 8, 8, -16, 1, -6, -1, -21, 14, -2, 9, 7, -23, 9, 3, 20, 2, -18, 10, -13, -9, -3, -8, -16, -9, 8}
, {-5, 11, -13, 3, 13, 1, -7, -1, 1, 11, 18, 16, 11, 35, 2, 11, 16, -9, 5, -6, -5, -7, 1, 0, -7, 4, -10, 8, -18, 0, -5, 22, 2, 17, 12, 7, 3, -24, -1, -7, -27, -23, -21, -12, 4, -2, -10, -3, 21, -15, -12, 14, -6, 16, -22, -15, 16, -4, 9, -12, -5, -5, -8, -7}
}
, {{-25, 11, 1, -11, 3, 17, -27, 11, -6, -6, 27, -5, -15, -12, -28, 3, -5, -8, 4, 6, -14, 3, 1, 6, -11, -6, 6, 7, -5, 4, 9, -9, -9, 5, 6, 5, -12, -13, -12, -3, 20, -16, 1, -11, 3, -15, -12, 12, 16, -6, 5, -15, -18, 17, 6, -9, 25, -13, -4, 1, -8, -14, 18, -8}
, {1, 4, -2, 1, -8, 3, -5, -5, -7, -10, 14, -15, -12, 7, -4, -3, -3, -8, 2, -9, -2, -4, -32, -28, -10, -15, 3, -11, 1, 2, -17, 5, -11, 8, 6, 3, -28, -7, -6, -27, -5, -23, 4, 4, -3, -13, -16, -11, 5, -13, 5, 5, -9, 16, 7, 2, -2, 0, -14, -12, -15, 18, -5, 0}
, {11, 1, 31, 9, -14, -14, 27, 2, 4, -12, -6, 25, 7, -18, 11, -9, -9, 1, -17, -12, 16, -1, -4, 5, 5, -6, 9, -5, 10, 16, 21, -24, 8, 2, -1, 7, -2, 9, 8, -9, -3, 8, -18, -1, 13, 12, 16, -8, -5, -17, 2, 2, 32, -18, -16, -12, 9, 4, 17, 0, -4, 6, -19, -7}
}
, {{6, 3, -32, 6, -5, -2, -25, 8, -4, -5, -5, -1, 1, 11, 2, 0, -1, -16, 7, 15, -5, -26, -4, -20, -2, -2, -4, -9, -9, 14, 13, -6, 10, -10, -8, -8, -11, -9, -1, -1, 1, -5, -18, -8, -8, -15, 1, 28, 3, -5, -3, 2, 6, 3, -11, -7, -6, 11, -3, -4, -1, -21, -5, 10}
, {17, 0, 3, 1, 35, 22, -10, 10, -7, 7, 16, 22, 7, 8, 26, -1, 0, 4, 19, 6, 17, -12, -17, 8, -1, 17, -9, -5, -13, 8, 17, 4, 12, 4, 3, 3, 22, -8, 0, -13, 5, -14, 13, -6, 7, -1, -1, 1, -11, 6, -1, 14, 16, -6, -1, 12, 1, 6, -2, -8, 9, -6, 2, 12}
, {10, -21, -18, -20, 14, -7, -6, 3, -5, -1, -4, 23, -6, 7, 18, -21, -1, 16, 10, 6, -3, -11, -7, 8, 9, 22, -6, 5, 19, -10, 5, 1, -2, 7, -4, 2, 4, 4, 6, 7, 10, -2, 3, 12, 7, 19, 19, 5, 2, -5, 0, 13, -14, -19, 10, -15, -3, 0, 5, -1, 3, 22, -9, 10}
}
}
, {{{0, 1, -4, 14, 6, 3, 1, 9, -18, 19, -15, 11, 5, -5, 10, -4, 11, 1, -6, -3, 17, -8, -6, -1, -5, 0, 24, 8, 4, 1, 1, 7, -13, -11, -27, -12, -4, -15, -5, -1, -10, -5, 0, 1, -3, 6, -3, 0, 4, -6, -13, 15, -2, -10, 2, 12, 1, -3, 7, 3, 1, -5, 13, 2}
, {2, -1, -10, 0, -7, 5, 8, 5, -4, -4, -13, 11, -1, 0, -1, -15, 8, 7, 8, 0, 12, 1, -5, -10, -1, 4, 7, 7, -13, 8, -14, 14, -7, 5, -8, -3, -3, -18, 4, -3, 1, 7, 0, 1, 23, -3, 6, 19, 7, -7, -19, 25, -5, -4, 10, -1, -3, 9, 9, -5, 11, 4, -2, 13}
, {-3, -7, -8, 0, 6, 2, 3, 18, -9, 7, -10, 8, -9, -23, -1, -2, -2, -12, -7, 9, -18, -7, 4, -5, -11, -4, 2, 9, 3, -4, 3, 5, 16, -5, 17, -7, 29, -9, 2, -14, -5, -14, 10, -11, -13, 3, 3, 2, 13, 3, -1, 17, -12, 3, 1, 3, 9, 0, -15, -5, -9, 4, -4, -8}
}
, {{13, -4, -16, -6, 14, -7, 0, 13, -9, -19, -22, 14, -7, 2, 14, 10, 10, -7, 13, -1, 5, -13, -2, -9, 12, 3, 13, 2, 6, 5, -12, 13, -12, -15, 10, -7, -7, 11, -5, -5, -12, 2, -7, 6, 2, 17, 1, -28, -1, -6, 5, -2, -6, -12, -8, -3, -23, 12, -1, -14, -7, 9, -3, -4}
, {6, 4, -11, 8, 8, -15, 17, 19, -3, 4, -8, 3, -5, -2, -13, -33, -3, 6, 10, -11, 3, -1, -6, -5, 5, -12, 3, -1, 14, -6, 6, 22, 4, 4, 2, -2, 9, -11, -20, 0, -11, 9, -6, 12, 0, -17, 0, 15, -38, -19, 10, -20, -4, -7, -30, -10, -21, -12, -1, -14, -9, -7, -12, 6}
, {9, 13, -3, 10, -3, 10, 0, -7, -7, -7, -10, -4, -20, -13, -12, -6, -21, 19, -17, 2, -4, 1, -10, 18, -25, 17, 3, 21, 3, -18, 12, 25, -6, 7, 7, -7, -11, -17, 7, 6, 8, -9, -2, 19, -14, 16, -10, 1, -5, -14, -7, 22, -10, 11, -2, 31, -16, 17, 10, -1, 6, 4, 6, 25}
}
, {{-12, -11, 2, -11, 8, -21, -3, 13, -6, -7, -13, -1, 5, 4, 14, -8, -1, 9, 4, -4, -4, -6, 11, 1, -3, 8, 15, -8, 2, 3, 0, -5, 2, 0, 12, 20, -17, 6, -10, 18, -9, 9, 0, 13, 1, -3, -4, -10, -6, -5, 0, -3, -15, 9, -8, -6, 1, 4, -3, 15, -16, 4, 6, -5}
, {9, 0, -3, 10, 4, 6, 9, 22, -6, 6, -28, -6, 15, -7, -6, 14, 21, 4, 4, -6, 3, 2, -20, -3, 21, 11, 4, -18, 5, 1, 0, 9, -14, 7, 30, 5, -9, 2, 2, 17, 18, -9, -4, -1, -4, 9, 13, -12, 5, 12, 19, 5, 12, -16, 3, 5, -3, 8, -1, 5, 2, 9, 13, -14}
, {20, -5, 0, -14, -6, -2, -27, 14, -1, 2, -5, 1, -13, -19, -6, 7, -3, -11, 37, -9, -10, -4, 4, 0, 15, 13, 15, 2, 3, 0, 14, -20, -4, 9, -2, 9, 8, -3, 7, -14, 15, -12, -6, 5, -17, -3, 4, 0, -5, -13, -2, 14, -24, -7, -5, 1, 10, 2, -12, -10, -13, 19, 10, 2}
}
}
, {{{-13, 11, 3, 5, 3, 5, 7, -7, 0, 1, -6, -1, 10, -9, 8, 6, 4, 0, 5, -4, -4, -5, -13, -15, 2, 9, -6, 7, -7, -6, -8, -16, -9, -1, -15, 9, -13, -5, 16, 7, 8, -4, 4, -6, -18, -7, -6, -1, 8, 0, 6, -8, -3, -3, 13, 6, -3, 29, 4, -4, 5, -4, 13, -2}
, {14, 0, -22, 0, -1, 2, 4, -5, -7, -8, -17, -4, -12, -27, -15, 8, 2, -9, -15, 17, 6, 9, -15, -15, -9, -6, -2, -4, 9, -5, -2, 13, -8, 2, 2, 7, -12, -18, 5, -20, -4, -9, 4, -5, 0, 0, -11, -12, 12, -15, -2, 18, 11, -9, 5, 4, -4, -14, -4, -14, 6, 3, -1, -2}
, {-14, -6, 6, 3, 1, 2, 9, 0, 2, -3, -1, -7, 3, -12, 10, -13, 11, 7, 2, 1, 3, -6, -10, 14, 6, 3, 3, -4, 4, 3, 0, 2, 0, 7, -19, -6, -7, 2, -1, 10, 17, -6, 8, -12, -1, -10, 6, -2, 9, -8, 5, 11, -3, -9, -2, 5, -3, -10, 10, 11, -3, -8, 2, 8}
}
, {{9, -13, 2, 7, -9, 4, -1, -8, -2, -1, 6, -5, -10, -6, 8, -9, -10, 2, -11, -2, 8, 20, -19, -14, 5, 21, 9, -20, 10, 9, -5, 6, 2, -4, 0, 19, 12, 1, 12, -2, 9, 8, -8, -16, -8, 5, -7, -17, 0, 10, 6, 9, 8, 7, 3, -12, -17, 9, 0, -5, 1, 7, 0, 5}
, {7, 6, -16, -5, 5, -1, 0, 6, -17, 42, -11, -24, 9, 9, -22, 6, 12, -34, 2, -5, -10, 10, -22, 3, -13, -7, 30, 22, 13, 15, -29, -8, 2, -28, -20, -10, 22, 6, -23, 0, -14, 17, -16, -16, -3, 6, -5, -13, -8, 4, -4, 23, -15, -23, -37, -18, -14, -23, -14, -16, 10, 6, 18, 2}
, {14, -14, 4, -15, 6, -11, 13, 0, 11, 7, -16, 0, -6, -9, 13, -18, -16, -3, 10, -21, -2, 13, 5, -15, -1, 15, 15, -12, 6, 13, -9, -1, -9, 10, 4, -15, -12, 2, 4, 14, 8, 1, 4, -12, 4, 12, -1, -12, -15, -12, -10, 7, -6, -6, -9, 3, 13, 4, 3, 5, 2, 10, -10, 7}
}
, {{2, -18, 8, 15, -7, 4, 3, 14, -7, -2, 4, -1, -3, 10, 12, -14, 9, 9, -6, -20, -3, -17, -16, -7, -11, 7, 11, -18, 21, -5, 6, 10, 9, -8, 7, 30, -2, -7, -16, 12, 12, -19, -13, -20, -18, -15, 2, -6, -16, 9, -3, -16, -7, 12, -13, 5, -12, 19, 0, -8, -12, 12, -4, 16}
, {-2, 19, 11, 11, -12, -20, -8, 12, -4, -10, -1, 9, 1, -31, -18, 23, 12, 16, -22, -12, -8, -17, 13, 13, -3, -7, -11, -33, 7, 29, -4, 5, 2, -4, -1, -9, -17, 4, -22, 15, -11, -3, -8, -28, 1, -14, -21, -16, 8, 17, 25, -2, 9, 2, 4, 3, -37, -38, 6, 16, -17, -10, -1, -5}
, {11, 1, -12, 1, -13, -6, -28, -1, -11, 8, -12, -7, -13, -2, -16, -5, -2, 11, 11, -6, -12, -16, 21, 3, -17, 7, -15, 16, 7, -11, -3, 7, -9, -3, -19, -21, 7, 12, -3, 13, 8, -2, -1, -23, -32, -2, 1, 17, 6, 12, -22, 2, -7, -7, 4, -8, -17, -25, 0, 12, -19, 12, -2, 16}
}
}
, {{{1, 0, -3, 15, -15, 3, -17, 7, -5, 6, 1, 8, 3, 6, -7, 3, -3, 1, -6, 10, -13, 1, 16, -9, -7, 2, 24, -4, -13, 8, 12, 19, -8, 12, 8, -4, 5, -5, 13, 2, -1, 3, 17, -16, 4, -10, 14, 3, -6, 10, -6, 15, 6, -9, 2, 1, -6, 1, -8, 10, -8, -9, 6, 1}
, {17, 7, -8, -4, -8, 3, -1, -1, 8, 16, -8, 5, -14, -4, -9, -5, -7, -3, -18, 0, -9, 0, 4, 1, -6, 10, 3, 1, 4, 14, -6, 12, 14, -2, -6, 8, -15, -18, 1, -5, -9, -1, -6, 0, -6, 9, -3, -2, -8, 0, 0, -8, 6, 0, 1, 9, 8, -7, -4, -8, -1, 5, 6, -6}
, {9, -15, 1, -7, -3, 1, -5, 6, 6, -2, -12, -2, -14, -17, -3, -6, -4, -24, -9, -18, 3, -17, 5, 9, -13, -14, 12, -6, 11, -3, 5, 4, -5, -11, -4, 13, 0, 10, -11, 9, 3, -8, -6, -10, -14, -6, -6, 4, -12, 10, -7, -5, -12, 3, -29, -15, -12, -19, 0, -10, -17, -9, -8, 4}
}
, {{-4, 10, 10, -16, 6, 0, -21, 5, -5, -5, -4, 18, 4, -1, -5, 0, 15, 7, -13, -16, 3, 4, 10, -6, 15, -5, -8, -9, -11, 7, 3, -11, -9, -2, 8, -4, -2, 2, 18, -24, 10, -7, -15, -12, 8, -15, 2, -10, -6, -11, 18, 1, 2, -4, -1, -8, 12, 16, 4, 0, -9, 1, -7, 1}
, {-18, -8, -17, -4, 36, -12, 18, 14, 16, 23, -5, -14, -6, -22, -12, -9, 29, -7, 10, 16, -2, -5, 10, 16, -17, -16, -30, -1, -25, -16, 17, 15, -33, -3, -15, -6, 19, -12, -2, -10, -26, 2, -9, 0, -4, -13, -15, 14, -6, -9, 4, 4, -5, 14, 5, -2, 0, 0, 2, -10, -12, 3, -10, 12}
, {22, 9, -21, -17, 23, -14, -7, -6, -8, 14, -11, 4, 0, -16, -17, -7, 18, -30, 19, -6, -14, -9, 7, 8, -16, 30, -11, 10, -22, 14, 8, -2, -19, -27, -16, -5, 4, -9, 4, 3, -6, -12, -13, -15, -8, 12, -1, -10, 4, -29, 4, 3, 10, 4, 10, -27, -8, -17, -2, -8, 2, 3, 4, -14}
}
, {{-11, -20, -8, 1, -5, 7, -17, 12, 1, -4, 3, 13, -10, -11, 7, -3, 9, 1, -14, 10, -5, -1, 2, -22, -6, 2, 21, -2, 0, -1, -10, 9, -1, -1, 13, -12, -9, -1, -11, -7, 9, -12, -7, 7, -11, 17, -6, -7, -10, -19, 3, -3, -17, 14, 15, -3, -1, -10, -14, 4, 0, 0, -16, 3}
, {-12, 3, 17, -15, -5, -10, -2, -1, 14, 21, -1, -1, 10, -16, -21, -13, 1, 6, 15, -13, 31, -1, 20, 9, -2, 15, 4, -9, 10, -7, 0, 10, 0, -24, -3, 8, 8, -17, 11, -9, -4, 26, -7, -16, -9, 1, 4, -28, 15, -22, -10, 0, 10, -6, -3, 4, 17, 4, 4, 8, -18, -17, -12, -7}
, {-11, -4, -6, -23, 23, 1, 9, -6, -2, -14, -7, -7, 13, -3, 6, -5, 17, 6, 11, -23, -5, 12, -1, -5, -8, 22, 11, -20, -10, -7, 4, 16, -4, -14, -22, -9, -12, -19, -2, -10, -17, -2, -14, 0, -4, 21, -4, -29, 23, -16, -7, -8, -9, -7, 14, 9, 6, 12, 1, -9, -11, -4, -33, 2}
}
}
, {{{-15, 5, -7, -15, 2, 4, 20, 1, 18, -20, -7, -4, -2, 4, -4, -19, -13, -1, 26, -15, 0, -22, -10, 11, 1, 16, 19, 19, 2, 21, 5, 1, -19, -16, -10, -6, 7, 2, -18, -17, 17, 3, -4, 8, 22, -7, 3, -28, 6, -7, 8, -12, -7, 12, -17, 0, 6, 4, -2, 7, 6, 10, -2, -12}
, {-8, 13, 13, -15, -1, -17, -9, 3, 0, 0, -18, -7, -10, -9, -12, 6, -12, 10, -5, -4, 0, 18, -4, -6, 1, 12, 28, 9, 15, 10, -18, 8, -16, 9, -4, 11, 3, 1, 6, 15, 4, 12, -2, 1, -10, -6, -12, -1, -12, -3, 0, -12, -4, -12, 25, -6, -12, 6, 4, -4, 3, 3, 14, -3}
, {1, 1, -6, 9, 0, -6, -5, 0, 3, 5, -2, 9, 1, -5, -11, -12, -14, -1, 18, 1, -7, -1, -13, 3, 11, 13, -11, 2, -15, -4, 15, -3, -4, -13, 12, -14, 1, 14, 17, 4, 4, 13, 17, 1, 15, 9, 7, -9, -16, 4, 10, -22, 10, -3, -4, -13, -14, 0, -8, 10, -1, 10, 7, 7}
}
, {{16, -10, -11, -8, -1, -17, -2, -28, -14, -17, 0, 20, 0, -15, 8, 25, -8, -4, -18, 3, 16, 3, -25, -9, -19, 2, 4, 10, 4, 9, -2, 7, -7, 18, 3, 7, -7, 14, 8, -12, 5, 9, -7, 41, 10, 0, 6, 13, -7, -13, 6, 21, 4, -7, -7, 8, -11, 14, -22, -16, 0, -5, 9, -17}
, {6, -2, 35, -1, -12, -13, 22, -1, 3, -6, -1, 8, 21, -15, 10, -19, 1, 14, -17, 4, -35, -20, 8, 4, 4, -3, -16, -14, -17, 0, -22, 7, 20, -13, -22, -18, -2, 19, 0, 14, 5, 6, -28, 6, -4, 1, 11, -3, 4, -1, 0, -17, -5, 0, -2, 3, 12, -11, 11, 3, 21, 1, 14, -11}
, {-2, -27, -20, -12, 2, -8, 0, -33, -6, 24, -3, -8, 21, 3, 9, 10, -31, 6, 0, -20, -5, 4, -6, -13, -4, -10, 6, -10, 5, -10, -13, -1, -2, 18, 9, -9, 10, 19, 7, -9, 2, -4, -21, 3, 0, 26, -29, 6, 12, 6, 1, -22, 10, 4, -8, -7, 0, 4, -3, 12, 0, 35, 16, 4}
}
, {{-4, 1, 9, -3, -4, 0, -21, 13, -9, -2, -6, -2, -22, -7, -16, 4, -21, 16, -7, -18, -36, 15, 11, 1, -16, 10, -3, 22, -4, -3, 2, -14, 3, -8, -24, 4, -9, 3, -5, -8, 21, -10, 5, -13, 3, -33, 9, -7, -5, -11, 8, -3, -4, 3, 4, 17, 0, -2, -7, -1, 5, -22, 11, 20}
, {3, 8, 18, -25, -10, 21, 7, 0, -7, -16, 21, -37, 0, 7, -6, 26, 3, -13, 0, 27, -12, 10, 11, 0, 14, -5, 1, -9, 3, -22, -12, 7, 3, -17, -24, -7, -17, -23, 5, -21, -2, 5, 2, -7, 7, 5, -11, 0, 4, -23, -34, -2, 5, 12, 15, -35, 17, 6, 11, -30, 28, -17, -13, 8}
, {-12, -17, -15, 5, -18, 20, -17, -7, 15, 0, -3, 7, 13, 21, 11, -29, 12, -4, 0, -9, -17, 13, 2, -8, 7, 0, -17, 13, -2, -2, 8, -6, 19, -6, 2, -10, -4, -2, 6, 28, 30, -22, 6, -31, -15, -16, 10, 9, -6, 11, -9, 0, 5, 36, -1, -18, -3, 2, -15, 0, -14, -16, -16, 10}
}
}
, {{{-2, 1, -9, -6, -4, -3, -8, 10, 6, -14, -14, -2, 3, -5, -7, 1, 5, -17, -3, -5, -4, 1, -10, -18, 15, -6, -8, -8, 21, 7, 5, 0, 17, 0, 11, -8, 4, -13, -1, 12, -13, -12, 3, -9, 7, -16, -3, 3, -4, 3, -2, -5, -9, -4, -12, 0, 10, -5, 6, 1, -4, 2, -5, -10}
, {-2, -7, 3, -11, -3, -10, -16, 2, -2, 3, -8, -2, -29, 9, -31, -2, 27, 21, 3, 4, -4, -6, -15, -4, -7, 9, 7, -6, -10, 3, 2, 0, -9, -4, -10, 28, -2, -14, 4, 1, -6, 8, -9, -20, 0, -1, -10, -14, -21, -14, -21, -1, -4, 9, 13, 16, 22, -1, 13, 3, 8, -7, -25, 8}
, {4, 7, -5, 8, -11, -7, -24, 2, -15, 19, -14, -7, -4, 5, 8, -4, 5, -7, -4, 3, 6, -26, -2, 7, -7, -8, 12, -18, 2, 11, -4, 14, 9, 11, 7, -10, 1, -7, 1, 4, 17, -22, 3, -4, -8, 5, -6, -7, -4, -14, -16, -14, -20, 8, 15, -14, 7, -15, -12, 20, -17, -7, -6, 4}
}
, {{-2, 4, -12, -12, 7, 19, 12, 3, 6, -17, -1, 28, -9, -13, 15, 2, -5, 6, -19, -4, -5, -3, 0, -12, -12, -3, -6, 6, -11, 11, 1, 3, 7, 2, -3, 0, 10, 1, 3, -16, -18, -8, -32, 3, -11, 5, -20, 2, -8, -5, -1, -11, -16, -14, -7, -3, 7, -2, 6, -22, 12, -8, 12, 13}
, {17, 5, -17, 3, 7, 19, 2, -25, -21, -22, -23, -39, -3, 3, 10, -8, 9, -2, 1, 12, 15, 3, 31, -9, 8, 8, -18, -11, -2, -5, 4, 28, -19, 24, -3, -15, 13, 1, -12, -23, -11, -3, -12, 17, -32, 18, 18, -14, -16, 23, 6, 5, -22, 6, -12, 14, -8, -2, -19, -25, 6, 0, 19, 18}
, {-2, 22, 16, -4, 2, 9, 1, 5, 14, 10, -33, 6, 0, 24, 7, -7, -3, 26, 3, 3, -2, -13, -16, 23, 0, 15, 0, -9, 9, 0, 6, -2, -5, 5, 11, -7, 11, 11, 10, 2, 2, -4, -21, 1, -5, -12, 0, -3, -5, -4, -20, 13, 23, 9, 15, -1, 2, 1, 18, 10, -6, -14, -7, -5}
}
, {{-3, -3, 1, -3, 5, -6, 10, -17, -5, -10, 2, -16, -1, 5, -10, 8, -10, -13, 14, 9, 19, -12, 0, -17, -21, -7, 11, 8, -21, 17, -20, -17, 8, -10, -9, -6, -27, -16, -15, -23, -12, 5, -12, -8, 3, -13, 0, 11, 0, -8, -5, 21, 8, -9, 17, -29, 14, -15, 22, -22, -4, 14, 7, 2}
, {-19, 1, 17, 0, 3, 3, 8, 36, 19, 6, 8, 3, 15, 0, 6, 4, -2, 4, -1, -39, -6, -12, 2, 5, 6, -2, 1, 5, -15, -10, -3, 3, 8, -14, 6, 37, 2, -38, 18, 6, 6, 14, -7, -15, -6, -28, -22, -28, -2, -3, -16, 3, -1, -1, -11, 1, 15, 2, -14, 15, -24, -4, -6, -2}
, {-4, -7, -5, -2, 5, 15, 0, -4, -4, 19, -4, 10, -38, 18, -2, 1, 0, -3, 30, -3, 4, 7, 11, 25, -28, 8, 15, -17, 5, -7, 18, 19, 5, -8, -4, -8, -13, 0, 5, 17, 6, -8, -16, -24, 1, -11, -10, -9, -4, 6, 0, 9, -19, 0, 3, -16, 8, 20, 11, -2, -5, 11, 2, 30}
}
}
, {{{3, -7, 5, -9, -16, -4, 0, -15, -9, -15, 1, -8, 12, 7, 27, -13, 0, 0, -2, 2, 12, -14, -6, -14, -20, 6, 2, -15, 1, 10, 5, 4, -22, -3, -4, -14, 16, 3, 7, 0, -8, 0, 4, -2, -14, 13, -5, 20, -3, -7, 7, 7, 6, -16, -6, -11, -9, -9, -3, 3, -12, 0, -4, -7}
, {-6, -11, -12, 6, -20, 1, 12, -13, 1, -1, -7, 2, -7, 11, 12, 7, 14, 7, -1, 8, -3, 8, 13, -11, -24, -8, -4, 23, -1, 0, -10, -15, 0, 0, -17, 15, 22, 4, 14, -6, 8, -4, 8, 1, 7, 1, -2, 15, -2, 4, -2, 5, 18, -16, -3, 15, 9, 1, -9, 7, 2, -2, -2, 12}
, {-1, 9, 9, -4, -8, 19, 11, -7, 9, 7, -5, 0, 3, -19, 11, 3, 15, 13, 0, 4, 4, 4, -5, 26, -8, 8, 3, 1, 10, 18, 12, -11, -2, -6, -7, -5, 12, 1, -7, 6, -3, -7, -1, 15, 8, 20, -8, 7, -12, 20, -3, -18, 4, 9, 17, 6, -24, -3, 25, -1, 22, -5, 5, 11}
}
, {{-14, -8, 0, 4, -14, -10, -4, 4, 10, -13, 6, 13, -14, 1, -8, 10, 11, 10, 6, 15, -10, 26, 14, -12, -6, 5, 1, 2, -4, 6, 19, 0, 0, 17, -10, 10, 11, 2, 5, -5, 20, -39, 0, -16, -22, -3, 9, -14, 0, -28, 6, 21, 14, 12, 18, 10, -14, 1, -6, 13, -7, -22, -14, -17}
, {-4, -6, 6, 9, 11, 0, -11, 17, 35, 21, 0, -8, -46, -43, -24, 0, -13, 0, 2, -15, -17, -26, -23, 22, -21, -6, -1, -1, -21, -7, 3, -11, -9, -9, -23, 8, 7, -1, -24, 14, 13, -5, -7, 9, -5, -16, -22, 23, -9, -8, 2, -21, 0, 7, 9, -2, -15, -17, 12, 15, -7, -20, -14, 17}
, {22, 11, -13, 13, 6, -12, 5, -16, 5, 6, -13, -11, -9, 4, -8, -1, 0, -14, -19, -9, 9, -5, -18, -1, -4, 21, -10, -12, -4, 0, 4, 11, -4, -4, -11, 3, -3, -17, -1, 2, -10, 7, -9, 11, 10, 8, -7, -21, -25, -6, -8, 18, 8, 0, 11, -4, -16, 17, -9, 4, 3, 3, -4, 13}
}
, {{0, 1, -8, -10, -3, -12, 13, -6, 6, 6, -5, 5, 9, -21, -5, 2, 25, 13, 4, 3, 9, -9, 5, -17, 1, -6, -1, -1, 3, 1, -13, -22, -7, 4, -11, -9, -6, 14, -1, 2, -9, 8, -14, 17, -12, -24, 0, 5, 7, 6, -7, 0, 3, -2, 6, 10, 13, 17, 3, 16, -11, 1, 2, -18}
, {-24, 3, 2, -19, 9, -6, 25, -12, -9, 28, -10, 0, -9, -2, -13, -7, 16, 12, 15, -34, -6, -2, 15, -1, -27, 14, 11, -5, 10, 9, -11, 3, -22, 2, -11, -6, -20, 21, -2, 12, 2, 26, 5, -2, -3, -43, -21, -24, -2, -1, -12, 9, 3, 13, 4, 7, 18, 8, 17, 8, -3, 20, 13, -7}
, {6, -4, -9, -9, 4, -19, 27, 10, 18, -19, 1, -6, -9, 3, -9, 14, 4, -2, 8, -16, -2, -3, 0, 2, 2, 10, 13, -19, 11, 2, -17, 18, 0, -4, -9, -14, 21, -4, -1, -10, -5, 1, 2, -6, 12, -39, -5, -4, 29, -6, -5, -2, 14, -32, 17, 4, 6, 21, 9, 12, 9, -10, 5, -29}
}
}
, {{{1, -10, -18, 9, -8, -6, 12, -23, -6, -1, -4, -3, 2, 2, 9, -1, -2, 3, 8, -18, -18, -12, -10, -3, -18, -6, -16, -7, -4, -7, -14, -10, -20, 1, -7, 3, 21, -20, -5, -2, 1, -4, 11, -2, -1, -3, -10, 25, 16, -3, -17, 2, -8, -15, 31, -7, -18, -9, -9, -18, 4, -2, 4, -8}
, {-4, -3, -1, 20, -2, 15, -5, 5, 15, 3, -20, 5, -15, -5, 12, 2, -11, 8, -12, 10, -9, -7, -7, 3, -11, 12, 7, 11, -4, 17, 3, 4, 6, -1, 0, 16, 5, -1, -12, 9, 19, -17, 5, -7, -5, -8, -15, 8, 9, 4, 15, -7, -1, -16, 10, 2, -9, -8, -4, 3, -8, -13, 7, 7}
, {-2, -7, -9, 4, 0, -13, -9, -14, -4, 16, -2, 10, -3, -20, 3, 11, 6, 18, 5, 1, -3, -8, -3, 15, 11, 13, 8, -8, -11, 25, 5, -16, -17, -2, -4, 6, -7, -4, 30, 6, 4, -6, 4, -23, -7, -18, -4, -5, 10, -3, 4, 5, -10, -4, 19, -4, 15, 1, 0, -5, 21, 2, 10, 3}
}
, {{-2, -34, -14, 17, -13, 7, -17, 13, -2, 3, -12, -2, 7, -11, -17, -4, 8, 21, -12, -16, -20, 0, -8, -11, -8, -11, 3, -8, 11, 18, -1, -6, 12, -3, -3, 9, 12, 0, -4, 0, 8, -15, 6, -11, 10, 7, 10, 21, -7, -11, 1, -21, 14, -5, 12, 9, 0, 7, 5, 5, 1, -7, -12, -18}
, {-11, -12, 13, 4, -3, -7, 6, 7, 0, 22, 1, 31, -5, -9, -17, -10, -6, 15, 0, -15, -9, -18, 25, -1, -16, 4, -6, -17, -30, -7, 21, 0, 17, 2, -22, 7, -31, 11, -18, 16, 12, 15, -28, -21, -3, -17, -12, 31, -6, 22, -5, -33, 29, 4, 18, 7, 18, 2, 39, 19, -3, -9, -8, -12}
, {-7, 3, 5, 2, 16, -11, 11, -5, 4, 5, -31, 13, -9, -7, 0, 2, 5, 6, -28, -8, -18, -9, -8, -23, 1, 18, -3, -11, -5, -3, 11, -3, -4, 1, -16, -9, -19, 15, 2, 18, 13, -20, -15, -8, -7, -16, -3, 15, -7, 6, -4, -23, 2, 7, 9, 5, -3, 1, 9, -16, -6, -7, 8, 13}
}
, {{1, -25, 2, 5, 4, 12, -2, -3, 2, -12, 3, -9, -6, 22, 15, 25, -14, -5, 1, -16, 5, -19, 4, 7, -2, 19, -6, 0, 4, 2, 4, -6, -12, -6, -3, -4, -1, 12, 5, 18, -1, 15, -7, -9, -9, -8, 25, -16, 2, 7, 19, -15, -1, -19, 7, 7, -11, -19, 12, 1, 13, -1, 9, 6}
, {-7, 1, -4, -6, 5, 7, 6, -3, 0, -6, 1, -8, -41, 1, 5, 25, 19, -9, -3, 10, -4, -23, -6, 12, -30, -16, -7, 6, 7, -16, 1, 6, 0, 7, 9, 22, -17, -18, 16, -5, -21, 8, 0, 0, 11, -13, 0, 34, 15, -30, -2, 5, -22, -19, -34, -10, -3, -2, 11, -29, 1, 14, -11, -6}
, {-10, -12, 12, -8, -6, -17, 2, -5, 28, 17, -2, 5, -10, -10, 10, 2, 2, -28, 5, -15, 14, -18, 9, 12, 19, -17, 2, 9, -6, 3, 8, 2, -12, -1, 3, -6, 14, 8, -3, 17, 13, 1, 5, -18, -5, -15, 5, 0, 8, 6, 18, 5, -2, 4, 13, -4, 15, 11, -6, -3, -16, -11, 16, -12}
}
}
, {{{-1, 16, 10, 9, -2, -5, -1, 17, 5, -1, -5, 6, -2, -5, 2, 2, -2, 5, 12, 11, -2, -10, 3, -11, -15, -4, -5, 6, 6, -8, -5, 9, 6, 0, 9, 4, -9, -7, 4, 16, 16, -17, -4, -12, 13, -7, -5, 32, -4, 3, 11, 1, -13, -9, 19, -9, -3, 7, -19, 7, -6, -12, 2, -7}
, {6, -29, -3, 12, -17, -3, 3, -8, -6, 3, 1, 0, -8, -17, -12, -1, 5, -10, 24, -11, 2, 6, -1, -10, -6, 4, 10, -14, -5, 6, -11, -6, -7, -3, 4, 4, -3, -1, -3, -15, 16, 2, -13, -1, -3, -2, 15, -15, -9, -13, -6, 3, -3, 15, 0, 2, 5, 6, 7, 3, 11, 1, 1, 6}
, {-17, -11, 0, 0, -16, 12, 13, -10, 5, 13, 1, 7, 1, -26, 0, -16, 16, -12, 2, -7, 8, -4, 8, -11, -27, -12, -1, 15, -14, 16, -2, -9, -15, -9, -5, 9, -8, 20, -5, 0, 8, -9, -6, -30, -23, 13, -1, 9, -18, -5, 0, 0, -8, 3, 9, 7, 30, -2, 3, -14, 1, -4, -7, 7}
}
, {{-7, 8, -3, 8, -12, -4, 21, 15, 7, 3, -2, -1, -16, -3, -7, 5, 2, -10, 11, 1, -23, -14, 9, 5, -10, -17, 2, -6, -20, 9, -2, -17, 10, -10, -19, 9, -9, 1, 13, 5, -12, -1, -11, -10, -10, -5, -2, -12, 27, -3, 2, -15, 4, 2, 13, 2, -14, -10, -4, 9, 0, 0, 15, 4}
, {14, 6, -41, 5, 3, -28, 0, 9, -15, 10, -12, -32, -18, -14, 11, -1, -3, -33, 9, -19, 11, 5, 1, 1, 30, -4, 2, -13, 5, -25, -21, -15, 7, 19, -3, 3, 20, -2, -16, 12, -8, 29, 4, -6, -14, 17, 2, 4, -11, 14, 5, 26, 21, -23, -8, -7, -14, -7, -29, -4, -8, 17, -1, 0}
, {7, -16, 4, 12, -14, -7, 5, -13, 18, 4, -10, -5, 8, -15, 11, 4, 8, -8, 14, -17, -13, -4, -10, -15, -15, 14, 2, 7, 4, -4, -24, -13, -9, -22, -21, -20, -4, 2, 6, -18, 19, -2, -16, -3, -4, -5, -11, 4, 2, -18, -8, 3, -1, 2, 3, 2, -5, 4, -7, -2, -1, 5, 8, 8}
}
, {{-12, -9, -4, -8, -6, 20, -1, 0, -3, 28, -4, 4, 0, 1, 11, -20, 24, 6, 9, -13, -10, -10, -5, 0, -11, 2, 0, -14, -11, 9, -5, -35, -4, -11, -7, 4, 23, 14, 5, 6, 0, -18, -10, -13, -2, -11, -9, -21, -11, 1, -2, -13, -3, -8, 13, 14, 6, -1, 3, 18, -12, 2, -9, 7}
, {22, 31, 16, -22, -7, -8, -12, 8, 2, 15, -11, -1, 0, -11, -30, -17, 2, 10, 14, -2, -8, -1, 1, 21, -7, 10, -13, -16, 1, 0, 18, 16, 7, 0, -14, 18, -3, -6, 0, 18, 18, -10, -4, -9, -10, -3, 0, -11, 25, -28, -36, -7, 0, -2, 6, -7, -5, 17, -4, 23, -8, -17, -8, 13}
, {3, -12, -7, -14, -5, -11, 7, 2, 5, 0, -13, -5, -8, -10, -3, 3, -16, -5, -11, -6, -7, -7, -22, -7, 0, 6, -5, 0, 8, 2, -22, -3, 0, -16, -10, 0, -11, -18, -2, 16, -14, -7, -9, -5, -5, 3, 26, 4, -4, -11, -1, 3, -10, 1, -2, -14, 11, 7, 4, 23, 0, 7, 4, -13}
}
}
, {{{-6, -17, -9, 12, 0, -5, -13, -9, 19, 11, 8, 10, -5, -43, -1, 7, 12, -4, -1, 0, 8, -9, 10, -9, 2, -2, -6, -2, -7, 28, 3, 8, -12, 19, -20, 14, 2, 0, -22, -10, -5, 5, -4, -1, -6, -7, 7, 17, -15, -9, 13, 3, -8, 18, 9, 3, 1, 2, 10, -2, 1, -13, -5, 10}
, {5, -21, -3, 9, 5, -8, 15, 4, 1, 3, 0, 3, 1, -15, -2, -19, -4, -13, 5, -13, 15, -25, 5, 2, -6, -5, -6, -6, -8, -6, -2, 4, 15, -13, 6, -3, 14, 2, 1, 1, 12, 10, -14, 7, 2, -8, 6, -7, -1, 6, -3, -2, 23, 9, 10, -7, 8, -9, -12, 8, 1, 15, -13, -6}
, {10, -10, 0, 3, 6, 2, -12, -4, 10, -4, -5, -7, -2, -16, 17, 14, 7, 1, 11, -8, -4, 6, -2, -15, 7, 7, 3, 2, -2, 5, 5, 3, 0, -1, 3, 3, 3, 3, -9, -2, 10, 17, 8, 1, -14, 14, 13, -18, 2, 16, -13, -10, -19, 2, 0, 2, 11, 10, -6, 13, 8, -4, -3, 8}
}
, {{13, -7, 1, 34, -5, -22, -24, 9, 9, -1, -3, 21, -10, -4, 13, 16, -4, -11, 15, -16, 7, 9, -18, -8, -3, -12, 0, 17, 9, -2, -6, 5, -2, 19, -2, 15, -15, -1, 7, 18, -4, -15, 14, 6, 3, -13, 8, 13, 8, 23, -14, 4, -3, -11, -4, 26, -6, 12, 3, 9, 4, -14, 18, 20}
, {7, 12, -5, 8, 18, -5, 13, 2, 11, -1, 24, -5, -10, -16, -16, 14, 2, 1, -10, -20, 9, -27, -28, 26, 10, 26, 1, -16, -1, -21, 9, -17, 14, -11, -12, 5, -8, 11, -24, 17, -9, 22, -15, -17, 9, 2, 16, 27, -17, 26, -7, -15, 6, -22, -10, -1, 1, 5, -15, -14, 8, 12, -9, -10}
, {-7, -4, -8, 33, 0, -2, 7, -4, 27, -5, 9, 2, -13, -2, -10, -3, -6, 10, -17, -5, 1, -16, -8, 0, -12, 6, 1, 5, -23, -1, 12, 17, -10, 28, -9, -2, 1, -12, 12, 9, 3, -2, -5, 9, -2, -7, -9, -6, -9, 15, 2, -23, -2, 8, -9, 11, 0, -2, 25, 6, 25, -9, -4, 3}
}
, {{-5, -4, 3, 2, 5, -14, -1, 12, 11, -3, -17, -3, 13, -8, 7, 1, 7, 2, 6, -10, 20, 3, -7, 5, 15, 5, 14, -18, 1, 11, 12, -14, -23, 20, 23, 19, 25, 10, -9, 18, -12, -2, 29, 2, -17, -7, -11, -7, 7, -1, -1, 14, -4, -14, -3, -9, -12, -13, 5, 8, -21, 17, 10, 7}
, {18, -6, 5, -6, -17, 0, -2, -10, -6, 8, -29, 5, -4, -18, -11, 14, 18, 11, -4, 4, 5, 5, -9, 6, -9, -9, 0, -6, -5, -20, 1, 6, -3, -5, -7, 16, -2, 11, -7, 1, 15, -2, 0, -7, -5, -3, 23, 11, 11, -15, -4, -11, 4, -25, -3, -8, -8, -16, 9, -11, -2, 0, 6, -1}
, {0, 5, -4, 17, 3, 5, 9, 4, -13, 7, 1, 18, 5, 1, -14, 2, 14, 15, 3, 1, -10, 15, -4, -11, 3, 9, 19, 15, 4, 16, -15, -5, -34, 9, -4, -6, 9, -12, 8, 2, 1, -12, -1, -7, 6, -2, 11, -2, -5, -8, -4, 3, 8, -6, 19, 1, -3, 8, -7, -13, 7, -1, 10, -13}
}
}
, {{{18, -1, -4, -4, 6, 3, 0, 3, -10, 1, -4, -2, 4, -1, 17, -13, -10, 4, 0, 0, -7, -1, 7, -5, -16, 20, -15, -4, 23, -14, -10, 8, -19, 9, -3, -16, 12, 2, -2, -29, -8, 0, -8, 19, 12, 7, -7, 3, -13, -24, 7, 12, 10, -21, 16, -7, -11, 6, 4, 2, -17, -5, 15, -5}
, {-5, -7, 6, -15, 16, -17, -12, -4, 11, -7, -11, 6, -10, 12, -10, -10, 16, 10, 15, -5, 9, 10, 8, -18, -19, 1, 11, 15, 8, -7, 4, 12, -1, 5, 16, 30, 3, 4, -7, -10, 12, -14, 6, 9, -2, -19, -10, -2, -9, 0, 3, -4, -15, 2, -16, 12, 6, 6, 6, -25, -8, 2, -11, -1}
, {-9, -7, -7, 9, -13, -6, 2, -5, 11, 15, -7, -5, -7, -13, 14, 1, -2, -4, 18, 12, 2, -6, -5, 13, 10, 5, 6, -2, 0, 24, 8, -7, -15, -12, 9, 5, -3, -6, 14, 3, -13, -5, -7, 6, -26, 1, 22, -5, -27, 3, 12, -14, 0, -16, -14, 17, -12, 10, 0, -8, 5, 6, 1, 3}
}
, {{23, 22, 7, 28, 0, -24, -7, -1, 24, 3, 20, 6, -9, 2, -10, -8, -11, 6, -10, -7, -17, -3, -3, -5, -3, -4, 4, -4, 14, -10, -9, -2, -30, 3, 11, -6, 6, 4, -7, 20, -12, 8, 5, 2, -20, -5, 3, -18, -20, -16, -1, 11, -8, -4, -12, 28, -9, 4, 0, 6, -27, -13, 8, -4}
, {4, 3, 4, -4, 6, -5, 6, -10, 32, -13, -4, -4, -19, -22, 24, -6, -22, 9, 5, -18, 19, -2, -10, 10, -3, -8, 12, 14, -11, -6, 0, -22, -21, -22, -20, 17, 1, 8, -20, 6, 4, 34, -9, 19, -20, 13, -16, -18, 4, 18, -7, 7, -6, -14, -16, 15, 23, -25, 29, -3, -7, 16, -16, -6}
, {-12, 4, -24, 3, 4, -4, 11, 0, 8, -5, -11, -3, -1, 9, 18, 0, 15, 1, 18, 8, 6, -3, -1, 1, 3, 20, 2, -17, 4, 2, 11, 2, 6, -4, 2, 11, -9, -2, 17, 0, -14, 16, -16, -10, -9, 11, 8, -24, -7, -6, -8, -11, -10, 3, -1, -14, -3, 10, 7, -3, 8, 15, 1, 11}
}
, {{-22, 9, -14, -10, 10, 3, 6, 9, -1, -15, 2, 3, -19, -8, -4, 5, 0, 12, 9, 4, -10, -7, -6, 10, -11, 10, 11, -7, -3, 1, 18, -21, -16, 6, -14, -16, -1, -1, -9, 18, -3, 3, 8, 1, -11, -9, -24, -9, -9, 17, 8, 11, 4, -14, -14, -14, -18, -2, 17, 16, -14, 4, 9, -2}
, {12, -2, -23, 2, 2, 5, 6, 10, -23, -21, -16, 10, -12, 7, 10, 18, 8, -9, -1, -20, 24, -8, -9, -4, -17, 10, 14, -13, 27, -17, -12, 8, -24, 5, 9, -6, 0, -12, -18, 2, 14, 9, 0, 3, -4, 6, 4, -2, -25, -18, -2, 11, -4, -9, -12, -18, -32, 0, 18, -1, 40, 25, -30, 22}
, {-4, -12, 7, -6, 3, 1, 19, 5, -3, 4, -28, 11, 11, 7, 15, 2, 8, 10, -1, -49, 0, 6, 9, -5, -14, -9, 24, -24, 10, -1, -8, 8, 4, -19, -3, -16, 11, -8, -1, -9, 31, -14, -12, -10, 1, -16, -19, -32, -13, 6, 7, -19, 5, 12, 0, -9, -7, -7, 2, -3, 5, 1, -12, 5}
}
}
, {{{-16, -9, 17, -13, 2, -4, 7, 17, 4, -9, -4, -17, 12, 0, -13, -21, -18, -7, -2, 0, -10, 7, 2, 8, -1, 5, 1, -10, -18, 15, -13, -10, 1, 3, 8, 5, -17, -16, 18, -23, 11, -4, 8, 2, 18, -9, 3, 4, 5, -13, -1, 11, -11, -7, -19, 7, -7, -5, -3, 9, 4, -26, 5, -15}
, {-1, 24, -8, -7, 5, 11, -4, 2, -6, 12, -3, -8, -14, -8, -31, -10, 13, -4, -8, 6, 2, -13, -6, -4, 21, -3, -10, -16, 6, -16, -6, 9, 15, -14, 28, 26, 28, -4, -2, -7, 8, 9, -6, 24, 4, 9, -4, -13, 10, 25, -11, 1, -3, 10, 6, -25, 3, -3, 6, -19, 11, -14, -16, -2}
, {-8, -7, -3, 3, 0, 3, -30, 3, -1, 12, 0, -9, -1, 3, -5, -13, 12, -6, 15, -8, -11, 12, -1, 2, -4, 15, -4, 7, 13, 8, -1, 14, 13, 2, 9, 18, -6, 11, 1, 12, 5, -9, 2, -5, -6, -7, 7, -12, -5, 6, 8, -29, 4, -4, -8, 1, 13, -4, 12, -6, 14, 9, -14, 21}
}
, {{21, 9, 4, 3, 0, -15, 0, 5, -19, 5, 6, 8, 9, 6, -6, -20, -7, -22, -1, -17, 7, -6, -10, 1, -9, -6, -8, 1, 0, 1, -4, -7, 7, -1, -14, 2, -7, -11, -3, 4, -10, 1, 5, 8, -11, 11, 6, -22, -2, 8, 6, -9, -17, -4, -5, -6, -9, 11, -29, 6, 10, 6, -15, 3}
, {-5, -8, -23, -13, -23, -4, 9, -6, 8, 11, 15, 7, -26, -8, 4, -6, -16, -26, -4, -33, -6, -7, -4, 0, -36, -17, 9, 1, -10, -12, 0, 3, -8, -6, -5, -4, 18, -16, -29, -33, -2, -5, -5, -9, -5, 2, -30, -5, -1, 7, -2, -6, -19, 15, 5, -5, -12, -9, -25, 3, 3, 2, -5, 4}
, {4, 3, -3, 6, 6, 2, 20, 16, 13, 28, -11, -6, -6, -7, 13, -18, 28, 17, 0, -24, -13, -13, -11, 8, -17, -6, -11, -7, -8, 4, 22, 4, 8, 18, 4, 2, 3, -5, 3, 8, 5, -24, 11, -7, -7, -2, -5, -2, 23, 4, 1, 4, -8, 17, 7, 3, 5, 14, 3, 6, 4, 9, -2, 24}
}
, {{-7, -4, -11, 5, -12, -14, -6, 11, 2, -6, 1, 0, 0, -12, -10, -16, -5, 3, 12, -2, 21, 14, 6, 10, 15, 20, 3, -13, -1, 16, -12, -6, 4, 13, 21, 13, 15, -12, -24, 17, -9, 8, -2, -1, 8, -29, -8, 11, 10, 15, -4, -4, -23, -1, 1, -5, -8, -4, -8, -2, -28, 8, 23, -5}
, {5, 2, -9, 21, -14, -23, 17, 10, 9, 23, -3, 7, 3, 5, -19, 14, -2, -10, 11, -10, 19, 7, 6, -10, 1, 3, 9, 2, -5, 14, -27, -20, 5, -20, 2, 18, 17, -8, -1, -8, 12, 27, -27, -5, -3, 3, 2, 0, 19, -8, 2, 36, 9, 3, 14, -11, 4, -14, -9, -10, -7, -12, -8, 17}
, {-8, 20, 12, 21, 16, -11, 34, 26, 10, 13, 11, 9, -2, -21, -7, 1, -1, -14, 2, -22, 1, 32, -1, 8, 13, -19, 21, -13, 10, -7, -10, -16, 2, 4, 8, -1, 3, -27, 9, 13, 10, 15, 0, -21, 2, -49, -18, -29, 27, -3, 3, 6, 7, -19, 17, -3, 3, -16, -25, -11, 6, 12, -3, -3}
}
}
, {{{20, -3, -5, 4, -10, 2, -2, -2, 7, -3, 8, -15, -7, 9, 0, 11, 11, 9, 21, 20, 8, 7, 7, -1, -3, -20, 27, 12, -13, 13, -5, 16, 6, -1, -15, 21, -12, 21, 2, 0, 16, 2, 21, 1, 5, 18, -5, 22, -23, -11, -6, -2, 18, -7, 2, 23, -12, 5, 12, -7, -3, 17, 7, -2}
, {11, 1, 11, 3, -13, -20, -7, -8, 4, 2, -1, -5, 1, -23, -4, 23, -15, -12, 2, -21, -14, 1, 6, 13, -6, 0, -21, -6, 20, 29, 16, -9, -7, 10, -13, -4, 9, -4, 21, -14, 8, -10, -6, 3, -7, 9, 4, -18, -4, -6, 5, 3, 0, 12, -11, 12, 2, 6, 1, 16, -1, 2, 3, 5}
, {6, -2, -16, -3, -33, 10, -20, -6, -13, -3, -14, -6, -2, 20, 2, 0, -10, 6, -11, 4, 3, -11, 2, 11, -8, -1, -4, -1, -7, 21, 23, -15, -6, 4, -10, -15, -4, 7, 3, -3, 0, 8, 4, 0, -13, 3, 9, -21, -10, -14, 2, 16, 9, -15, 8, -17, -13, 20, -5, 0, -13, -11, -1, -24}
}
, {{-10, 2, -1, 10, -18, 1, -12, 16, 9, -11, -18, -3, 10, -14, 7, 19, -3, -18, 3, 7, 1, -17, -1, -20, -15, -4, 6, 1, 4, -6, 10, -1, -16, -13, -15, 6, -16, 9, -25, 22, 3, 5, 21, -6, -25, -2, -4, -4, -1, 7, 5, 4, -5, 16, 19, 13, 14, -5, -8, -1, -6, -20, -11, 8}
, {5, -2, 14, -31, 12, 15, -4, -37, -3, -39, -1, 6, -1, 7, -9, -9, -8, -8, 12, -15, 14, -13, -13, 19, 26, -30, -28, -16, -10, -2, -34, -51, -23, -9, -9, 7, 8, 4, 6, -6, -39, 1, -12, 3, -5, 6, 23, 15, -11, 16, -14, -45, -18, -5, -8, -15, -5, -3, 9, -21, 5, 9, 5, -48}
, {-10, -21, -36, 16, -14, -8, 4, -18, 7, 9, -11, -5, -17, -2, 5, 2, -4, -2, -4, 8, -17, -7, 7, -14, 10, -2, -2, -2, 17, -2, 15, 2, 1, 18, 3, -3, 1, 0, -15, 6, 4, -17, 23, 9, -17, 5, -12, 7, -1, 2, 7, -5, -26, -10, 3, -16, -11, -2, -4, -5, -15, -6, -6, 8}
}
, {{2, 21, -1, 4, -6, 14, 15, 3, -1, -8, 5, -6, -5, -8, 4, -4, 17, 13, 3, 1, 8, 6, 20, 1, -19, 2, -5, 4, -15, 2, -2, -4, 14, -11, -2, 8, 6, -3, 8, -22, -12, 3, 14, -8, 1, 4, -4, 11, 9, -2, 6, -9, -2, -15, -3, -18, 16, 4, -2, -22, -8, -3, -4, -13}
, {-12, -13, -19, -23, 0, 15, -24, -1, -23, 1, -10, -31, -26, -3, 0, -8, -10, -20, -4, 5, -27, -6, 4, -17, -25, -10, 5, 40, -7, -27, -17, -12, -2, -14, 0, 1, -5, 25, 11, -8, -1, -5, -7, 11, 4, -22, 39, 3, -2, -1, 2, -2, -17, 4, 16, -20, 8, -19, -10, -8, 13, 3, 28, -4}
, {-10, 3, 9, -13, -2, -18, 11, 4, 0, -6, 2, -2, 8, 0, 4, -4, -10, -22, -1, -3, 6, 1, -7, 16, 18, -1, 3, 11, -9, 5, -27, -31, 12, -11, -3, 22, 18, 0, -9, -11, 0, -2, 0, -7, 8, -5, -1, 6, -14, -25, 1, -5, -12, -10, 1, 12, -16, 8, -7, -15, -15, -2, 12, -3}
}
}
, {{{3, -4, -11, 18, 2, 7, 5, 17, 2, 15, -5, -6, 1, 15, 1, -10, -8, 8, -11, 4, -12, 4, -6, -8, -5, 16, 5, -14, 4, 0, -5, 3, -14, -3, 12, 0, 9, 1, -15, 2, 3, 25, -12, 8, -13, -10, 2, 4, -8, -8, -9, 16, -11, 0, -3, 2, -11, -2, -9, -8, 10, -5, 5, -23}
, {-1, -14, -3, 1, 0, -11, 16, 3, -12, 7, 9, 1, -12, -5, 10, 0, -21, 9, -6, -8, -7, 14, -10, 0, -10, 17, 15, 8, 0, 5, 2, -7, -9, -2, -7, -13, 6, 3, 6, -13, 2, -12, -17, -5, 0, -3, -9, -18, 9, -4, 3, 7, -2, -10, -13, 10, -17, 11, -3, -3, -1, 13, 6, 12}
, {-8, -12, 0, -1, -7, 7, -7, -5, 4, -5, -27, 17, -5, -9, 10, -5, 7, 8, -5, -15, 14, -15, -13, 2, -5, 2, 9, -7, 1, -10, 12, 7, -22, 6, 4, -8, 5, 5, -6, 0, -10, -8, -5, -2, -14, 1, -6, -3, -5, -5, -12, 5, -11, 11, -3, 6, -7, 13, 8, 10, -18, -3, -6, 8}
}
, {{11, -17, -6, 18, -17, 11, -12, 0, -3, -10, 6, 16, -7, -5, 2, -1, 20, 19, -2, -17, -7, 10, 1, -3, -7, -2, -33, 5, 1, 3, 16, -1, -4, 3, 8, 2, 4, 23, 4, -17, 28, -18, -6, -22, -11, -17, 8, -14, -3, -2, 25, -18, 6, 0, 3, 6, -7, 22, 5, 6, 3, -3, -17, 5}
, {-46, -20, -5, 13, 4, 8, 15, 5, 12, 31, 21, 16, -3, -12, -13, -35, 1, 7, -18, 17, -46, -22, -8, 25, -31, 12, -23, -11, -34, -16, 29, 18, 0, 2, -32, 15, -23, -10, -14, -16, 16, 16, -26, -17, 13, -1, -41, 8, -8, -5, -22, -30, 8, 24, 22, -28, -5, -12, 14, 13, -19, -40, -18, 13}
, {0, 0, -14, -13, 0, 20, -12, 2, 2, 1, -21, 24, -9, 4, -1, -21, 2, -16, -15, -14, 4, -3, -7, 14, -23, -3, 7, 10, 2, 8, -17, -12, -7, -12, -8, 8, -2, 14, 4, 23, 1, -6, -9, 22, -8, 10, -6, -5, 6, 8, -8, 13, -6, 1, 3, 16, -4, 8, 8, 1, -3, -1, 8, 11}
}
, {{-6, 5, -6, 9, 19, -7, -7, -3, 7, -4, -8, -3, -8, -17, -4, -1, 6, -13, -3, 3, 2, -9, 3, -18, -1, 2, 7, 0, 4, -5, 7, 9, -11, 2, 20, -23, 1, -7, -10, 5, -1, -2, -13, -14, -23, -1, -12, -13, -10, -12, -7, 2, -6, 5, 12, 12, -11, 9, -4, 1, 2, -18, 9, 15}
, {-15, 14, -4, -21, 4, 2, 11, 16, -5, -28, 15, 0, -31, 9, -3, 13, 11, -1, -11, -1, 2, -15, 14, -11, -12, 1, 10, 9, 18, 4, 0, 7, -14, 9, -11, 2, -12, -11, 24, 3, -21, 10, 1, -11, 11, -8, -4, 0, -13, -15, 9, 19, -18, -10, -4, -7, -12, 4, 0, -11, 11, 0, 1, -5}
, {11, -20, -25, -19, -7, -18, -20, -10, 10, -1, -15, 8, -21, -19, 7, -23, -2, -16, -2, -6, -3, -26, -28, 10, -10, -2, -2, 0, -5, 6, 7, 7, -1, -9, -6, -13, -8, 7, -14, -10, 20, -1, -10, 7, -8, 7, 5, -16, 0, 11, -12, -1, -31, 13, 10, -11, -11, -9, -7, -8, -5, -21, 9, -14}
}
}
, {{{4, -3, 6, -1, -6, -19, 3, -7, 1, -24, -13, -1, -6, 0, 0, -4, -6, -9, 1, 20, 2, -10, -6, 8, -18, 4, 5, -4, 5, -2, -20, 1, -6, -5, -12, 3, 5, -17, -10, 2, -7, 6, 9, 12, -7, -9, 13, 13, 5, 7, -10, 1, 4, -17, 9, 7, -12, 15, -23, 0, -1, -14, 5, -4}
, {7, 11, -18, -17, -7, -4, -10, -11, -15, -9, 3, -9, -2, -19, 2, -13, 10, -3, 7, -13, 0, -5, -16, -6, -6, 5, -5, -9, 5, 4, -17, -1, -22, -2, -3, 25, 0, 14, -14, -13, 13, 0, -20, 2, -5, 9, -2, -15, 16, -15, -23, 2, -9, 15, 3, 15, 2, 2, -15, -21, 3, -1, 1, -2}
, {-10, 4, 3, -11, 16, 7, 4, 1, 15, -6, 16, 6, -2, -2, -2, -11, 15, 19, 12, 9, 11, 5, 10, -6, -15, 3, -4, 7, -6, 6, 19, 3, -5, 10, -6, 2, -15, -12, -22, 1, -1, -13, -5, -22, -6, 11, -3, 9, -1, 10, 8, 2, 2, 2, -15, 14, -5, 15, 12, 4, -12, -30, -8, 17}
}
, {{2, -2, 7, -5, 9, 0, 8, 4, -7, 5, -3, -9, -6, 5, -1, -3, -13, 4, 4, 10, 0, 3, -6, 1, -11, 9, 7, -20, -8, -3, -10, -17, -6, -10, -16, -7, 18, -5, -11, 2, -4, 10, -3, 17, -18, 6, -2, 0, 14, 6, -22, 9, 3, -6, -8, 7, -12, 11, -11, -6, 7, 15, -8, 0}
, {-3, -25, -3, -25, 13, -2, 39, -32, -45, 0, -52, -2, 11, 6, -4, 3, -8, -30, 26, -13, 14, 18, 20, 15, 21, -8, 1, 0, -8, 8, -35, -2, -4, 19, 20, -24, 3, 3, 15, -24, -36, 23, -10, 3, 15, 31, 3, 8, -9, 6, 2, 15, -42, -18, -23, -1, -21, -5, -20, -15, -4, 20, 5, -15}
, {-4, 2, -5, -7, -1, -27, 32, 2, 11, -10, -6, -2, 5, -18, 28, -10, 12, -5, 3, -2, 9, 10, 2, -17, -5, 4, 11, 4, 17, -12, 5, -22, -10, -5, -8, 8, -13, 4, 5, 15, 25, 9, -4, 12, -11, 4, -2, 4, 10, -2, 0, -6, 9, 7, 11, 5, 1, 3, 4, 9, -12, 15, 12, -6}
}
, {{1, -11, 10, 5, -19, 16, -5, 5, -22, -3, -1, -7, -12, -2, -11, 0, 0, -5, -8, -3, -9, -19, 1, 4, -21, 2, -6, 3, 4, 6, 3, 14, 13, -5, -14, -7, 6, -11, -4, 14, 20, -14, 3, -12, -1, 1, -15, 3, -2, -8, 5, 4, -5, 2, 4, 6, -17, -11, -1, -2, -10, 6, -13, 13}
, {-15, 14, 20, -2, -4, 12, -28, 2, -1, -5, 0, -3, 10, -12, -8, 4, 16, 18, 4, -28, -6, 0, 20, 8, -14, 22, -1, -7, 13, 8, 16, 4, 12, -31, -4, 12, -2, -11, 9, -6, 14, -15, 5, -17, 5, -26, -19, -25, 26, -14, -2, 2, 0, 17, 6, 3, -12, -6, -2, 8, -7, -36, 3, 14}
, {-15, 3, -16, -12, 12, -8, 17, -7, -22, -13, -1, 12, 0, 4, -5, 0, 10, -5, 18, 1, -2, 15, -9, -9, -1, 14, 12, -1, 16, 7, 9, 12, -2, -14, 18, 9, -12, -1, -10, 8, -2, -9, -3, -7, -9, 8, 15, 11, 3, -11, -22, 5, 1, -3, -8, -17, 1, 14, -16, -4, 1, 0, -6, 5}
}
}
, {{{16, 6, -1, 7, 14, 10, 5, -2, -16, -6, -12, 1, -20, -2, -14, -9, -25, -5, -18, 4, 14, 1, -3, -1, -4, -9, -16, -8, 3, -11, -16, -5, -3, -4, 5, -2, 13, -4, -12, 3, 17, 11, -7, 12, -6, 16, -2, 16, -22, 0, -6, 8, -11, -2, 0, -4, -13, 13, -7, 10, 8, 2, 2, 11}
, {-10, 10, 8, -25, -3, -12, -25, -8, -14, -11, 1, 6, -12, -13, 9, 8, 0, -15, -6, -8, -13, -18, -1, -11, -8, 4, 3, -6, 17, -15, 9, 10, 13, -1, 2, -6, 3, 1, -11, 3, 6, -16, 6, -3, 1, -12, -5, 6, 11, -15, 4, 2, -4, 10, -4, -5, 2, -1, -25, 10, -1, -12, -8, 5}
, {4, 2, -4, 10, 2, 9, 10, 0, 6, 3, 13, -6, -2, 10, 3, 16, 30, -13, -12, -9, 4, -8, -3, -8, -7, -9, -9, -2, -7, 17, 15, 26, 4, -9, -13, -5, 13, -15, -15, -15, 16, 1, 11, -6, -4, -13, -21, -4, -4, -9, 21, 3, 10, 16, 0, 11, 0, -10, 18, 11, 7, 2, -2, 8}
}
, {{4, 4, -7, -4, 10, -8, 17, 17, 4, -1, -4, -13, 2, -11, -14, 7, -28, -7, -10, 8, -3, 26, -1, -6, 11, 8, 25, -18, 9, 4, -2, -7, 4, -3, 12, 21, -8, -1, 0, -1, 3, 21, -10, 7, -15, 12, 3, -1, 9, 3, -6, -5, -11, 8, -12, -20, -8, -16, 6, -15, -6, 6, 9, 14}
, {-5, -6, -37, 1, -24, -20, -30, 11, 10, 5, 7, -6, 18, -20, -5, 10, -18, -32, 4, -10, -24, 15, 1, -38, -11, -43, 40, -13, 26, 1, -17, -5, -24, 3, 26, 7, -11, -13, -2, -6, 26, -10, 19, 5, 2, -15, -36, -15, -14, -34, 0, 16, -13, 20, 26, -11, -20, 9, -16, 8, -37, -2, 11, 6}
, {9, -31, -4, 6, -13, 9, -19, -2, 9, 1, 1, -12, -5, -15, 1, 6, -1, -15, -10, 0, 6, 8, -8, -12, 13, -4, -11, 6, 9, -7, -11, -2, 1, 17, -10, 11, -9, -13, -5, 4, 9, 2, 19, 15, 4, -3, 3, -9, -5, -3, 4, 9, -5, 16, 2, -18, 1, 1, -12, 5, 8, 0, 26, -2}
}
, {{20, -6, -3, -6, -4, -22, -1, 5, -9, -8, 8, 4, -8, 4, -5, 9, 0, -3, -3, -10, -9, 10, -2, -4, -8, -13, 14, 0, -3, -13, 10, -7, 11, 0, -1, -15, 0, 15, 12, -1, 8, -18, -10, 10, -6, 0, -15, 8, 9, -12, -3, 2, 7, 3, 6, -15, 1, 5, 2, -6, -9, 14, 10, -1}
, {-7, -17, 11, -17, 1, -15, 13, 13, -2, 11, 0, 5, 1, 5, 0, -14, 15, 1, 11, -2, -16, 13, 5, 8, 10, 6, 8, -10, -10, 24, 10, -16, 17, 12, 0, -3, -14, -8, -11, 1, -4, 9, 13, 13, -3, -12, -8, -23, 14, 7, 1, 9, 5, -7, -13, -29, 8, -6, -6, 0, 7, -11, 4, 13}
, {9, -28, -11, 5, -4, 6, -7, 2, -23, -14, -21, -1, 17, 8, 14, -17, 11, 10, 2, -12, 20, 14, -1, -14, 17, -13, 5, 7, 4, -15, -10, -4, 19, 19, -16, 3, 8, 3, 8, 6, 15, -2, -10, 11, -17, 18, 12, 6, 1, 3, -5, 17, -3, -25, -1, -24, -16, -19, -16, 17, -12, 14, 0, -10}
}
}
, {{{-6, 4, 1, 9, 10, 7, 11, -1, 5, -5, -15, 1, 4, 14, 3, 1, -9, 14, -6, -16, -4, 8, -6, 0, 6, 0, -11, 14, 10, 2, -7, 4, -3, -13, 3, 11, 9, 2, 10, -3, 15, 16, 4, -4, 13, -22, -5, 8, -8, -3, 12, -1, 13, 2, 17, 7, 1, -10, 2, -14, 2, 10, 0, 1}
, {-5, 8, 7, 1, -5, 24, -9, -11, -17, -1, -3, -5, -4, 1, -2, -8, 14, 7, -6, 8, 1, -12, 4, 1, -6, -5, -10, 6, -13, 0, 6, 5, -8, -11, -11, -5, -9, 9, 0, -6, 0, -9, -5, -11, 0, -1, -7, 6, 8, 1, 6, -9, -4, -2, 22, -9, 12, 1, 14, -13, 1, -10, 8, 8}
, {-13, -5, 4, 7, 1, 0, 1, -8, 1, -9, -2, 8, 1, 0, 3, -11, 15, -4, -1, 12, 8, 12, -3, -7, -6, 16, -1, -19, 1, -9, -13, -6, 2, -19, -16, -10, -6, 1, -4, -4, 0, -4, -3, -8, -2, -7, 15, 5, -15, 2, 10, 1, -7, -6, -2, -9, -2, 3, -17, -3, -2, 11, -11, -11}
}
, {{6, -10, -1, -1, 0, 2, 7, 10, -1, 9, 5, -1, 3, -5, 4, -8, -10, -13, 15, -11, -3, 15, 5, 10, 3, -9, 0, -16, 4, 8, -6, -4, -5, 1, 21, 6, 17, 8, -5, -6, -10, 5, -15, 15, 15, -19, -5, 0, -2, 2, 2, -13, -7, 8, 10, -3, -6, -10, 10, -5, -10, 7, -6, 22}
, {9, -7, -6, 3, 0, 0, -33, 5, -12, 26, 23, -51, 0, 12, -23, 7, 15, -1, -17, 19, -6, -12, -8, 8, -24, 34, 13, 31, 12, 23, -24, 9, -2, -18, 5, 20, 17, 2, -32, -9, 37, -2, -9, -35, -21, 3, 25, -10, 4, -2, 5, 28, -21, -4, -20, -31, -13, -14, -11, -24, -16, -33, 10, 35}
, {7, -7, -1, -14, -10, -22, 0, -1, 2, 12, -4, 9, 12, 9, 18, -18, 15, -6, 6, -2, 1, 1, 7, 3, 1, 6, 1, 2, 3, 0, -16, -3, 2, -3, 12, 8, -9, 6, -5, -10, -1, 11, -5, -19, 2, -3, 12, -4, 13, -10, 5, 9, -7, -3, -7, -10, 12, 2, -1, -3, -12, -18, -16, -24}
}
, {{11, -1, 8, -2, -12, -1, 13, -10, -2, 12, 2, 12, 1, 6, 4, -19, 6, -6, 7, -17, 10, -1, -4, 8, -3, 7, -17, -13, 13, 11, 21, -5, 3, -3, 4, -23, -5, 11, -1, 2, -2, -1, -18, -16, -3, -8, 6, -16, -1, 11, 14, 7, -9, 6, -8, -2, -6, 22, -2, 2, -8, 13, 0, 7}
, {-10, 12, -19, 1, 8, -21, -6, 14, 10, 2, 16, 5, -8, -35, -10, 15, 2, 10, -14, 11, -32, 7, 12, -11, 8, -8, -15, -29, -12, 21, 0, 14, -6, 14, -10, 0, -13, -14, -13, 10, -19, -24, -6, -15, 8, -29, -27, -6, 17, -7, 1, -4, -3, -6, -7, -9, -26, -6, -9, 0, -31, -6, -27, 0}
, {10, 5, -1, -7, -1, -13, 3, -14, -20, 10, -6, 0, 8, -3, 18, -6, -8, -10, -2, -9, 7, -6, 21, 17, 4, -13, -9, -9, 6, -16, -6, -1, 15, 8, 7, -12, -5, -7, 1, -9, 6, 4, 4, 3, -10, -2, -1, -3, 9, 8, 2, 1, -5, -9, 4, -1, -3, -21, -23, 4, -2, -27, 2, 1}
}
}
, {{{9, -1, 10, 2, -13, 4, -9, 7, 7, 5, 2, 4, 0, 6, 0, 1, -7, 4, -15, -3, 16, 0, -6, -6, -3, -1, 10, -1, 1, 1, 2, 13, -3, 17, 28, 4, 4, -8, -1, -10, 0, 3, -6, 2, 3, 10, 11, 1, 1, -21, 2, 18, -9, 14, -6, -13, -4, 0, -15, 1, -7, 7, -7, 11}
, {-21, -7, 11, 5, 5, 11, -4, 1, -1, 0, -15, -7, 7, -10, -4, -15, -3, -4, -4, -15, -3, 5, 12, 2, -7, -8, -9, -10, -9, -13, -6, -5, -11, -10, 6, -4, 8, 7, 0, 11, 6, 8, 5, -13, -2, -12, -11, -7, 10, 10, 12, 3, -13, 0, -4, 5, 9, -7, -7, 21, -4, -17, 1, -1}
, {-10, -18, 10, -6, 3, 14, 14, -10, -6, 2, -9, 2, -22, -9, -5, -3, 39, 4, 23, -4, 13, 11, -22, 6, -6, 12, 8, 12, 0, 6, -6, 3, 0, 2, 4, 4, 10, 19, -12, 10, -4, 7, 1, -5, 15, 2, 6, 11, 6, -20, -23, 5, 4, 19, -8, -1, 2, 9, 10, -7, 15, -1, 11, 12}
}
, {{-2, 20, -2, -6, 1, -5, -5, -15, -6, 15, 12, -10, -3, -2, 3, 1, 4, 6, -16, -10, -7, 2, 11, 18, 1, 8, 6, -18, -1, -1, 2, -9, -10, 2, 4, 4, -5, -1, -5, 8, 4, 21, 9, -13, 10, 7, -3, 4, 6, -3, 3, 9, 2, 3, 7, -12, 5, -13, -6, 0, 2, 1, 3, -12}
, {-21, 14, -13, 13, -16, -2, -1, 0, -14, 9, -2, -6, -4, 13, -17, -1, 11, 2, -19, -13, -23, -30, -11, -5, -14, 7, -17, -8, 1, 15, 3, 10, -10, 10, -26, -21, -9, -11, 21, -3, 5, -28, 42, -4, 4, -24, -41, -5, 3, -21, -9, 5, 3, -5, -15, -3, -13, -5, -2, 10, 5, 1, -12, 8}
, {-5, -5, -12, 0, 4, 12, 7, 0, 1, -11, -13, 18, 8, 5, 19, -12, 29, -13, -17, -25, -21, -14, -15, -15, 6, 19, -3, -2, -10, -2, 22, 14, 8, 16, -13, -16, -10, -12, 1, 17, 7, -8, -3, 14, -2, 10, -9, 3, -3, -34, -16, 0, -21, -3, -4, 10, -6, 17, -9, 17, 3, -2, -17, 3}
}
, {{3, 1, -20, 4, 4, -5, 7, 0, 5, 13, -2, -11, 2, -29, 15, -16, -1, -11, -12, 11, -21, -6, 6, -15, -3, -7, -7, -19, 1, 3, -3, -4, 12, 6, 3, -4, 15, -7, -7, -1, 4, -12, -10, 7, 1, -4, 3, 2, -2, -3, 12, -6, -22, -23, 5, 19, -2, -6, -15, 2, -2, 3, -20, -9}
, {-19, -10, 25, -17, -2, -19, -12, 2, -6, 27, -5, -2, 21, -24, 3, -25, -17, 10, 11, -12, -12, -12, 14, -21, -22, 2, -10, 2, -22, 4, 10, -16, 25, 9, -1, -19, 11, -10, 1, -10, 17, 24, -11, -19, -25, 8, -16, -7, 6, -14, -8, -15, 15, -9, -1, -6, 7, 3, 11, -6, -4, -13, -16, 1}
, {-5, -27, -20, 2, 5, 10, 3, -7, 6, -18, -7, 2, -29, -7, -1, 14, 5, 15, 18, 4, -20, 1, -8, -10, -15, 6, -4, 20, 2, -2, 12, -7, 5, -18, 0, 4, 7, 10, 0, -7, -2, -21, 11, -2, 9, -6, 2, -11, 3, 2, 4, -5, -8, -2, 4, 12, 13, 11, 17, 4, 0, -11, -12, -21}
}
}
, {{{6, 8, -3, 17, -20, 5, -9, 7, 3, -19, -2, 14, -4, 9, 0, 9, -9, 9, -18, 7, -5, -5, -12, -24, -1, -9, 10, -13, -5, 6, 9, -5, 3, 7, 5, -2, 7, 5, -12, 7, -7, -3, -6, 1, 6, 6, 2, -4, -17, 0, 0, 19, 20, -9, -3, -9, -2, 14, 0, 4, 3, 2, 19, -16}
, {-2, 7, 6, -5, -6, 5, 19, 2, -12, 5, -12, -10, -11, -5, -15, 7, 2, -11, 5, 15, 0, -11, -7, -10, -26, -7, 4, 17, 0, -4, 2, 5, 12, 8, 1, 1, 10, 21, -5, -1, -4, 1, -26, 4, -7, -5, -2, 8, 15, 5, 10, 4, -2, 23, 3, 4, -22, -6, -5, 9, 8, 13, 6, -8}
, {-19, 5, 2, -2, -9, 2, -1, 1, 5, 22, -2, 4, 17, -23, 25, 0, -11, -6, -3, -6, -1, 13, -17, -10, 0, 1, 7, -4, 6, 9, 2, -29, 2, -2, 13, 8, -10, -6, -2, 16, 6, -5, -15, 3, -7, 3, -19, -23, -13, 6, 20, 2, -5, -2, -1, -8, -3, 23, -7, 7, 0, -1, 1, -4}
}
, {{9, -6, 15, 27, 6, 8, -16, 5, 5, -7, -16, 2, -4, -3, -10, 16, -15, 12, -14, 9, 1, -6, -26, 4, 21, 24, 1, 3, 11, 18, 2, 1, -7, -1, 15, 0, 2, -14, 2, -1, 14, 1, 5, 7, 4, 3, -18, -3, -28, -21, -8, 3, -9, 13, -2, 23, -15, 9, -7, -4, -5, -11, 0, 17}
, {-2, 14, -5, -26, -15, -21, 13, 4, 11, -25, -36, -12, 4, -13, 26, -16, -30, -30, 2, -12, -13, -10, 2, 7, 40, -6, -7, -27, 15, 3, -9, -47, 7, -10, 12, 18, -16, 1, 7, 20, -23, 18, -8, -2, 2, 3, -1, 7, -17, 9, 21, 10, -8, -10, -4, -15, 13, -9, 12, -36, -8, 5, 8, -22}
, {-3, -20, -8, 9, -4, 12, 1, 9, -6, -12, -16, 18, -6, 0, 10, -29, 0, -5, 3, 5, 27, 16, -16, -13, 13, 7, 17, -11, 5, -7, 11, -5, 3, 11, 0, -20, -4, -2, 13, -20, 13, -3, -9, 12, 11, 13, -2, -26, 7, -18, -6, -2, -9, 11, -19, -5, 2, -3, -20, -8, 9, 20, 1, 17}
}
, {{-22, -11, 4, -2, -8, -17, 19, -14, 5, -26, 20, -4, -7, -9, -5, 27, 3, 2, 7, 1, -9, 8, 23, 22, -5, 11, -1, -6, 6, 8, -2, 5, -16, 4, -10, 2, 7, 1, 4, 0, -13, 2, -8, 0, -15, -12, -14, 11, -12, 6, 11, -5, -11, 9, 3, 0, -19, 8, -5, 9, -6, -12, 0, 7}
, {12, -22, -17, 3, -5, 23, 7, 18, -25, -3, 8, -13, -26, 13, -2, -2, -3, -3, 23, 21, -7, 17, -15, 18, 5, 11, 12, 10, 25, -44, 17, 12, -23, -7, 15, -4, -12, -2, 2, -11, 9, -7, -11, 22, -2, 16, 21, -12, -8, -21, 2, 10, 0, 7, 13, -24, 8, 20, -21, -14, 9, 7, 2, 26}
, {-9, -12, -16, -5, 13, -11, -5, -17, 17, 6, 27, 10, 11, 5, -9, -33, -12, -12, 8, -1, -15, -7, -1, 6, 9, 9, 12, 1, 10, -10, 6, 23, 13, -6, 11, -14, 1, -9, -20, 2, 21, -27, -9, -13, -1, -1, -2, 14, -4, 3, -2, -5, 10, 37, 16, -8, -17, 4, 8, 6, -12, -8, 4, 5}
}
}
, {{{-1, -2, -4, 0, -6, 1, 8, -11, 4, -5, -14, -11, -6, -23, 1, 7, -2, -4, -7, 31, 3, 6, -10, -8, 3, -9, -6, -8, 5, 6, -3, 12, -9, -6, -11, 14, -19, -22, -7, 16, 14, -4, -2, -12, -16, 4, -8, 6, -7, -8, -1, 1, 7, -16, -16, 5, -16, -10, -5, -12, -6, 2, 4, 3}
, {11, -2, 19, -1, 10, -14, 22, -7, 4, -6, -2, 2, 5, 8, -4, 24, 6, -14, -13, -13, -5, 7, -11, 4, 12, -20, -3, -20, 4, 13, -16, -3, 17, -1, 13, 9, -12, 18, 6, 9, 13, -1, 9, -2, 15, -16, -1, 0, 8, -1, -3, 0, -6, -3, 11, -31, 8, -26, 17, 8, 1, 24, -13, -23}
, {2, -5, 3, 8, -18, -7, -4, -4, -22, 16, -26, -7, 5, 8, 4, -5, 11, -35, 14, 5, -1, -12, 9, 4, 8, -3, -20, -14, -13, 24, 13, -17, 12, 5, 12, -2, -5, 6, 1, -2, -12, 22, 6, 2, -25, -2, 7, -7, 8, -8, 6, 22, -1, -2, -5, -10, 15, -15, -4, 0, -12, 4, -1, 5}
}
, {{1, -5, -11, -23, -3, 3, 13, -4, -13, -8, -10, -9, -1, 9, 16, 3, -28, -1, 3, 10, -13, -3, 23, -11, 2, 3, 16, -9, -9, -18, -2, 7, 21, -8, -10, 0, -19, -31, -2, -5, -24, 16, -6, -6, 5, 2, 6, 4, -7, 2, -11, 5, 4, -11, -7, 2, -14, -2, -1, 4, 11, 1, -9, 4}
, {-9, -21, 3, -22, 6, -6, -6, -5, -11, -8, -18, 7, -10, -14, 14, -6, -23, -17, 8, 1, -11, 12, -1, -7, -11, -12, -10, 11, 18, -6, 4, -8, 7, -2, 9, -26, 14, 0, -11, 6, 8, -13, -23, 19, -7, 13, -1, -7, -13, -9, -12, -17, -21, -1, -17, 9, 22, -14, -7, -21, 0, 9, -3, 11}
, {-4, 22, 13, 16, 2, 31, -6, 8, -10, 3, 23, -32, 7, 12, 3, -17, -15, 23, 1, 7, -4, 2, 8, 1, -4, 8, -2, 1, -29, 2, 11, 8, 11, 43, -12, -3, 10, -16, -1, -11, -2, -28, 36, -5, -8, 0, -4, -4, -12, 3, 0, -38, -3, 11, -8, 15, -18, 5, 4, -14, 7, -30, -5, 18}
}
, {{5, 5, 4, -6, 4, 32, -22, -8, -5, -4, 2, -4, -4, 24, -3, -3, -2, 9, 25, 9, -24, 19, 15, 6, -30, 13, 8, 21, -4, -6, 9, -18, 4, -30, -1, 4, -7, -13, 0, -24, 8, -25, -6, -14, -9, -12, -14, -31, 15, -23, -12, -8, 7, 10, 6, -25, -2, 22, 21, -7, 5, -11, 8, 14}
, {-16, 27, -13, 2, 2, -8, 0, 3, -9, -4, -8, -2, 4, -2, -20, 2, 1, -8, 1, 1, 3, 6, 4, 10, 4, -11, -3, -9, 12, 9, -1, 3, 17, -9, 5, -24, 12, -13, 15, 9, 0, -21, 5, -21, 13, -20, 2, -10, -12, -33, 15, 16, 8, -2, 5, -5, 2, -12, 9, 11, 7, -5, -19, 2}
, {10, 13, -6, -3, -11, 5, -21, 22, -16, -11, -8, 19, -1, 9, -8, -19, -11, 15, -9, -2, -11, 3, 3, -24, -13, 12, 5, 13, 24, 12, -3, 16, 17, 6, 11, -24, 2, -19, 11, -6, 18, -35, 22, -3, 11, 25, -3, -8, 8, -17, 14, -9, -13, 1, -3, 4, -25, 2, -23, 0, 4, -11, 9, 3}
}
}
, {{{-8, 1, -2, 6, -2, -16, -7, 8, -6, 5, -12, -2, 1, -5, -2, 5, -1, -2, 12, 1, -15, 2, 5, -16, -21, -11, 6, 3, -4, -5, 11, 0, 7, 1, 2, 1, -5, 22, 13, 4, 6, 18, -3, 7, 1, -14, -6, 3, 18, -24, -5, 5, -7, -2, 9, 14, -5, -10, 11, -8, -16, 10, 8, -20}
, {-9, 5, -7, 0, -1, -21, 3, -1, -18, 14, -14, 2, 9, -7, 1, -4, 6, -13, -5, -3, -3, 13, 6, 13, 0, -14, -9, -6, 24, 14, 13, -16, -13, -1, -5, -13, 16, -7, 13, 3, 4, -8, 1, 2, -11, -8, 3, -11, -6, 3, 12, 9, -7, -13, -5, -6, 1, -5, -2, 8, 7, -6, -10, 5}
, {4, 5, 0, 10, -2, -10, 2, 10, 1, -13, -27, -7, -2, 13, 19, 7, 12, 6, -1, 7, 8, 12, 3, 14, 8, 4, 8, -4, 2, 1, -6, -20, 6, -6, 5, -3, -17, 22, 3, -2, -16, 9, -9, 5, -1, -2, 13, -9, 3, 4, 1, 6, 3, -13, -6, 1, -8, 18, -2, 11, -1, 3, 2, 0}
}
, {{3, 5, 9, 1, 1, -10, 11, 11, -7, 3, 5, -13, 4, -10, 3, -4, 0, -9, 0, 5, 18, 16, 2, -10, -20, -24, -10, 13, -9, 2, -15, -11, 11, -8, -23, -4, -3, -12, -5, -15, -12, 5, -18, 8, 1, 16, -1, 9, -3, 5, 15, 9, 5, -7, 7, 17, 3, 11, 1, 9, 3, -24, -1, 0}
, {17, -7, 13, -2, 1, 22, -34, -40, -29, -11, -1, 2, -33, -6, 16, 2, 16, 24, 1, -17, -6, -2, 3, -1, -9, 19, -21, -31, 1, 0, 2, 6, -6, -3, -19, -3, 29, -1, 13, -5, 25, -43, 8, 2, -35, 4, 8, 8, -12, -23, -19, -6, -6, 3, -34, 8, 21, 6, 5, -13, 20, -1, -14, 26}
, {-8, -12, -7, 8, -9, -18, -15, -6, 6, -8, 12, 3, -1, -1, -2, -11, 16, -28, 9, 13, 3, -13, 3, 3, 3, 20, 11, -4, -11, -3, 29, 21, 19, 5, 0, 5, -4, -21, -7, 9, 5, -10, 20, -15, 3, 7, 7, -12, -12, 4, 11, -14, -3, 13, 4, -2, 3, -3, -3, 4, 6, -12, -20, 4}
}
, {{-3, 20, 3, -8, -2, -2, -2, 7, -2, -2, 1, -10, -15, 4, -12, -18, -12, 12, -8, 21, 11, 5, -1, -6, -2, 4, 0, -9, -4, -7, -5, -1, 2, 19, 14, 8, -18, -22, -9, 2, 1, -17, 16, 9, -9, 2, 4, -8, 3, -1, 8, -7, -3, 5, -1, -20, 9, -11, -5, 9, 18, -8, -22, 3}
, {-8, -2, -11, -11, -2, -5, -12, 10, 0, -16, -22, -2, -3, 5, 22, -7, -11, -13, 1, -7, -9, -5, -14, -33, 15, -21, 6, 9, 0, -18, -23, -5, -13, 9, -35, 10, -2, 8, 0, 9, 2, -14, -2, -2, -4, 21, 7, 10, 6, -5, 5, 7, -25, -2, 3, 3, -1, -12, 4, 10, -8, 16, 19, -2}
, {20, 5, -9, 31, -12, -3, -9, -7, -3, 7, 8, 9, -7, 4, 3, 15, -7, 19, -9, 6, -1, -24, -2, -1, -18, 15, 5, -15, -1, -14, -17, -8, 6, 31, -23, -8, -16, 12, -10, -14, 7, 1, 5, 8, 2, -30, -9, 15, -9, 12, 5, -17, -3, 8, 1, 18, -23, 18, 1, -15, 2, -3, -1, 8}
}
}
, {{{14, -7, 0, 8, 3, -8, 14, -8, -19, 1, -13, 3, 4, 10, -2, -17, 3, 0, -8, -1, 4, 5, 2, 5, -1, 2, -21, -18, -7, 2, 7, -1, 6, -9, 13, -1, 5, -7, -3, -7, -1, -1, -15, 2, 0, 8, 16, -15, -7, -5, -3, 18, -3, -10, 15, 7, 7, -7, 4, -11, -19, 14, -10, 10}
, {0, 1, -12, 11, 8, -2, -5, -5, -13, 11, 11, -17, 2, -16, -10, -10, 9, -8, -9, -5, 3, 21, -9, -13, 2, 3, 7, 8, 5, 9, 7, -8, -4, -3, 9, 1, 7, -3, -14, 6, 7, -1, -28, -12, -7, 17, 3, -19, -4, -17, -5, 0, 10, 7, 4, 7, -11, -5, 1, 15, -8, 8, 6, 2}
, {-3, -9, 1, -21, 3, 16, -17, -1, 6, -7, -8, 0, 0, -17, -4, -15, -12, 7, 20, 4, -7, 2, -5, -6, 1, 17, -17, 3, 10, -19, 6, 7, -4, 2, 7, 0, -3, -12, -2, -11, -4, 1, -6, 0, 8, 1, 8, -1, -3, -7, -4, 13, -6, 0, 18, 14, 0, 16, -4, -15, -3, -2, 13, 2}
}
, {{-12, -2, 7, -18, 9, -5, 3, -3, 0, -1, -5, -6, -10, 3, -7, -12, 1, -2, 5, -3, -10, -9, 1, 2, 18, 11, -19, -3, -12, 9, -2, -2, 0, 3, 20, 7, -3, -20, 3, -13, 1, 12, -2, -11, -1, -5, 0, -13, 5, -8, -2, -1, -19, -6, 9, -13, -17, -14, -3, -14, -11, 5, -7, 5}
, {-14, 35, 9, -12, -6, -6, -12, -2, 1, -18, -8, -3, -13, 22, 12, -13, 15, 14, -2, -26, 24, -23, -28, 11, 15, -1, -23, -14, -20, 5, -7, 18, 6, -4, 3, 7, -9, 0, 4, 0, -19, 3, 2, -5, -21, -5, 12, -8, -4, 22, -3, -14, 3, 5, -2, -9, 8, -13, 15, -10, -5, 0, -18, -3}
, {5, -8, 0, -5, 12, 1, 1, -23, -10, 0, -19, 7, 17, 5, 20, 7, 2, -13, -1, -7, 2, -7, -3, -14, 8, -11, -9, 1, -13, 2, -4, -1, -16, -1, -7, -25, 20, 6, -9, -12, 3, 4, -3, 2, 2, 0, 10, -9, 7, -16, -1, 9, -32, -6, 12, -7, -13, -14, -15, -14, 12, 2, -5, 2}
}
, {{-11, -10, 4, -4, 11, 11, -10, -10, -20, 2, 7, -3, -35, -16, 0, -15, 15, 9, -12, -3, 0, -15, -17, 20, -19, 6, -24, -20, -15, -18, 4, -3, -19, -16, -4, 8, 1, -17, 3, -18, -9, -20, -14, -18, 1, -2, -10, -14, 25, 11, -26, -9, 16, 11, -2, 2, 15, 2, -6, 12, -21, 8, -9, -2}
, {2, -21, -6, 18, 7, -11, -2, 18, -3, 7, 8, 18, -17, -4, 4, -13, 0, -10, 19, -4, -9, -7, -4, 19, 10, 11, 13, -3, 23, 4, 11, 12, -13, -10, 3, 1, 14, -9, -6, 8, 8, -27, 3, -6, -10, 5, 16, -6, -14, 7, -18, 0, 10, 4, 4, -4, -17, 2, -12, -12, 7, 17, -17, 8}
, {-9, 18, 14, 12, -9, -29, 9, 15, 9, 20, -1, 3, -8, -14, -3, 8, -24, -5, 23, -20, -2, -19, 3, 25, -3, 3, 11, -23, -8, 15, 3, -9, -14, 8, -8, -16, 0, 7, 18, 3, -14, 15, -7, -23, -4, -12, -2, 12, -9, 4, -5, -9, 4, 19, 5, 22, 6, -27, 14, -2, 0, -2, -15, -8}
}
}
, {{{-11, -2, -9, 1, 1, 2, 0, -16, -1, -2, -14, 1, 7, -14, 1, 1, -7, -18, -7, 7, -27, -15, -9, -6, 3, 7, 5, -10, 5, 7, 3, -6, -4, 5, -13, -7, -4, 2, 9, 23, 4, -6, 12, 3, -13, -10, -5, 3, -13, -3, 6, 2, -1, -12, -1, 4, 10, -11, 0, -10, 2, -1, -2, -3}
, {-10, 7, -1, -4, -8, -2, 0, -8, -9, -22, 6, -4, 7, -11, 6, -10, -14, -19, -5, -10, -12, -13, -1, 1, -6, 4, -1, -20, 11, 1, 13, 6, -15, 5, -11, -17, -26, 1, 7, -3, -5, 16, -1, -11, -29, 8, -7, -11, -24, -6, -2, 13, -17, -9, 3, 5, 1, 1, -17, 0, 9, 10, -2, -11}
, {-6, -24, -5, -3, 0, -4, -14, 8, -7, -9, 14, 1, -14, -5, 0, -2, -13, -1, -10, -18, -3, 4, -5, 2, -4, -7, 3, -23, 13, -5, 4, -1, -1, -8, -8, 8, 16, 4, -3, 4, 6, -5, -7, 3, -18, -21, -21, -5, 0, -7, 2, 13, -13, 14, -6, 8, -10, 3, 9, -12, 9, -11, 0, 1}
}
, {{-6, -3, -3, 2, 4, -6, 7, -9, -3, 6, -9, -11, 8, 2, -4, -2, -10, -3, 13, -3, -3, 2, -7, 5, -17, -3, 9, -5, -5, -10, -9, -1, -14, -7, -8, -1, 10, 16, -15, 4, -4, -3, -11, 1, 3, 4, -15, 0, -2, -5, 6, 11, 1, 14, -4, 7, 6, 8, -5, 4, -4, 10, 5, 1}
, {22, -11, -2, -15, 3, -3, 20, -33, -54, -9, -18, 35, 24, -22, -15, 0, 10, -3, 1, -7, 6, 9, 17, 17, 22, 21, -14, -9, -9, 24, 5, -5, 18, -11, -8, 1, 11, -3, 28, -18, -19, 16, -8, -27, -7, 26, 29, -9, 20, -1, -13, -32, -31, -10, -10, -9, 3, 15, -18, -39, 2, 1, 2, 1}
, {-4, -4, -6, -6, 1, 8, -19, -1, -11, 2, -26, 16, 3, 4, -3, -11, 9, -18, 2, 1, -16, -6, 7, -23, -2, 2, -5, 8, 6, 6, 17, -22, 8, -12, 4, -16, -13, -9, -13, 26, 7, 2, -31, -2, -8, -15, 14, 16, -9, -7, -13, 11, -18, 3, 7, -17, -4, 5, -2, 3, -16, 8, -1, -7}
}
, {{-3, -4, 11, -6, 3, 1, -3, -3, -6, -4, -1, 10, -6, -12, -1, -24, -5, 8, -8, -4, -6, 0, -1, 6, 8, 13, -3, -22, 4, 10, 19, 13, -15, 18, -15, 10, -1, 7, 5, 11, 8, 3, 5, -7, -3, -3, 3, -8, 4, -13, -9, -2, -7, -1, -2, 19, 6, 6, 5, 5, -20, 1, -10, 4}
, {-11, -9, -4, -10, -13, -3, -25, 10, -1, -21, -13, 5, -13, -17, 2, -9, 2, -4, -18, -14, -11, -5, 3, 5, -2, -9, 5, -8, -2, 15, -2, -6, -10, 8, 3, 8, -10, 18, -4, -11, 9, 12, -29, -3, -11, -22, 14, 5, 24, 11, -5, 0, -11, 7, 4, 2, -8, 5, 8, 4, -23, -3, 2, 1}
, {-26, -4, 2, -7, -4, -5, 20, 26, -2, 2, 2, -26, -5, -3, -2, 7, -5, -6, -9, -8, 4, 7, 10, -5, 9, -2, -2, -19, -17, 6, 16, -23, -19, -11, 5, 5, 3, 3, 21, -18, -8, -9, -7, -11, 0, -18, -3, 1, -7, -25, -8, 10, -16, 2, -4, 2, 7, -15, 0, 3, -10, -13, -9, 19}
}
}
, {{{-7, 8, -4, -2, -5, -17, -8, -3, -7, -2, 4, -19, 4, -6, 8, -6, -2, -6, -6, 8, -9, -12, 3, -6, -12, 3, 0, -2, -12, 1, -11, 8, 12, 10, 3, 5, -3, -13, -6, 14, -4, -8, -13, 4, -5, -6, -27, 9, -4, 18, 2, 1, -3, -14, -9, 5, -4, -6, -2, -8, 2, 10, 12, 12}
, {-5, -13, 5, 8, -4, 6, -3, -9, 17, -5, 1, 10, 1, -13, 14, -2, -10, -8, 3, -9, 2, 15, -12, -9, -3, 1, 4, 8, 2, -8, 1, -8, -6, 0, -21, 8, 1, -4, -2, -5, -1, -23, 1, 5, 6, -4, -23, -6, 9, 6, -11, -6, 6, 13, -13, -11, 21, -20, -3, -17, -7, -13, -2, 14}
, {0, -3, 1, 1, 18, 8, 8, -13, 6, 18, 6, 7, -1, 20, -4, 8, 3, -4, 19, -2, -10, -6, 2, 14, -1, -2, 2, -3, -3, -9, -10, -1, 10, -18, -18, 2, -6, 6, 18, 31, 6, -19, -3, -10, -4, -13, 5, -12, -4, 12, 14, -8, 5, 21, 10, -2, 4, 3, -18, 3, 7, 6, -11, 6}
}
, {{7, -13, -3, 23, -4, 8, -14, -2, -23, -1, 7, -17, -3, 2, -10, 11, 9, 11, 4, -7, 12, 3, -7, 4, -7, 7, 9, -13, 17, 7, 9, -6, -7, -12, 3, 2, 9, 9, 15, -23, 0, -17, 7, -10, 2, -11, 20, -3, -2, -15, 3, -5, 13, 2, 21, 6, 1, 0, -2, 10, -5, -6, -24, -7}
, {-29, -21, 4, -11, 4, 0, -12, 6, 23, 21, 4, 1, -10, -13, 9, 10, -4, 10, 14, 5, -47, -16, -9, 16, -8, -13, -24, 13, -42, -8, 14, -10, -10, -34, -11, 28, 16, -11, -15, -14, -4, 26, -15, -10, -14, 8, -19, 4, 16, 9, 8, -6, 12, -1, 9, -11, 1, 24, -12, -27, -9, -27, -9, -4}
, {13, 7, -8, -13, 6, -16, 8, 1, 8, -4, -5, -2, -4, -13, 2, 2, -24, -5, -1, 7, 11, 11, -3, -14, -21, 8, 1, -4, 17, 12, -1, 4, 3, -26, -13, 4, -2, -3, -1, -3, 10, 9, -8, -7, 10, -2, -5, -7, -22, -15, 20, 13, 7, -15, 17, -18, 6, -13, -5, 1, -1, -7, -2, -11}
}
, {{-11, -19, 12, -5, -11, 2, -8, -8, -1, 15, -4, -9, 9, 0, 4, -21, 20, -10, 14, 18, -28, -32, 8, -6, -8, 22, 6, -3, -1, 1, 8, -15, -13, -12, 4, -10, -1, 17, -9, 4, 13, -17, -13, -15, -7, -10, -5, -24, -2, 12, -1, -19, -5, 13, 6, 11, -2, 2, -9, -4, -1, -10, 8, 9}
, {0, 7, -11, -17, 0, -12, -16, -3, -13, -17, -14, 9, -9, -5, -11, -13, 8, 5, -12, 0, 6, -13, -14, -3, 3, -1, -1, -11, -14, -2, 9, 8, -8, -7, -16, 20, 6, 12, -4, -16, 12, -5, 9, -20, 9, -45, 18, -3, 4, 4, 8, -11, -21, 0, 18, 4, -20, -10, -7, -14, -14, -4, 24, -13}
, {5, 19, 19, 11, 1, -14, 3, -7, 23, 5, 3, -13, 9, 1, 12, 8, 2, 4, -16, 4, 10, -15, -4, -1, -7, 10, -10, -10, -1, -14, -16, 26, -19, 3, -12, -4, -8, 0, 8, 30, -5, 31, 2, 1, -9, -12, -11, 2, 1, 32, -3, 16, 12, -6, -13, 9, 7, 20, 5, 29, -6, 3, 3, -4}
}
}
, {{{14, -18, -4, 8, -23, -3, 6, -9, 23, -14, -6, -11, 19, -2, 19, -1, -24, 20, -5, 1, 4, 8, 6, -13, -2, 3, 13, 10, -4, 13, 3, 19, -15, -8, 4, 7, -7, -3, 6, 1, -3, 1, 10, 21, -6, 2, -12, -9, -25, 3, -4, 6, -1, -9, 10, 17, -5, 1, 4, -5, -3, 1, 14, 0}
, {-8, -16, 4, 4, 10, 3, -3, -8, -10, -22, -7, 1, -3, -5, 0, 2, -13, 15, -1, -17, 2, 4, -22, -1, 16, -11, -2, 21, -5, 0, 9, -1, 6, 0, 9, 11, 8, -1, 7, -9, 7, 11, 4, 10, -6, 19, 1, -15, 1, 4, 19, 6, 1, -1, 13, 0, 11, -9, 4, 4, 3, 0, 11, -3}
, {15, -5, -3, 16, -19, -16, 0, -17, -8, 3, -29, 12, 3, 8, 7, 0, 8, 0, 15, 2, 2, -3, -12, 11, 7, 16, 5, -11, 14, 30, 15, -6, -12, 1, -11, -10, -16, 15, 12, -16, 14, 20, -6, -9, -15, -2, 13, 5, -18, -9, 10, -2, 4, -2, -12, 2, 4, 3, -14, -9, 13, -5, -2, 2}
}
, {{20, -10, 14, 8, -6, -10, -8, 3, -1, 10, -13, -8, 5, 14, 0, 3, -1, 13, 7, 0, 8, -9, -9, -19, 16, 2, 7, -2, -7, 21, -6, -3, 15, 3, 0, -3, -10, -1, 26, 4, 3, 8, 11, -9, -7, -9, -8, 35, -8, 3, -2, -2, 11, 8, 14, -11, -12, -6, 14, -10, -4, 20, 12, -23}
, {0, 1, -7, -16, 31, -7, 6, -12, 0, -19, 1, 3, -26, -24, -4, -10, 1, -24, 18, -13, -13, 3, 30, -1, -14, 11, -33, -12, -10, 0, 7, 14, 8, -14, 19, -31, -15, 0, 14, -17, -7, -18, -27, -17, -30, 14, -9, -18, -2, 6, 8, 5, -37, 5, -17, 0, 19, 3, 0, -19, 4, 23, -15, 22}
, {-4, -6, -5, 20, -1, -1, -9, 1, -13, 9, -4, -16, 7, 0, 21, -12, 11, 5, 6, 2, 13, 14, 0, -3, 4, 16, 6, 4, -10, -3, 6, -2, 16, -8, 1, 4, -17, 2, 13, -1, 3, -9, 18, -26, 1, 13, 15, 7, 1, 20, 0, 6, -7, -5, -6, -6, 8, 28, -16, -7, -21, 0, 12, 13}
}
, {{-9, 2, 13, -6, -5, 12, -7, 7, -6, -20, -4, -6, 3, -19, -2, 9, -6, 13, -6, 18, -21, -5, 9, 1, -2, 10, -9, -23, 0, 5, 5, 10, 7, -12, -15, 12, 4, -11, -7, -1, -7, -7, 3, -17, -7, -3, 1, -1, -4, 0, 7, -3, -10, -21, -1, -21, -23, 0, 19, -12, -8, -4, 14, 8}
, {-15, 19, 20, 26, -12, 4, -1, 20, -9, 11, 23, -9, 12, -16, 21, 7, 3, 17, 3, -5, -9, 13, -9, -3, 2, -27, -14, -10, 1, -5, 9, 4, 12, -7, -5, 14, 1, -9, -7, -10, -11, -3, -1, -13, -29, -14, -21, -10, 4, 6, 0, 13, 2, 5, -5, -4, -1, 11, -6, 2, -28, -28, -7, 30}
, {-19, -3, 11, -3, 17, 7, 19, 3, 2, -3, -6, 16, -8, -8, 13, 5, 6, 1, -10, -18, 26, 19, 1, 4, 26, -6, -3, -9, -12, 1, -11, -4, 10, -4, 13, -1, -7, 3, 16, 20, -13, 2, -7, 6, 3, -18, -6, -18, -12, -11, -2, 17, -17, -3, 6, -5, 0, 2, -11, 9, -17, 7, -10, 5}
}
}
, {{{7, -24, -6, -5, -12, -5, 14, -2, -9, -4, -4, 14, -6, 4, -9, 8, 7, -6, -12, 5, 22, 4, -6, -13, 3, -19, 13, -17, 0, -2, -8, 3, -14, 1, -3, -15, 5, -7, -3, 3, -17, -3, -2, 1, -12, 0, 2, -8, -3, -5, -10, 21, -5, -19, 4, -15, 1, -4, -9, -7, -9, 3, -4, 2}
, {7, 13, -12, 17, -13, -2, -8, 0, -1, 9, -8, -4, -4, -2, 5, 0, 7, 5, 8, -6, -4, -8, -25, -8, -8, -3, -17, -12, -8, -22, -7, 1, 3, -6, -19, -7, -8, 5, -18, 15, 6, -5, -9, 4, -3, 5, -19, 10, -13, -4, 4, 5, 4, -12, -7, -15, -1, -21, -18, 4, -9, 9, 7, -6}
, {-3, -7, 12, -7, -6, 4, -6, -18, 5, -3, -10, 5, -12, -14, 2, -7, -6, 6, 7, -2, -10, 1, 5, -10, 3, -9, -5, -10, 3, 1, -6, 4, -5, -2, -15, 14, 13, 11, -2, 2, 3, -1, -5, 8, -13, -4, 10, 15, 11, -11, -18, -5, 3, 2, 4, -9, 7, 14, -2, 2, -3, 4, -5, 10}
}
, {{-2, -1, 1, -1, -3, 7, -18, 2, 0, -11, 16, -12, -8, -16, -21, -9, 11, -13, 17, -11, -4, 4, 1, 1, -13, 9, -10, 15, -2, -7, 4, -8, -15, -12, -21, 3, 3, 9, -11, -1, 3, -19, -23, -16, -13, -14, -2, -3, 1, -36, 0, 12, -7, -11, 2, 0, -8, 3, -13, -2, -18, -21, 7, 17}
, {-14, 26, 34, 1, 8, 8, -10, 10, 8, -17, 7, -1, 2, 10, -25, -3, 10, 25, 17, 4, -39, -33, 7, 5, -15, 17, -13, 0, -14, 5, 20, 24, -2, 14, 10, 13, -26, -28, 19, 8, 3, -16, 11, -21, 16, -26, 11, 11, 23, -21, -23, -23, 9, 2, 17, -12, -7, 3, -1, -12, -6, -31, -30, 0}
, {2, -7, -3, 2, 2, -14, 2, -3, -17, -12, -12, 22, 4, 11, 9, -22, 16, -8, -4, -10, -14, 6, -6, -16, 1, -9, -6, 3, 26, 14, 2, -9, -21, 1, 13, -11, -10, -11, 2, 1, -11, -2, 6, 14, 9, -5, 2, 1, 11, -28, -6, 15, -11, 15, 3, -4, -2, -2, -3, 10, -10, 1, -1, -8}
}
, {{-6, -14, -8, -6, 7, -3, -4, 3, -2, 9, -21, 9, 10, 13, 3, 8, 6, 3, 13, -13, -13, 9, 2, -8, -1, 16, -9, -10, -11, -1, 0, 1, 1, 10, -12, -14, -13, 1, -12, -3, 3, -8, -19, -19, -20, -2, -3, 5, 1, 3, 5, -9, -9, 5, -1, -6, 5, -1, -6, -13, -14, -15, -9, 25}
, {-25, -1, -1, 13, 8, -14, 15, 12, 9, 15, -6, 5, 26, -8, 4, -23, -3, 5, -11, 11, -24, -4, -7, 10, 8, -3, 0, -10, -28, 3, 10, 2, -6, -11, 18, 30, 11, -2, -5, -9, 14, 0, -9, -23, -8, -1, 15, -2, -10, -2, -5, -10, -13, -1, 16, 3, 6, -9, -7, -1, -18, 3, -6, 2}
, {5, -2, -8, -2, -15, -3, -11, -16, -9, 1, -1, 11, 10, -5, 6, 10, -1, -7, 21, -4, -7, -2, 2, 4, 25, 4, -14, -12, 9, 6, 0, 3, -22, 9, 6, 11, -18, 0, -8, -15, -15, 0, -2, 0, 15, 12, 14, 10, 7, 0, 1, -8, -2, -8, -1, 7, 10, 7, 3, 3, -5, 8, -1, -1}
}
}
, {{{1, -19, -13, -1, 0, 11, -13, -13, -6, -4, -5, 0, 6, 14, -15, 7, -10, -1, -12, -3, 8, 11, -11, -11, 9, -5, 9, -11, 21, 11, -14, -16, -14, 22, -14, -4, 10, -6, -3, -8, 36, 9, 13, -5, 5, 0, 2, 2, -6, -7, -20, 13, 6, 14, 4, 15, -3, 11, 7, 11, 1, -7, 16, -4}
, {-6, -5, 12, -9, -2, 0, 4, -10, -7, 5, -10, -9, 7, -6, -3, 4, -3, -10, 8, -1, -7, -13, -8, 6, 0, 9, 4, -1, 0, 6, -1, 4, 2, -1, -2, 3, 11, 6, -7, 0, -4, -3, -5, 3, -18, 20, -3, -19, 3, 5, 14, -6, 4, 15, -2, -5, 7, 4, -12, 15, 6, -8, 25, -10}
, {-9, -1, -14, 4, -23, 1, 12, -3, 7, 9, -23, 2, 13, 0, 1, -7, 4, -5, 22, -7, -17, -7, -3, 10, -8, 4, 8, -11, 1, 3, -3, 14, 14, 15, -1, 1, -6, 10, -6, 14, 5, 5, 0, -6, 1, 3, 4, 10, -13, -6, 3, 2, 3, -11, -8, -6, -4, 5, -19, 16, -5, -14, -5, 9}
}
, {{-1, 0, 2, -20, -2, -4, 7, -5, -11, 4, 1, 3, -18, -8, -12, -2, -5, -3, -22, -8, 5, -11, -4, 15, -3, 30, 20, -5, 34, -1, 11, 1, -11, 4, 0, -6, 0, -12, -14, -7, 9, 9, -28, 1, -17, -6, 3, 1, -10, -7, 3, 3, -16, -2, 0, -3, -2, -3, 8, -5, 11, 8, 5, -23}
, {-37, -8, -2, 18, -12, -8, -14, 18, 21, 0, 6, -26, -15, -36, 18, 2, 2, 8, -17, 8, -34, -5, -26, -23, -30, -14, 22, 0, 23, 10, 26, 6, -13, 25, 8, -1, -22, -8, -20, 5, 24, -38, 16, -6, -30, -44, -77, -26, 1, -35, 23, -28, 5, 38, 28, 14, -33, 10, -33, 14, -35, 4, -13, 10}
, {-5, -20, -11, -16, 7, -22, 8, -18, -18, 8, -10, -8, -3, -10, 4, 12, -5, -17, 2, 13, 0, 2, -8, 28, -8, 6, -9, 1, -1, 3, -2, -12, -6, -4, -9, -10, -11, 10, -12, 5, -9, 11, 12, -20, 2, -10, 2, 6, 7, 0, -16, 13, 7, 7, -4, -17, 2, 4, -24, -1, -7, -16, 5, 4}
}
, {{3, -8, -10, 8, -13, -8, 8, 2, 8, -5, -10, 5, 2, -17, 1, 25, 4, -3, 5, -11, 11, 14, 1, 4, -3, 1, 10, 3, 15, 0, -6, 2, -7, -6, 14, -4, -4, 12, 5, -18, 7, 15, 5, 0, -5, -12, 5, -3, -11, -3, 3, -10, 6, 4, 15, -2, -21, -11, 2, -12, -10, 2, 8, -7}
, {1, -2, -5, 19, -5, -4, -10, -12, 5, 15, -7, 8, 3, -7, -19, -28, -14, 0, -13, -15, -14, -21, 17, -11, -16, 25, -6, -13, -5, -1, 3, -5, 17, -10, -9, -5, 10, -1, -8, 1, 7, -43, 6, -13, -27, -13, -7, -12, -1, 10, 18, 2, -16, -20, -20, 12, -3, 28, -7, -6, -10, -15, 7, 6}
, {-9, -17, -1, 0, -3, -2, -5, 9, 9, 3, -14, 3, 3, 3, 12, 8, 8, 8, -17, 3, -15, -12, 8, -1, -4, 7, -7, 15, -5, 16, 7, -3, -1, -2, -1, 0, -22, 5, 2, -5, 1, 6, -1, 1, -18, -10, -3, -9, -7, -10, -11, 16, -14, 10, 2, -10, -6, 7, -8, 10, -6, -3, -8, 5}
}
}
, {{{-15, -2, -3, -4, -11, 1, -6, -18, -16, -15, 16, 12, 12, -13, 9, 0, -11, -11, -4, 7, -9, 8, 0, -4, -14, 2, 9, 4, 5, -5, 21, 1, -8, 3, 1, -13, -7, -16, -8, -1, -8, -22, -7, 6, -6, 3, -17, -9, -6, -10, -5, -21, 10, 2, 1, -3, -11, 0, 10, 1, 2, -14, 1, 6}
, {4, 12, -11, 3, -5, 5, 2, 0, -2, 2, -3, 7, 7, 0, -3, 9, 1, 3, -13, 8, -18, -19, -14, 7, 7, 6, -16, 5, -5, 15, 17, 5, 10, 7, -5, 10, 2, 0, -5, -11, -10, -5, 5, -7, -4, 15, -2, -8, 11, 9, -19, -15, -14, 13, 8, 5, -27, 9, -4, -7, -15, 16, 14, 4}
, {-6, 8, 13, -8, 13, -17, 16, 17, 9, 13, 7, 7, -8, -26, -7, -2, 15, -23, -11, 6, 17, -7, 12, 12, -4, 2, -6, -2, 6, 10, 23, -3, 20, 4, 1, 4, -27, -9, -17, -10, -11, -5, -12, 8, -20, -6, 2, -10, 5, -3, -12, -7, -11, 2, -5, -1, 2, -7, 0, -7, 5, -5, -4, 2}
}
, {{-12, 5, -5, 14, 7, 7, 0, -5, -3, -1, -4, -6, 4, -2, -3, 2, 10, 0, 8, 12, -5, 6, 0, -10, 3, 16, -11, 3, -21, -22, 31, -7, -7, -5, 14, -5, 1, -2, -1, 15, 20, -9, 0, -3, -16, -1, -5, 6, 1, -1, 2, -16, -2, -5, 5, -7, -10, 5, -4, -11, -2, 6, 4, 7}
, {-22, -1, -17, -25, 1, 11, 11, 3, -13, -9, -34, 27, 9, -1, -17, 6, -1, -6, 6, -17, -15, 6, -22, -14, 3, -27, -21, 14, -1, 12, -11, -18, -9, 21, -9, -17, 7, 15, 5, -25, -29, 20, -3, 7, 17, -8, -8, -12, -17, -8, 8, -1, 0, 11, 13, 4, 1, 2, 2, 16, -5, -2, 13, -4}
, {8, 6, -3, 0, -16, 28, 10, -19, -8, 16, -8, 19, -18, -5, -8, -11, 5, 17, 6, 15, -22, -7, 9, 7, -33, 15, -23, 2, 0, 5, 0, 2, 15, -9, 4, 15, -4, 4, -3, 11, 12, -2, -13, 1, -18, -5, -3, 12, -2, 14, 14, -16, -17, 0, 7, -2, -15, 1, -4, -26, -9, -2, 7, 19}
}
, {{-7, -13, 7, 8, -2, -9, -19, 1, 2, -18, 3, 5, -22, -2, -3, -8, 18, -14, 2, 2, 9, -3, -1, -6, 15, -11, 1, 2, 2, -13, -17, -1, -13, -13, -9, 2, 1, 1, 2, -2, 17, 1, 3, -12, -5, 1, 1, -8, 2, -12, -5, 14, 0, -2, -14, -10, -18, -7, 9, -1, 5, -11, 0, -16}
, {-3, -8, -12, -14, -4, -7, 3, -4, 1, -22, -1, -8, 5, -8, -5, -12, 18, -1, 2, -16, -1, 5, -13, 8, -10, 5, -2, 6, 0, 10, 4, 2, 6, -4, 4, -20, 7, 9, -1, -24, -22, -4, -10, -7, 14, -16, 24, -15, 10, 1, 3, 9, -1, -4, -16, -8, -17, -11, 10, -15, -19, -5, -16, -10}
, {-18, 8, -11, 6, 0, -4, -8, 24, -3, 5, 0, -3, -16, 2, -13, -25, 15, 3, 26, 7, -14, -14, 14, 14, -12, 9, -5, 16, 0, 1, -6, 10, -3, -5, 22, 6, -12, 9, -14, 3, 12, -8, -12, 0, 3, -7, 2, -6, 10, 2, -8, 13, -4, -18, 7, 1, -10, 15, -8, -5, -17, -4, -20, 3}
}
}
, {{{-8, 1, 9, 4, -9, 5, 6, 0, 5, 0, -22, -29, -16, 8, 1, 8, -14, 16, -21, -1, 25, -2, 5, 6, -5, -7, -1, -16, -9, -7, -6, -2, 7, -4, -10, 0, 7, 5, -7, -8, 7, 15, 18, 2, 3, 12, -3, 5, -34, 1, 0, 9, 16, -3, -4, 6, -3, -12, 27, 13, 3, -1, -1, -2}
, {2, -20, 3, 1, -2, 0, 3, 0, 18, 1, -11, -5, 8, 6, 16, 4, 4, -9, 13, 7, 3, -14, -7, 11, -19, -19, 7, -5, -4, 15, -3, 11, 12, 7, 14, 8, -2, 16, 2, 9, 18, 1, 1, -5, -4, 12, 5, 11, 2, -6, 8, -5, -6, -6, -4, 2, 2, -2, 13, 18, -1, 2, -2, -3}
, {1, -13, 2, -4, -30, -7, -6, -14, -13, 2, -1, 3, 16, -4, 6, -1, -6, -6, 15, 4, 0, 1, 12, 6, 12, -3, -8, -2, 4, 35, 15, -12, -4, 14, 0, -2, 7, 5, -9, 13, 14, 36, 9, 8, -8, 10, 14, -1, 3, -3, -6, 1, -6, -8, 12, -7, 3, 13, 11, 4, 0, -5, -6, -4}
}
, {{-4, 23, -1, 9, 8, -14, 3, -4, 6, -10, -18, 23, 4, -3, -4, 16, 4, -8, -9, 11, -17, -2, -13, 24, 13, 11, 30, -9, 7, -4, 14, 13, -6, 22, -4, -18, -13, 12, -5, 19, -1, 8, -1, 16, 12, 0, 16, 21, 0, -10, 1, -5, 0, 19, -25, 9, 3, 16, 5, 0, 14, 3, -23, -8}
, {-9, 13, -8, 13, 1, 3, -11, -6, 16, -14, -20, 7, -16, -29, 29, 10, -15, -16, 8, 4, 7, -12, 0, -7, -1, 5, -2, 1, 4, 0, 24, 1, -24, 19, 20, -13, -15, 5, -20, 16, 2, -15, 23, 9, -10, 7, 2, 18, -16, 8, -3, -13, 1, 0, -6, 25, -24, 19, -15, 21, -9, -3, 8, 1}
, {-4, 16, 7, 18, -10, 16, -12, 24, 11, 3, 0, -13, -6, -3, -1, 16, -6, 25, -2, -12, -5, 5, 0, 10, 16, -6, 5, -6, -46, -1, 4, 11, -8, 31, -10, -3, 12, -6, 12, 14, -16, 5, 12, 17, 10, -14, 1, -5, -21, 1, -9, -19, -3, 28, 13, 22, -13, -2, 5, 14, -11, -6, 8, 0}
}
, {{-3, -9, 9, 3, 14, -17, -4, 4, 17, 11, 10, -4, 9, -5, 9, -4, -14, 5, 2, 7, 0, 14, 13, 11, 7, -1, 23, -7, 11, -3, -4, 1, -15, 26, 29, -3, 22, 7, 0, 2, 0, -15, -6, 23, 12, -32, 1, -2, -8, 4, -1, -8, 3, -22, -10, 15, -2, 13, 3, 4, 2, 3, -3, -12}
, {-10, -20, 6, -1, -8, 13, 11, -2, 4, 13, -17, -17, 8, 6, -5, -5, -4, 17, 5, 2, 0, 15, 28, -6, -9, 9, 2, 7, -15, 6, -10, -17, 17, -21, -6, 2, 25, -7, 11, -14, -3, 20, 4, 12, -17, 13, -10, -28, -6, -14, -13, 11, 11, 10, 6, 15, 15, 9, 27, -8, -8, 6, -1, 13}
, {4, -11, 3, 9, -4, 12, 22, -6, 2, -9, 5, -9, 11, -14, -11, 0, -1, -15, 1, -18, 17, 28, -12, -3, 10, 9, 27, 18, 11, 4, -15, -4, -33, 8, 15, -16, 9, -12, -7, 0, -1, 2, 1, 12, -3, -14, 3, -10, 3, 19, 3, 0, -6, -1, 3, 18, -5, -6, -2, 8, 4, 12, 18, -25}
}
}
, {{{5, 2, 2, -7, -7, -5, 11, -1, -7, 2, -7, 4, 2, 6, -1, 1, 4, -1, -5, -9, 5, -10, 7, 9, -5, -1, 1, 3, 11, -7, -2, -22, 7, -24, -23, 7, 16, 14, -1, 5, 7, -4, -6, -9, 19, -11, 1, 8, 10, -9, -2, 1, 5, -14, 9, -19, -4, -6, -9, -6, -4, 4, 5, -18}
, {-19, -9, -7, -4, -2, 7, 15, -1, 11, -2, -6, -3, 1, -22, 2, -8, 21, -5, -6, 3, -4, -4, -18, -16, -9, -13, -1, 0, 1, -14, -1, 11, 9, -25, -14, 19, 6, -2, -10, 12, 12, -3, -7, -17, -14, -2, -18, -17, 29, 3, -13, -6, 5, 8, 24, 0, -4, -14, -14, -4, -7, -11, 14, -3}
, {-6, 0, 3, -2, -2, 4, -3, 4, 2, 2, 6, 18, 7, 9, 1, 0, 22, 18, -14, 1, 2, -12, 6, 8, 9, 11, -22, -6, -5, 2, 8, 12, 14, 5, 9, -18, -1, -21, 5, -18, -4, -4, 2, -24, 2, 0, 5, -4, 3, 10, 17, -25, 11, -5, 6, -4, -8, -11, 7, -17, -15, -8, -14, 18}
}
, {{3, -10, 4, -18, -22, 6, 3, 1, 1, -2, -9, 12, -2, 1, -6, -11, 8, -21, 12, -3, 6, -27, -6, -4, -26, 1, -12, 17, -3, 2, 2, 1, 7, 7, -8, -4, 9, 5, 1, -6, 19, -2, -13, 5, -15, -10, -3, -6, 5, -4, -3, -9, -1, 15, 7, -24, 20, -1, -15, -25, 3, -5, -5, -3}
, {-14, 11, 3, 2, -5, 21, -4, -34, -23, -21, -1, -28, -7, 34, -11, 1, 39, -3, -10, 5, 2, -10, -3, -9, -64, 16, -13, 26, -11, -1, -30, 27, -8, -32, 0, -23, 17, 1, -44, -40, -4, -14, -3, -5, -6, 1, -6, -12, 5, 20, -21, 19, -21, -1, 2, -16, 1, -13, 9, 1, 6, -21, -6, 12}
, {0, 6, -11, -11, 6, -8, 6, 12, 13, -15, -10, 8, 6, -3, 16, -7, 22, -2, -6, 14, -8, -26, -9, -19, -5, -8, -1, -8, 8, 15, -20, 5, 19, -2, 13, -6, 5, 10, -2, 4, 2, -2, 18, -5, 2, -31, 18, 0, 6, -1, 9, 18, -4, 20, 19, 8, -14, -5, -4, 0, -25, 0, 3, 5}
}
, {{-13, -13, -1, 19, -23, -8, 3, 11, -1, -11, -2, -6, -12, -4, 3, -7, 0, -13, 4, 3, -10, -21, 5, 1, 2, -14, 2, -8, -3, -5, 16, -13, -7, -15, 7, -5, 9, 4, -4, 8, -13, -6, 10, 12, -9, -13, -5, -8, 26, 4, 3, 8, 4, 13, -26, 5, 7, -2, -6, 2, 0, 2, 4, -2}
, {-2, 20, -12, 11, 8, -21, -7, 6, 8, -21, 15, 18, -2, -4, 11, 36, -12, 0, 8, 16, -14, -24, 22, -9, -18, -15, 6, -2, -34, 27, -1, 11, -27, -1, 23, -18, -2, -28, 19, -7, -19, -10, 5, -26, 18, -9, -29, 17, 5, -15, 6, 12, 7, -14, -11, -14, -29, -23, 11, -10, -1, -3, -15, -10}
, {10, 16, 18, 5, -3, -22, -21, 6, -4, -2, 10, 15, -7, -15, -1, -5, -24, -25, 19, 8, -15, -7, -4, 2, -3, -10, -3, -1, 13, 7, 4, -12, -3, 4, 14, -21, -5, 19, 6, 19, 3, 1, -18, -34, -15, 2, -3, 22, -5, 10, 2, 14, 6, 2, 18, -14, -26, -11, -1, -9, -3, -8, 0, -11}
}
}
, {{{1, -15, -6, 0, -15, -14, 18, 4, 8, 3, -5, -3, 19, -13, -6, -15, -22, -8, 0, 12, 2, 2, 8, -25, 2, -29, -2, 1, -10, -7, 9, -1, -4, 15, 3, 20, -6, -11, -5, 5, 18, 0, 12, 18, -33, 21, 13, 5, -2, -1, 2, -8, -14, -18, -5, 5, -9, 11, 0, -11, -4, -19, 11, 10}
, {2, -17, -16, 20, 5, 8, 23, -5, -14, 11, 8, 3, 17, 14, -6, -4, -12, 5, -37, 30, 3, -10, -14, -9, 14, 5, -12, -14, -12, 5, 7, -4, -4, 11, -8, 18, -17, 7, -15, -17, 8, 18, -9, 1, -9, 14, -14, 18, -6, -5, 8, -11, -6, -12, -4, -14, 1, -22, -7, 5, -17, -6, 4, -20}
, {-22, -17, 14, -15, -30, 9, 8, -8, -15, 12, -14, -4, -1, 4, 10, 3, -23, -14, 22, -16, 0, -11, 8, -7, -13, -4, -6, 2, 12, 5, 12, -4, -4, 29, -7, 8, -25, 11, 1, 0, 21, 6, -3, 11, -32, -1, -1, -15, 26, 5, 10, 4, 9, -13, 1, 1, 36, -11, -3, 22, -1, -12, 4, 10}
}
, {{13, 13, -9, -7, 0, 1, -16, 17, -4, 6, 15, -12, -5, -11, -1, 4, -9, -4, 3, -6, -21, 4, -14, 3, -6, -12, -3, -1, 19, -2, -5, -13, 2, 7, -10, -22, -9, -2, -7, -10, -15, -5, 1, -3, 0, -4, -18, 28, -6, -9, -16, -23, -2, -1, 9, 7, -6, -8, -2, 33, -4, -4, -28, 9}
, {-24, -5, 8, -37, 13, -25, 15, 12, 12, -3, 28, 3, -12, -4, -20, -34, 7, 8, -5, -17, -39, -5, 1, 2, -2, 14, -29, -10, -46, -1, -9, -14, 35, -18, -10, -4, -4, -13, 11, 3, 2, -21, -19, -29, 15, -15, -17, -4, -12, 11, 21, -23, -2, 3, -2, -5, 40, 0, -16, -43, 2, -11, -14, -19}
, {-11, -10, 7, 22, -26, 37, 1, -15, -6, 12, 12, -22, -16, -3, 28, -18, 13, 20, -21, -12, -9, 8, 27, 5, -16, 10, -7, -7, -47, -4, 14, -42, -15, 11, -26, 8, 6, 6, 9, -6, -4, -16, 7, 11, -6, -12, 12, -21, -15, 24, 2, -36, -10, 9, -8, 21, -12, 4, 5, 9, -2, -28, -6, 20}
}
, {{-7, -2, 4, -5, 20, 0, -5, 25, -5, -4, -3, 7, 10, 0, -6, 3, 0, 8, 2, 9, 4, 6, 17, -1, -4, 5, -23, -10, 11, -19, -10, -21, 3, 1, 5, -8, 16, 4, 14, -10, -25, -6, -5, -17, -8, -20, -11, 0, 7, -3, -12, 0, 5, -6, -11, -9, -16, -12, 20, 12, -14, 9, 4, 5}
, {-15, -18, -4, 6, -10, 12, -10, 21, -2, -1, -3, -10, -5, -2, -11, -11, 14, -5, 0, 18, -22, 5, -20, 16, -35, 11, -4, 0, 16, -7, 0, 7, 11, -7, 25, 2, 12, 1, -10, 2, 0, -31, 1, -14, 3, -21, 0, 20, 31, -18, 5, -15, -3, -5, 12, -14, -12, -12, -18, 6, -17, -9, 2, 0}
, {9, 2, -28, 12, 6, -2, 6, -10, -15, 23, -4, 7, -7, -4, 13, -34, 5, -24, 16, -7, 9, 12, 20, 0, 14, 6, 11, -3, 10, 1, -16, 2, -32, 11, 0, -16, 9, -11, 9, 5, 14, 3, 15, 10, 6, 9, -6, -3, 17, 6, -6, 11, 6, -7, 7, 20, -21, -14, -17, -13, 4, 16, -7, 19}
}
}
, {{{4, -15, 0, 12, -7, 17, 0, -5, 1, 4, -1, 11, -20, -16, 7, -9, -19, -4, 12, 0, -8, 8, 11, 0, 0, -14, 7, 3, 11, -19, 9, -12, 2, 6, 2, -1, -5, -12, 2, 3, 13, 3, -30, 3, 4, -7, -5, 4, -6, 4, 7, 12, 10, 9, -7, -3, -5, -1, -5, 11, 6, -6, -4, 12}
, {7, -31, -3, 14, 11, 8, 2, -10, 9, 2, -4, 13, -13, 17, 1, -24, 0, 3, 34, 4, 10, -25, -6, 7, 14, 4, 14, 7, 6, -7, 2, 3, -5, -21, -1, 10, -11, 4, 0, 9, 21, -1, 16, 11, 6, 5, -11, -6, 5, 3, 10, 14, -15, 7, 8, -7, 16, 8, 0, -6, 4, -8, -6, 17}
, {1, -23, -2, 12, -10, -10, 22, -6, 0, 5, -25, -4, 13, 7, 9, 11, -5, -12, 9, 15, 2, -17, -10, 6, 16, -19, 16, -15, 9, 16, 1, 3, -9, 2, 3, -1, -6, 9, 4, 18, -3, 19, 6, -7, -29, 0, 11, 11, -4, 1, 4, 18, -14, -8, -8, -17, 3, 16, -16, 1, 14, -1, 22, 5}
}
, {{-1, 6, 14, -4, 4, -1, 7, 13, -6, 10, -15, 23, 0, 6, 7, -26, -13, 3, -11, 6, -12, -1, -6, -3, -6, 9, -14, 16, -1, -13, 6, 6, 3, -1, -18, 19, 9, -7, -18, 25, -14, 1, 2, 21, -20, 16, -6, 3, -11, -11, 20, 6, -13, -25, -18, 5, -11, -2, 14, -21, 13, -9, 5, -20}
, {-13, -13, -26, -9, 3, -25, 9, -18, 5, -6, -25, 15, 6, -16, 23, 11, 2, -11, 19, -1, 17, 8, -4, -11, -22, -27, 5, 21, -3, 8, -2, 1, -9, 18, -28, 21, 1, 16, -20, -5, -3, 2, -2, 5, 9, 14, -27, -17, 7, -14, -20, 2, -15, 2, 11, 32, 23, -6, 22, -26, 12, -6, 6, -24}
, {-18, 1, 10, -9, 0, 10, 15, -13, 17, 17, 12, 13, 0, 10, 30, 1, 6, 19, 19, 5, 11, 1, -13, 21, -2, 26, 12, -3, -5, 9, 16, 1, 10, -7, 7, 13, -14, -12, 21, -13, -18, -10, -8, -21, -15, -5, -4, -11, -16, -1, -4, 2, 16, 12, 14, 19, 14, 11, 21, -9, -17, 8, 2, 11}
}
, {{-17, 4, -9, -2, -4, -2, 17, 9, 4, -2, 7, 6, -5, -9, -18, 21, -18, 6, 35, 7, -2, -6, 20, 6, -17, 17, 3, -8, 13, 12, -10, -21, 19, -9, -7, -11, 6, 4, -10, -8, 19, 15, -2, -7, -10, -12, -9, -7, 2, -6, 14, 3, -9, -6, -40, 9, 23, -21, 12, 0, 8, 4, -3, 7}
, {25, 8, 4, -5, 19, 3, -6, -16, 12, 1, 3, -6, -2, -7, 37, 8, 28, -7, 23, -5, 10, -12, -9, -6, 1, 3, -15, 8, -19, -17, -5, 1, -5, -9, -1, 14, -2, -16, -39, -1, 3, -21, 0, -11, -11, 13, -12, 15, -11, -16, -11, 17, -20, -5, -12, 0, 13, 17, 15, -11, 20, -7, -11, 2}
, {2, -19, 6, -25, 4, 16, 10, -12, -7, 0, -11, 2, 26, 10, 22, -12, -4, 5, -15, -6, -4, -3, -11, -3, -12, 9, -5, -9, 21, -19, -5, 13, 15, -12, 8, -21, 1, -6, 18, 4, 12, -10, -7, -8, 0, 3, -15, -3, -1, -2, 11, 23, -11, -27, 10, -8, -6, 14, -21, 2, 5, -2, 13, -16}
}
}
, {{{12, 0, -8, -2, 1, 5, 11, -2, -5, 2, -6, -4, -8, -8, -8, 2, 11, 7, -5, 9, -5, -8, 4, -2, -5, -10, 2, 2, 19, 5, -3, 13, 4, -1, 7, 6, 8, 9, 1, 9, 4, 2, -11, -8, 2, -2, 6, 5, 7, -18, 9, -7, 0, 12, 2, 3, 8, 4, -12, 14, 0, 15, 2, -20}
, {-6, -6, -7, 3, 8, -2, 7, -8, -18, 2, -1, 3, 1, 1, 4, -9, -2, 8, -7, -2, 5, -13, -1, -11, -12, 8, -9, 2, 9, -20, 6, 10, 7, -2, -26, 0, -1, 7, 13, -12, 8, 12, 4, -17, 0, 7, 11, 8, -2, -18, 15, 11, -5, -1, 3, -3, 10, 6, -4, 0, 1, -7, -18, 2}
, {-4, -18, 5, 4, 9, 7, 15, 6, 8, 6, -4, -1, -11, -9, 4, -14, 12, 2, -3, -4, -23, -16, -5, 5, -19, -4, 2, 0, -14, 31, -6, -4, 10, 1, 4, 10, 12, 3, 6, 15, 15, -3, -5, -7, -11, -2, -2, 9, -2, 26, 20, -3, -4, 4, -2, 4, 5, 6, 9, -1, -3, -12, 10, -10}
}
, {{0, 1, -7, -15, 8, 15, 10, 2, -1, -2, -16, 4, -1, -6, 3, -19, 12, -6, -4, -3, -7, -8, 6, -2, -24, -1, 17, -12, 0, 1, 11, 2, -3, -4, 10, 5, 10, -9, 5, 0, 1, -3, -10, 8, 1, -8, -7, -10, 7, -4, 0, -18, -16, -2, 0, -14, 1, -4, 17, -1, -25, -4, 17, -2}
, {19, -6, -19, 22, -18, -7, -38, 30, 8, 29, 25, -7, -35, -17, -24, -3, -6, -10, -7, -4, -4, -1, -21, -10, -44, 7, 44, 11, 23, 1, -14, -4, -3, 9, 9, 8, -8, 5, -21, 11, 35, -26, 18, -28, -7, -32, 4, -4, -10, -39, -5, 10, -12, 0, 1, -2, -23, -6, -7, 11, -7, -27, 7, 21}
, {10, -18, 10, 14, -11, -14, 1, -14, -7, 12, -11, -6, -14, 13, -11, 2, 2, -27, -4, -11, 15, 1, -7, 2, 7, 5, -9, -3, 1, -2, -18, -8, 3, -32, -8, -3, -14, -2, -14, -15, 5, 1, -7, -22, 1, 2, 5, -11, 6, -3, 1, 21, 12, -6, 6, 2, -7, -8, -5, -15, -4, -6, 1, -7}
}
, {{17, 4, 6, 9, 1, -4, 6, 3, -6, 11, 9, 0, 7, -11, 1, 6, 16, 15, -13, -14, -9, -11, 0, 2, -2, 4, -9, -23, 5, 12, 19, 12, -8, 4, -25, -6, -5, -1, 6, -11, 5, 5, -10, -4, -5, -15, -7, 16, 0, -5, 4, 8, -2, 4, 2, 9, -8, 15, 9, 0, -12, -2, 10, 3}
, {-16, 16, 9, -3, 4, -8, 4, 12, -1, 3, -11, 2, 17, -33, -8, -4, 0, 10, -19, -11, -13, 4, -18, -6, 13, -12, 9, -31, -4, 2, 10, 0, -10, 20, -2, -13, -15, 17, -34, 26, 0, 13, -6, -11, -7, -3, -10, -7, -6, 20, -11, -8, -2, 6, -5, 5, 4, -16, 14, 15, -10, 14, -15, -6}
, {29, -6, -17, -6, -19, 11, -11, 5, -21, -16, -37, -4, -10, -1, 11, 5, -2, 13, -6, 4, 14, -18, 1, -2, 17, -1, 7, 4, 5, -13, -15, -6, 15, -5, 1, 8, 15, 0, 2, 2, 16, -3, 17, 19, 2, -7, 14, -8, -7, -12, -6, -1, -7, -17, -15, -8, -21, -7, 1, 3, 2, 12, -3, 16}
}
}
, {{{7, -12, 2, 0, -8, -10, -7, -19, 0, -12, 0, -12, 13, 7, -10, -2, -14, -7, 12, 16, -3, -2, -2, -4, -14, 16, 11, 13, 4, 3, 3, 9, 1, -4, -14, -2, -2, -5, -8, 14, 13, -13, 21, 19, 5, -11, 0, -5, 10, -5, -1, 11, -4, -8, -9, 0, -11, -17, -5, 2, -6, 2, 16, 1}
, {-10, 19, 0, -9, -10, -16, -13, 10, 5, -2, -15, -1, 6, -14, -7, 2, -19, -5, 17, -3, -14, -14, -2, -8, 6, 1, 0, -9, 4, 11, 15, 10, -4, 4, 5, -2, -7, 26, 11, 4, -5, -5, -18, -7, -5, -9, 0, -10, 0, -15, 6, 19, -7, 8, -8, 10, 1, -2, -14, -9, -3, 1, -4, -1}
, {-2, -20, -5, -2, -1, 0, -1, 6, 10, 0, -3, 1, -3, 3, 7, 4, 0, 12, 16, -7, -2, -2, 10, -11, -25, 10, -2, -10, 1, 0, 18, 1, -12, 0, 3, -7, -12, -5, 4, -15, -7, -2, 2, -15, -10, -14, -11, -9, -6, -16, 14, 25, 6, 3, -17, -1, 1, 14, -7, -24, -16, -10, -14, -3}
}
, {{-4, 2, 3, 4, 7, 7, 19, 5, -2, 21, -16, -10, -4, -8, 5, -13, -8, -11, 4, -1, -14, -5, -11, -9, -11, -7, 4, -13, -17, -20, -19, -2, 10, -11, 0, 1, 8, -1, -21, 24, 10, 28, -7, -3, -3, 27, -7, -3, -11, 8, -7, -11, -9, -4, -3, 0, -18, -5, -11, 10, -18, 4, -7, 9}
, {-2, -16, 4, -11, -12, 3, 7, -52, -51, -8, -14, 30, 3, -5, -2, 13, -3, -12, -2, -48, 13, 4, 0, 1, 14, 19, 20, -9, -19, 24, -18, -17, -1, -5, -3, 13, -12, -4, 35, -19, -15, 8, 6, -18, 1, 20, 11, -10, 3, -3, -22, -29, -16, -1, -1, 21, -13, -8, -5, -23, -1, 29, -9, -20}
, {-20, -15, -8, 0, -16, 1, 16, -8, -14, -6, -14, 6, 0, 8, 18, -5, 6, -7, -13, 4, 15, -16, 12, -29, 6, -1, -4, -4, 11, 12, 39, -23, -3, -3, 10, 17, 1, 1, 17, 8, 16, 17, -30, 9, 4, 18, 3, 15, -5, 10, 11, 6, -4, 3, -5, -21, -2, -6, -15, 9, -30, 2, -18, -13}
}
, {{2, 3, 13, -11, 11, 5, 0, 5, 4, 1, -1, 5, -14, -14, -10, -11, 8, -13, 0, -1, -1, -18, 4, 13, -2, -7, -21, 5, 3, -3, -3, 2, -2, -11, 9, 3, 9, 6, -8, -7, -9, -3, 14, 0, 4, -12, 8, 0, 2, -6, 4, 3, 3, 0, 0, -3, 13, -2, 11, 5, -5, 10, -2, 8}
, {-9, -30, -18, -9, 1, -4, -32, -10, -9, -17, -14, 12, -19, -12, 3, 5, -2, -6, -6, -12, -26, -13, 16, 11, -16, -4, -7, 8, 1, 24, -19, -10, 0, -10, -2, -14, -9, 20, 0, -9, 14, -16, -43, 13, -10, -31, 17, 23, 19, 5, -11, 6, -8, 7, 10, -12, -6, -11, 12, -15, 12, 2, 17, 3}
, {-16, -11, -18, 0, -21, -4, -1, 2, -19, 0, -2, -25, -22, 0, 0, -2, 8, -3, -14, -17, -1, 0, -4, 6, 4, 1, 0, -12, 19, -8, 14, -20, 2, -14, -12, 5, 34, -14, -1, -7, 11, -9, -1, -1, 0, 0, 7, -5, -3, -14, -8, 1, -10, 3, 11, -15, -12, -2, -15, -5, -15, -16, -5, 13}
}
}
, {{{17, 0, -5, -19, -10, 14, 9, 13, -13, -4, 12, -2, -2, 3, -19, -10, -6, -11, 16, 11, -18, 0, 0, -5, -4, -24, 4, -17, 6, -6, -10, -3, 17, 7, -19, 2, -12, -3, -7, 5, 29, -2, 11, -12, -9, 11, -16, 9, -14, -13, -11, -14, 10, 9, 4, -3, 8, -10, -4, -15, 11, -7, 5, 17}
, {-6, -3, -1, 7, 1, -13, -16, -3, -5, 2, -7, -7, -1, -17, 3, 16, -3, -6, 0, -17, -14, -15, -8, 3, -1, 1, -2, -9, 13, 1, 6, -15, 0, -1, -27, -2, -5, 15, 6, -9, 11, -7, -1, 0, -9, -13, -6, 11, -3, -15, 0, 1, -19, -5, -4, 5, -1, -9, -7, 7, -6, -11, -4, -3}
, {6, 21, 4, -4, -21, -3, -7, 5, -26, -6, 12, -2, 4, 9, -13, -1, 11, -13, 0, -10, -3, -2, 2, 6, -3, -4, 7, -7, 2, -3, 6, -9, -2, -7, -12, 14, -2, -6, 9, 8, -2, 1, 11, -19, -23, -6, -6, -11, -6, -9, 7, -3, 14, -1, 12, -3, -3, 8, 3, 2, -15, 4, -8, 8}
}
, {{-13, 5, 2, -3, -11, -10, 13, 25, 8, -9, 10, 2, -3, -19, -10, 3, -18, 1, 6, 11, -16, -33, 7, 2, 4, 8, 3, -32, 3, -2, -3, -4, 3, 14, 2, 3, 1, -5, 3, 7, -11, 2, 5, -14, -4, -7, -5, 23, -2, -9, 1, -7, 11, -3, -4, 4, 12, -2, -7, -2, -17, -1, -8, 1}
, {12, -1, 19, -8, -5, 22, -2, -5, -20, -32, -1, -12, -2, -10, 13, -13, -15, 6, 6, -17, 22, -3, 26, 0, -1, 2, -20, -27, -3, -2, 8, -38, 6, -2, -6, -2, -2, 12, 3, 12, 5, -5, -27, -6, -34, 12, 28, 2, -5, 6, 6, -32, -4, 0, -8, 9, 3, 9, -26, -18, 7, 4, -5, -5}
, {-16, -15, -10, -1, -15, 15, 18, -5, 4, 12, 5, -10, -14, 10, 27, -19, -10, -2, -7, 15, 6, -16, 13, 19, -9, 1, 7, -7, -4, -2, -4, -1, -7, 12, -13, 16, 6, -2, -25, 7, 14, -10, 8, -19, -27, 4, -4, 0, 12, -5, 2, -23, 3, -4, 2, -26, -19, -10, -5, 10, -7, -9, -5, 16}
}
, {{0, 13, -6, 2, -15, 1, 2, -9, -5, 0, 18, 4, -4, -6, 3, -10, -5, -7, 7, 3, -3, -15, 8, -1, -32, 19, -21, -1, -10, -5, 15, -18, 2, 0, 7, -7, -9, 3, 7, -22, -11, -2, 12, -15, -18, 6, 8, -14, 0, -15, -2, -6, 4, -11, -17, -3, -3, 0, 4, 7, -10, -17, -9, 5}
, {2, 3, -28, -4, -19, -7, -26, 10, -19, -7, -5, -9, -10, 7, -10, -3, -2, -23, 2, 16, -11, -11, 20, -11, -11, -11, -11, 11, 0, -16, 5, 6, 10, 12, -9, 8, 9, 18, 9, 2, 5, -14, 10, 5, -15, -2, 8, 18, 16, -29, 13, -3, -38, -23, -17, -6, -8, -4, -16, -2, 6, 8, 12, -2}
, {6, 9, -19, 11, -1, -17, 6, 2, 0, 19, 10, -6, -17, 20, 1, -8, -11, 13, 5, 2, -3, -4, 16, -7, -2, 3, 7, -1, 7, -24, 1, -35, 11, 2, 3, 23, 13, -10, -14, -24, 15, 15, -8, 2, 6, -2, -18, 9, -4, -16, 11, 14, 2, -12, 11, 6, -12, 6, 1, 7, -16, -3, 9, -8}
}
}
, {{{0, 5, 18, 6, -13, 6, -15, -5, 7, 5, 6, -4, -2, -9, -14, -5, 11, 8, 7, -9, 15, -2, -13, -13, 20, -9, -12, 7, -6, 20, -2, 2, -7, 2, -22, 1, -13, 16, 2, -2, -4, 8, 10, -1, -3, 0, -10, 5, -21, 6, 17, 16, 6, 10, -4, 2, -4, 14, -6, -10, -5, 9, 4, -11}
, {9, -19, 11, 9, -10, -1, 0, 4, 4, 1, -3, 15, 7, 1, 4, -12, -5, 1, 12, -3, 10, -21, -12, -17, -19, 6, 4, 4, -1, 14, -10, 7, 8, -14, 3, -2, -7, 4, -1, -8, -1, -15, -19, 0, 5, 4, 0, 8, -10, 9, 4, 6, 5, 1, -2, -9, -11, -1, -9, 4, 2, 2, -7, -2}
, {4, 0, 0, -4, 1, 3, -6, -8, 2, 4, -3, 19, -10, 14, 0, -24, 0, -25, -3, 1, 2, -15, -5, -23, -1, 6, -10, -6, 0, -21, 4, -7, 5, 1, 1, -4, 2, 16, -1, 5, -12, 2, 10, -24, -17, -9, 15, -6, 6, 7, -14, 19, -7, 7, 0, -8, -5, 9, -10, 1, -14, 8, 9, 13}
}
, {{2, -9, 3, -1, -9, -26, -7, 5, 4, -7, -4, 7, 11, -4, 4, 0, 15, -17, 1, -16, 7, 18, -3, 0, 10, -4, -17, 0, 12, -7, 1, -8, 7, -6, -11, 1, -4, -3, 15, 12, 0, -16, 2, -7, -6, 32, 2, 6, -7, 2, -8, 6, 11, 3, -11, -13, 5, 18, -14, 3, 14, 25, 0, -5}
, {-40, 8, -10, 24, 5, 12, 14, 26, 24, -7, 24, -16, 10, -6, 18, 4, 12, 28, -18, 15, -43, -21, -8, 18, -17, 19, -13, -6, -34, -22, 27, 14, -8, 22, -11, 2, -19, -16, -19, 8, 4, -1, 6, 7, 12, -34, -42, 6, -14, 11, -7, -21, 4, 12, 15, -2, -24, -4, -17, 15, -24, -28, 1, 14}
, {-8, -3, -19, 11, -10, 14, -1, -9, -17, -15, 17, 18, 6, -6, 1, 7, -6, 8, -9, -2, -22, -14, -29, 8, -10, -19, 15, -11, 2, -12, -7, 0, -13, 23, 9, -15, -4, -1, -1, -6, -1, -9, 5, 7, 5, 2, -5, -6, 5, -19, -20, 3, -1, 2, -12, 8, 1, 3, 0, -8, -10, -11, -7, 1}
}
, {{6, -8, 7, 1, 9, -5, -2, 2, 16, -3, -7, -2, 5, -9, 9, 4, -19, -13, -19, -6, -10, 5, -9, -19, 11, -12, 16, 26, -11, 0, -21, -13, -10, -11, 14, 11, -5, 0, -7, -3, -14, -4, -17, 19, 2, 0, 2, -9, -3, -19, 2, 0, -15, -10, -5, 17, -4, 9, -12, -10, 6, 2, 16, -8}
, {-13, -21, 5, -5, 4, -2, 2, 11, -1, 13, 6, -8, -4, 14, 2, -13, 17, 1, 6, 19, -21, -12, 23, -11, -1, 14, 9, -1, -10, -7, 7, 19, 10, -15, -4, 10, 17, -25, 32, -3, -21, -8, 6, 6, -8, 16, -19, -9, 5, -21, 2, -4, 11, -23, -17, -8, 13, 17, 10, -6, -4, 3, 3, 1}
, {1, -17, -17, -20, -7, -16, -3, -3, 9, 8, 2, 12, 6, -5, -4, 11, 13, -11, 17, -1, -8, 9, -18, 10, -5, -15, 15, 33, -4, 14, -9, 5, -1, 1, 6, -14, -24, 1, 1, -5, 18, -16, 11, 3, 20, -13, 5, -12, -5, 3, -11, -9, -3, 16, 20, 17, -9, 2, -9, -4, 4, -23, 14, -18}
}
}
, {{{12, -6, -1, -5, -21, -5, -7, 4, 6, 1, 3, 18, -6, -13, -18, -9, -10, -14, 6, 7, 7, 2, 18, -12, 0, -13, -3, -11, 8, 10, 17, -18, -11, -5, -7, -13, -8, -15, 6, -5, 4, 21, -2, -31, -20, 6, 5, 7, 3, -12, 4, 11, 4, 7, -7, -14, -2, 4, -5, 5, -13, 4, 8, 4}
, {-2, -3, 5, -5, -12, 2, 0, 3, -14, 6, -4, 7, 6, -11, 3, -35, 8, 3, 3, 4, 13, -26, 10, 1, -8, 9, -18, -11, -1, 7, -3, -4, 6, 1, 3, -4, -2, 7, 6, -18, 0, -9, -3, 14, 2, 1, -5, -11, 4, -4, 8, 6, 3, -1, 14, -7, 17, 1, -23, -17, -14, -8, -3, -10}
, {-13, -7, 2, 1, -30, 8, 15, -1, 10, 7, -24, -1, 0, -3, 2, -18, 5, -7, 5, 0, 8, -13, -8, 3, -19, -6, -4, -12, -4, 0, 11, -24, 3, -11, -7, 0, 4, -1, 3, 16, 2, 7, -14, -4, 3, 4, 2, 14, -16, -8, 14, 3, 8, -1, -14, -6, 11, 6, -1, 10, 5, -14, -8, 11}
}
, {{0, 1, -10, 2, -8, 18, -2, 10, -7, 18, -1, 0, -8, 14, -3, -2, -4, 11, 6, -26, 7, 3, 16, -18, -2, -11, 1, -7, 6, -5, -3, -14, 12, 2, 8, 5, -7, 6, 11, -6, 5, 4, 6, -8, -16, 0, -13, -1, -4, -2, 10, 6, 14, -20, -2, -9, -10, 2, -4, 16, -7, 13, -4, -7}
, {-54, -25, -11, -10, 18, -22, 7, 36, 31, 15, 25, -7, -32, -20, -4, -19, 21, 4, -11, 6, -50, 10, 7, 7, -11, 4, 17, -18, -17, -20, 31, 5, -14, 3, 18, 20, 2, -38, -11, -23, 18, -2, 0, -12, 20, -30, -68, 8, 7, -36, -11, -3, -3, 24, 14, -6, 6, 8, -14, -21, -29, -24, -23, -13}
, {10, -7, -5, -17, -15, 22, -5, 7, -19, -7, -17, -1, 0, 12, -9, -2, 12, 9, 9, 13, 10, -4, -6, 11, 8, 27, -16, 7, -5, 23, 12, -15, -2, -14, 12, -9, -9, 2, -9, -5, -10, -2, -10, 5, -12, 7, 13, -20, 5, 10, 4, 13, 16, 4, -1, -3, 2, -8, 15, -7, 10, 0, 2, -16}
}
, {{11, -1, 10, -4, 8, -10, 13, 17, 1, -4, -1, -1, -7, -10, -10, 5, -8, 0, -12, 15, 3, -4, 9, -12, -14, -4, -10, 9, 3, -1, 15, 4, 10, -1, -2, -17, 21, -7, -11, 3, 8, 2, -3, -7, -5, 4, 4, 20, 2, 5, -9, 8, -2, -15, -4, -27, -9, -23, 5, -11, 8, -1, -8, -2}
, {-18, 8, -2, -25, 10, -28, 0, 7, -12, 0, 6, 15, -11, 2, 3, -21, 16, -27, 5, -17, -5, 5, 14, -12, -6, 3, -7, -3, -15, -23, 2, -12, 48, -2, 15, -12, 22, -6, 5, 1, -15, -15, 1, -19, 3, -24, -13, -19, 25, 2, 17, 3, 1, -15, -8, -5, 3, -4, -13, -1, -11, -9, -9, -27}
, {-3, -22, -12, -2, 3, -2, 10, -9, 4, 19, 4, -3, -7, 4, -6, -6, 13, -1, -10, 0, 5, -20, 7, 18, -14, 16, -3, -2, 8, -4, -18, -14, 3, -10, 5, -15, -13, 3, 2, -7, -3, -7, -9, -3, 4, -12, 16, -10, 18, 8, 14, 27, -10, 10, 21, 5, 0, 6, -20, -10, 4, -9, -5, -20}
}
}
, {{{-12, 4, -9, 4, 6, -10, 6, -3, 9, -7, -8, -5, -2, -19, -3, -2, -5, -7, 5, -28, 4, 0, -3, 20, 2, 7, 4, -21, 0, -3, -4, 5, -2, -6, 0, 4, -14, -4, -11, 5, -11, 3, -7, -3, 2, -11, -17, 0, 6, 9, -4, 0, -16, -2, -20, -14, -3, -6, 7, -20, -19, 13, -8, -5}
, {-6, 12, 9, 0, 3, 4, -2, -13, -17, -1, 7, 12, -3, 3, 10, -7, 1, -3, 14, -6, -15, -16, -3, 18, -4, -8, 9, 1, 0, -5, -20, 5, -24, -26, -7, 19, -11, 0, -4, -7, -14, 10, -11, -14, -4, 10, -1, -2, -1, 5, -10, 11, 1, 24, 2, 3, -9, 0, -19, 6, -13, 10, 2, 7}
, {-1, -12, -10, -12, -8, -8, -6, 4, -8, -16, 12, 5, -34, -3, 10, 4, 4, -4, 7, -19, -12, 5, 3, 12, -9, -1, 12, 10, 9, -12, -6, -6, -12, 0, 0, 12, 10, 2, -18, -18, -11, -15, -10, 2, 1, -20, -12, -5, 7, -19, -10, 14, -1, 3, 4, -5, 0, 6, 1, -13, -13, -10, 4, -16}
}
, {{6, 8, -15, -4, -6, 11, 7, -3, -2, 3, -13, -2, 12, -8, 0, -34, -1, 14, -24, -2, 3, -5, -10, 4, -9, -3, 8, -11, 2, -18, -19, 2, -5, -16, 7, 13, 11, 10, -19, 8, 6, -3, 0, -14, -4, 26, 5, -21, 6, 1, -7, -1, -9, 8, -15, -3, -26, -13, -19, 5, -18, 17, 2, 0}
, {-1, -16, -23, -14, 10, -10, 4, -55, -42, -14, -32, 51, 10, -4, 8, -7, 15, 6, -1, -29, -11, -14, -18, -13, 10, 9, 2, -20, -14, 16, -35, -5, 4, 22, -2, -18, -10, 4, 28, -39, -18, -14, -2, -8, -4, 20, -1, -11, 13, -12, -29, -20, -15, -3, 19, 22, 7, 6, 8, -7, 16, 24, 6, -16}
, {-1, -11, -5, -5, -6, 15, 3, -3, -1, -7, -13, 8, 5, -8, -7, -17, 15, -13, -21, 1, -15, -44, -9, -22, -19, 6, 1, 6, 2, 24, 27, -2, 20, -19, -4, 22, -3, 7, -8, 3, 23, -9, -37, -4, -3, -11, 8, 4, -6, 18, 3, -20, 4, 5, 0, -6, 0, 5, 11, 2, -8, -6, -11, 2}
}
, {{-12, 4, -1, -18, -5, -6, -5, -8, -12, 14, 5, 0, -19, -7, -15, -13, 17, 4, -4, -16, 2, -20, -9, 5, 14, -7, 9, -13, -3, -9, -3, 6, -4, -6, 4, -4, -1, -3, 4, -3, -7, -3, -3, -11, 22, -14, -18, 0, -4, 11, -1, 1, -8, -2, 6, -10, 9, -15, 0, -12, -8, 7, -5, -12}
, {2, -24, 4, -26, 3, -5, -32, -3, -3, -16, -9, -1, -34, 3, 37, -12, 2, -6, -1, -19, -14, -10, -1, 5, -23, 8, 18, 9, -23, 24, -1, -2, -20, -18, 0, -14, -10, 20, 0, -15, 5, 4, -40, -8, -14, 4, 15, 2, 16, 16, -16, -8, 2, 11, 12, -5, 5, 14, 11, -7, 6, -2, 12, 5}
, {-19, -18, -17, -11, -6, 8, -6, 4, 1, 2, -14, -25, -18, 0, 1, -4, 1, 17, -11, 8, -6, 2, 2, 13, -18, 22, -15, -9, 5, -6, -1, -12, -4, -23, -6, 24, 8, 15, 5, -13, 3, -3, -10, -17, -5, -8, 15, -4, -1, 2, 12, 10, -34, 1, 17, -21, -7, -9, -1, -23, -41, -12, 17, 7}
}
}
, {{{11, -15, 2, 8, -4, 13, -7, -4, -12, -7, 3, 7, -4, -6, 10, 5, -1, -11, -19, -1, -5, -3, 5, -4, -15, 22, 1, -17, 17, 21, 12, 10, -22, -3, -14, -12, 13, -18, 8, 1, -13, -8, -1, -3, -3, 4, -3, -10, -3, -23, -6, 4, -1, -5, 1, 5, -8, 0, 5, 6, 2, 0, 5, -5}
, {-6, 8, -7, -1, 5, 1, -9, -8, 0, -20, 7, 17, 2, -8, 21, -20, 4, -5, 5, -31, -7, -11, -16, -21, -4, 4, -8, -16, 4, 5, 3, -3, -6, -4, -11, -14, -16, -9, -6, 13, 2, 5, -7, 1, -15, 3, 6, -11, -15, 4, 4, 1, -11, -10, -8, 6, -16, 10, -17, -1, -2, 25, -14, 2}
, {0, -15, -2, -14, 11, 3, -12, -1, 13, -7, 2, 0, -26, -16, -13, 6, -12, 7, 6, -7, -1, -4, 10, 0, -14, 3, -6, -9, 11, -22, -9, 3, -5, -4, -18, -3, 2, -3, -7, -9, -1, -9, -9, -12, -18, -15, -5, 2, -13, 26, -10, 4, 0, -11, 13, -6, -2, 18, -7, 2, -20, 7, -10, -15}
}
, {{5, -4, -14, 11, -2, 5, 0, -1, 12, 10, 10, -20, 3, -3, -17, -1, 8, 12, 16, -8, 5, -10, 3, 14, 4, -18, 5, 5, 12, -11, 4, -3, -24, 1, -11, 5, 18, 15, -13, -14, 23, -16, 12, -2, -4, -11, 0, 10, -11, -25, -4, 10, -23, 5, 2, 11, 27, 24, -5, -1, -14, 10, 5, 6}
, {-10, 3, 27, 6, 12, 18, 9, -6, -10, -19, 0, 2, 23, 11, -14, -27, -9, 23, 8, 16, -13, -27, 1, -2, 7, 17, -8, -6, -14, -4, 16, 22, -1, -9, 8, -7, -9, -5, 25, -38, -9, -1, 2, -7, 14, 3, 34, 0, 4, -3, 16, -37, -34, 6, 19, -24, 2, 16, -3, -33, 17, -8, -5, -5}
, {-6, -16, -30, -1, -8, -13, 6, -3, 3, 12, -22, -7, -19, -9, 7, -13, 5, -19, 11, 0, 11, -13, 1, -18, -1, -5, 2, 10, 8, -2, -1, -8, -11, -10, 13, 7, 5, 7, 8, 10, 5, 6, 2, 0, -4, -11, 0, 13, 2, -13, 11, 22, -5, -1, 21, -5, 8, 14, -17, 4, -13, -9, 11, -9}
}
, {{1, -3, 16, -1, -5, 8, -7, -17, 2, -1, -8, 4, -3, -12, -7, -2, 9, 16, -1, -7, 3, 0, 2, 4, 12, 10, 11, -7, 6, 8, 5, 24, 3, 7, -3, -16, -3, -6, 10, 3, 7, 6, 3, -2, 7, -6, -11, -3, 15, -5, -9, 17, -2, -7, 10, 12, -5, 13, -11, -5, -3, -8, 2, 4}
, {-37, -16, -9, -9, -15, -13, 6, 15, -2, -12, -8, 1, 5, -21, 9, -6, -4, -5, 9, 3, -1, 8, -8, 0, 6, 0, 10, 2, -16, 17, -7, 0, -7, 4, 21, 2, -2, 9, 27, 0, 12, -5, 2, -11, 11, 3, 11, 3, -13, 0, -19, -3, -10, 8, -6, -11, -3, -16, -19, 0, -11, 2, 14, -12}
, {-2, -4, 4, 17, -9, 6, 6, 9, -19, -8, 2, -6, 10, -9, 8, -6, -11, 8, 13, -4, 10, -4, -18, -5, 12, 19, 8, -10, -1, 3, 27, -16, -18, -2, 14, 7, 5, -1, -7, -10, -1, -12, 2, -12, -7, 4, 5, -5, -12, -6, -12, 3, 3, -13, 6, 9, 7, 0, -6, -9, -14, -3, -8, 29}
}
}
, {{{-4, 10, 4, -1, -8, 10, -5, -3, 3, -11, -5, -4, -2, -4, 8, 0, 1, 3, 5, 1, -1, -8, -16, 14, 3, 10, -11, -3, 1, -7, -1, 3, 9, -1, 0, 3, 6, -6, 1, -4, 2, -19, -6, 11, -5, -17, 5, -12, 6, 9, -4, -22, -15, 6, 7, 3, 0, 7, -7, -8, 3, 1, -3, 6}
, {-8, 9, -7, 8, 7, 2, 4, 5, 3, 3, 7, 2, 8, -5, -3, -15, 4, 18, -21, 2, -13, -26, 10, 20, -2, 8, 7, -18, 14, 6, -3, 3, 3, -9, -2, 4, -7, 9, -16, -1, 1, 9, 4, -9, -4, 6, 2, -3, 8, 12, -9, -5, 2, 9, 26, -9, -3, -6, -18, 6, -1, 7, 7, 12}
, {-5, 5, 8, 7, -9, -2, 12, 4, 9, 1, 8, 4, 12, -28, 20, -5, 17, -7, -2, -9, 12, 6, 3, -3, -3, 7, 11, -10, 7, -8, 4, 4, 0, -7, -1, -3, -16, 11, -5, -10, 0, -3, 10, -10, -8, -7, -1, 13, 1, 3, 12, 4, 4, 9, 12, -3, 4, -15, -19, 3, -6, -2, -14, 9}
}
, {{3, -20, -1, 2, 5, 6, -4, 5, 17, -1, -2, -6, 2, -12, 12, -14, 4, 16, -3, 0, -3, -22, 8, 2, 2, 16, 11, -5, -11, 7, -23, 7, 17, -13, 7, 5, 9, -8, 10, -9, -4, -16, -7, 4, -10, 6, -44, -28, 4, 10, 5, -34, -3, -4, -3, 11, -11, -14, -3, -16, -7, -2, -3, 7}
, {10, -29, -38, -16, 3, 2, -19, -29, -22, -6, -5, -9, -16, -11, 15, -2, 19, -7, 10, 0, -8, 8, 33, -19, -25, 13, 3, -14, 15, -3, -19, 17, -2, -3, 6, -19, 5, -14, 3, -24, 36, -29, -9, -4, -45, 1, 6, 12, 12, -11, 6, 32, -22, 8, -6, -16, -18, 20, -31, -25, 5, 16, -7, 36}
, {-27, -5, 12, -30, -6, 2, 9, 13, 10, -13, -5, -6, 17, -19, -4, -4, 17, -16, 7, -3, -3, -16, -4, 0, 5, 12, 6, -3, -16, 4, 0, -4, 21, -2, 2, 1, 0, -1, -2, 1, -1, 6, -3, -18, 10, -13, 5, -7, 7, -10, 14, 0, 6, 1, 10, 1, 11, -4, -16, 6, 10, -15, -3, -27}
}
, {{15, -6, -9, -10, -4, 6, -8, -4, 2, -13, 9, 4, -12, 4, 11, 17, 5, -8, 9, 9, 3, 6, -19, -6, -18, -5, -12, 22, -9, -9, 17, 12, 4, 25, 0, -7, -16, -20, -6, -15, 5, -3, -3, 3, 0, 9, -11, 23, -3, 0, -9, 31, 0, 13, 4, -12, -14, -3, -5, 4, 8, -7, 1, 10}
, {-13, 11, 8, 6, -14, -11, 7, 15, 15, -3, 3, 2, 21, -8, -11, -9, 0, 23, -32, -20, 0, -22, -15, 0, 4, 1, 9, 2, -2, -6, 5, 9, -3, 11, -26, 15, 2, -23, -5, 8, 3, 9, 11, -15, -3, -16, -31, -11, 4, -1, -13, -6, -23, 4, 8, 13, -18, 4, -7, 11, -21, -21, -10, 17}
, {9, 3, -8, 7, -1, -5, -17, -21, -7, -4, 2, 3, -7, -1, 10, 15, -1, 0, 10, 3, -10, -17, -14, 2, -23, -1, 20, -12, 14, 4, 4, 13, 7, 9, -1, 4, -22, 24, 7, 11, -6, 4, 12, -7, -13, -16, 2, -6, -5, 8, 2, 5, 20, -5, -5, 8, -11, 3, -6, -15, 21, -6, -15, -8}
}
}
, {{{-3, 0, 1, -7, 20, 2, 16, 1, 14, -7, -26, 0, -3, 0, -4, -16, 8, -1, 5, -18, -11, -17, -35, 14, -9, 9, 21, -14, 23, 0, 14, 3, -2, 2, -2, 0, -3, 7, -9, 23, 17, -9, 12, 7, 4, -12, 6, -6, 7, -9, 3, -14, 5, 8, 11, -17, 0, 10, -2, 3, 13, -5, -2, 1}
, {-4, 5, -21, -13, -14, -6, -10, -16, -9, -4, -32, 6, -14, -17, 0, 16, 16, -13, 17, 0, 15, -8, -14, -17, -3, -10, 4, -1, 10, -3, -11, -4, -6, -2, -13, 1, 19, -6, -3, -28, 6, -8, -3, 1, 24, -7, 13, 13, 35, -12, 5, 8, -5, -14, -5, -13, 2, 6, -15, -14, 5, -8, 8, -3}
, {-24, 21, -15, 0, -19, -4, 7, 3, 1, -12, -25, 5, -5, -21, 11, 13, 22, -3, 2, 14, -6, -17, -8, 9, 1, 2, -4, -1, -20, 35, -12, -2, -2, -7, -24, 2, 0, -11, 1, -4, -11, -22, -10, -10, -6, -16, -18, 2, -2, 9, 2, 2, -7, 15, 5, 7, -4, -7, 7, -10, 1, -19, 19, -14}
}
, {{-5, -14, -15, -1, -13, 10, 6, 6, -21, 2, -14, 8, 7, 7, -3, -13, -9, 5, -12, 4, -8, 5, -7, -8, 14, 17, 8, -14, 9, 11, 2, 20, 7, 5, 14, -10, -18, 1, 1, -8, 8, -14, -18, -6, 3, 11, -9, 11, -7, 5, 7, -15, 6, 4, 15, -34, -19, -5, -5, -2, -6, 1, 29, -13}
, {-12, -13, -21, 1, -7, 0, 3, 13, 1, 33, -13, -1, -9, 8, -19, -4, -2, 2, -23, 5, -18, 8, -15, 4, -37, -5, 25, 9, -2, 10, -13, -11, 5, -19, -1, -22, -1, -2, -2, -9, 28, 8, -16, 13, -6, 11, 9, -12, 1, -5, -2, 12, -28, -14, -16, -4, -17, 3, -5, 12, -9, -27, 20, 10}
, {10, -25, 0, -11, -4, -17, 6, 0, 3, -8, -13, -8, -7, -9, 14, -3, -7, -5, -6, 2, 1, 18, -9, -33, -26, -15, 22, 5, 21, 11, -16, 3, -3, 4, 4, 0, -1, 10, -3, 14, 16, -21, 10, -2, -31, 5, -12, 11, -4, -11, 4, 21, -1, -8, -8, 15, -21, -2, -26, 2, -11, -12, 12, 13}
}
, {{10, -33, -12, 11, -2, 1, 19, 0, -16, -4, -4, -7, 10, 16, 7, -11, -3, 11, 10, 6, -7, -21, -2, -7, 5, 0, 14, -3, 10, 12, 10, 5, 3, -11, 25, 3, -13, -19, -11, -1, 0, -2, -7, 9, -5, -3, 5, -13, -15, -12, 14, -10, -10, 7, -9, -20, -15, -2, 11, -23, -6, 0, -2, 7}
, {-20, 0, -18, -10, 2, -9, 7, -4, -23, -6, 3, 2, 15, 12, 3, 30, 6, 8, -11, -12, -6, -4, 2, -7, 7, -7, -7, -9, -23, 27, -8, -14, -34, 20, 11, -11, -9, 9, 10, 2, -42, -2, 5, -5, -2, 0, -27, 1, 10, 8, 11, 3, -6, 8, -3, -2, 1, -10, 4, -11, -8, 17, -14, -15}
, {18, -6, -2, 2, -1, -15, -21, -7, -9, -15, -13, 26, -4, -6, 7, -7, 3, 3, -13, 8, -9, -30, -5, -5, -8, -9, -4, 14, 2, -9, 5, -13, 14, 6, -6, -24, -24, 4, 2, 20, 21, 5, -3, -3, -15, 5, -8, 6, 13, -11, 0, 0, -3, 4, 8, 5, -18, -2, -14, 4, -10, -1, -4, -9}
}
}
, {{{5, -19, -9, -3, -23, 3, -20, -7, -2, 7, -8, 4, -2, 11, -3, 3, -18, 12, 7, 4, 10, 4, -6, 0, -4, -4, 22, 4, 2, 5, -4, -14, 6, 16, -20, -10, 8, 3, -9, 0, 6, 4, 3, 1, 12, -4, -7, 10, -5, 5, -11, 8, -7, 8, -3, 15, -5, 15, 3, 8, 8, -3, 5, -1}
, {34, -3, -15, -15, -21, -4, -12, -14, -3, -17, -12, -10, -1, 14, 2, -3, 1, -5, -10, -10, 3, -11, 1, 2, -2, 19, 5, 2, -2, 15, 5, -9, -7, -3, -9, 3, 10, -16, -3, 3, -2, 0, -3, -3, -7, 15, -5, -10, -14, 1, -6, -14, -12, 1, -6, -16, 4, 15, -3, -11, 15, 0, 14, -13}
, {-15, -9, -1, -20, 4, -4, -7, 8, 7, -4, -3, -8, -3, 1, 9, -15, -10, -14, -9, -6, -8, 9, -13, -4, -9, -11, 17, 3, -15, -23, -2, 7, -20, 7, 4, 9, 2, 5, 2, 12, 1, 1, -5, -14, 19, 2, 15, 15, -19, -3, -27, 5, -9, 4, -24, -19, -20, 5, -24, 7, 1, -11, -5, -9}
}
, {{4, -9, -3, -25, 5, 0, 2, -13, -1, -5, -8, 12, 5, 15, -1, -13, 2, -3, -22, -6, 3, -18, -1, 5, 20, -10, 0, -9, 6, -1, -5, -16, -11, 12, -16, -4, 5, -29, 4, 0, 17, -5, -12, 2, -18, 2, -5, -11, 7, -2, 3, -12, -5, -14, 1, 2, -13, 8, -5, -4, -9, 14, 20, -19}
, {-27, -6, -29, 10, -20, -12, 17, 14, 39, 9, 8, 0, -16, 11, 11, 9, -12, 16, 7, -5, -12, 13, -2, -18, -18, -16, 22, -21, 4, -4, 9, -2, -35, 39, 17, 13, -19, 2, -2, -24, 5, -3, -5, 8, 28, -25, -68, 18, -5, 3, 10, 18, -14, 30, 40, 9, -33, 17, 0, 28, -23, 7, 0, -23}
, {-10, -3, -11, 7, -4, -23, -1, 4, -1, 3, 6, 7, 12, -8, 0, 5, -5, 7, 8, 2, -12, 1, -9, 21, 0, 5, -5, -7, -19, 13, -1, -5, -22, 8, -27, -3, 4, 14, -5, 7, -8, 4, -18, -15, -10, 5, -7, 2, -2, 8, -3, 4, 8, 20, -4, -10, 7, -6, 1, 10, -6, -16, 1, 14}
}
, {{12, -15, -2, -5, 8, -1, 19, 7, 7, -13, -14, -2, -12, -15, -6, 9, -9, -16, -17, 13, 13, -9, -5, -8, -4, 3, 14, 2, 9, -3, -10, 19, 12, -17, 2, 2, 7, 2, 11, -29, -4, 2, 11, 12, -9, 1, -8, 12, 2, -6, -4, -9, -3, 3, 7, -18, -16, 11, -9, -11, -13, -14, 5, -8}
, {-11, 10, 19, 1, 16, 0, 5, -4, -15, 0, -2, -1, 2, 14, 6, -12, -8, 16, 13, 12, 19, 6, 20, 23, 17, 16, -11, -10, -15, -9, 13, -12, 11, -9, -4, -3, 17, -5, 0, 10, 1, 23, 7, -2, -9, -21, 1, -6, -9, 3, -26, 9, -2, -2, -16, 9, -7, -9, -5, 12, -11, -20, -6, -10}
, {2, 2, 7, 1, 12, -22, 1, -8, 11, -8, 5, -9, -13, 15, 21, -4, -9, 24, -3, -12, -4, 13, -8, -10, 3, -10, 12, -15, -7, 0, -6, -6, -6, -15, 6, 19, -11, -4, -7, -4, 15, -13, 3, 9, -15, -12, 8, 1, -12, 5, 8, 1, 2, -3, 4, 5, -2, -6, 3, -2, 1, 11, -4, 2}
}
}
, {{{2, -10, 5, 15, -8, -17, 15, 21, 18, 0, -23, -4, -9, -26, -2, -2, -4, -3, -11, 4, 3, 2, 14, -7, -10, -19, -5, 5, 9, -7, 13, 4, 8, 4, -17, -1, 0, 3, 9, -5, -7, 26, 2, 1, -8, -1, -13, 16, -14, 4, 2, 3, -6, 5, -6, 4, -11, 2, 5, 12, 0, -4, 9, -2}
, {7, -11, 4, -7, -16, 9, -23, 3, -27, -15, -1, 1, -10, 17, -2, -16, 11, 31, 15, -3, -2, -8, 6, -3, -20, 0, 3, 12, -18, -7, 1, -7, 9, -6, 1, 14, -2, -3, 21, -3, -6, 2, 6, -11, 11, 9, -6, 12, -19, -6, -5, -7, 22, -22, -6, -6, -26, -6, 18, -3, -5, 4, -12, -4}
, {-5, 8, 10, -1, -19, 14, -1, -3, -12, -13, -3, 16, -8, -19, -26, 6, 13, -8, -2, 9, -11, -14, 8, -15, -17, -7, -17, -2, -16, 37, -2, -6, 0, -6, 8, -17, 13, -11, 7, -4, 1, 10, -5, 1, -14, 15, 1, 8, 6, -25, -2, 10, -17, -6, 16, 14, 7, -4, 7, 11, 28, 13, 29, -22}
}
, {{12, 8, -6, 2, -2, -5, -4, -4, 10, -2, -23, 13, -20, -4, -6, 19, 12, -8, 1, 23, -9, 16, -10, -7, -4, -4, 12, -3, 3, -3, 3, 2, -4, -9, -10, 6, 8, -6, -1, 20, -11, 15, 1, -26, -16, -1, 3, -15, 9, 8, 5, 3, -3, -9, 3, 0, -5, 9, 0, -3, 0, -3, 2, 0}
, {4, 35, -18, 5, 6, -26, -26, 7, -6, -16, 2, -10, -15, 5, -4, 14, -28, -20, -7, 12, -19, -9, -15, -12, -1, -13, -11, 7, 12, -8, -16, -14, -10, 15, -9, -13, -10, -5, -19, 9, -9, -49, 24, -9, -12, -20, -34, 12, 13, -16, 8, 20, -12, 13, 4, 8, 0, -23, -19, 4, -21, -3, 0, 3}
, {16, 1, 15, 4, -9, 14, -5, 1, 2, 17, -5, -4, -2, 0, -9, 7, -2, 15, -8, -8, -4, -23, -3, 25, -13, 8, 1, -21, -13, -13, 6, 26, -1, 0, -30, -17, 16, 7, 0, -7, 1, -3, -22, -16, -15, 13, -6, 1, -9, -5, -9, 1, 13, -9, 14, 19, -8, 3, 13, 2, -13, 11, -5, 21}
}
, {{-17, -8, -6, -13, -20, 17, 3, -13, 0, -4, -19, -3, -19, -19, 2, -22, 9, 6, -7, 1, -20, -2, -10, -15, -5, -14, -7, -24, -1, -7, 5, -16, 0, -26, -4, -10, -8, -3, 6, -12, -4, -12, -11, -11, 2, -11, 12, 5, 9, 14, -10, 2, -14, -16, -3, -7, 1, 25, -8, -15, 1, 2, 6, -5}
, {-20, -7, 38, -18, 13, 8, 26, -14, 6, 38, 18, -34, 8, -20, -1, -28, -8, -5, 20, -18, -4, 0, 24, 1, -24, 10, 11, -28, -9, 3, 3, -8, -1, -9, -41, -11, 9, -7, 12, -2, 7, 28, -19, -18, -12, -2, -26, -3, 0, -16, -49, 16, 19, 14, 1, -15, 34, -22, 34, 7, 9, 1, -23, 39}
, {-23, -34, -14, -5, 0, 2, 26, -25, -10, -17, 6, 11, 13, -7, 1, -3, -6, 6, 3, -17, 4, 1, -19, -5, 0, 1, 10, 1, 4, 16, -8, 9, -2, -17, 13, -4, 14, -22, 8, 1, 16, -9, 3, -11, 19, -7, -10, -24, 17, -6, 8, -12, -11, 16, 15, 4, 6, -14, 18, -13, -4, 9, 0, -15}
}
}
, {{{-14, -10, -12, -1, -6, 28, 1, 3, -3, -14, -13, -20, 0, 28, 2, -3, 3, -2, 3, -2, 21, 12, -21, -10, 9, 11, 20, 5, -4, -3, -6, 10, -6, 7, -12, -4, 13, 2, 6, -1, 12, -19, -9, 5, 5, -7, -10, -6, -16, -8, -8, -8, 0, 19, 4, -14, -5, 14, -4, -5, 0, -18, 16, -11}
, {-6, 1, 9, 6, -12, -3, 17, 2, -1, 5, -2, -23, -6, 8, -12, 0, 6, -4, 13, 0, -20, -12, 5, 13, -8, 3, -1, -2, 15, -5, -8, -10, 17, -11, 2, -4, 25, -4, -15, -5, -2, 13, -9, -1, -14, -11, 12, -8, 3, 6, -11, -17, -2, -3, -4, -10, 12, -2, -11, 5, 2, -16, -2, 0}
, {9, -5, 0, -11, -11, 8, -9, -7, -13, 6, -6, 2, 3, -5, -5, -1, -10, -2, -11, -13, -16, -9, 12, 8, 19, -3, -9, 7, 0, -5, 11, -7, 4, 5, -5, 20, 13, -6, 23, -7, 7, -8, 11, -2, -10, -9, 17, -6, 26, -3, -10, -20, 3, -8, 5, -3, 3, -13, -17, 2, -10, -14, -12, -10}
}
, {{8, 0, 2, -5, -11, 9, -4, 4, 0, -22, -17, -11, -13, -10, -5, 11, -2, -9, -8, 0, -16, -6, -8, -13, -10, -14, 19, -14, 24, 7, -9, -8, -14, -6, 1, 1, -7, 2, -3, 12, 12, 10, -1, 3, -21, -22, 2, -6, 8, -14, -13, -12, -7, 8, -2, 16, -19, 7, -12, 11, 8, -2, 11, -8}
, {16, 0, -9, 9, -14, 4, -38, -1, 1, 15, 6, -7, -25, -8, 16, -18, -16, -7, 19, 9, -3, -7, -12, -18, -16, 11, 8, -13, 29, -11, 5, -5, -14, -4, 16, 15, -5, -19, -24, 14, 14, -20, 11, 14, -12, -42, -6, -17, -35, -13, 23, -1, 32, 16, 30, 3, -27, -1, 5, 11, -21, 11, -1, -12}
, {-3, -2, 11, 0, 4, 21, -8, 1, -17, -5, -33, 15, -9, -3, -11, -2, -4, -4, -16, -11, -8, -8, 13, 3, -10, 4, -4, -8, -15, -1, -6, 2, 10, -10, 2, -9, 13, 17, -30, -14, 4, -11, -18, -3, -23, 3, 9, 3, -6, 9, 3, 7, -11, -12, 2, 25, -16, -9, 3, -19, 6, 13, 7, 1}
}
, {{10, -18, -16, 3, 18, 13, -8, 5, -7, -3, -8, 1, -4, 19, -9, -20, 12, 6, -15, -10, -2, 8, -12, -5, -1, 25, -17, 5, 17, 7, 3, -12, -16, -10, -37, 5, 5, -9, 7, -2, -11, -13, -7, -15, -10, -11, -15, -9, 11, -1, -7, -7, -7, 22, -3, 6, 2, -4, 17, -3, -20, -13, 13, 12}
, {4, -15, -3, 6, 9, 4, -5, 7, 21, -9, -1, 12, 4, 18, 1, -7, 8, 20, 0, 2, 4, 18, -29, 0, 10, 6, 10, 0, 18, 2, 5, 8, -8, 9, 23, 7, -6, -13, -14, -5, 2, -7, 13, 16, 18, 4, 7, 19, -8, 2, -5, 15, -14, -17, 5, 5, -1, 17, -6, -11, 0, -11, 19, 9}
, {-4, -3, -8, -11, -6, 27, -12, -13, -20, -7, -4, -10, -8, -8, 7, 6, -3, 0, -13, 9, 0, 1, -19, -2, -22, 4, 2, -15, -8, -4, -1, -13, -11, -9, 11, -1, 24, -10, -3, -16, 7, 16, 0, 0, 4, 3, -5, 10, -7, 0, 4, 8, 5, -19, -1, -26, 0, 2, 17, -28, 1, -5, 22, 1}
}
}
, {{{-4, 1, -16, 4, 0, 6, 13, 18, 7, 8, -24, -2, -17, -10, 1, 6, 8, -1, 11, -5, -2, -9, 3, -13, -5, 2, 7, -12, -11, 2, -9, -3, 5, -2, 0, 4, 17, 2, 0, 10, -1, 18, -5, 17, -4, -2, 1, 10, -1, -6, 10, 4, -6, -30, -4, -2, -5, 16, -10, 0, -8, 11, -4, -15}
, {0, -10, 5, 4, 10, -8, 8, 3, -2, -6, 4, -7, -7, 8, -2, 12, -7, 29, -15, 15, -10, 10, 10, -9, -15, 8, -1, -3, -2, 4, -8, 3, 1, 5, -8, -16, 4, -12, -11, -7, 1, -6, 17, -7, 3, -3, 9, 1, 0, -10, 0, -5, 3, -12, 3, 0, 7, -4, -14, -12, 6, 9, 16, 6}
, {-13, 2, 11, 0, -8, 13, -1, -7, -4, -8, -5, 21, 23, -20, 22, 2, -10, 1, 13, -9, -9, -5, -15, 9, 15, -6, 7, 9, -14, 9, 5, -13, 0, -2, 4, 2, -3, 0, 7, 2, -13, 10, 8, -9, -7, 7, 4, -4, -13, -5, 15, -6, 17, -2, -4, 0, 19, -10, 6, -2, -2, -4, 9, 0}
}
, {{30, -16, -2, 12, -8, 11, -9, 15, -2, -3, 7, 2, 12, -8, -4, -4, 13, 32, -2, -4, 1, -6, -6, -8, -4, 7, -30, 4, 7, -17, -20, 1, 3, -11, 2, 14, -4, 3, -4, -1, -18, -11, -14, 1, -8, -6, 8, 2, -1, -4, 4, -21, 8, -28, 7, -6, -3, 2, -14, 2, -4, 4, 9, -10}
, {-11, -16, -1, -3, 16, 6, 25, 12, 10, 2, 1, -10, -11, -8, -38, -9, 7, 20, -4, 10, -2, 10, 19, 4, -9, -2, -30, -21, -19, -9, -2, -5, -1, -3, -11, 7, -7, -15, 0, 11, 18, 2, -13, -15, 9, -26, -8, 20, -4, -22, 10, 3, 5, -12, -21, -23, 15, -3, 20, 13, 6, 7, -12, -1}
, {6, -8, -11, 13, 4, 15, -3, -3, -4, 14, 19, -16, -1, 19, 7, -19, -4, 5, 0, -17, 0, -3, 12, 2, -13, 7, 11, -5, -3, -5, 4, -18, -2, 1, 7, 9, -23, -6, -1, -18, 5, -21, -3, -5, -7, 4, 3, -33, 3, 13, 6, -16, -7, 2, 8, 1, -1, -6, -7, -3, -3, -9, 3, -3}
}
, {{1, 10, -14, 11, 16, -7, 1, 2, 9, -15, -10, -7, 16, -10, 7, 8, 9, 2, -4, -15, -1, -14, 14, -16, -12, -9, -9, -4, -18, 12, -11, 6, 20, -10, 1, 5, -4, -12, -1, -3, -11, -4, -22, 5, -13, 13, -5, -7, -9, 8, -1, 3, -18, -17, -7, 11, 0, -2, -2, -14, -3, 1, -13, -3}
, {-7, 18, 27, -13, 5, -11, 3, 20, 0, -9, 9, -12, -4, -20, -21, -2, 25, -12, -7, 23, -8, -25, 11, 13, 4, 0, -10, -1, 1, -8, 3, 28, -6, -4, -2, -1, -15, -35, -13, -18, -26, 12, 3, -14, 10, -11, 2, -8, 14, -9, -3, -9, -34, 0, -20, 0, -25, -11, -10, -11, -7, -20, -6, 4}
, {18, -3, -16, 4, 4, -12, 1, -8, 4, -11, -10, 19, -4, 23, 16, -17, 11, -12, -4, -10, 3, -11, -5, -14, -7, -12, -15, -2, -6, -6, -5, 29, 9, 4, 27, -20, -14, -11, -7, 8, 11, 7, -6, 8, -2, 17, -10, -9, -7, -11, 0, 23, -10, -12, -10, -2, -4, 10, -22, 2, 1, 20, -8, -2}
}
}
, {{{-2, 11, 9, -7, 19, -3, -15, -4, 8, 12, -13, 0, 5, 8, -3, 5, 6, -7, 8, -11, -3, -17, -17, 4, 0, -1, 4, -2, -8, -3, -27, 2, 20, 8, -2, 4, -2, -9, -3, 0, 5, -1, 11, 2, 8, -8, 8, 0, -13, 0, 9, -1, -12, 3, 10, 8, 1, 9, -3, -1, 12, 3, -11, -2}
, {-2, -11, -24, -8, 2, 8, -6, -11, -13, 8, -2, 0, -12, -7, -18, 10, 13, -2, -3, -15, 3, -24, -6, -8, -21, 14, 3, -3, 20, 2, -12, 7, -9, -2, -5, 4, -4, -21, 9, 2, 9, 0, -5, 1, 7, 18, -22, -22, 22, -15, -7, 4, 12, 10, 14, 4, 11, -1, 2, 1, 1, 2, -16, -15}
, {11, -17, 0, -12, 16, -1, -8, 3, -3, 3, 17, -1, -3, 1, 8, -5, 15, 8, 3, 1, -3, -13, 7, 8, -21, 10, 12, 4, -15, -1, 8, 14, -6, 12, 2, 9, -20, 3, -7, -5, -5, -14, -14, -10, -1, 6, -6, -17, 7, -10, -6, -1, 14, -1, -18, -5, -10, 0, 11, -1, -12, -8, -2, -3}
}
, {{-12, 24, -7, 0, 14, 0, 5, 3, -15, -1, -3, 10, -2, -7, 6, -11, 11, 2, 3, -8, -13, -11, -8, 4, -12, 22, 16, -22, -4, 9, 5, 18, 13, -2, 10, 4, -10, -8, 5, -1, -23, -20, 14, -26, -9, -16, 0, -10, 12, 10, -15, 7, -19, -7, 4, -4, -22, 0, 11, 2, 1, 5, 4, -6}
, {5, -2, -20, -37, 16, 10, 14, -12, -19, -8, -28, 16, 5, 6, -21, -13, -1, -1, 15, -30, 25, -5, 19, 18, 8, -5, -13, -11, -15, -18, -24, -6, -3, -8, 9, -5, 3, 18, 13, -26, -48, 11, -29, -7, 17, 17, 11, 2, -12, 19, -9, -7, -17, -20, -11, -6, -35, -4, -9, -10, -2, 12, -1, -25}
, {17, -1, 7, -2, -21, -20, -7, -3, 8, 15, 5, -10, -20, 0, -9, 3, 11, 9, 2, -5, -5, -6, -8, -1, -14, 15, -5, -8, -8, -2, 6, 6, -16, -19, -11, -3, 8, -4, -1, 18, 10, -20, -11, 1, -3, -16, 10, -1, -3, 8, 0, -6, 8, 13, -9, 22, -3, 25, -4, -3, -5, -9, -14, -3}
}
, {{-4, 16, -2, -4, 27, -6, 3, 5, -8, 5, 11, -4, 2, -10, -1, -1, 8, 11, -4, -21, 1, 0, -13, 11, 2, 5, 12, -12, -19, 3, -10, 6, 10, -9, 3, -1, 10, -13, 8, 7, 6, -8, -5, -6, -3, -9, -15, -18, -10, -11, 5, -4, 12, -4, -12, -14, -2, -4, 25, -25, -11, -11, -9, -8}
, {-19, 9, -6, 19, 12, 9, -20, 9, 5, 24, -1, -11, -25, -4, 0, -11, 18, 5, -4, -12, -16, -22, -2, 23, -9, 7, 1, -1, -2, 8, 27, 4, 0, -5, 7, -2, -10, 6, -5, -11, 11, 8, -30, 3, -3, -8, -7, -16, -1, -9, -24, -10, -12, 9, -8, 4, 0, 15, 6, 10, -8, -19, 0, 34}
, {21, -21, -5, -5, 0, 6, -9, 17, 7, 5, -14, 10, -23, -2, 6, -5, 7, 17, 19, 11, 3, -10, -1, 5, -7, 10, 15, 6, -2, -5, 9, 9, -30, -2, -19, 15, 3, -14, 13, 4, 18, -1, 9, -8, -13, 4, 5, 3, -32, -12, 1, 2, -30, 5, 3, -2, -4, 24, -10, -2, -11, -1, -6, 18}
}
}
, {{{5, 2, 1, -10, -6, 1, 6, -1, 10, 10, 0, -7, -12, 3, 3, -13, -18, -2, 8, -20, 2, 7, 7, 8, -9, -1, 11, -1, -12, -6, 3, -10, -5, -9, -26, 13, -3, 6, 5, 4, -4, 2, -10, 8, -8, 4, 5, -11, 16, -1, 10, -1, -12, -1, -6, 4, -9, 10, -5, 4, -17, 5, -6, 3}
, {-12, -3, -6, 23, -2, -6, -5, 2, 5, -3, -1, -9, -10, -22, -12, -3, -1, 1, 2, -13, 3, -8, 1, -7, -11, -14, -7, -1, 3, -7, 4, 6, -7, 6, -8, -5, 7, 10, -7, -22, 18, -13, 11, -10, 0, -8, -10, -4, 18, 1, 5, -5, 5, 1, -14, 16, -6, -11, -19, -9, -5, -11, -25, 16}
, {-22, -1, 12, -1, 4, -8, 0, 0, 7, 9, 9, -7, 13, 6, 7, -2, 21, 12, 7, -8, 1, -20, 17, 14, -5, 11, -13, -15, -9, 12, 7, -7, -6, -17, -14, -2, -10, -23, -12, 12, -9, -2, 7, -8, -2, -8, -6, -25, -5, -20, 8, -12, 5, -5, 7, 0, 2, 0, 2, 1, -2, -16, -9, -3}
}
, {{7, -7, -13, -15, -1, 2, 22, -14, -12, 11, -12, 1, 1, -6, 5, -27, -10, -11, -20, -8, 12, -8, 15, -9, -24, 4, -3, -13, 3, 10, -7, 1, 3, 1, 8, -23, -6, -3, -14, 1, -9, 9, -18, -7, -15, 31, 2, 17, -1, -2, -1, -7, -8, -1, -1, -32, 1, 1, 11, -20, 0, 6, -2, 1}
, {-18, -1, -15, -1, 3, 15, -9, 10, -2, -18, -38, 37, -1, -7, -6, -32, 18, 1, -49, -16, -28, 3, -31, -43, -28, -5, 11, -5, -2, 35, -6, 9, 2, 18, 28, -12, -27, -9, 0, -6, 21, -43, 2, -2, 6, -17, -10, -15, -1, -28, -16, 3, -26, 28, -7, 8, -10, 15, -3, 2, -7, 7, 9, 12}
, {-6, -33, 14, 8, -5, 5, 2, 4, 9, -1, -11, 8, -9, -12, 2, -15, -3, -6, -15, -2, 7, -16, 4, -2, -5, 13, -3, -5, 4, 0, 17, 4, 17, 5, 2, 17, -7, -4, -8, -2, 10, -14, -18, -6, -14, -3, -3, -6, -14, 6, -2, 8, -10, 7, -7, -12, -4, 5, -5, -4, -6, 6, -12, 10}
}
, {{8, -2, -2, 9, 2, 12, 0, 11, 24, -14, 10, -1, -9, -12, 19, -9, -20, 18, -16, -15, -12, -13, 6, -5, 0, -14, -5, -15, -19, 18, 5, 4, 23, -2, -2, 14, 5, -11, -8, -22, 5, -19, 1, -6, 4, -2, 1, 5, -18, 10, -3, -24, -13, -12, -7, 7, -8, -2, 14, -6, -9, 6, -4, -8}
, {-6, 13, 10, -11, -1, -7, -4, -12, -11, 23, 15, -6, 9, -13, 15, -9, -5, -15, -10, -17, 6, -2, 15, -13, -2, -5, -19, 0, -12, 22, 3, -10, 4, 15, -2, -21, -3, 36, 6, -5, -13, 29, -16, -8, -6, 7, 8, -6, 14, 40, 11, 1, 12, 15, -5, 15, -12, -16, 10, -2, -12, -2, 12, 14}
, {4, -8, -28, -5, 0, -19, -3, -18, -20, 9, -23, 8, -29, 0, -17, 5, 2, -8, 11, 2, 11, -9, 7, -7, -29, 1, 3, 25, 14, 6, -26, -16, 7, -7, -20, 14, 13, -4, -12, 3, 3, 5, 5, -8, -18, -12, -16, -9, 9, -1, 1, 17, -8, -3, 6, 4, -16, -5, 7, -2, -19, 0, -3, -3}
}
}
, {{{-1, -2, -9, 1, -3, 19, 5, 1, 1, -4, -4, -4, -13, 4, -3, -7, -6, 8, -4, 5, -3, -1, 0, 5, -15, 3, -5, 14, 10, -10, -4, 4, -15, -15, -2, 6, 12, 0, -11, -7, 3, -9, 3, -19, -6, 11, 0, -11, 16, -13, -6, 13, 12, 11, -2, -9, -15, 4, 1, -9, -17, 12, 1, 1}
, {-14, -3, 5, -8, -5, 3, 11, 7, 4, -5, 2, 15, 4, 3, 0, 16, 6, 7, 1, -21, -4, -19, -13, 20, -13, 6, -21, 9, 12, 12, -6, -10, 4, -29, -2, 6, 11, 4, 10, -15, -1, -2, -3, -13, 4, 0, 2, 2, -5, 4, 5, 0, 6, -6, 7, -7, 4, -15, -2, -3, -10, -3, -15, -3}
, {-8, 1, 7, -10, -5, 1, -17, -7, 3, 11, -13, 9, -4, -3, 7, 0, 12, -12, -8, -8, -5, -2, 10, 17, -7, 0, 8, 1, -3, 13, 5, -9, -3, -7, 5, 1, 0, 3, -11, 12, 6, 6, -4, 6, -12, -12, 5, -16, 9, -5, 25, -3, -15, -1, -4, -20, -5, 4, -11, -12, 1, -21, -2, 13}
}
, {{-8, 11, -8, -6, 16, -14, -4, 6, 6, 14, -1, 4, 13, 2, -18, -8, 4, -7, -8, 11, 0, -9, 7, 8, -14, 1, -12, 6, 2, -2, 3, -1, 0, -5, 8, -4, 3, 3, -15, 2, -1, 10, -5, -18, 8, 6, -8, 5, 2, -9, 2, -7, 2, 1, -10, -5, -8, -16, 4, -4, -9, -4, -3, -7}
, {-5, -21, -11, 10, 8, -9, -34, -10, -16, -19, -11, 25, -38, -23, 34, -6, 17, -9, -15, -11, -32, -2, 3, -32, -18, 5, 14, -22, 10, -13, -24, -2, -4, 4, 10, -6, 14, -3, 16, -11, 20, -64, 12, 10, -30, -7, -11, -13, -5, -25, -14, 5, -27, 15, 9, 38, 0, 25, 0, 13, -1, 27, 0, 19}
, {-8, -15, -19, 11, -23, 16, -9, 10, 3, -3, -9, 7, -12, -17, 6, -1, 19, 11, -19, 8, 11, -37, -4, 20, -3, 23, 1, -13, -25, 15, 50, 6, -2, -7, 2, 21, -18, 2, -20, -12, 26, -10, 3, -23, -4, 15, -3, -11, -1, 9, -4, -26, 12, 3, -6, 4, 9, 7, 16, -4, -17, 1, -1, 7}
}
, {{10, 12, -10, -5, 8, 3, -7, 14, -19, 3, -11, 2, -2, 7, -14, -10, -5, -10, -4, 27, -10, 7, -1, -15, -1, -7, -4, 8, 3, 0, 5, -3, -2, 3, 4, -20, 1, -5, 16, -9, -3, 0, 19, 0, -12, 7, 0, -2, 16, 10, -5, 16, 4, -4, 13, 6, -9, -6, 0, 7, -2, -8, -8, -12}
, {-6, -5, 8, -29, 14, -4, 10, -2, -2, 21, -15, 5, -9, -9, 29, -27, 5, -19, -8, -27, -24, 8, 3, -9, -20, 0, -10, 5, -10, 12, -13, -37, 3, -11, -19, -19, 2, 24, 12, -7, -2, 11, -15, -1, -7, -2, -4, -2, 17, 9, -25, 17, -12, 11, 9, 2, 22, -12, 23, -13, -2, -12, 13, -3}
, {-5, -12, -7, -5, 19, -2, 10, -20, -15, 1, -11, 9, -24, -11, -4, 0, 34, 8, -17, 0, 13, 0, -9, -20, -33, 23, -3, 8, -14, 1, -10, -4, -2, 6, 1, 0, 3, 16, 7, -5, 6, 12, 21, 1, 3, -9, -9, -10, 6, 19, 24, 25, 2, -9, 8, -15, 3, 2, 2, -25, 11, 10, 3, -12}
}
}
, {{{37, -21, -6, 11, 9, 4, -23, -6, 6, -6, -12, 4, -11, -5, 0, -16, 14, 9, 3, -4, -5, -21, -8, 4, -18, 5, -10, -12, 2, 11, 11, 11, 9, 13, -18, 5, -17, 0, -8, -5, 22, -26, -5, 2, 9, 17, -3, 9, -13, 4, 3, -20, -3, -8, 4, 4, 6, -1, 0, 9, 11, 11, -18, 19}
, {1, -5, 19, -6, 8, 9, -11, 12, -4, 1, 0, 14, -7, 2, 6, -9, -2, -3, 4, -14, 0, -8, 9, 8, 1, 4, 5, -12, -15, 14, 10, 6, 14, 2, 24, 6, 8, -14, -3, -2, 13, -2, -15, -9, -7, -4, -11, 4, -10, -10, 10, -14, -29, -4, -15, -5, 10, 8, 11, -7, -22, -16, -13, 10}
, {24, -21, -13, 8, 9, -14, -8, -5, -22, -5, -10, -4, 2, 31, -7, -2, -2, 0, 8, 20, 10, -22, 0, -5, -5, 15, 0, -16, -3, 8, 0, 1, -4, 1, -4, -16, 2, 10, 6, -6, 4, 7, -5, 11, 4, 11, 1, 2, -3, 1, -2, 6, -12, -22, 14, -3, -2, 12, 12, -2, -13, -4, 6, -11}
}
, {{4, -10, -7, -25, 1, -14, -6, -17, -24, 15, 1, -10, 13, 0, -4, -5, -14, -9, 7, 1, -7, 12, 7, 2, -5, 0, 13, -3, 0, -13, 4, 0, -13, -12, -1, 8, 3, -8, 20, 13, 0, 3, -8, -16, 0, -2, 11, -15, 24, -18, 4, 6, -15, -19, -14, -10, 9, -28, 2, -16, 2, 5, 0, -5}
, {-3, -17, -19, -45, -12, -4, -9, -19, -20, -16, -42, 21, 16, -3, -6, -22, 13, -24, 22, 16, 3, 1, -6, -9, 9, -19, 2, 25, -12, 3, 7, -18, -2, -15, 18, -1, 19, 13, 13, -11, -17, -16, -8, 4, 0, 5, 7, 2, 20, 5, -22, -24, -24, 9, -10, -15, 20, -4, -4, -12, -2, -10, 31, -20}
, {20, -1, -35, -17, -8, -20, -10, -10, 11, -22, -3, -7, 2, 15, -14, -8, 10, -1, 12, 6, 7, 0, -24, -5, -1, 2, 21, -6, -2, 6, -17, -3, -19, 5, 3, 23, 23, -1, 0, -20, 13, -22, 13, -14, -16, -20, 5, 10, 9, -12, -4, 16, -6, -10, -9, -10, -1, 13, -5, -26, -14, -30, 16, 0}
}
, {{-11, 13, -24, -23, 7, 7, -17, -4, -13, 5, 1, -2, -4, -10, -12, 6, 1, 25, -5, 0, -2, 18, -15, 10, -5, 10, 7, -5, 5, 12, 3, -13, 1, 22, 3, -15, 12, -5, 7, -16, 4, -1, 13, -21, 13, -9, 2, 8, 18, -23, -12, -11, 5, -10, -11, -9, 12, -14, -3, 5, 9, 5, -15, -13}
, {-12, -17, -21, -6, 18, 8, 4, 1, 5, -9, -1, -13, -18, -8, 14, -1, 16, -14, -2, 14, 0, 1, -8, -9, -15, -4, 7, 8, -28, 18, 0, 1, 22, 1, -3, -23, 6, 0, -19, -2, 9, -2, -14, 4, 1, 9, -6, -3, 0, -7, -6, 9, -6, 18, 1, 1, -11, -7, -13, -4, 11, 1, -16, 14}
, {-4, -8, 21, -7, -21, 10, -5, 7, -6, 6, -12, 14, 7, 0, 19, -21, 23, -3, -4, 3, 9, -7, -1, 11, -6, -5, 10, -16, 4, 17, 12, 6, 17, -15, 4, -14, 19, -5, -11, -7, 15, -9, -21, -2, -3, 8, -10, -15, 18, -3, 10, -5, -8, 0, 4, 6, 17, 4, -1, -1, -21, -1, 3, 16}
}
}
, {{{7, -13, -11, 3, -6, -13, -5, -3, -9, 7, 2, 4, -9, -15, 15, 0, -6, -4, 10, -4, -13, 12, -16, 12, -8, 7, 14, 5, 17, -8, -5, -1, -4, -22, -17, 6, 6, -17, 13, 4, -10, -14, 1, -1, 3, -7, 1, -18, -4, -3, -19, -5, -12, 5, 10, 24, -4, 3, 0, -4, -6, -2, -3, 3}
, {-12, 1, 11, 11, -5, -1, 12, -12, -13, 16, -2, 8, 18, -4, 2, -7, -5, 1, -20, 0, -15, -11, 5, 31, 4, -2, 12, -11, 0, 7, -9, -19, -5, -14, -27, 3, 14, 2, 3, -6, 0, -3, 9, 2, 11, -8, -10, -8, -11, 16, 10, -2, -3, 6, 22, 3, -3, -15, -2, 6, 3, -2, 2, -1}
, {13, -3, 1, -12, 3, -13, 5, 6, 7, -10, -4, -6, -10, -1, 1, 2, 7, -9, -7, -8, 16, -8, -3, 14, -12, 5, -4, -5, 12, 11, 13, 3, -6, -12, 9, 5, 10, 8, -7, -25, 20, 5, 0, 3, -18, 13, 10, -9, -5, 8, -3, -13, 0, -9, -2, -14, -7, 6, -9, -2, 7, -8, -33, -6}
}
, {{-9, 8, 19, 1, 22, -13, -1, 3, 4, 10, 2, 3, 2, -17, 19, -39, -1, 16, -19, -8, -16, -34, -9, -1, 5, 17, 4, -22, -11, 6, 10, 12, 11, -1, -18, -8, -7, -3, -13, 6, -19, 0, -7, -11, 0, 7, -23, 4, -8, 5, 4, -13, 0, 2, -11, -22, -23, -9, -2, 1, -5, -21, -5, -21}
, {17, 2, -6, 12, 23, -2, 0, -5, -24, -20, -30, -5, -13, 2, 15, -9, -6, 5, 0, -29, 8, 4, 24, -20, 0, -2, -41, -12, 0, 5, 10, 12, 11, 24, 7, -21, 6, 3, 16, -4, 3, -12, 6, 15, -32, 14, 5, -11, -7, 9, 34, 12, -25, 1, -14, 26, -6, 11, -3, -10, 11, 24, -5, 8}
, {-11, -12, 3, 8, -7, 17, -2, -12, 14, 1, -24, -24, 9, 3, 11, -15, 10, -10, -7, -7, 2, -9, 12, -24, -6, 16, 16, -3, 2, -8, 12, -9, 1, 3, 7, 7, -20, 2, -21, 1, 17, 4, -11, 3, 0, 1, -10, -10, -11, 15, 8, -7, -14, -4, -3, 7, -9, 8, -2, 10, 5, -5, -6, -1}
}
, {{0, -10, -1, -8, -3, -22, 7, 4, -18, -14, -5, -20, 12, 6, 4, 6, -24, 10, -15, 6, 9, -4, -13, -4, -4, -11, 5, -3, -8, -5, -12, 18, -4, 8, 0, 0, -9, -24, 10, -6, -6, 0, 12, 5, -14, 12, 6, 10, 6, -26, -9, -6, -2, -1, 20, -3, -3, -7, 3, -2, 3, -17, -16, -14}
, {-6, 22, 19, 4, -29, 9, -5, 8, 11, -23, 2, -4, 22, -5, -9, -19, 2, -4, -12, 0, -6, -3, -9, -12, 17, -1, -7, 12, 4, 8, 1, -3, -7, -5, -7, 8, 2, -2, 0, -11, 19, 5, 1, -24, 15, -1, -16, -14, 9, -18, 6, 2, -10, 3, 10, 10, -4, -13, 1, 8, -25, -41, 24, 0}
, {-15, -4, -6, 4, 3, 1, -3, 9, 10, 2, -2, -7, -16, -1, 12, -2, 3, -1, -4, -1, 4, -7, -7, 6, -6, 17, 1, -8, -18, -14, -17, 3, 0, 11, -4, 7, -16, 13, 2, 4, 1, 11, 5, 7, -17, 7, 15, -2, -9, 15, 2, 11, -2, -23, -11, 5, -12, 14, -8, -21, -1, 2, 5, 5}
}
}
, {{{8, -7, 3, 12, -14, 14, 3, -10, 4, -2, -12, -3, 9, -4, -18, -3, -13, 8, -6, 9, -8, 11, -13, 4, 2, 0, -11, 4, 8, -12, 13, -9, 2, -2, 17, -3, 8, -12, -2, 0, 11, -8, 11, 12, 10, 2, 5, 11, -18, -2, 15, 2, -4, -3, 9, -20, 0, 4, -25, 7, 15, 10, -11, -5}
, {-4, -7, 14, -11, -6, 8, 3, 10, 9, 3, -16, -11, 0, 5, 2, 7, 13, -7, 8, 10, 0, 1, 2, -10, -5, 14, 2, -3, 0, -6, 13, -2, -14, -10, -3, 1, -10, 20, -16, 8, 10, 6, -3, -14, -7, -27, 6, 12, 2, -4, -2, -20, 2, -11, -18, -25, -12, 2, -32, 2, -20, -21, 0, -8}
, {-9, 4, -6, 9, 1, 10, 9, -4, -20, 0, -5, 19, 6, -1, 6, -5, 11, 5, -2, 4, -19, -10, -16, -10, 11, 1, -9, 9, -8, 15, -10, -9, 2, -2, 8, -15, -6, 0, 20, 3, 2, 0, -3, -2, -15, -4, 13, -20, -4, -8, 10, 2, 3, -17, 14, -3, 5, 11, -1, 2, 17, 13, 8, 4}
}
, {{-5, -5, -11, 9, -19, -9, -2, -21, -15, -4, -4, -8, -3, 6, -9, -1, -9, -7, -21, -6, 0, -10, -4, 2, 2, 19, 4, -9, 4, 5, 7, -13, 3, -2, 20, -28, -3, 5, -8, 4, -10, 5, 13, -11, 0, -2, 5, 17, 4, 6, 6, 1, -3, -16, 5, 3, 4, -22, -9, 4, -26, 10, 4, -19}
, {-1, -6, -5, -9, -21, 6, 7, 3, -13, -2, -1, 4, 17, 0, -18, 5, -2, -17, -20, 29, -4, 0, -23, -13, -2, 7, -27, -1, -4, 1, -28, -5, 21, -17, -15, -10, 12, -14, -13, 10, 14, -6, -1, -20, -2, -6, 11, 12, 18, -2, -2, 9, -7, -13, 5, 1, -16, -9, -19, 13, 4, -10, 19, 9}
, {-1, 16, 8, -3, 3, 13, 9, -1, 2, -21, 27, 5, -9, 0, -7, -10, -24, 12, -3, -10, -1, 14, -1, -11, -18, -16, 3, -9, 7, 3, -5, -4, -2, 24, 5, -17, -8, 10, 6, 8, -1, 3, 2, -9, 6, 2, 3, -1, 8, -18, -24, 0, 0, 5, -6, 10, -26, 6, -2, 8, -4, 6, 6, 7}
}
, {{-5, 10, -10, 22, -12, -3, -7, -23, 6, -2, 2, -4, 1, -21, 8, 17, -8, -18, 9, -14, -23, -12, 18, 4, -18, 14, -2, -15, -9, 17, 17, -8, 1, -15, 6, -9, -6, 8, -5, -22, -10, 11, -16, -10, -15, 9, -3, 5, -13, 3, 13, -16, -4, 14, 4, 7, 1, 27, -1, 1, 13, -10, 5, -5}
, {11, 8, 18, -20, -1, -15, -4, -5, 19, -3, 29, -25, -9, -19, -15, 15, -6, -5, -2, 17, -8, -11, 6, 13, -16, 6, -3, -8, -14, 4, 11, 3, -30, -13, -12, 8, -4, -36, 21, -36, -26, 2, -11, -7, -13, 4, -18, 12, -1, -33, -25, 4, -3, 7, -9, -15, 15, 11, 18, -14, 12, -20, -9, 22}
, {7, -7, 16, -7, -14, -32, -11, -19, 27, -3, 10, 26, -4, -15, -3, -30, 0, -14, 2, -7, 11, -2, -20, -18, 5, -20, -12, 17, -3, 5, 13, 5, -10, 17, 4, -5, 4, 8, 5, 24, 23, 4, -5, 0, 5, 12, -12, 13, -15, -12, 1, 0, 3, 10, 11, 5, 2, -8, 2, 10, 32, -25, 4, -17}
}
}
, {{{-11, -11, 13, -21, -18, -5, -8, -3, -24, 7, 3, -12, 15, -6, 17, -7, -18, -1, -22, -7, 8, 12, -3, 2, -14, 6, -4, -7, -3, -34, 10, -1, -13, 11, -8, -8, 5, -3, -2, 17, -3, -18, 11, 1, 0, -2, -12, 2, -19, -12, 10, 1, 19, -6, -12, 8, 4, -24, -9, 2, -2, -10, -13, 13}
, {-7, -2, -8, -1, -15, -2, 3, -11, 19, -11, 9, 0, -23, 3, 0, 3, 12, 6, 13, -5, 6, 12, -4, 4, -22, -13, -7, 9, -21, -12, 3, -7, 0, -7, -1, 11, -4, -4, 5, 20, 9, -19, -1, 1, 1, 5, 7, -9, -6, 4, -6, 2, -15, 4, -10, 29, 15, 9, 3, 0, 9, 18, -17, 7}
, {-2, -3, -2, 3, -3, -1, -3, -8, 11, 12, 8, 13, -5, -1, 1, -10, 7, -4, 2, 2, 16, -4, -28, 10, 19, -1, -2, -1, 6, -1, 9, -15, 1, -3, -5, -13, -17, 18, 1, 17, 1, -9, -2, -15, -6, 21, -4, 20, -39, 30, 5, -13, 3, 18, 1, 4, -19, 4, -5, -12, -7, 14, 9, -6}
}
, {{0, -7, 14, 11, 3, 21, -10, 6, -11, -9, 14, -3, -4, -10, -16, -8, -8, 8, -5, -3, -6, 19, 1, 1, -6, -7, -3, 12, -6, -22, -4, -10, -2, 5, -11, 13, 0, 2, -19, -11, 19, -23, -7, -7, -7, -5, 19, -20, 0, -18, 10, 16, 11, -10, 13, -10, -10, -16, -1, 8, -4, -17, -22, 3}
, {-13, -17, 0, -4, -9, 8, -3, -12, 0, 6, -4, -11, -1, -4, 20, -9, 29, -1, -4, -4, 3, 2, 1, -5, -26, -17, -9, 18, -28, -14, -4, -7, -28, -17, -23, 14, 2, 1, -20, -9, -1, -26, -20, 6, -11, -9, -21, -21, 13, -25, -11, -13, -10, 18, 6, 9, 6, -30, 11, -17, 4, -30, -9, -12}
, {5, 13, 7, -9, -3, -4, -6, 2, 24, 5, -17, 13, 15, 13, 3, -4, 1, 10, 25, -2, -2, -4, -1, 10, -21, 16, 6, -3, 7, -3, -21, 21, 11, -5, 6, 25, -5, -4, 13, 0, -12, -1, -5, -10, -5, -10, -6, -9, -21, -3, -10, 12, 18, -1, 24, -15, 4, 1, 3, 7, -14, -8, 0, 19}
}
, {{-1, 3, 0, 2, 16, 10, 11, 7, -10, 9, -10, -3, -15, -12, -10, -2, 2, -3, -10, -16, -4, 13, -2, -14, 12, 1, -10, 18, -11, 7, -2, 3, -10, 15, 4, -17, -2, -11, -11, 3, -7, 9, -4, 13, 20, -3, -11, 2, -3, -8, -4, 13, -4, -10, -14, 22, 8, -6, 10, 4, 4, -22, -13, 8}
, {12, -11, -9, -6, -4, 6, -8, -15, 7, -16, -22, 13, 2, 7, 17, -12, 19, 12, 2, -3, -16, -13, -26, 4, -5, 17, -4, -7, 10, -10, -12, 16, -17, -11, -3, -14, -20, -6, -14, 0, 8, -14, 6, 15, -7, 9, -2, -26, 0, -11, -1, 21, -19, 1, -12, -7, -34, 12, -9, 1, 14, -1, -1, 17}
, {-12, -3, -28, -5, 1, -11, 0, 2, 19, -19, -22, -7, -7, 21, 23, -1, 14, 7, 12, 13, -2, -1, -4, -7, -1, -16, 1, -4, 8, -10, -8, 12, 18, 1, 0, 2, -9, 15, 1, 7, -24, 13, 3, 8, -6, -7, -23, -8, 2, 9, -4, 21, -8, -4, -5, 16, 11, 4, 5, -4, 4, 0, 8, -30}
}
}
, {{{0, -1, -13, -6, -3, -14, 14, 14, 2, 5, 3, 5, 6, -13, 0, 1, -1, -4, 14, -22, 6, -10, -8, -8, -10, -10, 20, 11, -5, 4, -17, 3, 12, -7, 3, 4, 3, 12, -3, 3, 0, 7, 3, 12, 5, 1, -25, -2, 16, 2, 6, 4, -6, 3, 20, 4, 9, -14, 16, 3, 7, -1, 3, -18}
, {9, -9, -4, 2, -22, -2, -4, 4, -1, 8, 19, 1, -3, -30, -4, -11, -9, -5, 12, 11, 6, -11, 0, -15, -6, -8, 0, 3, 5, 4, 4, 1, 2, -23, -14, 3, 10, -24, -15, -9, 12, -6, -7, -8, -11, -6, -5, 9, 3, -3, 6, 3, -10, -6, -10, -1, -3, -4, 12, 3, 0, -8, 3, 7}
, {-11, 0, -1, -5, -6, -2, 2, 7, 6, 9, 18, -18, -13, -6, -10, 14, 6, -12, 18, -8, -8, 3, 8, 7, -3, 2, -20, 11, -30, -19, 4, -19, -17, -12, -3, 4, 7, 0, 7, -13, -18, 0, 8, -19, -21, -20, -4, 6, 8, -10, 4, 23, -14, 3, 0, 15, 22, 4, -7, -6, -3, 2, 6, -7}
}
, {{-5, -12, 4, -20, 10, 16, -4, -10, 4, -8, -23, -7, -11, -12, -11, -6, 4, 10, 0, -8, 12, -13, -5, -14, -21, -3, -3, -26, -5, 11, 8, -2, -1, -1, 12, -5, -1, 0, 3, -10, 25, 2, 3, -5, -1, 17, -4, 11, 2, -3, 7, 16, 8, -2, -2, -20, -4, 20, -4, -4, 2, 11, -4, -9}
, {-5, 5, -6, -5, -8, -20, 20, 20, 4, -6, 0, 19, -9, -11, -13, -18, -13, 1, -4, -2, -5, -32, -4, -10, 2, -10, -6, -40, -5, 19, -19, -13, 21, -9, -30, 21, -10, -4, -6, 9, 11, 13, -21, -13, 13, 1, -22, 12, 4, -22, -6, -29, 0, 7, -3, -18, 19, -32, 5, 0, 1, -4, 0, -7}
, {9, -9, 14, 3, -3, 1, 6, 2, 1, 0, 3, -8, 0, -14, 20, -18, -16, -12, -7, 6, 9, -6, 12, -7, -1, -11, 16, -2, 7, -8, -6, 1, -20, -1, -2, 12, -18, -5, -4, 2, 15, -18, -9, 6, -26, 22, -18, -8, 0, -21, -7, 2, -12, 4, -5, -11, -11, 5, 6, -2, 0, 3, -9, 30}
}
, {{4, 2, -23, -6, 11, 7, -14, 10, -6, -2, -2, -4, 3, -6, 10, -1, 8, -26, 6, 1, -2, -4, 7, -22, 1, -7, 3, -17, -4, 0, 14, 0, -8, -4, 12, 32, -2, 1, 7, 3, -9, 1, -7, -5, -14, 0, 13, 3, -4, 3, 19, -4, 0, 15, 15, 18, -2, 5, -1, 4, -18, 14, 16, -1}
, {31, 6, 2, -12, 1, 0, -9, -6, -15, -5, 3, 8, -8, -17, -9, 13, 23, -5, 0, 24, 16, -6, -23, 23, 17, -11, -6, -18, 9, -16, 21, 6, -22, 14, -7, -3, -3, 8, -19, 7, -8, 0, 5, 6, 20, 18, 24, 24, -13, -7, -9, 7, -8, 4, -5, 7, -14, -2, 9, 17, -6, 4, 6, 10}
, {15, -7, -15, 7, -7, -2, -24, 20, 5, -5, -25, 13, -25, -2, -12, -15, -14, -8, 7, -9, -17, -28, -14, -16, 11, 7, -4, -3, -2, -3, 2, -8, 19, -23, 3, 4, 9, -10, -1, 22, 46, -11, 4, -20, 2, -5, 9, 0, -1, -3, -12, -1, -11, 9, -3, -10, -8, 13, -7, 17, -11, -3, -3, 9}
}
}
, {{{1, 6, 8, -3, -17, 20, 7, -5, 16, 0, -7, -9, 3, 16, -5, -7, -9, 12, 18, 5, -2, 11, -7, -3, 1, -15, 10, -2, -10, -1, 6, -17, 11, 1, -13, 28, 17, 20, -22, 1, 13, 18, 4, -1, 2, -14, 0, -14, -5, 8, -16, -13, 9, -3, 8, 2, -16, 0, 8, 9, 1, -12, 5, -2}
, {-12, -4, -2, 2, -8, -15, 8, 3, 3, -6, 5, 6, -5, -27, -22, 10, 11, 3, 6, -13, -4, 7, -6, 8, -11, -4, -6, 3, 17, -3, -4, 6, -2, -8, -3, 11, 6, 0, 15, -1, 23, -10, 2, 1, 8, -7, -10, 4, 11, -5, -16, 10, -4, 1, 4, 2, 15, 1, -3, 7, -6, -15, -6, 5}
, {-1, -21, -11, 25, -9, 15, -3, 18, 4, 14, -5, -1, 11, -1, 1, -15, -3, 21, -9, 6, -30, 6, -19, -1, -7, 10, -10, -15, -5, 3, 20, 16, -7, 1, -6, 3, -4, 7, -4, 22, 9, -8, 4, -14, 16, 4, 0, -3, 0, 5, 2, -16, -3, 16, 9, 4, -8, 18, -6, 1, -11, -11, 9, 11}
}
, {{-12, 17, 7, -1, 21, 2, -6, 10, 0, -41, -7, 6, -11, 5, -12, -9, 2, 5, 4, -12, -16, -1, -11, -4, -8, 4, 1, 16, 10, -14, -13, 21, 1, 5, 5, -1, -11, 0, -16, 3, -11, 0, -8, 7, 11, -20, 1, 7, -13, -1, 0, -6, -3, -2, -6, 22, -18, -1, 6, -5, 0, -11, -10, 2}
, {2, 17, 22, 14, -15, -22, 11, -10, 10, -30, 6, -19, -35, -11, 0, 9, -15, -15, 9, -29, 18, -14, 13, 2, -1, -9, 1, -24, 15, 12, -3, -18, -6, -6, -8, -5, 2, 11, 9, 20, -25, 24, -14, -6, -25, 4, 9, 2, 5, 19, -2, -30, 0, -15, -3, 23, -13, -19, 10, 2, -28, 15, -21, -17}
, {8, -18, -3, 32, -2, -29, 26, -11, 19, -11, 11, -4, 14, 18, 18, 6, -8, 11, -12, -7, 20, 10, 2, 3, 0, 6, -4, -18, 10, -14, 3, 16, -19, 15, -1, 17, 2, 1, 6, -8, 17, 1, 8, 2, -12, 0, -6, -22, 8, -3, -5, 9, 17, -13, 1, 20, -2, 1, 15, 18, -4, 7, -3, 18}
}
, {{-1, 3, -18, -7, 6, 6, 6, -29, 17, -6, 8, -19, -10, 21, -11, 6, -18, -3, 11, -3, 8, 11, 17, 8, -4, -3, 4, 13, -6, 8, -21, -23, -4, -9, -4, -9, 10, -5, 15, -12, -23, 6, -15, 3, -10, -2, 0, -7, 25, -3, 6, 8, -8, 8, -9, -8, -17, 1, 11, -10, -4, 11, -14, -1}
, {-3, -4, -38, 18, 5, -9, 5, -5, -29, 5, -13, 9, 6, 10, -2, 16, 7, -6, -7, -4, -3, -9, 1, -11, 2, 16, 17, 4, 8, 10, -6, -18, -2, -19, -16, 11, -3, 5, 10, 6, -1, 4, 12, -20, -13, -18, 0, -28, -1, 8, 12, 9, 5, -18, 12, 13, -1, -5, 10, 3, -6, 7, 0, -15}
, {17, 10, -13, -2, -2, -24, 8, -12, -8, 14, 6, 18, -4, 5, -18, 17, -18, -16, -6, -3, -8, 8, 23, -7, -5, -1, -3, 10, 12, 2, -6, -13, 8, -9, 9, -11, -12, 8, -1, 11, -8, 17, -18, -25, 11, 12, 9, 5, 7, 2, 11, -8, -2, -4, 5, 0, -6, -6, 20, 9, 7, 6, 10, -19}
}
}
, {{{14, -25, -15, 5, -17, 3, 10, -25, -15, -14, -1, -4, -5, 6, -3, 2, 6, -5, 5, 12, 8, -12, 6, -27, 3, -19, 8, -5, 13, -6, -6, -12, -15, -12, -13, 7, 15, -9, -5, -32, 10, -1, -8, -8, 5, -19, -16, 4, 13, -4, 3, 4, -5, 6, 12, -5, -8, 6, -23, -17, -17, -5, 25, -2}
, {-1, 2, 3, 7, -2, 1, 8, -5, -33, 14, -8, -5, 3, -12, -1, -4, -14, 3, 6, 1, 14, -8, -6, -1, 1, 10, 0, -6, -9, -3, -3, -4, 10, -6, -16, -14, 23, -16, -10, -12, -1, 11, -21, -23, -11, -13, 8, 4, 3, 1, 14, -3, 17, -26, 15, 0, 8, -8, -7, -7, -1, -4, -5, 5}
, {3, -12, 7, 3, 3, 2, -2, -5, -4, 6, 1, -15, -5, 3, -4, -9, -25, 11, -20, 6, 6, -8, -3, 14, -9, 20, -7, 5, -14, 14, 12, 8, 5, 5, 0, 6, -15, 1, -6, 6, 3, -1, -12, -5, -14, -5, 14, 2, 2, -5, 1, -5, -13, 10, 10, -10, -12, 1, -5, 2, 6, 9, 4, 0}
}
, {{-13, -10, 17, -7, -7, 24, 5, 5, -13, 2, -11, -10, -10, 2, -17, 2, -1, -4, -16, -22, -1, -1, -25, -5, -10, -24, -21, 0, -8, -2, -13, -13, 8, -7, -17, -11, -1, 10, 1, 4, 6, -6, -12, -5, -12, -10, 17, -5, 2, -29, 1, -3, 6, -1, -2, 1, 5, -5, 4, -3, 15, -3, -10, -4}
, {-19, -5, 20, -8, -8, 6, 13, 11, -10, 17, 33, 48, 2, 8, -16, -18, 5, 14, -3, 6, -15, -14, 21, 2, 9, 7, 20, -25, -45, 1, 39, 6, -1, 0, 5, 20, -28, 3, 5, 4, -29, 2, -32, -6, 21, -4, 0, 10, -24, 15, -26, -55, 21, 9, 13, -27, -17, -10, 40, -13, -17, -7, -36, -34}
, {-21, 13, -12, 17, -17, -13, -3, 17, 1, -32, -24, 19, -13, -5, 15, -16, -24, -4, -24, -9, -2, -6, -9, -14, 8, -10, 11, 7, 20, -4, 14, -9, -2, -4, 13, -3, -10, -1, 3, 18, 31, 4, 9, 18, -15, -2, 6, 6, 14, -2, 1, 7, -12, -7, 11, 2, 3, -5, -15, -15, -5, 12, 19, -8}
}
, {{-17, -37, 4, -15, 18, -10, -6, 5, -10, 10, -6, 13, -18, 6, -14, -11, -2, -34, 9, -6, -6, -1, -12, -5, -20, 13, -8, -2, 8, 11, -8, -12, -14, -8, -4, -1, -4, 11, 10, 21, 2, -8, -20, -19, -3, -9, 6, -19, 22, -9, -6, 11, -1, 25, 18, -4, -7, -9, -16, 8, -2, -6, 10, -6}
, {-26, -29, -29, 13, 20, 8, 3, 15, 7, -6, -16, 9, -33, 18, 10, 10, 10, 15, -2, 23, -24, -4, -11, 6, -15, -5, 3, -5, 13, -18, 2, 16, 21, -12, 29, 30, 16, -15, -8, -5, 28, -43, 19, -26, 14, -18, -2, 12, -9, -18, 25, 22, -6, -14, 2, -8, -5, -6, -9, -28, -20, 14, 13, 11}
, {-23, -5, 5, -3, 1, 7, -2, -13, -12, 10, 6, -10, -13, -7, -3, -3, 10, 3, 5, -3, 10, -15, 8, 24, 19, 19, -7, -43, 0, -8, 2, -20, -13, -17, 6, 0, 3, 9, 1, -27, -1, -1, -4, -11, 9, -9, -2, 9, 4, 3, 3, 10, 12, -1, -8, 0, 25, -6, 10, -26, -18, -4, -2, -3}
}
}
, {{{4, 19, -10, -13, 14, 1, -1, 15, -9, 7, 12, 1, 5, 7, 15, 2, 10, 13, 4, -20, 3, 14, -8, 5, 13, -4, -14, -8, 17, -21, -6, -5, 19, 1, 12, 10, -6, -5, 2, -9, -9, -14, 5, -1, 13, 1, 19, -20, -20, -4, -7, 2, -1, 3, 7, 3, 0, 14, 4, -2, -2, 12, -6, 4}
, {-13, 16, -12, -15, 4, 0, 0, -6, 2, 4, 20, -3, -11, -14, -10, -17, 0, -10, 8, -1, -15, -9, 4, 7, -13, 4, -7, -15, 0, 1, -9, -12, -9, 5, -12, 2, -30, 15, -6, -10, 4, -8, -26, 11, 4, -15, -17, -1, -12, -9, 12, 0, -10, 2, 6, -8, -5, -4, -2, -12, 6, 3, -2, 13}
, {-10, -3, 1, 8, 3, -8, -25, 7, 9, 4, 13, -6, -11, -27, 1, -11, 13, 0, 5, -18, -12, 9, 2, -6, -9, 3, -1, -2, 9, -8, -4, 12, 0, -7, -8, 5, -6, -15, -3, -4, 6, -6, -19, -13, 1, -15, -11, 7, -18, 28, -22, -20, -5, 9, 14, 23, 9, 15, -3, 3, -7, -13, 9, 19}
}
, {{-2, 7, 9, -22, 2, -4, 6, -24, 15, -6, 17, 5, -4, -11, 4, -24, 10, -3, 21, -23, -18, -18, -9, 14, 13, 2, 0, 6, 5, -17, -1, 23, -22, 6, -1, -8, 4, 10, 4, 3, -9, -15, 4, 14, 0, 14, 14, 4, -19, -4, -15, -1, -1, 7, -14, 20, 7, -8, 0, -21, 11, 16, -10, 6}
, {24, 8, 8, 10, -2, -3, -12, 2, 17, -38, -21, -11, -13, -12, 20, -4, 11, 18, -10, -9, 17, -39, -12, -16, -13, 6, -27, -22, 6, 14, -7, 23, 1, 8, -2, -11, -13, 10, 12, 11, 9, 4, 11, -8, -28, 33, 14, -10, 3, 2, -7, -21, -14, -13, -18, 16, 3, 11, 19, 12, 5, 1, -7, 9}
, {-18, 0, 8, -13, 6, -9, 1, -12, 11, 2, 7, -14, 19, 3, 33, 4, 7, -21, -3, -4, 25, -3, 7, -19, -2, -16, -2, -14, 3, -1, -9, -25, -2, -1, -13, 1, -1, 9, -24, -2, -25, 17, -16, -12, -15, 1, -1, 2, -8, 1, -10, 1, -5, 2, 3, 1, -9, -9, -5, 2, -13, 22, -4, -21}
}
, {{15, 2, 9, 2, 1, 10, 2, -21, -15, -10, -7, 9, 15, 2, 1, 9, -8, 7, 10, -35, -1, -3, -7, -6, 14, -1, 11, 9, 2, -4, -2, 6, 3, -5, -7, -7, -14, -3, 17, 4, 0, 2, -5, -11, 1, -12, 1, -19, 1, -5, -7, -4, 11, -3, -11, 1, 5, 9, -1, -9, 23, 14, -11, 8}
, {5, 4, -22, 11, -7, -6, 2, 3, -3, 7, -1, 1, 11, 11, -9, -10, 4, -1, -23, 15, -23, 2, -6, -15, -3, 14, 5, 3, 7, -2, -3, -1, -10, 5, 3, -2, -11, -6, -4, 4, 2, -17, -20, 4, -25, 9, -11, -3, -31, 17, 4, 13, -12, 8, 10, 1, -3, 2, 1, -1, 1, -2, 7, 2}
, {5, -3, -21, 8, 15, 6, 16, 6, -13, 29, -11, 16, 9, 18, 7, -13, 9, 3, -12, -7, 1, -2, 10, 12, -18, 14, 14, -4, 11, 11, -1, 5, 12, -3, -4, 14, -2, 6, 1, -7, 5, -14, -3, -15, -1, 12, 6, -13, -10, -11, -8, 2, -3, -16, 22, -13, -16, -4, -8, -6, -4, 14, -9, 13}
}
}
, {{{4, -12, -10, -7, -12, 1, 7, -10, 14, -7, -15, -1, -17, 9, -8, 3, 6, 18, -7, 14, -18, 3, 4, -10, -5, -25, 2, 10, 4, 6, -14, -9, -10, 11, -5, 13, 14, 2, -5, -10, 24, 9, 13, 3, -3, 3, -6, 14, -2, 5, 8, -8, -1, -1, 14, -1, -4, 1, -14, 1, 3, -11, 15, -16}
, {3, 6, 1, 4, -11, 5, 10, 11, -8, -2, 10, -17, -15, 6, 1, 10, -1, 9, 10, -7, 6, 3, -3, 9, -3, -6, 9, 5, -32, -15, -3, -4, -11, -1, -12, 9, 15, -9, -1, 9, 9, 9, -13, -8, 0, -17, 2, 16, 6, -9, -9, -4, -10, -6, -1, -2, 16, 3, 20, -17, -11, 1, -7, -7}
, {6, -26, -28, -14, 7, 25, -9, -13, -14, -6, -16, -3, -23, -4, -3, 2, 10, 20, -6, 15, -15, 8, -12, -12, -14, 24, -14, 20, -4, -14, -3, 1, -15, -5, 12, -14, -8, -14, -1, -10, -1, -17, -21, 9, 19, 17, -10, -13, -13, -9, -3, 0, -28, 3, -1, 4, -4, 2, -10, -13, 18, 11, -25, 14}
}
, {{-17, 5, 6, -5, 11, 6, 4, 17, -2, -11, 6, -2, -8, -4, -6, 3, -3, -15, -5, -10, 3, 6, -13, -5, 4, -17, 19, 5, -14, -8, -15, -30, 4, -1, -9, 4, -28, 0, 3, 1, -2, 26, 10, 7, 0, -12, 27, 4, -9, 34, -5, 7, 14, 1, 10, 16, -2, -6, 10, 6, 3, -7, 17, -17}
, {9, -5, 4, 13, -12, -2, 25, -15, 0, 23, 9, 0, -12, 5, -13, 7, 4, -3, -18, -15, 4, -15, 10, 10, -13, 15, 3, -2, 5, 14, 16, -9, 8, 4, -1, -15, -20, 4, -19, 19, 8, -6, -18, -6, -2, -11, 10, 16, -6, 16, 8, -23, 6, 17, 0, 26, -9, -7, 24, 24, -5, -2, -16, 12}
, {-4, 23, -6, 8, 4, -29, 6, 11, 16, -16, 2, 12, -12, 6, -5, 9, -4, 9, -33, 9, -12, -2, 5, 17, -10, 11, -9, 3, -10, 1, 7, -18, 9, 15, -2, -10, -30, 40, -7, -9, 3, -4, -16, -3, 18, -10, -3, 11, -1, 8, 7, -12, 12, -2, 8, -2, 5, -3, 7, -5, 3, 7, -8, -7}
}
, {{21, -24, 9, -4, 3, 12, -5, -15, -6, 16, 10, 3, -1, 16, -12, -13, -6, -11, 20, -17, 2, 5, -11, 6, -11, 13, -3, -3, 2, -5, 8, -2, 11, -2, 1, 7, 5, -6, 0, 1, -7, -21, -12, 15, 12, 11, 15, -12, -6, 4, 4, 9, -15, 8, 14, -4, -7, 17, -15, 7, 13, 19, -9, 18}
, {-4, -7, -5, -16, 22, -6, 2, -3, 13, -14, -8, -4, -4, 5, 5, 14, 0, 0, 16, 8, -11, 16, 6, 4, 15, -18, 8, -2, -6, -1, -12, -10, 11, -9, -22, -3, 6, -29, 5, -7, -3, 4, -20, -16, -1, 2, -22, 3, 24, -23, -3, 20, -6, -24, -28, -5, 14, -4, 16, -4, 12, 9, -1, -3}
, {-16, 3, -12, 5, -12, -7, -9, -7, 9, 0, 11, 0, -9, 14, 9, 5, 6, 15, -1, 22, -29, 23, 1, 12, -29, -14, -6, 14, 8, -2, 11, -4, 2, 8, 6, -7, -10, 9, 18, -11, 14, 1, 17, -12, -7, 4, -7, 10, 0, 1, -7, 13, -11, -4, -2, -1, 2, 23, 19, 1, -4, -21, 5, -4}
}
}
, {{{7, 7, 0, -7, -3, -9, 2, 16, -7, -4, 8, 1, -13, 1, 3, -4, 1, -16, -6, 5, -1, -26, 10, -8, -26, 16, 3, -11, 3, -5, 16, 17, -14, 0, -10, -15, 12, -12, 1, 2, -10, 2, -7, -12, -13, -9, 6, 12, 11, -8, 13, 14, -7, -3, -12, -3, 6, -5, -5, -3, -5, -6, 1, 4}
, {4, 18, -2, -4, -2, -6, 7, 0, -12, -6, -2, -1, -5, 15, 18, -5, 16, 16, -5, 0, -22, 13, 11, 5, -22, 4, -11, 13, 8, -3, -6, -6, -1, -6, 9, -7, 3, -16, 8, -6, -13, -1, -3, -3, -10, 17, 10, -10, -13, 4, -14, 7, -3, -8, -2, 16, 2, 12, -8, -19, 7, 1, 5, -2}
, {-5, 5, 22, -2, -29, 13, -14, -1, 8, 0, 5, 6, 9, -20, 12, -2, -7, 20, -1, -7, 11, -4, -1, 21, -2, 0, -1, -15, -10, -23, -6, 2, 1, 3, -5, -1, -3, 1, -4, 4, -3, -12, 6, -8, -1, -13, -5, 4, -25, 1, 2, -15, 8, 12, 8, 12, -1, -14, 3, 21, -33, -10, -12, -8}
}
, {{-1, 7, 2, -14, -14, 9, -2, 15, 15, -9, 15, 1, -3, 7, -14, 8, 9, 23, 0, 3, 2, 0, -1, -18, 6, -8, 10, -16, 6, 1, 7, -19, 2, 9, 10, 25, 5, 6, 17, -12, 6, -14, 2, -22, -22, -13, -1, -21, -6, -25, 5, -10, 10, -13, 7, 6, -11, 8, 7, 2, -18, -5, 17, 7}
, {-19, -9, 12, 2, 9, 2, -15, 1, 29, 9, 10, 0, -35, -20, 4, -20, 14, 12, -19, 11, -14, -11, 13, 18, -16, 21, -8, 10, -11, -6, 24, 13, -40, -14, -14, 29, -3, -26, 1, -15, 1, -20, -7, -15, -15, -30, -16, -7, 3, -18, 2, -1, 0, 26, 13, 11, -1, -28, 29, 4, -37, -19, -28, 0}
, {12, 14, -10, -33, 5, -10, -8, 0, -30, 2, -21, 10, -16, 18, -8, 5, 21, -27, 2, -8, 9, 4, -7, 8, 9, 9, 5, 14, 15, -12, 7, 0, -1, -4, 0, -2, 5, 23, 9, 2, -14, 15, -10, -12, -9, -8, 4, -14, 3, -5, 13, 30, -2, -10, 4, 7, 9, -9, -15, -8, 14, 2, -8, -18}
}
, {{-15, -2, -2, -1, 15, 9, 8, 6, -13, 6, 12, 2, -16, -12, 2, -17, 14, -18, -14, 11, -2, -14, 4, 4, 5, 9, -9, -10, 19, -4, 7, -6, -4, -7, -12, -12, -9, -23, -16, -6, -11, -12, -6, 4, -12, -26, -10, 6, 21, -2, -5, 10, 2, 1, -1, -26, 7, 8, -18, -5, -21, -21, -22, 16}
, {-5, 5, 0, -3, 21, 16, 15, 8, -11, -7, 3, 9, -11, 12, 18, -20, -1, 8, -4, -16, -4, -11, -25, -13, 11, 8, 1, 1, 17, -23, -23, 2, -1, -6, 11, -2, 4, -10, 1, 9, -15, 1, -6, 4, 9, 5, 13, -31, 5, 8, -7, 33, -12, -3, -3, -13, -3, -9, -7, 1, 1, 9, 4, 8}
, {-16, 12, -2, -10, -7, -21, 13, 5, -18, 7, -6, -6, 3, -16, 13, 1, 5, 18, -4, -2, -9, -15, 15, 15, -20, 6, 4, -38, -11, -5, -4, -5, 9, 9, -12, -14, -7, 10, 16, -3, -5, 0, 0, -17, -4, -14, -3, -5, -6, 13, 2, -5, 7, -5, -1, -18, 5, 0, 19, -5, -7, 4, -22, -6}
}
}
, {{{-1, -13, -1, 13, -14, 1, -7, 4, -14, 7, 9, 2, -5, -6, 11, 2, 4, -3, -16, 4, 5, 3, -9, -6, 1, 6, -4, 6, -10, 10, 9, -5, -14, -5, 11, -2, -11, 4, 6, -8, 3, 21, -21, -9, -7, -8, -12, 8, 8, -5, -1, 0, 18, -9, -10, 12, -11, -7, -6, -14, -9, -3, -2, -4}
, {5, -6, -5, 12, 1, 1, -6, -3, -7, -5, 4, 5, -7, 5, -7, -5, 0, 15, -3, 11, 2, 9, -1, -1, -22, 17, -1, 9, -12, 19, 5, -7, 10, -10, 0, -21, -10, 3, 4, -4, 5, -13, -16, 0, -7, 6, 10, 1, 9, 12, -3, 1, 4, -2, -3, -1, -4, -4, -3, -7, 4, 7, -1, -7}
, {1, 9, 4, 11, -20, -1, 1, -2, -7, 5, -14, 15, 2, -9, 10, -11, 7, -2, 8, 8, -2, -13, -1, 0, -15, 10, 3, -3, 15, -8, 0, 16, 3, 12, -2, 6, 0, -7, 3, 16, 3, -4, 4, -1, -22, -6, 7, 8, -13, -13, -1, -6, -3, -5, -9, -10, -8, -5, 0, 5, -4, 5, -7, -1}
}
, {{-8, -17, 8, 22, -20, -7, -32, -14, 3, 2, -3, 21, -12, 19, 3, 0, 16, 4, 12, -31, 22, 33, 6, -10, -2, 1, -24, 14, 20, -15, 25, -2, -2, 3, 14, 18, 0, 16, 2, -16, 18, -30, -22, -22, 6, -28, 14, 0, -17, -8, 26, 4, 1, 2, 18, 14, 13, 13, 2, 15, -15, 16, -10, -5}
, {-52, -19, 18, 12, 7, 1, 20, 14, 37, 34, 27, -14, -28, -26, -30, -7, 24, 26, -14, 20, -52, -29, 2, 48, -11, 4, -24, 20, -41, 17, 39, -5, -13, -26, -54, 29, 16, -23, -8, 8, -8, 7, -22, -4, 13, -47, -54, 13, 17, -10, 13, -13, 2, 30, 23, 6, 22, -17, 14, -15, -32, -50, -34, 13}
, {8, 9, 6, -6, -7, 18, -7, -10, -18, -3, -5, -2, -10, 8, -4, -6, -4, 11, -5, -12, -5, -10, -1, 16, -1, 14, 0, 3, -3, -7, -5, -12, -9, -12, -36, 17, -1, 6, 10, 8, -10, 3, 3, 19, -11, 16, -6, -28, -12, 29, 5, -4, 2, -10, 1, 16, 4, 0, 32, -8, -15, -15, 11, 0}
}
, {{-12, 8, -9, -4, 7, 1, 4, 5, -2, 4, -2, 10, 12, -15, 13, 0, 6, 4, 1, 13, 4, -5, 7, -11, -6, 14, -10, 2, 11, -11, 6, -3, 3, 5, 9, -6, 2, -2, -23, 2, -7, 3, -5, 0, -8, -6, 5, -10, 2, 2, -14, 0, -7, -12, 1, -11, 0, -15, 0, 0, -3, -13, -3, 3}
, {-23, -2, -8, 0, -15, 18, 8, 11, -13, -12, -4, 12, -11, 1, 20, 2, 17, 2, -12, -4, -30, -17, -6, -12, -19, 4, 7, 3, 8, -11, -11, 4, 2, -5, -2, 7, 13, -13, 12, 8, -9, -19, 4, 4, -1, -31, -15, -5, 19, -15, 13, -1, -11, -26, -8, -9, -18, 4, 2, -10, -21, -2, 2, -5}
, {0, -7, -14, -6, -6, -6, 4, -10, -24, -3, -4, 4, -25, 4, 13, 4, 25, -12, 2, -4, 18, -46, -13, 12, 0, 6, 22, -2, 4, -5, -13, 25, -22, -17, -18, -9, 1, -4, 0, -17, -3, 17, 11, -1, 12, 14, -1, -22, 8, 13, 7, 10, -13, -9, -7, -8, 4, 8, -7, -7, 3, 6, 8, -6}
}
}
, {{{1, -13, 7, 0, 8, -1, 14, -21, -9, -7, -9, -17, -13, 0, -14, 4, -16, -3, -16, 19, 5, 11, 3, 6, 5, -3, -10, 0, 16, 0, -8, -3, -1, -6, 12, -10, -1, 2, -1, -9, 0, 3, 8, 11, 9, -7, 0, 7, -9, -11, 6, 6, 6, -25, 4, -12, -12, -14, -18, 3, 1, 1, 10, -18}
, {-4, -1, -1, 8, -11, -17, 1, -15, 16, -15, -9, 1, 1, 2, 2, -22, -8, -19, 17, -5, 0, -27, -7, -8, 1, -12, 7, -3, 6, -7, 13, 6, -3, -3, 24, -7, -31, 24, -5, 11, 3, 7, -7, -7, -16, 19, -8, -9, 20, -10, -17, 12, -20, 5, -13, 23, -8, -2, -18, -11, -1, 1, 17, -11}
, {-4, -24, -8, -7, -8, -13, -2, 7, 5, -3, -10, 13, 5, 9, 20, -10, -8, 2, 18, 2, 8, -15, 4, -16, -3, 5, 6, -23, 8, -4, -6, 19, -10, 25, -17, -20, -5, 15, -5, 8, -12, 18, 6, -14, -32, 7, -9, -10, -18, -1, -7, 16, -5, -7, -15, 0, -13, 6, -16, -3, -8, 7, -1, 5}
}
, {{-26, 10, 2, 2, -1, -16, 6, -5, -9, 13, 1, 8, 3, -28, -12, -41, -17, 4, 0, 0, -17, -7, 1, 22, -3, 23, -19, -27, -7, 16, 18, 3, -11, 6, -1, -8, 15, -4, -27, 31, 20, 8, -4, 8, 6, 9, 1, -1, -16, -10, -6, -7, 2, 15, -1, 7, -1, -12, 7, -10, 0, 0, -10, -13}
, {-23, -12, -11, 4, 8, -21, 36, -20, 8, 4, -15, 29, -10, 1, 35, 6, 9, -14, 14, -14, 11, -22, -7, 5, -15, -11, 20, 1, -29, -11, -24, 0, 0, 1, -14, 2, -12, 11, 17, -13, -26, 20, -24, 9, -2, 27, -7, -3, -16, 15, -39, -9, -12, 29, -11, 12, 23, -12, 6, -9, 13, 11, -30, -23}
, {-7, -8, 6, 6, -14, 3, 0, -8, 4, 4, -6, 21, 19, 1, 40, 10, -13, 10, -4, 5, 2, -3, -16, -13, 5, -13, 5, -1, 8, 12, 12, -27, 4, 27, 12, -3, -35, 6, -1, 2, -13, 6, 12, 11, -15, 23, 7, 4, -2, 14, -7, -21, 0, 4, -12, -1, 2, 15, 15, -1, -15, -4, 10, 5}
}
, {{0, 13, 6, -6, -14, 10, -11, 4, 9, 3, 6, 4, 10, -11, -11, 2, 2, -1, -7, -8, -21, -11, -1, 7, 20, 22, -29, -13, 7, -5, 16, 2, -6, 7, -4, 3, 23, 1, -8, -5, 4, -13, -4, 8, -4, 10, 0, 0, -8, -7, 12, -8, 11, -8, -1, -3, 1, -2, 9, -7, 14, -1, 1, -1}
, {7, -33, -22, -8, -5, 11, -18, -19, -6, -10, -7, 12, -4, -21, 29, 3, 9, -1, 4, 10, -7, -12, -5, 13, -45, -8, -4, 7, -7, -20, 2, 2, -18, 8, -8, 1, -1, 6, -18, 5, 25, 4, -2, 2, -12, -12, 13, 9, -3, -13, -14, 7, 3, 8, 4, -4, 4, 9, -3, -17, 5, -8, 1, 16}
, {-16, 5, 15, -16, 3, 3, 19, 9, 18, -5, 1, -20, 22, 10, 2, -18, 12, 17, -3, -5, -5, 8, -2, 10, 10, 5, 3, 4, 6, -3, 7, 0, 12, -20, -6, 4, 10, -22, 5, 16, -11, -2, 2, -7, -12, 10, -3, -1, 1, -14, -8, 3, 3, -4, 19, -6, -10, -10, -5, -7, -13, -6, 8, 17}
}
}
, {{{-3, -12, -4, 8, -5, 3, 9, 0, 4, -29, -10, -22, -1, -7, 11, 1, -8, 19, -4, 23, 10, 3, -5, -12, -5, 9, 11, -13, 0, -4, 3, 5, -1, -10, -2, 15, 8, -6, -11, 3, -3, 3, 2, -14, -12, 9, -12, 11, 10, -16, 5, -19, 10, -19, -1, -8, -7, -4, 17, -12, -27, 0, -15, -11}
, {-7, 1, -4, 4, -10, -8, -4, -3, -11, 13, 3, 4, 7, -6, 6, -17, 2, 5, 9, -5, -4, -15, 0, -8, -2, -10, -2, -2, 4, -12, 7, -24, -15, 4, 2, 16, 0, 2, 26, -2, -18, 5, -11, -7, 3, 9, -5, 9, -1, 6, 6, 5, -3, -6, 8, -7, -5, -2, 7, -6, -2, -4, -3, -11}
, {-10, -23, 1, -15, 1, 10, -7, -5, 6, -8, -9, 11, -12, -4, -3, -22, 9, -2, -12, 11, -12, -10, 12, 0, -14, 1, -6, 2, -10, 13, 5, -16, 1, 3, 5, 16, -5, 6, 1, 3, -2, -17, -34, -5, -22, -2, 13, -14, 9, 1, 2, -11, -15, 8, 2, -3, -6, 5, -1, 2, 12, -1, 1, 2}
}
, {{-4, 9, -6, -14, 0, 0, -1, -8, -19, -1, 4, -10, -17, 2, 0, -3, -2, 2, 17, -8, 2, 13, -2, -2, -11, -8, 10, 4, -14, -3, -5, -5, 7, -2, 16, -23, 9, 4, -9, 4, 14, 20, -24, 9, 2, 16, -2, 18, 3, -3, -8, -4, 1, -14, 2, 4, -5, 2, -14, 0, -1, 4, -12, -13}
, {0, 9, 1, -6, -6, 11, -22, -10, -15, -19, -14, 26, -21, 9, -1, 14, 13, 12, 0, -21, 5, 11, -28, -21, 10, 13, -5, 4, -1, 4, -31, 8, -13, -10, -6, -2, -14, 1, 14, -12, -10, -31, 21, -8, 2, -2, -2, -14, -17, -15, -11, 5, -5, -7, 8, 50, -9, -7, 2, 3, 13, 23, 2, 0}
, {-1, 13, -11, -8, -10, 10, -8, -14, -4, 12, -1, 19, -27, 11, -6, -8, 18, -7, 9, 3, -9, -25, 8, 29, -26, 12, -21, -8, -21, 2, 14, -14, -8, -3, -21, 15, 7, -8, -21, -21, 4, 3, -8, -15, -8, 11, -7, 9, -6, 4, 2, 0, 16, -12, -14, -6, 16, -3, 19, -15, -4, -3, -16, -27}
}
, {{-2, 9, 2, -13, 1, 15, -1, 11, -7, 3, 13, 3, -20, 4, -15, -1, 12, 2, -13, 4, 0, 0, 2, 14, 13, 7, -13, 4, 8, 5, 3, 1, -26, -16, 4, 13, -2, 14, 7, -4, -6, -3, 2, 9, -1, -4, 6, -1, 10, -3, -15, -1, 2, -10, -2, 1, -11, -2, 10, 7, 2, 9, -7, -5}
, {-13, -15, -4, -12, 20, -12, -7, -10, -12, -11, -10, 2, 4, 8, 13, -20, -9, 1, -9, -11, -17, 0, -16, 6, -9, 6, -6, -6, 2, 10, -7, -4, -22, -4, 3, -4, -4, 15, -7, -12, -5, -4, -4, -3, -1, -3, 0, -7, 1, 2, -8, 12, -7, 4, 4, -14, 4, 1, 1, -14, 10, 8, -2, -10}
, {2, 1, -4, 11, 3, 4, 2, 13, -13, 18, -35, -3, -19, 11, -12, -15, -2, 7, -10, 1, 4, -11, 5, 7, -11, 14, -23, 2, 9, -15, 15, 3, 16, 5, 10, -3, 2, 8, 15, 10, -5, -1, 15, -8, 3, -3, -13, 5, 0, 1, 18, 11, -5, -22, -5, -32, -9, 13, 7, 5, -17, 5, 1, -9}
}
}
, {{{17, -3, -12, -1, -13, -17, 3, -21, 17, -9, 8, 12, 4, 0, 14, -12, 8, -9, 2, -5, 14, -2, 0, -9, 1, -8, -9, 13, -3, -9, -8, 6, -18, 1, -9, -2, 6, -9, 1, 7, -2, 12, -5, -9, -11, 17, 4, -3, 10, -6, -3, -4, -5, -2, -5, 8, 10, -5, 1, -6, -2, 0, -7, 16}
, {12, -8, -9, 10, 11, -3, -5, -14, 17, 1, 1, -7, -3, 3, 1, -10, -1, -4, 9, 13, 12, 10, -4, 9, 8, -5, -14, -5, -15, 8, 13, 6, -1, 3, -12, -9, 11, -2, -12, -16, -12, -8, -4, 4, -14, 16, -21, -9, -7, -4, 29, 5, -13, -6, -9, 10, -13, -1, 12, -13, 8, 7, 5, -12}
, {6, 4, -2, -8, -32, -24, 2, 1, -1, -6, 1, 12, 6, -13, -6, 0, -3, -3, 48, -1, 16, 2, 2, -17, 3, 8, -13, 5, 23, -8, 12, -10, -2, 1, -18, -3, -16, 35, 13, -3, 6, 6, -3, 0, -8, 13, 0, 6, 21, -8, 8, 2, -1, 2, 12, 8, -5, -11, -4, 9, 10, 11, 16, -13}
}
, {{14, -6, -13, 5, 5, -17, -2, -16, -13, -4, -2, 14, 18, 0, -6, 6, 18, 17, -19, -9, 6, 0, -13, -7, -6, 11, 1, -11, 16, -2, 11, 17, -6, 5, -14, -8, 1, -15, 17, 10, 13, -21, 17, -6, 1, 36, -27, 5, -6, -13, -21, -1, -17, 4, 2, -7, 9, -6, 4, -15, -15, -27, -6, -18}
, {-23, -8, 30, 9, -5, 7, 2, 17, 8, -16, 15, -5, -5, -20, 5, -18, -3, 22, -25, 4, -44, -26, -17, -10, -1, 10, -27, -13, 10, 5, 37, 15, 13, 27, -2, -27, -33, 11, -9, 9, 21, -47, 16, 7, -12, -24, -60, -2, -18, -9, 9, -10, -8, 10, -9, 19, 24, -18, 3, 3, -1, -4, -5, 5}
, {-25, -18, -8, 7, -8, -5, -10, -6, -17, 13, -7, -8, -4, -14, 24, -2, 1, -20, -30, 13, -7, -29, 6, -11, -8, 0, -25, -13, -29, -16, -2, -10, -8, -8, -17, -24, -10, -1, -25, -8, -17, -10, -8, 6, -15, -2, -23, 4, -13, 14, 10, -10, -31, 0, -16, 0, -1, -6, -9, -1, 7, -40, -11, 13}
}
, {{3, -1, 10, -5, 0, 4, -13, 6, 2, 7, 2, -4, 17, -10, 4, -1, -9, 4, -9, 4, 3, 24, -4, -13, 14, -16, -3, -3, -7, -19, 5, 14, 14, 11, 17, -3, -1, -13, 6, -22, 5, 4, -2, 4, 0, 4, 10, 15, 2, -8, -5, -15, 2, 19, -12, -4, -13, -12, -16, -12, 2, 8, 2, 11}
, {-13, -9, -5, -2, -11, 23, -5, -18, -3, 9, -4, -13, 16, -14, -12, -37, 2, -2, -8, 0, -17, -17, 4, -11, 1, 20, -18, -7, 10, 9, 10, 9, 12, -23, -19, -1, -4, -23, 2, -26, 15, -27, -5, -10, -15, -2, 0, -7, 27, -39, 17, 24, -13, -11, -15, -5, -8, 20, -8, -36, -12, -14, -12, 24}
, {-4, -7, 11, 25, -10, 12, -2, 9, 10, -12, 2, -4, 7, 7, -6, -2, 9, 9, -2, -6, -12, -11, 11, -17, 2, -16, 2, 5, 1, 9, 8, 2, 5, -7, 2, -17, -3, 3, -6, -24, 14, -12, 11, -17, -9, -19, 4, 0, -11, -12, -3, -13, 23, 14, 9, 32, 5, 14, -5, -1, -13, -14, 5, -6}
}
}
, {{{-13, -4, 3, 10, -16, 10, 3, -15, -11, -2, -5, 0, -7, -1, 1, -15, -6, -16, 9, 3, 6, -7, -13, -22, -8, -7, -8, 17, -1, 3, -7, 1, -4, -6, -19, 11, 5, 2, -7, 6, 11, 15, -8, -8, 6, 5, -11, 9, 5, -2, 19, 8, -5, -13, 3, -14, 2, -10, 1, 11, 1, 1, 12, -2}
, {8, -19, 21, 14, -7, 11, 2, -8, 0, -4, 3, -6, 7, -1, -3, -10, 9, -4, -5, 5, 22, 3, 6, -16, -22, -4, 14, 9, 7, 5, -6, 13, -27, -7, -17, 2, 3, -9, -1, 1, 19, -27, -15, -12, -14, -1, -15, -4, 11, -7, 6, 3, -6, 9, 11, 9, 10, 3, -19, -9, -8, -18, -9, 4}
, {3, -5, -1, 11, -20, -3, 7, -2, 1, 21, -2, 4, -5, 17, 4, 1, 13, -5, -8, 15, 1, -5, -3, -2, -11, 15, -3, -3, 0, 13, -7, -26, -4, -6, -18, -13, -2, -18, 5, -19, 22, -4, -1, 12, 9, 3, -15, -13, 3, -13, 5, 15, -18, -5, 4, 5, 3, -12, 0, 4, -9, -1, 6, -1}
}
, {{-4, -10, 12, 0, -4, -1, 15, 12, -16, 1, -10, -12, 6, -5, -23, 10, 3, -2, 0, -3, 4, -14, -8, 0, -2, -15, -4, -14, 11, 12, 0, -11, 8, -17, -8, 7, -2, 8, 8, -3, 7, 0, -1, 3, 4, -11, 19, -1, -2, 5, 8, 10, 3, 6, 10, -18, 19, -2, 1, -6, 5, -4, 2, -7}
, {-2, -1, -33, -3, -18, -24, 1, 23, -2, 26, -12, 12, 11, 13, -11, -3, 26, -25, 13, -3, 0, -11, -25, 7, 15, -27, 21, 19, 1, 10, 6, -6, 2, -17, -7, 14, 0, -17, -9, 4, -12, 9, 2, -14, -1, -22, 0, -4, -2, -20, -23, 13, 0, -20, -16, -9, 14, -3, -1, -19, -7, -37, 21, 0}
, {11, -3, -1, 5, -24, -14, -10, -14, 8, 11, -3, 7, -4, -17, -2, 2, 8, -6, 10, -12, -15, -7, -1, -9, -7, -11, 4, 8, 0, -10, 3, -2, 2, -21, 6, 8, -16, -17, 10, 0, 13, -20, 16, 2, -17, 14, 1, -5, 4, -13, 9, 23, 3, -9, -10, -1, 8, 4, -15, -4, 12, 4, 9, -12}
}
, {{-12, -18, 2, -10, -13, 4, -2, 5, -31, -6, -11, -1, 5, 1, 5, -6, -14, -5, -2, -5, -13, 2, 2, -1, 0, 2, -12, -9, -1, 0, -21, -14, 4, 5, -2, -2, 18, 13, 1, -6, 1, -14, -22, 9, 3, 0, 10, -14, 12, -3, 21, -2, -12, 3, 4, 23, -6, -19, -23, 11, 3, 16, 4, 9}
, {23, -5, -17, -10, 16, -10, -3, -9, -21, 2, -20, 27, -16, 20, 22, 6, 11, -25, -5, 5, -10, 22, -5, -11, 9, -9, -6, 7, -6, 2, -14, -9, -1, 10, 4, 6, 0, 14, -33, 6, 5, -36, 8, 9, -11, 27, 20, -6, -3, 14, 14, -8, -6, -9, -14, -2, -22, 10, -15, -5, 9, 7, 12, 12}
, {-12, 0, 2, -21, -4, 1, 1, 0, 3, -1, 1, 18, 1, -9, 6, 5, 14, -27, 2, -2, 4, 4, 3, 14, 21, 3, 8, 1, -9, -5, -26, -9, 5, -11, -28, -15, 8, 1, 22, 8, -1, 19, -7, -6, 9, 5, -10, 1, 4, 6, -27, -17, -7, -9, 3, 8, -8, 0, 2, -5, -16, 17, 0, -8}
}
}
, {{{-8, 16, 10, -4, -11, -8, 4, 2, 14, 11, -5, -10, 5, -4, -11, 1, -9, -6, -9, 5, -6, -11, 0, -1, -12, 12, -3, 1, -2, -9, 11, 6, -8, -6, 11, -12, 1, 13, 5, -11, -10, -2, 7, -2, 11, 5, -5, 1, -19, -4, 6, -4, 5, -6, -10, -14, -1, 7, -6, -1, -15, 13, -3, -2}
, {9, -7, -7, -9, -23, 2, -22, -8, -1, -5, 1, -12, -9, -18, -12, 10, 6, -18, 18, -19, 8, -1, -16, 9, -10, 4, 18, 23, -1, 11, 5, -13, -23, -1, -4, -16, -13, 12, -5, 7, 14, 8, -12, 4, -10, 0, 4, 11, -26, -6, 3, -21, -10, 22, 9, -9, 6, -8, -19, 16, -6, -4, 3, 6}
, {9, 1, 15, 0, -12, -2, 7, -16, 12, -10, -14, 10, -1, -2, -13, 11, -9, -8, 14, 2, 9, -4, 1, 12, -11, 16, 11, -6, 3, 16, 1, -20, -4, -23, -7, 5, 14, -8, 12, 15, 13, 7, 12, -26, -20, -19, 19, -5, 1, -5, -3, -2, 8, 7, 1, 6, 14, 16, -14, 15, 23, -17, 0, 13}
}
, {{5, 11, 1, 4, -17, -10, 16, -22, -10, 6, -2, -13, 3, 7, -11, 11, -24, 14, -8, 2, -2, 7, 19, 4, -2, -1, 0, 2, -15, -5, -5, -8, 9, 6, -13, -3, -6, 0, -1, -3, -3, -4, 9, 4, -13, 6, 10, 9, 4, -11, 0, -7, -8, 5, 9, 7, -4, -3, -1, -8, 4, -5, 3, 2}
, {-31, -8, -30, 12, 1, 6, -15, 0, 7, 11, -17, -1, -7, -10, 22, 9, -5, -5, 4, -5, -2, -8, -7, -15, -13, -12, 14, -2, -12, -20, -16, -20, -5, 19, 25, 17, -22, 9, -7, 14, 14, -12, 11, -3, -5, -5, -40, 7, -1, 5, 1, -21, -11, 26, 11, 23, 3, 9, 3, 28, -11, 6, 20, 0}
, {6, -20, -20, 17, -10, 5, -11, 0, -33, -3, -15, 7, -11, -3, 10, 6, 0, 0, 12, 7, 12, -3, 5, 20, -17, 7, 29, -10, 1, 3, 0, 9, -27, 5, -2, -16, -6, 11, -1, 14, 1, 4, 9, 8, -17, 12, 0, -4, -14, 13, 2, -4, -11, -12, -8, -17, 3, 27, -1, -4, 18, 11, 7, -3}
}
, {{-8, 2, 12, -11, -4, 23, 4, -13, 4, -9, 5, 5, 0, 0, 8, -3, -5, 1, 13, 8, -8, -7, 10, 14, -4, -4, -5, 1, 11, 0, -5, -10, -7, 6, -2, -6, -3, 9, 3, -21, -10, 2, -1, 19, 23, 10, 5, 0, -8, -6, -13, -10, -2, 5, -3, 6, 4, -8, 9, -17, 7, 5, -6, -21}
, {4, 1, 7, -27, -13, -16, -8, -17, -3, 13, 3, 3, -10, 2, 2, -30, 10, -32, 18, -10, -6, -7, -14, 1, -18, -18, 0, 11, -5, 10, 5, -22, 13, 5, -18, -20, 0, 11, -15, 11, -12, 14, -14, -11, 5, -8, 17, 7, 15, -4, -29, -21, -13, 18, 1, -3, -1, -17, 8, -13, -5, 3, 1, -11}
, {20, -13, 0, 13, 2, 17, 1, -16, 10, 9, -5, -6, -2, 5, 1, 25, -6, -8, -7, -4, 12, -4, -13, -2, 6, 19, 12, -2, 9, 20, -5, 17, -5, 9, -14, -10, 14, -17, -8, 10, 10, 8, 5, 20, 7, -14, 26, -11, -10, 8, 5, 8, 13, -5, 2, -7, 6, 12, 4, -13, 9, -11, 15, -4}
}
}
, {{{-5, -13, -4, 11, 10, -2, -17, -12, 0, 7, 8, -5, -27, -16, 8, -13, 9, -15, 4, -8, 2, -15, 15, -6, -26, 10, -10, -6, 12, 1, -11, 9, -13, -22, -6, 6, -5, -4, -2, 1, -10, 4, -8, -23, 14, -6, 0, 6, -5, 4, -13, -3, 0, 11, -2, 3, 11, -3, 4, 1, -12, -3, -4, 6}
, {-13, 3, -12, -16, 19, -7, -13, -7, 5, 6, 5, -14, -16, 5, -12, -3, 8, -20, 1, -11, -12, -15, 14, 9, -17, 5, -9, 3, -22, -9, -10, -12, 2, -16, -7, 10, -5, 10, -14, -3, 4, 9, 3, -25, 6, -24, -3, 8, -1, 6, 0, 1, -6, -7, 11, -6, 2, -7, -1, 17, -7, -9, -12, -1}
, {-19, 7, 1, -21, 9, -10, -16, -9, -2, 5, -4, 5, -5, -6, 3, -8, 25, -5, 3, -9, 1, -8, -5, 4, -12, -7, 8, 0, 3, 14, -4, -9, 1, -18, -5, 5, 9, -9, 5, 7, 14, -14, 12, -11, 2, -2, 14, -1, 9, -8, 5, 0, -13, 5, -1, 11, 13, -2, 4, -20, -21, 11, 3, -12}
}
, {{10, -4, -8, -11, 11, -5, -3, 0, 2, -6, -19, 9, -13, 0, 17, -2, 9, 20, 9, 19, -13, -10, -2, -10, -16, -3, 25, 8, -12, -10, 6, 0, -4, 9, -15, -14, -11, 7, -3, 0, -18, -23, 8, -17, 1, -5, 1, 8, -5, -6, -4, 0, -9, -7, -5, -3, -7, -5, 16, 6, 10, 1, 12, -5}
, {-2, -1, -15, 8, -5, -5, -19, -10, -23, 3, 12, -2, -12, -6, 4, 15, 11, -4, 4, 11, -6, 5, 11, -2, -12, 3, -8, 17, -8, -20, -10, 4, 2, -12, -23, -2, 16, -7, -10, 5, -4, -49, 34, 7, -7, -4, -21, -2, 27, -22, 12, 4, 2, -16, -5, 5, 5, -5, -15, 10, -29, 3, 22, 4}
, {8, -9, 24, 4, 7, 23, -12, 13, -4, -1, -3, 11, -1, -17, -2, -2, 21, 16, -17, -8, -19, -25, -5, 28, 4, 11, -3, -27, -25, -14, 16, 23, 9, 6, -10, -4, -6, -9, -8, 5, 14, -7, -10, -11, -5, 7, 11, 8, -16, 3, -15, -23, -4, 4, 7, 14, -23, 17, 14, 16, -7, 5, -10, 6}
}
, {{8, -16, -16, -2, -13, -11, -8, 17, 8, -3, -16, 6, 1, 5, -3, 3, 7, -6, -12, -3, 0, 7, -9, -11, 6, -19, 10, 4, 20, 8, -2, -9, -15, 4, 14, -23, 17, 5, 10, 0, 9, 15, 15, -7, -8, -1, -7, -1, 8, 9, -22, 1, -5, -12, -6, 13, -18, 18, -7, 7, 3, 3, -1, 0}
, {8, 8, 18, -11, -11, -7, 0, -4, 4, 36, -2, -17, 6, -27, -28, -41, -19, -5, 1, -31, 5, 8, 19, -13, -39, 8, -10, 1, 1, 4, -1, -10, 4, -10, 15, -24, -17, 7, 0, -4, 17, 16, 4, -11, -19, -15, -2, 4, 17, -16, -24, -22, 12, 13, 10, 7, 6, -16, 31, -19, 1, 12, 4, 3}
, {-16, 7, -13, -7, 4, 6, 11, 15, 2, -23, 2, -22, -17, -8, -9, 4, -10, -31, 7, -12, 7, -12, -22, -10, -9, -8, 1, 21, 13, 26, 0, 6, -6, 1, 9, 9, 0, -4, 12, -18, -23, -6, -6, -4, 27, -24, -6, 3, 3, 20, 19, -3, 2, -5, 3, -6, 13, -2, 18, -14, 13, 9, 13, -27}
}
}
, {{{7, -4, 1, 12, -17, 20, -4, -2, 1, -1, -16, 18, -8, 3, -17, -10, 5, 17, 13, -13, -1, -9, -19, 8, 6, 11, -21, 2, 2, -18, 2, -12, 10, -16, -7, 5, 21, 7, -15, 3, 11, 0, 4, 10, -1, -3, -11, -15, 9, -7, 7, -6, 2, -8, 25, 0, -6, -5, 2, -5, 11, 2, 4, 1}
, {-9, -12, -8, -13, -13, -4, 2, 7, -1, -12, -11, 7, -3, -10, -2, 12, 7, 8, 4, 11, 17, 0, 2, -8, -30, 7, -11, 16, -8, -5, -1, -5, 7, -7, -14, -6, 4, 4, -2, 9, 17, -4, -13, -29, 11, -30, -8, 0, 11, 0, 10, -2, -10, -7, -7, 10, -6, -4, -7, 1, -11, -12, 1, 8}
, {-14, 15, 1, 20, -10, 8, 14, -12, -7, -5, -3, 10, -5, -4, 12, 3, 22, 8, 4, 4, 9, -7, 9, -11, -1, 2, -10, 0, 5, 6, 0, -3, -5, 0, 2, -7, 7, 2, 1, 5, -5, 1, 5, 10, -10, -3, -11, -20, -5, 11, 24, 2, -1, -11, 6, 8, 7, -4, 9, -1, 19, -5, 6, 1}
}
, {{3, -14, -27, -4, -17, 1, -18, 1, -21, -10, 13, -5, 8, -13, -14, -8, -1, -4, 1, -14, 13, 17, 9, -9, -2, 13, -18, -8, 21, 3, 5, 1, 3, 2, 20, -1, 4, 1, 3, -1, 0, -14, -18, -14, 12, -4, 0, 16, 0, -6, 6, -6, -2, -21, 13, -15, 2, -1, -14, -17, -9, 11, -14, -14}
, {-18, -8, -1, 23, -32, 21, 31, 15, 6, -2, -29, 0, 12, -19, -13, -15, 19, 16, -24, -2, 9, -7, 19, -11, -30, 8, -36, -16, 27, 7, -11, 7, 9, -3, -41, -13, -12, -5, -23, 16, 31, -9, 2, -16, -7, -7, -26, 10, -9, -10, -25, -20, -29, 7, -2, 10, -1, -14, -6, 32, -18, -22, 2, 31}
, {3, 4, 7, 9, -5, -16, 13, 0, 5, -27, 11, 6, -5, -1, 15, -7, 3, 7, -2, -17, -4, 17, -14, -1, -22, -22, 19, -20, 26, -6, -11, -2, 2, 12, 4, 18, -14, 5, -10, 4, 3, -19, 7, 11, -15, -1, -13, -2, -1, -31, -26, 6, 17, 13, -5, 8, -20, 7, -5, 5, -3, 17, -12, 3}
}
, {{-13, 0, 0, 4, -5, -2, -2, 2, 4, 12, 6, -10, 6, -13, 5, -21, -15, -4, 8, -11, -14, 7, 17, 3, -15, -9, -13, -14, -14, -1, 22, -11, 21, 10, 20, 1, 6, 4, -20, 6, 5, -4, -5, 1, -9, -7, 9, -10, -9, -3, 19, -7, 3, -16, 3, 0, -1, 15, 12, 1, 2, 4, 3, 6}
, {4, 15, 12, -15, 7, -9, 5, 8, -25, -5, 31, -12, -15, -1, 8, 18, 29, 0, -8, 29, 12, -23, 17, 7, 0, 4, -23, 12, -15, 8, 2, -2, -28, 0, -6, 11, 4, -35, 33, -20, -38, 12, -24, -10, -8, 15, -32, 12, 9, -51, -21, 10, 4, -14, -18, -19, 11, -9, 8, -14, 3, 5, -14, -8}
, {1, -34, -26, -7, -12, -21, -23, -31, 20, 3, -27, 37, -19, 10, 17, -37, 17, -3, 29, -7, -28, 2, -14, -15, -1, -9, -25, 14, 8, -1, 19, -10, 10, 7, 7, -16, -7, -8, -1, 22, 44, -37, -4, -28, -24, 13, -15, -7, 13, -33, -8, 14, -14, 22, 15, -8, -22, -1, -22, -2, 2, -16, -8, -5}
}
}
, {{{1, -8, 11, 1, 6, 13, 3, -4, 3, -7, 9, 2, -20, -5, 6, -9, -12, 6, 10, -2, 7, -1, -17, 4, -5, 3, 11, -2, 6, -15, 8, 1, -1, -16, -28, -1, 11, -7, 14, 4, -14, 12, 5, 4, -6, 2, -17, -6, -15, 19, -6, -11, 0, 4, 6, 14, -9, 13, -18, 9, 1, -2, 3, -4}
, {-4, -1, -19, -14, 13, 10, 5, -23, -18, 3, 12, 11, 0, 14, 11, -20, -21, 4, -15, -13, -11, -2, 5, 10, -6, 16, 0, 10, -10, 16, -19, -7, -13, -3, -5, -2, -7, 4, -17, -4, 5, 6, -18, 8, 2, 12, -10, -17, -6, 5, 3, 5, -17, -14, 6, 5, -18, 0, -7, -6, 7, -4, 7, -2}
, {-1, -11, 13, -3, 7, -18, -20, 5, -2, 0, 9, 8, 2, -1, 2, 12, -13, 6, -13, -16, 10, 0, -6, 11, -3, -18, 7, 1, 21, -3, -10, -4, -5, 14, -12, 9, 9, -5, -2, -5, 6, -14, -4, 6, -12, -16, -17, 4, 0, 10, -9, -10, -3, 0, 3, 10, -12, -6, -14, -3, -20, -26, 3, 17}
}
, {{-22, -12, -23, -3, 2, 0, -7, -15, 2, -9, -7, 0, -7, -4, 2, 2, -8, 12, 5, 3, -11, -26, -7, 4, -1, 17, 14, -5, 15, -7, 6, -4, -4, -4, 2, -2, -10, 4, -8, 10, 17, -13, -2, 3, 3, -26, -8, 13, 7, 4, -8, -1, 11, -4, -10, -10, -9, 6, 4, 4, 8, 17, 1, -16}
, {8, -21, -6, -33, 33, -40, 34, -7, -10, -20, -24, 11, 20, -5, 21, -1, 0, -26, 14, -4, 11, 7, 29, -4, 16, -8, -10, -11, 3, -2, -28, -8, 5, 7, 4, -9, 3, 20, 4, -22, -27, 26, -23, -8, -2, 40, 2, 12, 0, 1, 20, 9, -33, 0, -6, -14, -5, 22, -22, -25, 10, 13, 5, -11}
, {3, 21, 0, -16, 14, 9, -13, -3, -25, 9, 0, 1, 3, 4, -14, -15, 6, -13, -2, 7, -12, 5, 9, 9, -2, 9, -12, 4, -5, -1, 2, -6, 0, -2, 6, -3, 0, 16, 0, -1, -15, -5, -13, 0, -18, 1, -10, 23, -15, 8, 1, -16, 2, 9, 5, -2, -4, -4, 13, -7, -8, 10, 14, 20}
}
, {{-5, -5, -12, 3, 5, 9, -6, -10, -13, 1, -16, -9, -7, -35, 0, -10, -5, -20, -9, 3, -12, -15, -11, 2, -8, -5, 1, 10, -3, -11, -4, -1, 3, -7, -8, -8, -6, 3, -3, 13, -2, -6, -9, -10, 10, 4, 10, -11, -12, -12, 5, 2, -9, -4, -2, 14, 1, -18, 0, -1, 6, -6, 10, -10}
, {-1, -10, 13, 9, -10, 0, 3, -10, -7, -11, 13, -2, 10, 7, 2, -7, 8, -9, 3, -9, 15, 7, -9, -6, -13, -6, -14, -8, 16, -16, 8, 5, 11, -22, -19, 7, -7, -9, -3, -12, -1, 8, -9, -30, -2, 1, -23, -11, 17, 7, -9, -5, -10, 21, -6, 0, -13, -8, -31, -15, -34, -4, -19, 2}
, {-7, -20, -6, -16, 29, -14, 0, 10, -15, -14, 7, -9, -16, -31, -9, -16, 12, -15, -1, -2, -4, 27, -21, 5, 1, -9, -8, 1, -15, -4, 5, -19, 8, -13, 8, 12, -1, -1, -5, -5, 5, -7, -6, -20, 0, 9, -3, 7, 4, -1, -9, 5, 13, -18, -13, 0, 18, 2, 8, -18, -20, 6, -17, -3}
}
}
, {{{-1, -10, -8, -8, -1, -6, -1, -1, -15, -5, -11, 8, -10, 0, -5, 14, 11, 18, -1, -8, -4, -6, 0, 5, 19, 11, 1, -2, 3, -5, -6, 19, -4, -6, -3, 10, 18, -3, 4, -2, 13, -2, -11, 3, 12, -16, -13, -8, -18, 11, 17, -2, 4, -17, 15, 9, 1, 3, -1, -4, 4, -16, 0, 4}
, {-6, 2, -8, 0, 15, -11, 3, 14, 17, 5, -18, 17, 6, 5, -6, -6, 12, 11, 6, 8, 2, -4, -7, -12, -1, -5, 6, -4, 8, 11, 4, 12, -2, 8, 23, -1, 8, 4, 15, 4, 22, 5, 16, -12, 12, -33, 24, 0, 13, 3, 5, 8, -15, -10, 1, -5, 0, -9, 19, 5, 2, -16, 14, -7}
, {-5, 4, -1, 2, -1, -7, -10, -29, -5, 3, 6, 11, -1, -2, -16, -5, -15, 5, 4, 3, 5, -19, 23, 2, 6, -12, -32, -1, -20, -8, 1, 1, -5, 7, -11, -9, -5, 3, -6, -12, 0, 8, 5, -11, 10, -10, -1, -10, 1, -3, 14, 7, 18, -19, -19, -10, 11, -2, -8, 11, -9, -24, 0, -6}
}
, {{25, -5, -6, 3, 1, 5, -7, -14, -7, -1, 0, -1, -9, -7, 2, -7, 1, 6, -19, -18, 2, -20, 5, 15, -15, 19, 0, 1, -17, -10, 7, 11, 10, -19, -9, -5, -11, 10, -12, 12, -11, 5, -14, -13, 9, 1, 21, -9, -6, 1, -21, -19, -5, 3, -7, -4, -12, 1, 15, 23, -5, 25, -8, -17}
, {-7, -4, -6, 9, 0, -2, 1, 10, 11, 0, 1, 26, 19, -9, -7, -4, -1, 2, -14, 7, -9, 0, -10, -31, -2, -7, 4, 6, -9, 19, -4, -6, 3, -8, 4, -6, 2, 6, -6, -16, 24, 9, -15, -13, -11, 6, 22, -15, 2, -18, 3, -11, -12, 17, 4, 4, -7, 7, -7, 12, -4, -25, -1, 17}
, {4, 20, 8, 10, 7, -16, -17, 2, -23, 1, 27, -16, 11, -7, 1, 2, 6, 0, -5, -12, -12, -9, 3, 2, 10, -8, -8, -16, -11, 15, 6, 18, 2, 16, -1, -1, 2, -8, 10, -5, 1, -9, 3, -30, 8, 0, 4, -1, 7, -8, 6, -26, -5, 5, 7, -17, -12, 3, -11, 1, -21, -15, 6, 11}
}
, {{-7, -8, -8, -24, 2, -2, -18, 4, -7, 16, -2, -2, 10, 18, -4, 0, -25, -27, -14, 1, -6, 10, -2, 12, 3, -4, 1, 27, 7, -17, -7, 3, 10, 1, 29, -5, -16, 18, 6, 2, -12, 9, -26, -15, 27, 2, -9, -1, 16, -5, 10, 18, 6, 0, 6, 2, -8, 6, 6, -5, 23, 16, 12, -10}
, {-6, -5, -27, -5, 25, 8, -3, 0, -15, 6, -21, 12, -1, 3, 7, 8, 12, -5, -20, -2, 10, 10, 22, 4, -1, 14, -18, -20, 9, 8, -11, 10, 12, -13, -2, 12, 3, -25, 9, -27, -5, -14, -11, -15, 14, -3, -13, -18, -1, -15, 27, 31, -16, -22, 26, 12, -20, 6, 6, -11, -8, -2, -26, 12}
, {-34, 14, 20, -2, 13, -14, 17, -4, 7, 12, 3, 4, -2, -9, 17, -9, 1, -1, -3, 10, -17, 3, 9, -13, 3, -13, -2, 1, -15, 3, 0, 10, -5, 11, 15, 4, 3, -19, -2, -5, -2, 12, 3, 7, -25, -4, -19, -4, -2, -17, 9, 17, 6, -2, -16, 15, 25, -18, -6, -6, 2, -42, -3, -12}
}
}
, {{{6, -1, -11, 32, -2, -3, 4, -2, 2, -4, -7, 2, 3, 1, -2, 5, 1, 7, 9, 9, 9, -18, 3, -9, -2, -18, 6, -8, -24, -5, -1, -5, -15, 22, -7, -9, -18, -12, 9, 13, -21, -2, 19, 4, 6, -15, -1, 12, -8, 9, -18, 5, 21, -15, 0, 8, 5, 18, 18, 19, -7, -4, -7, -5}
, {6, 8, -11, -2, 8, -12, 3, -17, 0, -8, 2, -7, 4, 9, -15, 1, -18, 1, -1, 18, -8, 23, 0, 1, 3, -1, -11, 35, -6, -15, 6, -6, -13, 12, 0, -16, 16, -1, -10, -2, -7, 2, 15, 6, 8, 15, -16, -7, -6, -3, -19, -2, 11, -5, -1, 23, 10, -6, 1, 1, 6, 1, 15, -1}
, {0, -3, -13, 5, 2, -4, -4, -11, 7, -8, -1, 4, 9, 10, -2, 8, -4, -3, 3, 3, 9, -7, 3, 22, -9, -26, 0, 15, -11, -19, -10, 7, -28, 13, 12, -16, -2, 3, -10, 6, -20, -12, -17, 13, 7, 6, 0, 9, -1, 9, -11, 0, 11, 1, -9, 7, -15, 11, 11, -9, 16, -15, 9, -9}
}
, {{15, -2, -18, 6, -11, -8, -11, 4, -7, -10, -1, 8, 15, -11, -4, 9, 0, 4, 2, -14, 11, 13, -3, -11, 12, -12, -9, 19, 9, -5, -22, 11, -19, -2, -5, 3, 6, 4, 7, 2, 10, -12, -9, -2, -15, -6, -3, -25, -7, -23, 3, -1, -7, -23, 9, 3, -1, -28, -10, 4, 9, -8, 13, -3}
, {-4, -8, 7, 2, 2, -10, 5, -8, -9, 4, -11, 1, 14, 10, -4, -12, 4, -7, -9, 5, -11, 11, -16, -19, -9, -24, 5, 25, 0, 7, -11, 2, -50, -14, -11, 10, 1, -12, 15, -23, -22, 3, 7, -7, 8, -11, -9, -3, -14, -8, -13, -15, 4, 3, 13, -20, -6, -4, -1, 1, 13, -5, 0, -35}
, {7, 4, 13, 12, 4, -3, -12, -11, 2, -8, 1, 18, -3, 14, -12, 6, 18, -3, 4, 7, -18, -25, -9, -14, -11, -13, 1, 18, -5, 32, 10, 0, -10, -36, -15, -12, -13, -2, 16, 3, -4, -18, -13, -13, -11, -16, 14, 7, 2, 3, 8, -10, -1, 8, -2, 9, 0, 14, 7, 10, -8, -5, 19, 10}
}
, {{-2, 5, -6, -6, -18, -4, -3, 3, -3, -2, -11, 14, 0, -8, -10, 1, 13, -14, 5, -22, 18, -8, -3, 5, 15, 15, 22, 5, 7, 11, -14, 6, 0, 12, 5, -16, 10, 5, -4, 20, -16, 14, 8, 2, 9, 10, -14, -3, -13, 18, -2, 0, -4, 4, -9, -10, -5, -15, 3, 1, -14, -2, 2, -11}
, {17, -16, -21, -12, -9, -15, 10, -19, -9, -24, -13, 1, -11, 8, 11, 16, 0, -23, 20, -9, 8, -9, -6, 4, -18, 5, 20, 17, 1, -8, -16, 6, -28, -8, -8, -9, -14, 11, -19, -21, -6, -2, 1, 0, -4, 9, 4, -17, -7, 15, -21, 4, -10, 8, -10, -11, 6, 8, -8, -6, 8, -7, -4, -4}
, {-42, -7, -8, 8, -7, 0, -10, 0, 20, -14, -12, -2, -4, 8, 19, 14, 2, 12, 18, -8, -21, -5, -8, -3, -18, -9, -7, -16, -13, -3, 18, -7, -2, -6, -5, -19, -16, 13, -1, 18, -12, -11, -2, -1, -6, -7, -10, 4, 2, -11, -14, 7, 12, 5, -2, 7, 7, 10, 10, 4, 10, -2, -8, 12}
}
}
, {{{-1, 7, 16, -1, -2, 10, 8, -15, 8, 22, 12, -10, -21, 2, -17, 11, -7, -8, 28, 10, 12, 0, 4, 11, -19, -12, 4, 1, -9, -3, -4, -7, -6, 3, -20, -11, 0, 5, -2, -7, 2, 5, 1, -19, -11, -11, -21, 6, -1, 0, -11, -6, 15, 10, 1, 13, 4, -9, -15, 6, -1, -10, 14, 16}
, {-8, -4, 16, 4, 2, -5, 22, 5, 9, -7, -1, 3, 3, -16, -2, 13, -25, -15, 28, -6, 16, -15, -2, 1, 8, -5, 4, -4, -17, -11, -26, -21, 10, -19, 4, 11, -1, 2, -11, 6, -14, 20, -39, 5, -17, -3, 6, -5, 15, -1, 9, 1, 6, 7, -9, 4, 20, -3, -3, -16, -4, -6, -9, -1}
, {5, -27, -5, -2, 3, -14, -14, 8, 7, 7, 4, -2, -20, -10, 10, -1, -12, 2, 17, -20, 16, -1, 9, -15, -5, -4, 6, -16, 11, -9, -8, 4, 15, 6, -5, 10, -1, 14, 14, 12, -7, 5, 3, -3, -6, -17, -3, -16, 18, 12, -1, -3, -2, 11, -4, 8, 10, 5, -4, -11, -24, 4, 1, 16}
}
, {{-18, 11, -2, -7, 5, -9, -7, 20, -7, 6, -1, -3, -3, -3, 9, -23, 1, -13, 17, 5, 4, 0, 11, 0, 4, -4, -16, -3, 4, 3, 7, 21, 1, 30, 0, -2, 10, 10, -1, 12, 9, 15, 10, 6, -2, 10, 10, -9, 1, 6, -8, 10, -14, 4, -12, 1, 12, -9, 5, 0, 1, 9, 14, 2}
, {-12, -19, -10, -19, -15, -4, 24, -23, -39, 5, -36, 16, 17, -2, 2, 0, -23, -6, 6, 6, 33, -31, -11, 11, -11, -15, 28, -1, 1, 12, 6, -8, -6, 18, 6, -9, -14, -2, -2, -42, -47, 22, 3, 23, 15, 39, 13, 5, 6, -24, -4, -18, -10, -4, 9, -15, -8, -10, -29, -21, 6, 17, -2, -59}
, {9, 15, 6, -1, -17, -14, 17, 7, -21, -12, -17, 10, 17, -10, 26, -12, 0, -8, -13, -12, 14, -8, -4, -20, 15, -28, 0, -10, 10, 5, -1, -17, 1, 5, 12, -6, -4, -1, -2, 6, 0, 0, -11, 22, -13, 6, -9, 1, 16, -17, -12, 14, -10, -14, -21, -23, 1, 3, -4, -3, 2, 1, 13, -16}
}
, {{10, 2, -7, -4, -2, 3, -3, -9, 0, 8, -1, -8, 1, -18, -7, 7, -3, -17, 30, 0, 3, -20, 15, 21, -4, -13, -12, -18, 3, 5, 5, -16, -11, -16, -18, -6, -4, 0, 14, -7, 8, 11, -16, -7, -11, 13, -11, -7, 3, 0, -3, 8, -3, 13, 3, -9, 8, 0, 9, -13, 15, -6, 4, 2}
, {0, -24, -12, -12, -8, 25, -2, -5, -28, -6, -11, -2, 4, 9, 13, -17, 15, 4, -3, 1, -4, -6, -4, 4, -23, -16, 8, 0, -21, -27, 15, -19, 4, -6, -21, -6, 10, 15, -22, -27, 3, -5, -6, 10, -18, 7, 5, 19, -3, 3, -22, 0, 10, 3, 13, -19, 5, -25, 1, -2, 0, -1, 2, -2}
, {13, -16, 3, -14, -21, 8, 33, 20, -28, -15, -21, 3, 9, -9, -1, -34, 0, -7, -2, 4, 2, -5, -4, -6, -3, -2, 8, 7, 19, -5, 14, 5, -1, -6, 8, -6, -9, -3, -9, -5, 25, 6, 5, 4, 7, 37, 19, -11, 7, -30, 0, -5, -14, -26, -8, -9, 10, 20, -16, -6, -11, 13, 6, -3}
}
}
, {{{0, 3, 3, 13, -5, 4, 7, 1, 11, 3, 15, -13, 0, -17, -3, 1, 0, 8, 0, -8, 19, 2, -8, -3, -12, -9, -7, -3, -1, -24, -21, 5, -23, 3, -24, -10, 3, 2, -13, 2, -13, -2, -2, -13, -9, -10, -10, -1, -18, 9, 3, -5, 12, 6, -8, 19, -7, 7, 12, 11, -7, 0, 5, 34}
, {0, -7, -19, 3, -1, 8, -35, -3, -14, -7, -5, 8, -27, 14, -10, -13, 21, 13, 4, 3, 5, -13, 13, 3, -17, 14, -3, 14, -31, -22, -9, -5, -2, -14, -4, 8, -6, -19, -12, -2, 8, -15, -4, 4, 11, 10, -14, 14, -11, -5, -5, 3, -9, 9, -7, -2, 7, 8, 4, -22, 2, -12, -14, 7}
, {-8, -2, -2, -3, -6, -11, 8, 16, 11, 0, 3, 3, -8, -16, -12, 12, 21, -14, 12, 3, 20, -5, 6, -3, -18, -19, 3, -1, 0, -6, -22, -11, 8, 5, -10, -10, 11, -4, 14, 13, 2, -9, 4, -32, -7, -8, -4, 17, 3, -10, -2, -6, -4, -1, 2, -6, 12, -6, -20, 12, -17, 16, 8, 7}
}
, {{6, 1, 8, 10, 0, 16, -11, -7, 2, -7, -39, 4, -29, 1, -14, -16, 1, -4, -7, -9, 3, 24, -24, -2, 5, 6, 13, 2, -3, 7, 3, -12, -11, -15, -10, -4, 16, -1, -7, 10, 5, -7, 3, -1, -12, 9, -1, 16, 2, -12, -15, 21, 3, 5, 9, 2, -30, 33, -1, -8, -6, 3, 5, -6}
, {-15, 19, -43, -22, 9, -8, -5, 0, -7, -25, -11, -20, 8, -4, -6, 29, 3, -18, 6, -1, -21, 4, -12, 14, 20, -21, -24, 10, -7, 22, -8, -22, 8, -7, -13, 38, 4, -12, 7, 0, -20, 3, -15, 13, 3, 1, -1, -24, 18, -4, 15, 7, 11, 2, 27, 7, 23, -1, -10, -16, 7, -6, 10, -11}
, {12, 14, 16, -3, -6, 15, 10, 8, 6, 2, 4, 13, -4, -16, 9, -4, 8, 5, 5, 1, 2, -3, 0, 21, -40, 17, 11, -20, 11, 11, -15, 5, -7, -20, -15, 4, 2, -6, -16, -8, 1, -11, -33, -6, -11, 4, -9, -9, -16, -12, 5, 2, 9, -15, 13, 3, 6, -6, 20, 15, -19, -6, -11, 2}
}
, {{-18, -7, -15, 20, -20, 21, -7, 14, -10, 10, 2, -2, -14, 9, 19, 8, 4, -7, -10, -9, -18, -7, 6, -11, -11, -1, 12, -6, 20, -1, 1, -7, 3, -9, -2, 18, 0, 11, 7, 32, -7, -12, -1, -15, -25, -40, -5, -10, -27, 12, 2, -6, -8, -6, 0, 5, 0, -2, 11, 16, -28, 2, 8, 7}
, {5, 2, 10, -3, 30, 13, -1, -10, -2, 15, -2, -14, -28, -5, 2, -9, 7, 3, -18, -17, 14, -19, -16, 18, -21, 9, 0, -17, -14, -8, 10, -11, -8, -7, -6, 6, 12, 15, -27, 6, -28, -13, -14, 12, -2, -5, -6, -13, -28, 17, -11, -8, 11, -12, -4, -1, -9, -16, 25, 5, 9, -2, -10, 2}
, {6, -7, -11, -29, -2, -10, 6, 5, -14, 3, -6, 19, -5, -3, 11, 4, 6, -6, 10, -7, 2, -3, -5, -3, -10, 14, -21, -5, 12, 7, -4, 0, 8, -4, 6, 0, -14, 7, 1, 14, 10, 11, -8, -7, 8, -5, 5, -9, 12, 10, 22, 18, -24, 5, -5, 3, 1, 3, 10, 7, 8, 14, -1, -4}
}
}
, {{{7, -5, -2, 4, 17, 8, 1, -3, -2, -7, -16, 5, 16, 13, 2, -10, -4, -1, 9, -11, 2, 3, 14, 7, 12, -8, -5, -5, 3, -15, -6, -2, 7, 2, -23, 4, 16, -8, -13, -16, -5, -5, 5, -3, 3, -11, 3, -14, 13, -8, 8, -3, -27, -24, -3, 1, -3, -1, -5, 4, 1, 5, 9, -2}
, {2, -6, 6, 7, 6, -5, 10, 6, 13, -5, -10, 3, -3, -15, 3, -5, -7, -6, 12, 10, -5, -6, 11, 9, 2, 0, 8, -8, -15, -18, 1, -1, 9, -9, -17, 13, -11, -5, 3, 21, 12, 6, -4, -2, -10, 1, 2, 2, 13, 18, 2, -20, -11, -5, 8, -2, -12, 13, -4, -14, 3, 12, 7, 14}
, {6, 7, 9, 17, 1, 6, -3, 0, -11, 8, -1, 2, 0, 13, -14, 12, -6, 23, 1, 13, -22, -11, -3, 18, 17, 19, -3, -7, 20, -7, 0, 1, -5, -7, -9, -2, 4, -17, 20, -4, 9, -4, 8, -18, -17, -20, -1, 12, 4, -7, 4, -20, 1, -6, 9, 0, 1, 5, -1, -9, 1, 10, 22, -8}
}
, {{3, 12, -13, -11, -12, -1, -2, -11, -3, 7, 5, -4, -6, -15, -14, 4, 5, -3, 13, -7, 14, -36, 0, -1, -19, -14, 5, -2, -6, 18, 9, -9, -25, -3, -12, -14, -9, 18, -6, -26, 4, -14, -11, -5, -7, 0, 4, 13, 11, -26, -15, -3, -8, 5, -1, -8, 10, 8, 5, -31, -13, -15, -18, -21}
, {17, -14, 15, -18, 3, 8, -2, -2, 2, -22, 1, 2, 11, 19, 0, -6, 3, 1, 5, 26, 10, -31, 17, -27, -30, 11, 0, -7, 6, -3, 17, 30, 13, -13, -1, -8, -18, -13, -25, -31, 15, -10, -17, -15, -3, 2, 24, 2, -15, -13, -8, 1, -14, 11, 3, -38, 0, 10, 4, 7, 5, -28, 9, 5}
, {-17, 5, 7, -22, 7, -10, 14, -1, -4, 12, -2, 11, -2, -12, 8, -7, -9, 18, 14, -12, 9, 0, 14, -1, 1, -17, 6, -19, 5, 5, -14, 0, -1, 6, 11, -10, 20, -9, 8, 4, -5, 5, -1, -14, -20, -2, -12, 5, 4, -5, 4, 4, -1, -7, 6, 6, 17, 3, 14, 16, -13, -2, -3, 9}
}
, {{-8, -12, -13, -10, 0, -12, -3, -12, -3, -14, 3, 10, 1, -2, 2, 2, 4, -9, -14, -8, 9, -9, 13, 9, 8, -3, 5, -3, -27, 8, 0, 1, -10, 3, -4, -22, -4, 4, -4, -11, 8, -11, -7, -8, -5, 0, -18, -13, 12, 10, 10, -4, -6, -1, 8, 7, 5, -7, -3, 0, -5, 5, -2, -3}
, {-8, 4, 2, 5, -1, 1, 8, 14, 2, -32, 8, 1, 8, -6, 1, 34, 4, -14, 0, 13, -7, 8, 0, 2, -7, -15, 12, -16, 10, 10, -17, 0, -15, 0, 4, 17, -1, -27, 29, -2, -2, 14, -10, -27, 10, -6, -13, 16, -8, -23, 9, 21, -11, -2, -4, -1, -10, 7, -8, -6, -13, -8, -8, -6}
, {6, 7, 5, 12, 1, 9, -8, -10, 9, -6, -17, 19, -2, 3, -12, -3, -4, -5, 3, -12, 2, -11, -5, 9, -8, 0, 13, 6, -8, -9, 0, -26, -5, 4, 20, -4, 20, 22, -9, 29, 13, 16, -22, -24, -8, 21, 9, 13, -6, 22, 7, 14, 11, 9, -3, -1, -9, -5, -4, 4, -1, -11, 6, 6}
}
}
, {{{14, 0, 2, -8, 18, -3, 8, 0, -13, -1, -8, -8, 14, 11, 3, 6, 4, -14, 0, -2, 12, -19, -13, -1, 0, 12, -3, -19, 5, -10, -17, -3, 11, -1, -4, 26, 17, -2, 7, 4, 4, 11, -13, -25, 8, 10, -3, -6, -12, 0, 5, -5, -6, -7, 6, 2, 1, -8, -5, 5, -1, -15, -17, -8}
, {-8, -16, -9, -9, 2, 7, -6, -16, -5, -6, -14, -3, -10, 2, 19, 8, 6, -12, 0, -18, -15, -2, 7, -8, 5, 6, 2, -2, -1, -14, -13, -10, -1, -17, -3, -13, 1, 14, -8, 5, -7, -2, -1, 0, 2, -25, -16, 7, -3, 3, 13, -13, -8, -9, 17, -10, 3, 8, -2, 6, -13, -37, -7, 3}
, {8, -4, -5, 1, -4, 12, -8, -30, 4, 2, -4, -16, -16, 4, 0, 0, 15, 2, 14, 13, -5, 0, -17, -5, -14, -16, -2, -10, 6, 15, -24, -1, 0, -25, -24, 6, 23, -1, -2, -7, 12, -11, 14, 7, 2, -23, -9, -8, -2, -16, -6, -6, 10, -2, -1, 7, -6, -3, 12, -1, 1, -14, 3, 5}
}
, {{-9, 8, 11, -3, 14, 3, -3, 26, -8, -16, 6, -3, -19, 15, -14, 4, 12, 7, 12, -7, -1, 4, -4, 21, 16, 16, 25, -20, 17, 2, 12, -3, 12, -6, 7, 7, -11, -12, 7, -13, -11, -7, 0, -13, -7, -12, 24, 12, 4, -6, -6, -9, 4, 7, -11, -11, 16, 11, 12, -3, 2, 14, 5, -19}
, {26, -17, -30, 15, -24, -16, -7, -1, -18, -2, -6, -8, 5, -20, 1, 17, -2, 8, 9, 4, -6, -37, 19, 5, -20, 3, 3, -12, -2, 1, 5, -16, 1, -7, -14, -11, -27, -5, 5, 10, -3, 4, 15, -8, -19, 18, 17, -4, 30, -21, 10, -11, -6, -6, -1, 8, -15, 19, -32, 9, -12, 1, -9, 5}
, {-1, -2, 4, 0, 12, 27, -5, -1, -7, -8, -14, 9, 5, -3, -16, 2, -6, -25, -32, 3, 3, 7, -24, -16, 3, -7, -4, -13, 8, 6, 2, -7, 13, 13, 0, -7, -21, -4, -8, -1, 10, -4, -3, -4, -5, -6, 3, 21, -23, -2, -5, -1, 15, -6, -6, 13, -26, -11, -10, -4, 15, -11, 5, 4}
}
, {{-3, -5, -14, 13, -7, 7, -2, -9, 2, -3, 9, -14, 11, -16, 24, -4, -16, -2, 9, -11, -8, -9, 11, -1, 5, -7, -7, -8, 7, 7, -11, -11, 2, 9, 2, 8, 0, 3, -5, 4, 2, -14, -31, -14, 3, -36, 10, -23, 5, -18, -13, -32, -25, -10, 4, 14, -19, -11, -6, -4, -8, -7, -13, 14}
, {-10, -5, 7, 17, 8, -1, -7, -12, -11, 24, 2, -13, 27, -10, 1, -29, -1, -2, -19, -21, 2, -31, -15, 0, 8, -1, -22, -19, -16, -6, 6, -9, 2, -2, 3, -6, -8, 16, -21, 8, -2, -13, 9, -25, -19, -29, 9, -1, -18, 26, 23, -20, -6, 5, 9, 13, -1, 11, 4, -19, -47, 9, 20, -4}
, {-6, -6, 22, -14, -20, 5, -3, -2, 0, 12, 4, -5, -18, -14, -12, 19, -4, -11, -2, 17, 2, -15, 5, -9, -4, 14, -13, 10, 6, 13, 11, 2, -4, -1, -11, 4, -6, 10, 13, -2, -19, 23, 14, 11, 4, -18, -1, 15, -13, 1, 8, 9, 1, 1, -6, -2, 24, -1, 17, 11, -13, 5, 11, -14}
}
}
, {{{-3, 3, -4, 5, 11, -3, -2, -12, -13, -10, -21, 20, 1, -2, -21, 1, 5, 0, -2, -11, 1, -2, -1, -14, -11, -14, -8, -2, 4, 5, -6, -2, -16, -7, 6, 8, 5, 19, -15, 7, 4, 3, -13, -12, -22, -2, 3, -9, -6, -18, 30, 1, -1, -2, 17, -6, -9, -11, -3, 11, -9, 3, 6, -6}
, {-13, -10, -1, 6, 4, 13, 9, -10, -10, -2, -25, -2, -9, 0, -10, 6, 8, 2, -10, -8, 14, -5, -7, 3, -4, 3, -8, -4, 9, -1, 1, 6, 2, 1, -7, 1, 4, 20, 0, -4, 11, 2, -15, -14, -11, 5, -7, 38, 15, -14, 8, -2, 6, -12, 4, -8, 1, -11, -1, -2, -3, 1, -11, -6}
, {3, -18, 8, 14, -1, 6, 10, -14, 2, 3, -26, 11, 15, 7, 1, -19, 16, 10, 5, 19, 4, -5, -4, -3, -1, 10, 7, 0, -1, 28, 16, 0, 1, -15, -1, -4, 5, 11, 13, 10, 29, -13, 0, 9, 8, -8, 16, -4, -3, 3, 15, 0, -6, 1, 0, 7, 8, -1, 3, -5, 9, -3, 7, 14}
}
, {{-1, -8, 15, 7, -5, 6, 2, 12, -14, -2, -8, 9, 0, -5, 8, -16, -17, 6, 8, 1, -9, 9, 6, 5, -8, 8, -7, -9, -5, 18, 7, -1, 27, 13, 2, 18, 3, -13, 0, 4, -5, -1, 9, -7, 4, 7, -9, 2, -4, 12, -8, -6, -9, -20, -2, -4, -15, -10, -1, 9, -22, -15, 12, 16}
, {29, -7, -38, -1, -11, -11, -17, -13, -48, 28, 16, -27, -16, -3, -19, 30, 7, -52, -7, -5, 22, 12, 4, -4, -4, 19, 20, 21, 28, 0, -18, -5, -15, -10, -15, 14, 28, 4, -26, 17, 8, 12, -1, -15, -29, -7, 12, 7, -1, -8, -5, 17, 18, -29, -28, -2, -22, -13, -38, -1, -12, -5, 24, 23}
, {-10, -8, -5, -2, 6, -24, 1, -28, -1, -5, -16, -7, -7, -17, 19, -3, 4, -11, 4, 4, -3, -14, 5, 13, -11, 8, -2, -2, -12, 0, -7, -13, 4, -11, 29, 5, -14, -4, -6, -11, 4, 8, 6, -16, 5, -1, -11, -6, 10, -6, 9, 7, -6, 1, 4, -1, 3, -12, -17, -8, -4, 10, 2, -26}
}
, {{17, -17, 1, 3, 5, 16, 1, 17, -8, 9, 14, -3, -4, 27, 7, -2, -9, -8, -1, 15, -3, -12, -3, -8, 1, 1, 12, -12, 0, 4, 13, -11, 2, -1, -2, -5, 7, -18, 10, 1, 3, 9, -19, -7, -3, -10, 11, 5, 6, -1, -1, 8, -9, -10, -16, -21, -17, 15, -6, -8, 2, 16, 4, -3}
, {-7, -3, -23, -10, -4, -8, 4, 0, -2, 17, -16, 18, -11, -9, -14, -4, 23, -8, -11, -36, -12, 6, 2, 3, -4, 2, 3, -22, 23, 8, -5, -7, 3, 8, -8, 5, -3, -8, -17, 17, 7, 7, 6, -5, -1, -26, -23, -9, 33, -11, 6, -13, -6, 13, 7, -10, 1, 8, 0, 5, -31, 17, -19, 15}
, {11, -11, -15, -3, -8, -10, -1, -4, -2, -2, -23, 0, -9, 4, -3, 9, 9, -5, -1, -34, -1, -19, 4, 1, 2, 4, -19, -8, 11, -2, -6, -13, 3, -15, -14, -10, -14, 4, 1, 3, 7, 11, -7, -11, 2, -16, 3, -4, -6, 10, -1, -9, -26, -18, 1, 5, -1, 9, -6, -5, 15, -4, -23, -7}
}
}
, {{{9, -9, 3, -2, -19, -20, -3, -10, -5, 5, -8, -27, 16, -2, -1, 4, -24, 1, 1, 7, -13, 4, 3, 4, -1, -3, -7, -3, 18, -16, 9, 0, -5, 12, 27, -7, -2, 5, 1, 13, -1, -12, 14, 18, -12, 22, 9, 12, -4, 12, -11, -10, 2, -4, 7, -9, -14, 12, -4, -5, 3, 12, -14, -8}
, {7, -6, -17, -16, -16, -1, -4, 0, 8, 10, -10, 5, -6, 0, -11, 8, -7, 6, -4, -8, 15, -3, 10, -4, -8, 2, 22, 3, -15, 7, 11, -16, -30, 1, 13, -11, 9, 20, -13, 20, -5, 4, -5, -15, 2, -23, 7, 15, 2, -9, -16, -6, -2, 5, -2, -17, -16, -27, -1, 11, -4, -4, -3, -29}
, {-9, -1, -3, 1, 23, 22, 13, 11, -5, -13, -14, 7, 20, 6, -7, 5, -3, 8, 1, 15, -16, -8, -7, -16, 0, -6, 2, 11, -31, -11, -2, -5, -5, 7, 17, -1, 6, -15, 17, -4, -15, 2, -10, -10, -6, 7, 1, -9, 2, -5, -1, 8, 7, -12, 5, -4, 14, 12, -14, -2, -10, 14, -1, 7}
}
, {{13, -6, 6, 26, -21, 23, -30, -24, -20, -1, 2, -14, -15, 15, -13, -2, -8, 6, -2, 2, -6, -10, 15, 1, -8, 1, 2, 9, 7, 1, -1, 5, 4, 4, 13, -15, 7, 1, -7, -11, -1, -20, 29, 0, -6, -8, 4, 34, -18, 12, 2, 1, -11, 9, 7, 13, -3, 0, 1, 20, -9, 21, 0, 16}
, {-10, 6, -1, -19, -1, -9, -3, -4, -13, 9, -14, 16, 15, 16, -30, 13, 6, 0, -1, -6, -4, -10, 3, -1, 1, 16, -34, -6, -55, 11, -14, -1, 13, -15, -14, 9, 1, -3, 12, 2, -13, 11, -2, -25, 23, 11, -4, 10, 35, -11, -18, -5, 8, -22, 7, -13, 25, -6, -2, 17, -11, -8, -11, -27}
, {2, -4, 10, -6, 2, 22, 6, 7, -12, -12, 9, 22, -8, -12, 3, 6, -26, 9, 4, 10, 1, 9, -15, -27, -6, -6, 2, 1, -2, 24, -14, 13, -9, 6, -9, 9, 9, 3, -6, 3, -11, -14, -3, 4, -9, 5, 11, 0, -3, -5, -6, -20, 6, 0, 2, -5, -30, -3, 6, -11, 12, 18, 5, -8}
}
, {{-6, 0, -8, 13, 2, -1, -6, 1, 1, -15, -13, -9, 7, 3, 20, -6, -14, 3, 11, 30, -15, -7, -6, -15, -8, 19, -3, 7, -1, 13, -10, -14, -1, -9, -1, -8, -6, 9, -19, 5, -8, -1, -12, 7, -9, 12, -1, -6, -10, -1, 4, -12, 11, 7, 2, -3, -12, 0, 3, -6, 12, -10, 18, 3}
, {7, 2, -6, -12, 1, -9, -16, -8, 10, 8, 12, 9, -23, -14, -6, -15, 7, -36, 18, -6, 13, 8, -14, 22, -17, -18, 1, -17, -11, -11, -5, 0, -17, 18, 6, -3, 15, -30, -9, -27, -28, 2, -6, -6, 4, -9, 5, 10, -8, -17, -18, -12, -27, 3, -4, -2, -2, -3, -3, -8, -5, -2, -14, 2}
, {5, -4, -6, 1, -16, -18, -11, -8, 4, 19, 6, 10, 4, -12, 1, -3, -17, -12, 22, 5, 6, -9, -14, -13, -5, 3, 7, -7, -1, 2, 7, 19, -20, 17, 8, 4, -10, -2, -3, 14, 11, 5, 17, 18, 23, 18, -19, 7, 2, 9, 13, -14, -16, 23, 14, 20, 15, -5, 15, 18, 17, -10, -3, 2}
}
}
, {{{-14, -4, 9, 18, 0, -11, -11, 10, 11, 21, 12, 7, -2, -14, 3, 7, 10, 5, -2, -11, -11, 3, 20, 4, -21, -10, 13, -20, -25, -16, -1, -6, 4, 7, -9, 14, -18, 7, 4, 5, -15, 0, -2, -1, -14, -9, -18, 7, 13, 10, -8, -7, 28, -6, 1, 16, 3, 9, 14, 21, -18, -34, 2, 15}
, {8, 3, -1, 11, -3, -2, -16, 0, -1, -3, 8, -2, -12, -9, -8, 13, 6, -6, -8, 18, -5, -1, 3, 2, 3, -14, 13, 5, -19, 0, 2, 0, 13, -6, -1, 8, 7, -15, -1, -1, 23, 5, 7, -10, 2, -9, -9, -6, 25, -4, -1, 9, 13, 8, 5, 10, 24, 11, 8, -4, -5, -13, -4, 18}
, {14, -6, -7, 3, 3, 1, 7, 0, -7, -3, 16, -15, -1, -5, -16, 17, 14, -20, -2, 5, -5, -2, 2, 7, -16, 4, 1, 4, -12, -5, -3, 11, -1, -3, 19, -2, 10, 6, -22, -8, -26, 7, -12, 0, -12, 16, 12, 11, 14, 0, -13, 0, 9, 16, 0, 4, -5, 3, 0, -13, 23, -11, -5, -5}
}
, {{-12, 6, -3, 8, 10, -12, -30, 17, 3, 1, 0, 6, -9, 3, 6, -13, 15, -2, 17, -7, 14, 2, 13, 3, 18, 10, -3, -9, 2, 0, 0, 5, -11, 12, 14, -17, -6, -5, -5, -9, -23, -14, 1, -14, 10, 4, -3, 5, -2, 9, 14, 10, -8, 9, 9, -7, -12, 8, -2, -5, -10, 15, -10, -8}
, {-21, 33, 2, -3, 11, -8, 12, 18, -5, 0, -12, 12, -20, -20, -3, -10, -8, -13, -13, -28, 18, -7, -2, -8, -12, -26, -6, -24, 4, 22, -1, -33, -13, 10, -20, -11, -11, -8, 1, -2, -15, 15, -8, -5, 4, -3, -22, 20, -5, -4, 16, -12, -11, -13, -4, -12, -5, -15, -2, 1, -24, 3, -21, -9}
, {19, 18, -1, 7, 0, 18, -1, 21, -3, -9, -1, 21, -15, -19, -11, -10, 8, -1, -25, -3, 19, 0, 3, -3, -23, -7, 3, 19, -7, 3, -2, -8, 16, 17, 20, 12, -22, -21, 13, 13, -2, -12, 9, 7, 11, 7, 4, 0, -7, -1, -19, 10, 10, 4, -12, -5, -7, 11, 17, 19, -9, -18, -9, 21}
}
, {{-12, 10, -11, -4, -3, -9, 1, -3, 0, -15, -18, 0, 6, 1, 20, -16, -22, -3, -3, -11, 10, -8, -2, -17, 11, -30, 21, -12, -14, 3, 20, 3, 20, 24, 19, 20, -9, -1, 1, 12, -16, 3, 23, 22, -14, 8, 4, -13, -13, 12, 21, -10, -16, -6, -31, -20, -7, 13, -7, -16, -9, 3, 8, 4}
, {14, 8, -6, 7, 1, -15, 14, 1, -18, -3, -8, -20, 18, -10, 13, 19, -3, -3, -1, -11, 18, 2, 6, -1, 1, 15, 6, -23, -15, -6, 5, -23, -15, 6, -15, 12, -2, 18, -10, 1, -31, 19, -2, 26, -19, 15, 9, -5, -18, 15, 6, -14, 29, -11, 1, 12, -4, -17, 8, 16, -1, 14, 6, -43}
, {2, 6, -29, -10, -6, -17, -10, -2, -12, -5, 8, 5, -4, -18, -10, 9, -9, -14, 22, -7, 6, 18, 9, -10, 31, -3, 10, 8, -11, -24, -14, -12, -27, 0, -17, 11, -1, 20, -13, -9, -2, 11, -7, 29, -5, 5, -7, 5, 12, -15, -11, -25, -31, -30, -1, 6, -10, 0, -3, 1, -5, 21, 0, -3}
}
}
, {{{-11, 10, -12, 6, 5, -11, 6, -13, -13, -7, 1, 32, -13, -23, -2, 4, -16, -18, -9, 0, -8, -2, 14, 3, -16, -22, -3, -5, 12, 3, -2, -6, -5, -8, -7, -25, -5, -8, -3, -3, 11, -17, 17, -13, 20, -8, -8, -1, 12, -14, -4, 3, 11, 7, 20, -2, 1, -12, 2, -5, 22, -3, 5, 3}
, {2, -6, -15, 6, -11, 9, 6, -5, 2, 0, -8, 5, 6, -25, -1, -7, 13, -17, -13, -6, 4, -10, -5, 8, -11, -7, -12, 0, 8, -1, 16, 14, -9, -22, -9, 12, -4, -3, -17, -8, 10, -19, 14, -15, 0, 3, -20, -1, 2, -1, -22, -8, 0, 9, 6, -10, 0, 5, 6, -7, -2, -2, -13, 4}
, {-3, 8, -7, -3, -14, -2, -2, 8, 12, 3, 9, 15, -2, -5, -6, -4, 14, -21, -15, -12, 7, -15, 4, 7, 11, 3, -5, -4, -4, 1, 7, 0, 10, -8, 3, -19, -13, -24, -14, -9, -1, -4, -8, -19, 11, 11, -15, 9, -17, -1, -3, 7, -13, -5, -26, -14, -13, 7, 2, -19, -14, 3, -22, 2}
}
, {{-17, -16, 3, -11, 24, -4, -8, 10, 13, -6, -4, 1, 4, -9, -22, -4, -9, -1, -7, -9, -3, -23, 1, 14, -11, -16, 3, -3, -21, -7, -6, -31, -5, -16, -5, 19, -13, 11, -1, 16, -14, -6, -26, -1, 2, 6, -11, -3, -7, -17, 11, 17, 17, -2, -3, -15, -1, -6, 10, -18, 6, -11, -4, 0}
, {-21, -8, -15, -18, 12, -15, 23, 15, -18, -6, -18, 19, -3, -15, 12, -16, 26, -19, 3, 9, 24, -7, -8, -29, -3, -30, 9, 1, -9, -7, -28, -21, 24, -15, -6, -2, -4, 10, -1, 22, -9, 19, -26, 2, 38, 12, -10, 12, -11, -5, -5, -11, 1, -23, -4, 1, 23, -25, 9, -28, 7, 9, -4, -23}
, {27, -19, -14, -7, 10, 13, -4, 5, 8, -9, -15, -6, -5, 10, 6, 1, 17, 6, -4, 25, 5, 1, -10, -22, -16, 10, 5, 12, 3, 21, -7, 2, 11, -15, 16, 14, -17, -5, 11, -6, 6, -16, 3, 15, -10, -21, 0, 7, 0, -2, 15, -22, -16, 6, 8, -15, -12, 8, 3, -24, -16, -5, 24, 7}
}
, {{-8, -6, -16, 9, 8, -11, 0, 14, -6, -6, -15, 7, -5, -6, -6, 17, -10, -27, 0, -17, -17, 12, -1, -7, -5, -10, 1, 6, 8, -11, 3, 5, -3, 17, 12, -10, 7, 2, 9, 15, 16, -8, 27, 13, -4, -10, -13, -3, 9, -20, 11, 20, -7, -1, 9, 3, -6, -4, -17, 8, -2, 7, 14, 6}
, {31, -10, -15, -10, 8, 5, 13, -35, -34, 7, -8, -12, -9, -5, 18, 2, 23, -31, 11, 24, 6, -10, 17, 14, 2, -6, -3, -16, 4, -25, 0, 2, -13, -3, -4, -12, 8, 8, -24, -32, -15, -1, 3, 11, 6, 25, 7, -23, 17, -25, -28, -3, 2, -13, -11, -5, -4, 14, -1, -7, 4, 10, -5, 5}
, {-20, 3, -12, -25, 1, 14, -24, 18, 14, -2, -16, 3, 8, 8, 14, -17, 5, -14, -10, 0, -2, 0, 3, -34, 7, -4, -17, -9, -16, -18, 2, 10, 17, -2, 12, -4, -16, 3, 12, 7, -18, -30, 10, -13, 3, -3, -6, 4, -1, 13, -25, 16, -9, 4, -3, -21, -14, 12, -6, 2, -30, -23, -7, -17}
}
}
, {{{5, 2, -8, 13, 20, 15, -7, 2, -2, 8, -16, 6, -12, 5, -10, -5, -11, 7, 4, 5, 14, 0, 2, -6, -2, 4, 13, 3, -4, -2, 0, -9, 5, -11, 14, -2, 22, 15, 2, -5, 14, 14, 2, 5, -15, 9, -2, 15, -5, -5, 12, 4, -4, -11, -12, -10, -10, 22, 0, -11, 19, 1, 0, -11}
, {-12, -13, -1, -13, -5, 14, 6, -5, 3, -2, -2, -20, -18, -3, 3, -16, -5, -10, 1, 9, 12, -1, -1, 5, 2, -7, -1, -6, -4, 3, -5, -4, -1, -15, -8, -4, 7, -14, -14, 7, 3, 1, -10, -6, 3, -2, 7, 3, 21, -9, 5, 10, 5, 4, -1, 9, 13, 0, -13, -6, -20, 2, -6, -6}
, {4, 2, -1, 13, 3, -11, -5, 1, 2, 17, 8, 9, 2, -19, 0, -12, 29, -11, 15, -4, -6, -2, -2, -6, 4, -5, -4, -9, -5, -8, -3, 7, 6, 0, -11, 2, 16, -7, -2, 7, -19, -3, -10, -13, -12, -11, 2, -7, 3, -15, 4, 2, -7, 22, -3, -1, -1, -4, -6, -8, -6, 8, 16, 2}
}
, {{-7, 2, -14, 10, 6, 9, 3, 7, 3, -3, -3, -5, -9, -5, 2, 0, 6, -11, -2, -9, 5, 16, 1, 0, -8, 10, 6, -8, -6, 7, 10, -3, 7, -13, 0, 12, -5, 11, 12, 0, -4, 6, -6, 2, -4, -16, 11, -5, -13, 24, 13, -1, 1, 12, 5, -2, -22, 7, 1, -1, -16, 14, 15, -2}
, {2, -4, -27, 7, -17, 11, -33, 3, -8, 17, 2, -4, -13, -9, -13, -17, -3, -8, -23, 5, -9, -22, -9, 10, -30, 16, 3, -6, 12, -1, 7, 4, -10, 3, -6, -16, 16, -10, -4, 5, 5, -24, 2, 4, 0, -26, 4, 2, -4, -26, -6, -2, -12, -5, -7, -1, 7, -16, 7, 3, -12, -13, -30, 20}
, {-4, -3, 11, 3, 7, -12, 0, 3, -5, -8, 3, -1, 1, -15, -13, -1, -3, -22, -9, 2, 11, -16, -2, -11, 3, 2, 7, 5, 10, 24, -15, -1, 11, -5, 11, 6, -3, -4, 4, 8, 14, -13, 5, 1, -7, 15, 4, -2, 7, -18, 6, 3, 1, -8, -14, 1, 3, 3, -10, 6, -5, 0, 16, -10}
}
, {{-1, -20, -6, -19, -7, -7, -2, -6, -12, -2, 2, 0, 7, 5, 10, 6, 1, -2, -8, 1, -1, -3, 0, -15, 9, -10, 11, -5, -4, 4, -12, -6, -14, -9, 8, 4, -9, 14, -13, -2, 3, 4, -10, -1, -18, 10, -1, -6, -3, 5, 15, -18, -6, -9, -6, -10, -1, 16, -12, -21, -5, -7, 11, 1}
, {-9, 14, 12, -22, -11, 1, 16, -1, -6, -11, 6, -11, 11, -18, 13, 4, 10, 15, -6, 12, -16, -1, -4, 4, 10, 4, -10, -6, -29, 8, 8, 1, -17, 10, -4, -12, 5, -15, -21, -6, -7, -4, -10, 1, -9, 15, -10, -11, 7, -18, 11, -4, -11, -29, -4, -5, 18, -21, 10, -30, -1, 1, 5, -14}
, {14, -10, -14, -5, -19, 10, -20, 2, -17, 6, 8, 17, 3, 4, 7, -11, 9, 16, 13, 12, -18, -24, 4, -11, -7, 5, -4, -1, 9, -28, 2, 2, 3, -3, -19, -23, 0, 4, 0, -8, 24, -17, 4, -10, -19, -1, 12, 6, 18, -20, -2, 7, -19, -6, -2, -12, 4, 1, -28, -4, -16, -11, 17, 4}
}
}
, {{{-13, 4, -9, 9, 8, 2, 12, -2, 13, 5, -10, 5, 20, 3, -9, 6, -8, -2, -2, 2, -16, -17, -8, -11, -23, 5, -13, 3, -14, -8, 8, 4, -4, -1, -10, -3, -10, 24, -9, 5, 20, 5, 1, 10, 3, -6, 5, -2, -5, 2, 8, 0, -12, -8, -19, 3, -11, 1, 2, 5, 8, -3, 22, -8}
, {14, -20, -16, 8, -8, -1, -25, 1, -3, 3, 1, -6, -10, -6, -13, 3, 4, -1, 11, 2, -3, -9, 9, -12, -16, -7, 8, 7, 19, 8, -1, 10, -3, 1, 8, 11, 4, 17, 0, -4, 17, -26, -15, 3, -15, -2, -1, 1, -14, -9, -6, -5, 6, 5, -7, 15, -3, 11, 16, -5, -12, 19, 22, 3}
, {-7, -19, -1, -1, -3, -9, 6, 7, 14, 8, 17, 4, -1, -7, -3, -11, 11, -17, 25, -5, -12, -16, 12, -7, -4, 25, 0, 2, -14, 6, 18, -9, 12, 16, 7, 11, -21, 3, 8, 5, -8, 10, -18, -6, -8, -11, 17, -7, -6, 10, 5, 2, -10, 18, -6, 12, 17, 5, 2, 0, 12, -2, -3, -2}
}
, {{-6, -1, -11, 3, -16, -13, 15, -9, 0, 3, -7, -14, 12, -5, -2, -29, -20, -16, -13, 3, -15, 4, -8, -18, 18, -11, 5, -26, -3, -4, -6, 5, -21, 13, 5, 0, 6, 6, -20, -3, 16, 12, 13, 11, 3, 23, -19, 24, -3, -26, -13, 4, -9, -5, 7, -6, -7, 0, -16, -10, -11, 0, 5, -14}
, {-26, 4, -7, -30, -2, -4, 10, -8, 2, -10, -2, 27, -4, -3, 18, -10, -1, -8, -5, -1, 0, -2, -16, 2, 14, -23, 24, 6, -17, 4, 0, -25, 12, 25, -7, -3, -18, -1, 14, -4, -28, 20, -8, 7, -1, 17, -20, 7, -22, 13, -2, -6, -1, 17, 2, -2, 12, -1, 15, -24, 7, 23, 3, -23}
, {-19, -22, -18, 5, 0, 9, 6, -15, -19, 14, -8, 12, -11, 11, 21, -3, 5, -3, -12, 4, 8, -22, 4, 9, -6, 3, -6, 3, 0, 6, 18, -15, -35, 21, -1, -6, -22, 25, -2, -1, -4, 16, -11, 29, -20, 27, -4, 0, -13, -4, -2, -12, 1, 9, 5, -3, 4, 2, 1, 7, 16, 11, -7, 12}
}
, {{6, -11, -3, -25, -2, 25, -4, 13, -5, 0, -21, 0, 13, -9, 0, -4, 10, -15, -17, -3, -5, 9, -11, 1, -3, -2, 1, -8, 10, -8, -7, 6, -8, -3, -6, -2, 13, 4, -1, 3, 9, 2, -15, 14, -7, 20, 18, -3, -2, -14, -23, 2, 7, 1, 6, -10, -9, 6, -9, 7, -11, 1, 7, 1}
, {1, -19, 22, -16, -21, -19, -16, 8, -15, -6, 10, 13, -7, -15, 1, -12, 5, -16, 1, -17, -19, -1, -10, 19, -24, 9, 2, -16, 21, 29, 19, 5, 19, -22, -1, -8, -22, -20, -5, -4, 16, -35, -29, -15, 1, -1, 12, 4, 18, -18, 7, 4, -3, 15, 10, -32, -3, -2, 2, -37, 16, -11, -17, 9}
, {19, 9, -9, -3, 5, 23, -4, 19, -4, -3, -16, -3, -9, 10, 3, -11, 13, -7, -12, 1, -18, 17, 18, 6, -3, 5, 8, 2, 21, -13, 1, -10, 4, -8, -11, -16, 10, 2, -3, 10, 2, -8, -4, -6, -23, 10, 33, 0, 18, -21, -3, 2, 4, 5, 5, 1, 1, 14, -18, 4, -5, -6, -4, 24}
}
}
, {{{6, 3, -6, -5, -10, 6, 3, -20, -9, -5, -8, -12, -8, 8, -22, 8, -7, 18, -7, 20, 13, 0, -15, -22, -11, -11, -4, 8, 1, -5, -12, -1, 1, 12, 8, -18, 4, -12, 0, 6, 5, -8, 9, 2, -11, -6, -4, 13, 3, -12, -8, -11, 0, -10, -6, -13, -2, -8, -4, 3, -3, 13, -8, -5}
, {-15, 19, -2, 5, -3, -2, 9, -7, -1, 13, 2, -10, 11, 7, -11, -3, -12, -10, -16, -5, -1, -11, -11, -12, 6, -10, -3, 4, -4, -1, 8, -15, 8, 8, -14, 9, 13, 6, -13, 5, 8, 13, -9, 6, -8, 10, 2, -16, 5, -3, 2, -4, -8, 9, 9, 0, 4, -4, -15, 9, -9, -7, 7, -11}
, {-10, -2, 8, -12, -15, 3, 8, -3, -11, 11, 5, 10, 3, 4, -7, -12, 6, -11, -3, 7, -28, -13, 4, 14, -15, -2, 9, -17, -1, 8, 18, -11, -11, 12, -1, 3, 4, 4, -5, -2, 1, 13, -6, -6, -25, -9, 6, 15, 0, -12, 7, 0, 5, 13, 0, -7, 5, -9, -6, 9, -6, -17, 3, -11}
}
, {{-13, 4, -17, 1, -9, -4, 3, -9, -5, 3, 3, -13, 4, -11, -14, -17, -14, 3, -12, 5, -9, -10, -2, 8, -4, 12, 7, -1, -7, 2, 0, 12, 3, 3, 0, 3, 9, -11, -19, 15, 17, 6, -5, 16, 7, 11, -14, 14, -1, -20, -6, -16, -19, -20, 11, -12, -15, -7, 6, -2, -20, -12, 15, -14}
, {-27, -31, -12, -15, -1, -20, -23, 12, 30, -17, 20, 13, -8, 3, 8, -13, -9, 3, -4, -16, -33, 1, -31, -54, -14, -7, 28, -24, 19, -9, -27, -1, 16, 11, 30, 17, -45, -13, 18, 2, 22, -37, 9, 7, 2, -11, -50, -24, -12, -21, -13, -9, 2, 13, 37, -7, -18, 19, 2, 10, -7, 26, -2, -15}
, {-14, -22, -4, -2, 7, -13, -5, -9, -21, 8, 11, 12, 0, -1, 1, 1, 9, -17, 5, 0, -14, 6, -2, -5, -3, 8, -13, 0, -12, 4, 3, -32, -11, 8, -9, -14, -14, 17, 1, -7, -17, 2, -14, -21, -13, 8, 0, -7, 2, -6, 1, 5, -5, 1, -18, -5, -7, -10, 0, 0, -12, -5, 5, 2}
}
, {{5, 7, -16, -2, 2, 7, -11, 18, 5, 8, -5, -9, -17, -10, 0, 14, 11, -1, 10, -19, -10, -12, 11, -2, -11, 5, 9, 2, 5, 13, 13, -5, 12, -7, 4, -2, 5, 10, 1, -4, 0, -3, -21, 0, 7, -6, -5, -5, -2, 0, 6, 3, 10, 16, -3, -7, -8, 0, 16, 1, -4, 5, -13, 5}
, {-4, 1, -8, -26, -1, -15, 1, -4, -21, -1, 4, 1, -9, -12, -2, -5, -3, -34, 12, -8, 4, -12, 7, 15, -5, 16, 12, -11, 15, 17, 12, -5, 5, -2, 9, -19, 0, -3, -2, -6, 6, 10, -2, -14, -1, -15, -5, 1, 4, 8, -11, 1, -33, 16, 3, -28, -13, -5, -2, -12, 8, -9, -11, -17}
, {-7, 9, -7, 0, 8, -18, -4, 5, 18, 8, -3, -2, -13, -2, -4, -17, 16, 2, -3, -4, -9, -7, 9, 2, -23, -4, -12, -2, -13, 7, 12, 9, 2, -4, -10, 0, -1, 2, -8, 12, 13, -13, 3, -15, -11, -5, -3, 2, 4, -2, 15, 11, -8, 12, 9, -16, 0, -12, -2, 14, -5, -14, -5, 15}
}
}
, {{{3, -4, 11, 17, -19, -7, -4, 4, -3, -1, -14, 1, -9, -1, -1, -1, 3, -2, -16, -1, -3, 20, 11, -6, -8, 12, 10, 6, -8, -11, -2, 1, -3, 17, 11, 3, -6, 2, 3, 8, 13, 1, -3, 1, -4, 0, 0, 3, 3, 3, -7, -5, 3, -3, -1, 4, -15, 1, 8, 12, -20, -12, 4, -2}
, {-4, 13, 10, -15, -23, 4, -10, 9, 14, 3, -13, -23, 2, 14, -17, 4, -1, 7, 1, -8, -3, 7, 3, 11, -4, -30, -13, -2, -15, 3, 12, 9, -3, -6, 23, 10, 14, -1, -3, -11, 21, 2, -8, -9, -3, -8, -34, -15, 36, 0, -24, -26, 5, 21, -5, 8, -11, -25, 4, 13, 7, -10, -2, 0}
, {-12, -5, -13, -14, 2, 4, -11, 3, -10, -3, -16, -7, -10, 4, 3, -15, -7, -13, -1, 9, -10, 5, -17, 2, 1, 15, 1, 14, -6, 0, 7, 7, 9, -13, 18, -7, -11, -20, -6, -18, 5, -5, -1, -9, -9, 1, 5, -5, 14, -12, -5, -9, -9, 4, -1, -8, -8, 4, -19, -9, -15, -5, -20, 16}
}
, {{-8, 9, 4, 19, 3, -3, -3, 5, 12, -1, 26, 8, 2, 11, 12, 5, 1, -16, -18, 6, -9, -4, -5, -8, 14, 10, 13, -12, 4, 3, -8, -18, -16, 6, -20, 1, -17, -14, 7, 11, -12, 19, -3, 5, -10, -8, 15, -2, -8, 3, -3, 10, 5, 17, -4, 7, -5, 5, 6, -14, 5, -5, -5, -2}
, {-34, 4, -3, -11, 8, 15, 4, -35, -29, -3, -21, -10, 3, 35, 16, 8, 6, -16, -17, -3, -2, 7, -3, 15, -7, -6, -7, 20, -4, 11, -1, -3, -15, -3, 15, -14, 21, 1, -22, -23, -5, 11, 19, 20, 13, -2, -17, -16, -12, 31, 16, 16, -10, 1, -3, 11, 6, 8, 3, 12, 0, -12, -1, 0}
, {-2, -14, -32, 5, -32, 10, 8, 5, 2, -7, -5, 1, -2, 16, 8, -17, -6, 2, -15, 2, -7, 1, -6, -11, 14, 14, 15, 6, 5, 6, 15, 2, -3, 30, 8, -14, 14, -32, -17, 2, -2, -15, 17, 11, -3, -1, 11, -3, -11, -9, -8, -8, -7, 21, -1, 6, -23, 3, -8, 13, -11, -5, -3, 16}
}
, {{-6, -19, -1, 0, 12, -19, -13, -6, -5, -13, -28, 8, 8, -17, 17, -20, -19, -3, 6, 18, -24, -4, -7, 3, -4, -6, -19, -5, -1, -3, -4, -4, -7, 15, -6, 5, 4, -4, -4, 6, -12, -24, 2, 11, -13, 0, -14, 7, 3, -6, 15, -7, 4, 9, -8, 4, -8, 3, -16, -5, 2, -22, -6, -23}
, {-19, -6, -2, -1, 1, 13, -2, -12, 8, 9, -5, 12, 2, -3, 24, 3, -18, 4, -3, 3, -1, 10, -1, -6, -5, 4, -7, -16, -3, 9, 3, -23, -25, 0, 11, -26, 0, -12, -11, -29, -5, 11, 5, 10, 5, 5, 9, -5, -5, -6, 14, 16, -3, -7, -17, -1, -23, -15, 26, -3, 25, 0, -16, -9}
, {8, 17, 9, 7, -10, 14, 24, 4, -2, -15, 20, -15, -14, 2, -3, -2, -16, -4, 2, 0, 17, 32, 4, 10, -7, -7, 22, 10, -6, 7, -5, 0, -29, -3, 11, 9, 22, -5, -11, -10, -6, 14, -5, 22, 8, -3, -18, -13, -3, 30, 11, -5, -2, 2, -7, 23, -11, 9, 16, -17, 15, -15, 20, -26}
}
}
, {{{2, -2, -5, 5, 12, 22, -25, -6, -11, 12, -5, 10, -16, 0, 4, 1, -11, 3, 7, -15, -2, -8, -3, -7, -13, 12, 2, 3, -3, 5, -19, 1, 6, -1, -15, -4, -14, -7, 8, 1, 9, -4, -19, -13, -6, 8, -9, 3, 2, 2, 5, 7, -30, 12, 9, -2, 6, -1, -11, -10, 4, -2, -7, 1}
, {12, -14, 6, 2, 9, 21, -7, 2, -8, -1, -4, 11, 0, 16, -13, -14, 8, -1, 0, 11, 9, 2, 8, -5, 2, 15, 0, 11, -2, 4, -21, 11, 12, -15, 3, 0, 12, -20, -5, 0, 0, -9, -13, -4, 8, -15, 1, -9, 1, -2, 10, 24, 1, -11, 4, -2, 1, 3, 8, -1, 11, -18, -1, 5}
, {10, 0, -11, 4, 13, -12, -12, -1, 2, 2, 13, -8, 9, 0, -12, 9, 10, -9, 6, 5, -16, -9, 12, 4, 5, 4, 1, -4, -19, -9, 6, 11, 24, 6, 25, 17, 28, -11, -8, -13, -20, -10, 0, 17, 1, 15, 1, -9, -4, -2, 2, 3, -17, -1, -9, -7, -6, 3, -8, -16, -39, -9, -3, -17}
}
, {{-5, 4, -11, -10, -13, 6, -6, 13, -5, -2, -6, 4, -10, 2, -6, 14, -8, 4, 3, 7, 23, -7, 15, 4, 15, 25, 13, -38, 4, -1, 9, 7, -29, -18, 2, 4, -13, -4, -11, -7, 12, 0, -15, -23, -7, -8, -5, -2, 9, -12, -4, 1, -19, -2, -3, -11, -1, 14, 1, -28, -25, -10, -21, -1}
, {7, 9, -37, -4, -10, -10, 3, 25, -5, -15, -5, 18, 14, -24, -20, -10, -13, -21, -1, -4, 4, -9, -19, -10, 17, -40, 25, -3, 17, 23, 2, -11, 14, -2, -1, 7, -7, -3, -14, 0, 5, 30, -2, -4, 18, 5, -2, 14, 23, -21, -7, 10, -28, -4, -8, -8, -9, -12, -18, -28, -4, -16, 9, -18}
, {3, 19, -6, -8, 33, -1, 6, 12, 1, 13, 7, -6, -14, -15, -20, -13, 3, -25, -13, 1, -5, -9, -11, 5, 10, 3, 5, 19, 1, 2, -9, 1, 1, -10, 23, 12, 8, 5, -5, -16, 10, -1, -9, 4, 13, -8, -24, -1, -2, 8, -8, -4, -13, 14, -11, 6, -6, 2, 20, -23, -2, 8, -9, 15}
}
, {{-8, -10, -27, -4, 1, -14, -15, -4, 0, -7, 0, 1, 18, 20, 15, -1, 13, 16, -3, 0, -3, 1, 6, 11, 0, 3, 15, -12, -31, 9, 23, -1, 1, -10, 3, 13, -2, -3, -11, 0, -10, -12, 11, -3, 3, -1, -2, 2, -3, -1, -21, -8, -1, -8, 0, 11, 2, 23, -9, -11, -21, -2, -3, -9}
, {7, 7, -2, 3, 7, -4, 17, 5, -15, 1, -13, -7, 13, 5, 1, 12, 11, -3, -14, -7, 23, 15, -17, 0, 30, 10, 4, -3, 4, 11, 12, -2, 4, -6, -13, -18, -6, 20, -20, 6, 3, -3, 4, 22, 8, 17, 11, -1, -28, 23, 13, 6, 14, -1, -4, 13, -9, 2, -12, 0, -21, -12, 15, -23}
, {1, -15, -6, -10, 8, -2, -13, 8, 15, -7, 10, -6, -8, -9, -18, -1, 6, 11, -3, 0, -21, 5, 2, -12, -10, 4, -9, -1, -9, -8, 2, 3, -1, -5, -14, 4, 0, 2, -22, -3, 19, 1, -15, -2, -25, 0, -13, 17, -2, -13, -17, -16, -38, -5, -16, 5, -9, -8, -11, 8, -14, 20, 0, 6}
}
}
, {{{4, -3, -1, -12, -14, -5, 17, -5, 11, -2, -10, -13, -9, -10, 5, 1, -1, -15, 2, -11, 5, 6, 3, 3, 11, 3, 10, -12, 4, 18, 7, -2, -4, 11, 0, -2, 6, -27, -8, -6, 7, 17, -25, 15, 12, 2, -9, -10, 0, -21, -10, 3, -13, -2, 5, 6, 8, -16, 11, -5, -14, -15, -14, -7}
, {-19, -3, 3, -3, -9, 2, 10, 1, 2, -3, 18, -6, -2, 7, 15, -5, -9, 8, -9, -15, -13, 2, -11, -8, -6, -1, 6, -12, 8, 0, 3, -9, -22, -9, -15, 16, -1, -12, 6, -14, 15, 10, -13, 6, 5, 2, -8, -2, -20, -26, 4, 15, -15, 5, 4, 4, -5, -3, 2, -7, -11, 0, -12, 5}
, {-3, -2, -4, -1, -1, -4, 5, -3, 0, -17, 3, 1, -13, -10, -2, -7, -14, 3, -4, -21, 7, 7, 5, -8, 0, -3, 4, -5, 2, -9, -11, 8, -5, -16, -18, -9, -11, -17, 5, -16, 6, 6, -6, -7, -23, 0, -12, 10, -7, 19, -1, -6, -25, 5, 5, 12, -1, 19, -24, -14, -9, -15, 9, 0}
}
, {{11, -7, 8, -9, 6, 9, -7, 13, 13, -2, -15, 6, 12, -14, 7, -11, 0, 11, 20, -11, -1, -13, -9, 0, -4, 7, -16, 16, -16, 8, -1, 22, -10, -9, -3, 0, -7, -1, 0, -17, -17, -2, -14, 3, -4, -1, 0, 13, -10, -7, -2, 3, 3, -4, 1, 3, 2, 17, 1, -9, 19, 2, -19, -11}
, {19, 23, 5, 6, 10, 10, -30, 8, -10, -38, -17, 12, -7, -9, -6, -18, 16, 6, 18, 5, -18, -22, -15, -4, -14, 17, -39, 8, 1, 8, -3, 25, 20, 3, -13, -11, -7, 31, -1, 15, 3, -26, -5, -14, -21, 27, 42, -16, -1, -3, 15, -16, -12, -2, -10, -2, 1, 4, -2, -10, 12, -18, -2, 9}
, {9, -11, -16, -6, -2, -3, 18, -28, -12, 1, 10, -30, 9, 5, -11, -2, 13, -9, 12, -1, 16, -3, 7, -9, -13, 0, 20, 5, 3, -5, -1, -18, -8, -27, -22, 9, 0, -18, -3, 3, -3, 15, -11, 14, -7, 3, -4, -7, -2, -8, -1, 12, -10, -1, -2, -4, -1, 0, -14, -1, -8, 3, -2, -8}
}
, {{-10, 7, -3, -4, -11, -12, 1, -6, -27, -6, -2, -18, 13, -7, -3, -17, 8, 7, 3, -19, -1, 8, -5, -4, 7, 21, -6, -8, -6, -15, 8, 8, -5, 7, 0, 19, -8, -10, -3, 11, -4, -15, 1, -20, -5, -3, -10, -8, -4, -12, 6, 7, -7, -12, -6, 3, 1, -2, -5, -8, -13, -33, -3, 0}
, {-7, 1, 12, 6, -7, 17, 2, 18, 9, -6, -2, -11, 26, -17, 5, 5, -6, 1, -4, -6, -7, 3, 2, 10, 16, 7, 6, -13, -15, 4, 1, 20, -35, 3, -6, 14, 1, 1, -15, 5, 6, 11, -4, -3, 1, 2, -6, -17, -16, 6, -2, -12, -3, 0, 13, 13, 7, 5, -13, 6, -11, -11, 17, 12}
, {0, 1, -12, -19, 4, 0, -8, 15, 5, 3, -1, 11, -13, -17, 9, -9, 2, -1, 12, -10, 8, 7, -6, -2, -5, 14, 20, -26, -18, -10, -9, -11, -1, 1, -1, 10, 2, 1, 10, 1, -6, -5, -19, -7, 7, -17, 12, -6, 17, -23, -11, 5, -14, -11, 8, 1, -19, -8, -12, -6, -18, 13, -14, 1}
}
}
, {{{7, 3, -6, -3, -6, 1, 0, -11, -6, 10, 13, 11, 9, 3, 3, 0, -8, 6, -10, 6, -2, 3, -1, -19, -25, -17, 9, 2, -4, -7, -17, 22, 9, 20, 5, 3, 1, 3, 10, 9, 16, 4, -5, 6, -4, 16, 0, -2, 3, 4, -7, 2, -17, 9, -12, 15, -4, -13, 11, 10, 26, 23, 10, 9}
, {25, -1, 2, -14, 11, -2, -9, 8, 2, 2, 7, 2, -5, -11, -8, 4, -11, -10, -4, 1, 6, 7, 5, -7, -1, 5, -4, -12, -6, 12, 6, 9, 12, 14, 2, 2, 10, -10, 1, -21, 7, 1, -6, 0, -12, 6, 4, -6, 4, -5, 7, -3, -3, 9, 7, 7, -5, -10, 8, -7, 10, 22, 3, 5}
, {10, -8, 10, -8, -4, -1, -10, -10, 2, 4, -2, 9, 5, -1, 8, -2, 19, -9, 28, -6, -3, -19, 13, -1, 6, 9, -4, 2, 13, 3, 21, 16, -12, 22, 2, -7, -11, 5, 12, 4, -9, -3, 5, 14, -19, 28, 14, 1, -8, -12, 13, 15, 17, 20, 8, 11, -3, 7, -7, 9, -3, 18, -9, -12}
}
, {{19, 6, 5, 13, 1, 3, -27, -9, -7, 6, 8, 1, -24, -9, -15, -2, 3, -3, -8, 1, 9, -5, -6, 5, -11, 12, 0, -14, 29, 15, 10, 3, -10, 12, -1, 14, 8, -7, -4, -6, 26, 0, 4, 5, 26, 14, 0, 8, -3, -13, -4, 2, 1, 11, 14, 7, -5, -10, 17, -7, 6, -15, -8, -13}
, {-13, 5, 9, 5, 22, -19, -9, 26, 32, -9, 6, -10, -31, -21, -18, -19, -14, -4, -22, -23, -30, -14, -30, -4, -4, -10, -9, -14, -11, 24, -10, -5, 13, -14, -3, 15, -15, -2, -10, -1, 20, -8, -15, -4, -4, -38, -30, -2, -18, 7, 21, -13, -1, 16, 15, -10, 23, -15, 12, -2, -14, 3, -9, -7}
, {-5, -15, 2, 22, 6, -16, 4, -8, -14, 27, -22, -5, 8, -8, 29, -1, 17, -18, -7, 9, -7, -14, 11, 1, -2, 21, -19, -19, -4, 6, 16, -23, 3, -7, -4, 17, 4, 21, 4, -5, -17, -20, -9, -26, -3, 5, 10, -24, 7, 22, 20, 2, 2, -10, 5, -14, 12, -2, -3, 1, -13, -3, 0, 17}
}
, {{0, 10, -14, -10, 21, -8, -3, 22, -5, 7, 1, -1, 13, 2, -2, 5, -3, -2, 0, -8, -17, 0, 7, -4, 4, 17, -6, 2, -12, 9, -14, -7, 17, -2, 10, -1, 2, 10, 11, -6, -12, -4, -8, -9, -10, -4, 18, 2, 1, 5, 11, -7, -3, -10, 3, -2, -20, -19, 16, -3, -28, 2, 11, 17}
, {13, 12, 2, 10, -16, 9, -8, 9, -23, -7, -2, 0, 16, -6, 2, -2, 10, 2, 1, 11, -4, -12, -15, -9, 8, 4, -24, -24, 21, -13, 2, -6, 13, -3, -3, 3, 3, 5, -12, -9, 9, -11, -6, -18, 17, -23, 4, 5, 8, 3, 15, 20, -1, -13, 13, -1, 0, -22, -1, -6, 1, -13, 10, -9}
, {-12, -15, -5, -1, -4, 4, -10, 5, -14, 11, -16, 1, -17, 19, 9, -1, 11, 6, -10, 3, -10, -7, 9, -6, -10, 5, -6, -4, -7, -9, -23, 6, 9, -5, -7, -7, -6, 16, -1, -6, 2, -10, 2, -8, -6, -20, 9, 0, 4, -5, 2, 3, -11, -29, 5, -9, -7, 8, -1, 1, -16, -24, -3, 10}
}
}
}
;

#undef INPUT_CHANNELS
#undef CONV_FILTERS
#undef CONV_KERNEL_SIZE_X
#undef CONV_KERNEL_SIZE_Y
#undef CONV_GROUPS
/**
  ******************************************************************************
  * @file    flatten.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 january 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _FLATTEN_H_
#define _FLATTEN_H_

#ifndef SINGLE_FILE
#include "number.h"
#endif

#define OUTPUT_DIM 128

typedef int16_t flatten_output_type[OUTPUT_DIM];

#if 0
void flatten(
  const number_t input[1][1][128], 			      // IN
	number_t output[OUTPUT_DIM]); 			                // OUT
#endif

#undef OUTPUT_DIM

#endif//_FLATTEN_H_
/**
  ******************************************************************************
  * @file    flatten.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 2.0.0
  * @date    26 november 2021
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "flatten.h"
#include "number.h"
#endif

#define OUTPUT_DIM 128

#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t

static inline void flatten(
  const NUMBER_T input[1][1][128], 			      // IN
	NUMBER_T output[OUTPUT_DIM]) {			                // OUT

  NUMBER_T *input_flat = (NUMBER_T *)input;

  // Copy data from input to output only if input and output don't point to the same memory address already
  if (input_flat != output) {
    for (size_t i = 0; i < OUTPUT_DIM; i++) {
      output[i] = input_flat[i];
    }
  }
}

#undef OUTPUT_DIM
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    fc.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 january 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _DENSE_H_
#define _DENSE_H_

#ifndef SINGLE_FILE
#include "number.h"
#include <stdint.h>
#endif

#define INPUT_SAMPLES 128
#define FC_UNITS 64

typedef int16_t dense_output_type[FC_UNITS];

#if 0
void dense(
  const number_t input[INPUT_SAMPLES], 			      // IN
	const number_t kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const number_t bias[FC_UNITS],			              // IN

	number_t output[FC_UNITS]); 			                // OUT
#endif

#undef INPUT_SAMPLES
#undef FC_UNITS

#endif//_DENSE_H_
/**
  ******************************************************************************
  * @file    fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "dense.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_SAMPLES 128
#define FC_UNITS 64
#define ACTIVATION_RELU

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void dense(
  const NUMBER_T input[INPUT_SAMPLES], 			      // IN
	const NUMBER_T kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const NUMBER_T bias[FC_UNITS],			              // IN

	NUMBER_T output[FC_UNITS]) {			                // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short k, z; 
  LONG_NUMBER_T output_acc;

  for (k = 0; k < FC_UNITS; k++) { 
    output_acc = 0;
    for (z = 0; z < INPUT_SAMPLES; z++) 
      output_acc = output_acc + ((LONG_NUMBER_T)kernel[k][z] * (LONG_NUMBER_T)input[z]);

    output_acc = scale(NUMBER_T, output_acc, WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

    output_acc += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


    // Activation function
#ifdef ACTIVATION_LINEAR
    // Linear (MEANS NONE)
    output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
    // ReLU
    if (output_acc < 0) {
      output[k] = 0;
    } else {
#if defined(ACTIVATION_RELU6)
      if (output_acc > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
        output_acc = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
      }
#endif
      output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
    }
#else
#error "Unsupported activation function"
#endif
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_SAMPLES];
#ifdef WITH_CMSIS_NN
  arm_fully_connected_q15(
#elif defined(WITH_NMSIS_NN)
  riscv_fully_connected_q15(
#endif
                             (q15_t*)input,
                             (q15_t*)kernel,
                             INPUT_SAMPLES,
                             FC_UNITS,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR,
                             (q15_t*)bias,
                             (q15_t*)output,
                             (q15_t*)bufferA);
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, FC_UNITS);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, FC_UNITS);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_SAMPLES
#undef FC_UNITS
#undef ACTIVATION_RELU
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_SAMPLES 128
#define FC_UNITS 64


const int16_t dense_bias[FC_UNITS] = {11, -4, 2, -1, 9, -3, 3, -5, 7, 4, 1, 3, -8, 6, 17, 1, -7, -2, -10, -4, 4, 8, 6, 2, 3, 6, 11, -9, -7, -10, -7, -1, 0, 8, 3, 5, -1, 3, -7, -5, 9, -2, 7, -1, 14, -3, -3, -2, -3, -4, 5, 6, 0, -3, -1, 10, -4, 8, -8, -4, 7, 5, -1, -4}
;

const int16_t dense_kernel[FC_UNITS][INPUT_SAMPLES] = {{0, -42, 31, -11, -12, -56, 20, -27, 7, 3, -3, -16, 7, -10, -4, -36, -10, 33, 15, 18, 22, 22, 11, 16, 3, -3, 30, 16, -21, 4, 17, -40, 11, -17, 42, 7, -19, -5, 20, 4, 45, -19, 7, -1, -28, -8, 11, 50, 5, 12, 19, -20, 2, 14, 20, 13, -10, 8, 14, -15, 18, 23, -15, 0, 13, 5, 27, -4, -12, 13, -4, -3, 24, -25, -36, 11, -28, -2, -7, 31, 0, 28, -11, 19, -26, -5, -11, -26, -1, 7, -10, -4, 24, 1, -28, 32, -33, 20, -21, 9, -2, -14, -11, 25, 7, 20, -7, 3, -1, -12, 23, -6, 35, -18, -32, -15, -24, -4, -5, 2, -14, 10, 20, -17, -14, 1, 22, -13}
, {-39, 10, -20, -20, 9, -15, 27, -51, -7, -3, -14, -23, 6, 2, -6, -20, 13, 1, 18, 11, 30, -2, -18, -3, 12, 9, 0, -15, -12, 21, 8, 12, 7, 17, 31, -19, 13, -11, -5, -9, -11, 10, 11, 5, -22, 0, 10, -8, 12, -9, -22, 32, -13, -36, -45, 22, -10, 1, 24, -6, 0, -10, -14, -18, -2, -12, 0, -6, 9, -16, -12, 18, -3, 21, -30, -32, -26, 6, -17, 16, -52, 40, 10, 22, 26, -9, -15, -11, 14, 20, 18, 40, 14, -8, 19, -26, -7, -6, -16, 5, 7, -33, -10, 29, 20, 24, -4, -4, -26, 46, 31, 39, -13, -4, -29, 4, -9, -16, -12, -13, -26, -5, 1, 10, -11, 4, 23, -49}
, {-26, -7, 17, -6, 12, 3, 1, 13, 15, 25, -16, 0, -10, 14, -14, 7, 21, -22, 17, 23, 7, -8, 5, 2, 16, -29, -36, -23, -6, 7, -32, -31, 18, -32, -9, 18, 31, 18, 22, -10, -21, 16, 9, 18, 1, 1, -14, -21, -2, -18, 7, 9, -29, 8, -22, -2, 7, 1, -32, 4, -12, -7, 15, 15, -14, -39, -55, 13, 18, 11, 39, 17, -2, -18, 24, -19, -7, -17, -2, -8, 5, -29, -26, -20, 15, -25, -35, -5, -19, 10, 12, -4, 2, 14, 17, -48, 9, -2, -20, -27, -21, -16, 31, -8, -8, 31, 17, -9, -19, 18, 29, 6, -12, 21, 4, -17, 25, 24, 15, 27, 19, -3, -22, 16, -2, 40, -13, 9}
, {-10, -48, 22, -14, 5, -34, 7, 15, 2, -40, -20, -8, -1, -6, -23, -37, -20, 10, -25, 10, 29, -19, 16, -12, -11, -2, -3, -9, -31, -2, 24, -1, -6, -4, 8, 13, 21, -9, 15, 7, -9, 1, -7, 24, 6, 0, 2, 18, -13, -29, 1, 15, -5, 16, -7, -15, 0, 17, 2, -18, -15, 22, -36, -18, -1, 22, 9, 8, -11, 10, 14, 20, -6, -1, 19, 1, -1, 11, -11, 16, -33, 14, 14, -1, 18, -9, -12, -38, 16, 15, -20, -42, -10, 12, -15, 30, 26, 3, -22, 26, -7, -15, 13, 21, -9, 57, -29, -10, -11, 45, 26, -20, 14, 5, 6, 14, -15, -36, 5, 37, 12, 20, 17, 11, -32, -18, 23, 28}
, {17, -38, -17, -25, -7, 3, -31, 24, -18, -15, 18, -15, 3, -3, 38, -9, 8, 0, -18, 24, 7, 11, -29, -7, -5, -1, -12, 17, 18, 27, 13, 1, -31, 9, 9, 2, -27, -15, -16, -14, 3, -25, 7, -8, -5, 34, -9, -6, 19, -8, 17, 17, 10, 4, -41, -33, 37, 26, -14, 17, 26, 5, 23, -14, 14, -3, 11, -24, -22, 17, -19, -13, -7, 21, -55, -44, 36, 12, 1, 22, -14, 14, 9, -7, 3, -10, 39, 16, -8, 0, -24, -27, 11, -26, 7, 18, 32, 14, 38, -23, 20, 8, -15, -6, -2, -1, 3, -11, -29, 21, 5, -6, 22, 0, -27, 19, -29, -10, -27, 34, 26, 26, 1, -33, -24, 16, 24, -30}
, {-7, 7, 14, -10, 26, 17, 16, -13, -26, 45, 2, -8, 14, -6, -24, 2, -28, 31, 20, -23, 16, 6, 14, 14, 3, 14, -14, 6, 5, 0, -3, 19, -32, -17, 26, 13, 9, -27, -23, 18, -12, -10, 13, 14, -8, -2, 8, -8, 30, -43, -45, 3, -29, 13, 13, 32, 1, 1, 13, 20, 13, -6, 2, -27, -32, -13, -37, -7, -12, 7, -10, -13, -11, 21, 15, -9, -11, 38, -18, -5, -2, 7, 13, -12, 26, 24, -40, -8, 12, -7, 14, 17, -6, -44, -9, -25, -7, 10, 18, -23, 7, -17, -24, 0, -22, 18, -9, 1, -15, 25, -1, 16, -25, -19, -26, 5, -24, 19, -10, -26, -34, -9, 47, 16, 6, -9, -18, 9}
, {4, -19, 6, -10, -9, -33, -18, 14, 20, 12, -4, 7, -20, -9, -4, -58, 19, -3, -27, 15, -30, -9, 19, -5, -20, -9, -16, 29, -3, -7, 18, -20, -18, 0, 9, 5, -17, -2, 10, 27, 27, -12, 4, -21, -23, 15, -9, 15, 6, 17, -3, -26, -20, 2, -31, 17, 12, -24, 34, 9, 2, -30, -21, 24, -3, 20, 39, -18, 18, -12, -35, 4, -31, 29, -1, -26, -24, -1, 25, 10, -43, -7, 33, 28, -13, -22, -26, -18, -15, 17, -22, 0, -33, 22, -5, -12, 10, 1, -34, 24, -16, 4, -56, -11, 4, 16, -35, -4, 27, -15, 11, 15, -4, -17, 9, 4, 0, -3, -13, -2, 23, -8, 4, -4, -4, -20, 28, 5}
, {-21, -21, -7, 0, -1, 7, 15, -39, 5, 21, -6, 16, -15, -29, 16, 15, 8, -2, 4, 2, -27, 2, 9, -24, 5, -1, -11, 2, -11, -8, -3, 16, 14, 6, -29, 10, -19, -2, -2, -7, -23, -34, 7, 20, -16, -2, -31, 14, -33, -7, 36, 39, 17, 10, 36, 32, 7, 13, -17, 3, -36, -1, -1, -3, -15, 0, -35, -9, -11, 7, -37, 32, 11, -1, 41, 5, 10, -29, -15, -4, 10, 3, -34, -4, -18, -31, 43, 29, -3, 29, -22, 42, 12, -44, 9, -15, -12, 14, 21, -36, -23, -5, 10, -14, 30, 23, -43, 1, 38, 1, -14, 16, 25, -14, 4, 9, -12, -3, -19, -4, -19, -32, -14, 39, -17, 1, -4, -29}
, {18, -18, 6, 13, -16, 49, 25, 17, -15, 42, 7, -22, 10, -31, 8, -5, 14, 3, 8, 9, 5, 40, -15, 32, -44, 9, 13, 14, 18, 1, 35, -38, 12, -19, 23, -29, -49, 15, 19, 12, 4, -35, -18, 23, 7, -5, -1, 22, 10, 24, 1, -10, 12, 5, 2, -13, -1, 11, 25, 34, -1, -10, -19, 21, -24, 3, 23, -7, -3, 6, 7, 29, 12, -29, 19, -38, 6, 20, -11, 7, 25, -15, -15, -8, 1, 15, 2, 26, 16, 14, -17, -6, 21, -29, -17, -23, -15, -21, 6, -35, -4, 22, 29, -24, 23, -6, 7, 16, -24, -16, -17, 2, 7, 0, -22, -6, -28, 20, 4, -44, -32, -1, 2, -17, 25, 17, -21, 1}
, {13, -7, 19, -30, -39, -6, 9, 26, -15, 4, 10, 13, -32, 26, -1, 7, -29, -39, -10, -23, -10, 13, -1, -28, -1, -9, 9, -18, -2, 6, -5, 5, 24, 0, -47, 24, -39, 33, 4, -5, 23, 15, -13, 29, -3, 10, 25, 9, 10, -5, -6, -34, 19, -11, -7, -26, -26, 31, -35, 4, 7, -19, 12, -21, 12, 20, -22, -7, -7, -6, 28, -9, -32, -10, 4, -36, 37, -21, -5, -8, 4, -47, -13, 25, 8, -12, 2, 29, 9, -8, 0, -22, -12, 8, -27, 12, 26, 9, -2, 9, -19, 31, 21, -25, -9, 16, 2, -34, -16, -20, -14, -26, -10, 16, -9, 8, -27, 19, 18, 1, 23, 13, 2, 24, 0, 19, 12, 22}
, {-20, 8, 26, 34, -11, 8, -17, 30, 17, -15, 11, -5, -18, -10, -29, -35, -11, 13, -23, -15, 19, 16, 23, -7, -14, 17, -22, 28, 34, 25, 20, -44, 12, -6, 6, 21, 9, -16, 4, -19, 3, -15, -17, -5, 21, 6, 23, -2, 4, -5, 1, -17, -14, -15, -12, 9, -7, -22, 2, 1, -16, -21, -25, -28, -40, 19, 22, -23, -38, 5, -6, 14, 12, -34, 10, 28, -35, 21, -29, 15, -23, -3, -2, -20, -14, -4, -32, -24, -17, 1, -2, -17, 2, -29, -16, 23, -20, 3, -10, 23, 24, -4, -3, 28, -3, -31, 42, 8, 15, 3, 32, 17, 10, 8, -7, -29, -3, -4, -9, -11, -6, -22, 6, 9, -24, -7, -10, -33}
, {-31, -6, 7, -1, -38, -23, -12, -12, -16, 19, 13, 16, -7, 28, -15, -33, -21, -27, 8, -5, 14, 0, -1, 3, -13, 27, -8, -22, -39, 3, 14, 34, 21, -8, -30, -13, -3, 10, -10, -2, 2, 1, -10, -15, 20, -11, -13, 16, 20, 13, -33, -6, -12, -31, -22, -15, 41, 35, -6, 34, 0, 0, 30, 3, -34, -5, -34, 2, 8, -8, 14, 21, -14, -1, -17, -20, 24, 7, 9, 12, 27, -11, -9, 0, -12, 21, -11, 4, 14, -6, 17, 16, -18, -14, 26, -8, -3, -33, -1, -2, 7, 1, 34, 27, 3, -6, 4, 15, -29, 34, 3, -7, 26, -36, 0, -14, -60, -26, 23, -10, -1, 9, 7, 29, -13, 10, -10, 8}
, {-2, -28, -8, 6, 22, 2, -21, -30, 0, 26, 15, 17, 3, 26, -11, -48, 12, 24, 12, 2, 11, 7, -19, 25, 6, 15, 7, -44, -7, -32, -8, 19, -28, 1, 5, -4, 10, -4, -18, 9, -17, -5, 20, -21, -15, 9, -13, -20, 38, 25, -16, 9, -15, -20, 34, -9, 35, -15, -7, 4, -19, -1, 12, -1, 25, -3, 2, -9, 4, -10, 24, -11, 10, -4, 30, 36, -15, 16, 41, 21, -2, -16, -22, 9, 0, 13, 27, -25, 12, -12, 28, 14, -13, 18, 15, 11, -3, 17, -26, 26, -22, -22, -15, 23, -26, 7, -25, 26, -30, 22, 15, -5, 5, -9, 36, 8, -11, -27, -14, -28, -12, -25, 6, 10, -7, -42, 23, -10}
, {-10, -15, 9, 9, -19, 17, -21, 26, 15, -26, -46, 2, -3, 22, 14, 14, -25, -16, -11, 28, -1, -7, 4, -19, -38, -15, 2, -22, 9, 7, 13, 31, -35, -35, -16, 10, 2, 39, -9, 2, -34, 27, 0, 37, 12, 4, 2, -6, -7, 2, -14, 23, -10, -17, -3, -33, -18, 18, -35, 9, -7, 13, -22, -4, 8, -19, -10, 12, 42, -12, -13, 4, 13, -10, 45, 4, 23, -8, 9, -29, 7, 30, -11, -29, 31, -29, 13, 15, 17, -15, 18, -5, -16, 37, 15, -4, 31, -21, -17, -12, -19, -2, 20, -8, 25, 2, -18, 2, 9, 1, -6, -1, -21, -11, 10, 18, 4, -8, 7, 21, 20, -1, 9, -6, -15, -6, 29, 20}
, {10, -12, -40, 13, -4, -22, 34, -5, 31, 2, 7, -15, -28, -18, -13, -13, -26, 10, 14, -7, 18, 14, 0, 36, -20, 44, -1, 25, -10, 5, 28, -14, -30, -5, -7, 13, -25, 8, -4, 11, 30, 4, 35, -6, 10, -25, -26, 20, 6, 0, -19, -2, -9, 0, -4, 4, 5, -5, 0, -1, 7, 22, -27, -11, -12, 22, 14, -52, -11, -5, -10, 15, 7, 17, -8, -40, -6, 15, 20, 4, -19, 20, 20, 21, -8, 9, 26, 12, 14, 16, 14, -12, -31, -4, 14, -11, 4, -30, -50, 37, -10, -10, -55, -9, 5, -40, -25, 23, -3, 9, 10, 22, 14, -18, -5, 11, -31, -26, 27, 12, -31, -11, -29, 19, 8, -15, 8, -14}
, {16, 0, -26, -1, -16, 25, -22, 9, -8, -2, 22, 15, -44, -16, -9, 1, -11, -26, -42, -3, -9, -6, -57, -7, 20, -6, -41, -11, 8, 10, -21, -36, -3, 12, 17, -5, 9, -52, -2, -18, -7, -8, -20, 11, -7, 22, -34, 7, -44, 6, 21, 7, 17, 21, -5, 0, 36, 22, 18, -15, -18, 19, -13, 29, 25, -7, 9, 3, -36, 30, -17, -2, 12, 2, -3, -1, -2, 7, 20, 16, 5, 10, 29, -44, -30, 21, 39, -9, 12, 27, 12, -11, 1, -12, -10, 23, 3, 12, 5, -38, 16, 15, 30, -1, 3, -9, -4, 3, 22, -30, -20, -3, 7, -6, 20, 5, -3, -26, 8, 30, 2, -23, -37, -4, 39, -28, -1, 11}
, {-41, 30, -22, -1, -32, 14, 3, -8, 3, -39, -29, 29, -16, 15, -2, 33, -42, 12, 10, -12, -6, 5, 5, -23, -8, -20, -25, -15, 9, -45, -25, -10, 8, 20, -33, -17, -1, -23, -34, -21, -23, 3, 7, -5, 16, 6, 27, 23, 0, 26, 16, 6, -1, 1, 11, 25, 29, -1, -2, 31, 15, 23, 22, -37, 2, -21, -30, -12, -38, -12, -7, 18, -4, 25, -33, 33, 17, -8, -25, -4, -26, 7, -23, 12, 33, 1, 24, 5, -7, -2, 24, 27, -33, 27, 7, -5, 6, 5, -10, 14, -19, -8, 10, 20, 8, -25, 10, -1, 13, 37, -17, -32, 18, 5, 1, 10, -3, 7, -19, 23, 40, 12, -30, -15, 10, -2, 24, -18}
, {-5, 27, -21, -22, -20, 10, 23, 2, -4, 19, 25, -9, 27, -10, -6, 29, -27, -7, 0, -28, 18, -9, 32, 22, -4, 36, -25, 7, 0, -33, -15, 29, -40, 3, -18, 13, 5, -38, -38, 21, -6, -5, -16, -40, 22, 7, 5, -9, 3, -11, 5, -15, 16, -19, 5, 34, 16, -50, 18, -6, -6, -8, 21, -34, -28, 30, -22, 14, -24, 22, -14, 1, -2, 29, -30, 20, 4, 28, -23, -43, 7, 16, -8, 14, 15, 21, -27, 16, -14, -3, 12, -17, -29, -23, -8, -20, -27, 7, -9, 10, 24, 13, -55, 17, 3, -14, 9, -5, -21, -21, -20, 8, 9, 21, -4, -16, 0, -14, -1, -30, -21, 11, 8, 15, 5, 0, 4, 4}
, {-29, -18, -2, -3, 1, 3, 38, -41, -11, 12, -10, -1, 6, -16, -5, -14, 1, 5, 10, -4, 0, -14, 16, 13, -10, -7, 5, -20, -29, -14, 12, -3, -14, 13, 33, -3, -38, -6, 25, -4, -1, 14, 2, -19, -42, 15, -13, -1, -5, -16, 25, 40, 16, 32, 23, -6, 7, -6, 13, -5, -29, 15, 12, -32, -2, -19, 13, -23, 11, -35, -47, 31, 31, -26, -4, -4, -32, 33, -7, -16, -24, 30, -1, 8, -56, 11, -31, -17, 31, 20, 30, 18, -26, 25, -61, 9, -15, -15, 8, 4, -21, -15, 9, 30, 7, 51, -27, 8, -6, -17, 17, -10, 4, -2, 2, -22, -37, -18, -21, 4, -11, -12, 21, -7, 22, 1, 2, -3}
, {-24, -18, -15, 45, 35, 22, 12, 6, 0, -26, -11, -42, -9, 10, -22, 11, 4, -1, -12, -24, 15, -22, 32, 11, 19, -12, 2, -41, 3, -13, -1, 18, -6, 5, 8, 11, 21, 4, 17, -22, -17, -41, 12, -13, 13, 20, 15, 8, -8, 28, -21, 22, -17, -24, -35, 13, -3, -5, 7, 28, 22, -26, -48, -11, -32, -16, 7, -10, 12, -19, 21, 30, -9, 26, -19, 18, -7, 0, -11, 8, -31, 27, 19, 6, 20, 7, -13, 3, 14, 22, -6, -16, 20, -13, -10, 13, -13, 4, 22, -27, 22, -10, 1, -14, -1, 9, 19, -32, 3, 26, 22, 16, 10, 4, 22, 32, 34, -9, -27, 8, 5, 8, 8, -24, -30, 25, 3, -7}
, {17, 0, 16, 5, -40, -49, -20, -14, -31, -13, 25, 0, 24, 21, -20, -19, -6, 3, -10, 17, -3, 1, -56, 14, -24, 18, -1, -5, -24, 4, 21, 3, -15, -24, 19, -15, 34, -16, 23, -2, 14, 2, -23, 26, -24, 3, -11, 23, -29, -45, 20, 11, 13, 5, -10, -8, 16, -3, -17, -17, -38, 11, 6, 2, 10, 6, 16, 13, -10, -3, -19, -8, -27, -26, 12, -2, 7, 25, -20, -1, 26, 39, -5, -20, -34, -9, -16, 6, 30, -1, 6, 34, 18, -9, 4, 1, -17, -44, 6, -24, -9, 8, 4, -4, 21, 16, 15, 32, -16, -34, -17, -45, 30, -7, 3, -36, -17, -27, 6, -7, -14, 15, 7, 18, -11, -21, -9, 17}
, {23, -26, -24, 3, -2, -39, 15, 15, 29, -3, 14, 8, -13, -13, 2, -7, 5, -31, 14, -8, -17, 26, 6, -6, -31, 28, 3, 2, -5, 12, 20, 4, -3, -19, 6, -10, -26, -11, -22, 26, 27, -15, -1, 11, 4, -9, 10, 9, 52, 8, -18, -5, -7, 1, 25, -8, -24, -30, -8, -5, 31, -36, 17, -11, -5, 8, -17, -1, 16, -5, 15, -17, -22, -8, 46, -23, 24, -15, 5, -36, -29, -32, -16, 25, 23, 11, 9, 23, 37, -5, -12, -3, 10, 10, -4, -12, -4, 15, -18, 5, 6, -6, -28, -39, -15, 20, -13, 3, -17, 7, 24, 16, -3, -35, -35, -1, -44, 5, -9, -16, 7, 5, -1, 1, 7, -19, -12, 32}
, {3, -38, 38, 16, -37, 1, 6, -7, 6, -10, -23, -35, -25, -25, 12, -43, 18, 1, -3, 3, -5, 10, 27, -49, -18, -8, -2, 22, 7, 23, 2, 16, -4, -1, -4, 29, -18, 4, 2, 14, 1, -23, 17, 4, -22, 17, 23, 18, 33, 8, 2, -6, -12, -20, -2, 17, 1, 19, 18, 11, 11, -32, -41, 43, -20, 8, 12, 16, -2, -20, -14, 6, -8, -16, 28, 18, -3, 0, -23, 24, 14, -22, 15, 8, -29, -9, -28, 15, 5, 21, -33, -44, 14, -9, -44, 41, -16, -19, 26, 12, -24, -10, 30, 23, -12, 23, 13, 4, -14, -45, -5, 38, -4, -9, -26, 20, -37, 20, -22, -26, 32, 9, 39, -23, 5, -1, 14, 16}
, {-14, 11, -12, -25, 43, 0, -10, 10, -22, -60, 24, -1, 34, 10, 29, 29, -24, 12, 3, 11, 1, -1, 16, 25, 6, 2, 17, -21, 15, 29, 23, -3, 7, 10, 13, -18, -10, -25, 21, -12, -7, 23, -6, 27, -6, -43, 17, 16, -36, -19, 24, 12, -14, -17, -19, -15, -9, -6, 19, -30, 23, 22, 12, -15, 19, 15, 26, 4, 4, 3, -7, 10, -27, 23, -41, -41, -7, 28, -35, -18, 9, 15, -12, -2, 19, -10, -21, -16, -18, -19, 0, 7, -12, 4, -15, -1, -19, -1, 25, -13, 24, 31, 8, -18, 14, 16, 12, 9, -9, -17, -29, -13, 1, -22, -28, -1, 9, 24, -12, 6, -1, 5, -36, -32, -5, -30, -13, -18}
, {-10, -26, 14, -28, -12, 46, -20, 5, -12, 10, -1, -5, -9, 8, 27, -15, 14, -20, -6, 10, -11, 8, -40, -3, -9, -7, -21, -6, 21, 31, -2, -30, -35, -16, -36, 13, 16, -40, -8, 12, -16, -8, 4, 19, -7, 31, -9, -22, 7, 8, 2, 25, -8, 21, -3, -34, -4, 28, 5, -19, 27, -11, -6, 28, 14, -6, -1, 11, -31, 22, -41, -5, 4, 8, -15, 9, -2, 0, 1, 22, -8, -8, 29, 8, 18, -15, -19, 13, 29, 5, -54, -18, 10, -12, 16, 33, 14, 18, 47, -23, 39, -2, 1, -10, 16, -15, -3, -22, -47, 30, -19, 15, -34, -14, -6, 12, -11, 1, 18, -33, -13, 16, -16, -7, 11, -25, 20, -15}
, {23, 1, 9, -36, -16, -17, -33, 12, -23, -32, -19, 13, 26, 8, 4, -37, -39, 19, -3, 6, 15, -32, 21, -12, 14, -27, 18, -10, 2, 10, 19, -21, -19, -25, 21, 27, 20, 28, 33, 5, 6, 23, 7, -14, 0, 5, -13, 4, -4, -24, 9, -6, 3, -15, 15, -14, 17, 7, -25, -48, -28, -10, -21, 10, 14, 34, -6, 39, 2, 30, -14, -11, -21, -29, -2, 22, -18, -13, 7, -19, -38, -4, 9, 8, -22, -31, -22, 3, -1, 5, -36, 11, 27, 21, -15, -5, 8, 21, 23, -2, -24, 13, 27, -6, -18, 33, 18, -1, -8, -42, 16, 20, 3, 33, 4, 7, 10, -23, 11, 19, -25, -8, -23, -14, -17, 16, 34, 29}
, {18, 5, -3, 18, -28, -27, -1, 20, 22, 5, 10, -19, -7, 24, -1, -15, 34, 6, 22, 18, -1, 19, -13, 5, -40, -8, 22, -3, -13, -17, -2, -40, 20, -21, 31, -17, 32, 41, -30, -18, 19, -28, -16, -6, 18, 0, 0, 14, 17, -13, 14, -4, 29, 8, -33, -25, 13, -24, -5, 12, -23, 15, -8, 7, 10, -10, 22, 20, -9, -19, -11, -14, -36, -29, -20, 11, 10, 9, -1, 16, -21, 12, 5, -7, 18, 1, -17, 3, 2, 41, -3, -4, -5, -14, -16, -26, 38, -5, -20, -39, -9, 1, -3, -27, 22, -24, 23, 9, -6, -17, 6, -6, 3, -8, 5, -31, 19, 18, 10, -2, -33, 6, 4, 32, -37, 32, -7, 17}
, {23, 24, 10, 5, 19, 12, -4, -1, -26, 13, -21, 12, -44, -38, 17, -11, 17, 5, 2, 4, -21, 4, 8, -10, -9, 32, -14, -5, -6, -4, -21, 10, 0, 17, -3, -21, 6, -50, 14, -15, -44, 26, -6, -12, 17, -21, -23, -25, -3, 14, -8, 38, -12, -26, 0, -35, -32, 30, -1, -28, 5, 12, 12, 12, -15, 12, -2, -25, 32, 9, -61, 4, 3, 18, 48, 16, -42, 24, -19, 7, 23, 17, -34, -22, -10, -14, 26, -26, 13, -24, -5, -29, -11, 0, -11, 1, 9, -19, 13, -21, 3, -6, 22, 23, 13, -3, -24, 25, 24, 4, 2, 20, -48, -2, 21, -16, -12, -21, 33, -12, 20, 0, 38, 3, 0, -32, -40, -24}
, {14, -19, -23, 37, 30, 24, -11, 27, -16, -8, 20, -4, -16, -1, 15, 16, 12, -10, -18, -23, -9, 19, 19, -2, -45, 21, 14, -17, 3, -25, -3, 30, 8, -24, -20, 2, -20, 22, 12, 6, 3, -25, -9, -5, -2, -28, -18, -9, 28, 2, -3, -29, 4, -18, 1, 7, 21, 9, 16, 19, -16, -4, -10, -2, -19, 4, 0, -23, 11, 0, 21, -3, -12, 11, -10, 36, 5, 16, 8, -35, 19, 5, -4, -29, 12, 23, 26, -4, 18, 18, -13, -4, 31, -24, -25, 2, -11, 26, 10, 7, 42, 23, 0, 22, 13, 13, -19, -10, -41, 2, 6, -31, 16, 30, -14, -6, -19, 8, -41, -9, 9, -24, -2, 35, 4, -35, -31, 8}
, {15, -17, 9, -8, 4, 5, 15, 2, 13, 30, -36, 9, 12, 14, 28, 9, 34, -26, 3, -6, -36, 2, -8, -18, -29, -33, 2, -37, -11, -28, 2, 5, 48, 19, 15, -40, -13, 41, -3, -21, 11, -31, -28, 18, -44, -5, -20, 14, 26, 10, -7, -5, 11, 6, 15, -36, 18, -16, 1, 18, 11, -33, 7, 38, 9, 15, -6, 10, 29, -27, -32, 16, 29, -5, -7, -8, -2, -9, 11, -21, 2, -46, -27, -4, -7, -43, 9, 1, 24, -6, -20, -11, 39, 3, -7, 13, 9, -21, 22, -10, 2, 12, -11, -9, -19, 25, 20, -13, -10, 0, -4, 9, -6, -27, 17, -20, 12, 25, -36, -45, 26, -8, 1, -28, 2, 27, 6, 29}
, {1, 17, -29, 31, 51, 1, 10, -13, -21, 3, -2, -33, 10, -23, 20, 10, 26, 2, -31, 24, -11, -37, -3, 22, -14, 27, 15, -19, -1, -10, -19, 10, 7, 14, 10, -24, -9, 20, 1, -31, -7, 36, -22, 5, 6, -31, -17, -25, -16, -4, -45, -3, -34, -11, -19, 33, 23, -2, 14, 10, -8, 24, -31, -13, 4, 5, 22, -2, 33, -27, -21, 2, -4, 27, 25, -36, -32, 29, 2, -13, -1, 48, 18, -6, 10, -27, 28, -34, 1, 13, 21, 2, 7, 12, 22, -19, -23, -26, -13, -21, 3, -9, -3, 17, -10, 24, -3, 10, 27, -22, -25, 33, -3, -15, -9, -10, 18, -33, 10, -3, -31, -35, 0, 7, -1, 10, 6, 31}
, {15, 4, -26, 28, -10, -56, 0, -8, 29, -15, -2, -21, -15, 5, -10, 31, 16, 7, 10, -29, 19, 26, 11, -5, -4, 29, -4, -12, -2, -32, -14, -15, -29, -2, -4, 0, 37, -59, -43, -6, -10, -31, -7, 4, 2, 15, -4, 3, -16, -8, 7, 23, 35, 9, -2, 30, -29, -20, 19, 28, -3, 17, 3, -42, -15, -17, -15, 4, 7, -13, 28, -15, -2, 2, 14, -7, 20, 1, 0, 13, 20, -8, -26, 20, 27, -3, 10, -18, 15, 9, 38, -14, -35, 2, 21, -24, -1, 9, -47, -22, 4, -13, -34, -20, -15, -11, -4, 0, -20, 63, 19, 2, 18, -11, -9, -17, 4, 23, -8, 32, 2, 15, 18, 9, -1, 22, 17, -1}
, {11, -21, -9, -2, 20, 9, -40, 2, 0, 30, 42, 3, 2, -9, -29, -9, 19, -17, 15, -4, 2, -21, 23, 11, 30, -26, 8, -14, -30, 32, 6, -3, 11, -9, -17, 3, 12, -3, 15, -29, -17, 0, -12, -3, 4, -18, -36, -36, 14, -20, -27, 36, -16, 21, -20, -21, 22, 5, 24, -27, -21, 11, -17, 10, 12, 16, -21, -13, -11, 34, 12, -12, -7, -8, 45, -3, -44, 12, 47, 25, 16, -33, 8, -35, -7, -20, -26, -20, 12, 0, 3, -13, -10, 6, 11, 15, 8, 38, 34, -2, 14, -31, 6, 20, -37, 31, -25, 18, -19, 20, 5, 37, -35, 16, -22, 12, 4, 15, 10, -30, -5, -20, 18, 12, -8, -5, 0, 5}
, {5, -25, -10, 14, -1, -17, 6, 14, 9, -14, -2, 16, -3, 20, -8, -4, -2, 37, 12, -12, 4, 8, 18, -5, -5, 21, 0, -26, 2, 5, 7, 30, -19, -35, -24, 21, -15, 16, -12, 21, 2, 10, 2, -42, 34, 18, -2, 17, 8, 26, -23, -38, 5, -16, -22, -4, -23, -41, -4, 8, -25, -12, -24, -18, -4, 12, 14, -25, -11, -5, 1, 9, -26, 37, 8, 4, 23, -18, -7, -46, -23, 36, 9, -11, 8, 0, 7, 23, -6, 24, 25, -25, -21, 19, -49, -11, 15, -31, -45, 20, -14, 3, -41, -6, 21, -35, -17, 2, 0, -23, 28, 26, 21, 12, -1, 2, 1, -17, 1, 17, 35, -15, -13, 18, -35, 12, 5, 3}
, {20, -32, -10, -30, -1, 28, 32, -15, -13, 16, -37, -4, 3, 12, -9, -36, -37, -21, 20, 6, -27, -25, 6, -27, 7, -9, -7, 30, 49, 25, -8, 29, -21, -14, 10, 13, 9, 7, -6, 22, 15, -31, 11, -16, 4, 30, 0, -15, 0, -11, 19, 33, -33, 26, -2, -12, -7, -7, 16, -18, -13, -6, -5, -4, 7, 8, 24, -13, 26, -12, -27, 22, -19, 13, -2, -23, 14, 2, 3, -10, -32, 2, -1, -17, -7, -46, 26, 13, -10, 1, 17, 28, -3, 19, -32, -19, 17, -16, 22, -1, 0, -30, -36, -10, 14, 32, 15, 0, 6, -20, 10, -4, 13, -5, -30, 6, 10, 35, -32, -9, 27, -2, 8, 4, -28, -6, 12, -10}
, {4, -14, 36, -24, 8, 0, -16, -12, -8, 5, -10, -9, 17, -4, 14, 15, 9, -20, 30, 24, 29, 18, 8, 9, 9, -11, 25, -24, -21, 29, -11, -22, 11, -12, -16, 16, 5, 3, -3, 23, 0, 14, -9, 3, 1, 0, -20, -19, -1, -8, -32, -35, -24, -11, -27, -23, -12, -33, -48, -10, -13, 21, 5, 7, 18, -20, 22, -8, 5, 21, 1, 6, -23, -9, 8, -1, 23, -3, 16, -11, 5, -18, -11, 22, 13, -16, -21, 17, -32, -9, 17, 9, -14, -15, -24, -2, 12, 18, -33, 10, 16, 18, -43, 4, 16, 34, 7, -17, -10, -8, -19, -5, -13, -10, -10, 6, 27, 6, -38, 8, 24, 27, -24, -15, -26, -3, 7, 5}
, {0, 18, 23, 12, 45, -16, -28, 8, 26, -5, -13, 19, -9, -24, 14, 32, -23, 6, -1, -12, 19, 12, 9, -6, 2, 1, 4, -27, -25, 29, -27, -29, -28, 2, -2, 15, 0, 25, 9, -13, -8, -12, -5, 22, -6, -12, -26, 0, -23, -4, 4, 15, -35, -31, -32, 7, 16, 30, -10, -23, 34, 0, -11, 22, -10, 9, -8, 21, -29, -8, 33, -1, 13, 6, 4, -21, -18, -25, -2, 23, 16, -17, 7, -4, 3, 19, -28, 7, 13, -10, -23, 10, 19, -35, 32, 6, -32, 25, 13, 12, 18, 3, 10, -22, -16, -7, -23, -14, -1, 5, 15, 36, 15, 5, -16, 12, -16, -35, 15, 10, -15, 4, -21, -17, -24, 24, -15, -24}
, {-7, 18, -13, 1, 19, -48, 2, -16, 31, -22, 9, 43, -7, -26, 0, -19, -22, 14, 19, 7, 10, -11, -15, 10, 7, 16, 30, -19, -25, -20, -37, -41, -21, -29, -15, 20, -8, -36, 9, 11, -4, 1, 16, 5, -13, -16, -41, -1, -4, -34, 37, -31, -3, -32, 6, -17, -2, -5, -24, -10, 8, 25, 7, -34, 19, -10, -15, -26, 19, 5, 25, -21, 14, -17, -10, -1, 14, -19, -10, 9, 15, 10, 10, 26, -18, 10, 35, 9, 2, -11, 27, 22, -31, 10, 14, 19, -38, -6, -46, 6, 8, 28, -5, -2, 17, 26, -35, 18, 52, 12, 1, 5, 3, -33, -29, -12, 34, -16, -13, -4, -29, -24, -22, 6, 9, -24, 9, 28}
, {-10, 3, -18, -4, 11, -4, 24, -16, 20, 8, -29, -18, -22, -10, -53, -35, -12, -24, -27, -28, 21, 4, 31, -1, 27, 9, 1, -6, 2, 5, 2, 9, 16, 31, -4, 1, -17, -21, 30, 8, -8, -30, 21, -46, 30, 24, 27, -1, -22, 39, -24, -27, 18, 0, -38, 4, -19, -34, 22, 10, -26, 2, 12, 4, 6, 24, 0, -7, 6, 17, -27, 23, -18, 1, -2, 11, -19, -11, -1, -5, -14, 20, 20, 7, -4, 2, 34, -8, -31, 40, 7, 14, 5, 16, 3, 19, 2, 1, 23, -13, -5, -27, -14, -12, -11, -24, 22, -29, -1, -28, -28, -16, 6, 9, 39, -10, -8, 5, -8, 10, 15, -5, 2, -23, 3, 10, 3, -12}
, {-15, -10, 13, -7, 1, 30, -9, 6, -11, -4, -19, -21, -16, -28, -37, -11, 2, 15, -19, 11, 16, -5, 27, -7, -8, -2, -13, 9, 13, -5, 21, -22, -17, -49, -4, 27, -16, 2, 2, -6, -15, -14, 2, -4, 4, -27, -16, -8, -2, 11, -3, -9, -9, 30, -23, 17, -1, -31, 21, -15, 2, -12, 11, -17, -44, 0, 19, 1, -13, 26, 38, 0, 33, -2, 48, -17, -43, 14, 23, 15, 5, 2, -7, -22, 2, -3, 21, -27, -8, 12, -14, -19, -4, -28, -16, 12, -29, 29, -5, 34, 11, -39, 23, 37, 14, 33, -19, -15, -7, 9, 4, -9, 4, 28, 19, -24, 15, -27, -38, 16, -21, -1, 21, 9, -23, -14, -5, 14}
, {-2, 15, -2, -3, 2, -36, 18, -20, 18, -9, 20, 2, 6, -11, -5, 26, 6, -7, 18, -24, -1, 21, 14, 3, 10, -3, 32, 17, -13, -14, 19, 11, 42, -19, -4, 14, -16, 57, 10, 13, 9, 23, 12, -13, 20, 6, 50, 30, 43, 1, -2, -15, 20, 10, -22, -8, -37, -20, 15, 7, 11, -29, -21, -21, -13, -12, -9, 22, 12, -23, -10, 0, -32, -16, -2, -53, 13, -25, -14, -40, 7, -1, 3, 41, 22, 18, -36, 10, -5, 7, 18, 4, -25, 19, -11, 4, -4, -23, -29, 15, -16, 13, 3, -50, 6, 6, -11, -25, -13, -48, -4, 14, 27, -37, -4, 15, 2, 26, 13, -8, -3, 12, 6, -22, -12, 9, 5, 34}
, {9, -31, -15, -22, 28, 49, 16, -25, 26, 5, 7, -1, 25, -27, -13, -17, -43, 2, 22, 5, -16, -12, -56, 25, 24, 23, -4, 32, 13, 16, 2, -39, -3, -17, -4, 41, 1, -34, 13, 18, -7, 10, 25, 11, 0, 10, -37, 6, -34, -7, 6, 7, -8, -30, 7, -3, 0, -3, 7, -26, -19, -5, 18, -3, -5, 18, 16, -8, -2, 6, 16, 11, -5, -4, -35, -27, -11, 8, -2, -15, 23, 29, 19, -17, -24, -11, -10, 27, 2, -3, -35, 33, -17, 7, -34, 2, -5, -2, 9, 39, -17, 34, -10, -1, 18, -15, 0, 16, 22, -22, 17, -1, -2, 1, -12, -1, -15, -26, -22, 4, 22, -8, -4, 18, 13, -1, -5, -30}
, {7, -23, -32, -41, 2, -10, 4, 15, -10, 6, -7, 30, -16, -36, -18, -10, -26, -19, 1, -3, -7, 20, -1, -23, -39, -3, 8, 33, -11, 13, 31, 18, -44, -22, -21, 40, -22, -3, 14, 14, 27, 37, 5, 14, 17, -24, -14, 29, -22, -35, -11, 9, -43, -5, 19, 5, 15, 17, -32, -4, -2, 20, 23, -24, -24, 37, 10, -8, -10, 6, 48, 24, 4, -14, 15, -43, 13, 6, -8, -32, 11, 1, 31, -12, 11, 20, -17, 1, -11, -5, -17, 4, -13, 6, 13, -22, -1, -10, -25, 25, -5, 4, -4, 8, 10, 8, -12, 23, 31, -29, 3, 5, 26, -12, -26, -17, -15, -17, 21, -9, -1, 3, 22, 46, -11, -24, -17, 19}
, {-1, 10, -45, 13, 21, 6, 18, -7, 14, -1, 21, -4, 32, 34, 2, -1, 4, 5, -12, 0, 9, 1, 17, 29, 4, 29, 17, 1, -6, -15, 27, 28, 38, -35, -16, -18, 12, 18, -22, 9, -18, 12, -19, -16, 23, -24, -20, 15, -3, 23, -19, -19, 6, -13, -8, 3, -17, -42, 33, -13, -10, -18, 20, -19, 12, 4, -13, 25, 27, 24, -2, 2, -6, -15, -3, -2, -16, -13, -17, -8, 28, 14, 36, 26, -21, 31, 28, 10, -9, 9, -11, 21, -35, -15, -1, 3, 1, 14, -29, 18, 6, 1, -29, -40, -1, -17, -19, 0, 14, -36, 9, 11, -12, -8, 32, 20, -19, -19, 33, 6, -19, -11, -29, 5, -28, -10, -1, -5}
, {38, 11, 18, -5, -5, -21, -15, -9, -3, -21, 7, -3, -12, 23, 8, -2, 7, -23, 27, -4, 6, 6, 10, -18, 30, 19, 11, 5, 10, 11, -15, -7, -18, 12, -24, 2, 49, -72, 19, 26, 3, 7, 25, -7, 4, -1, 8, -20, -27, -44, 4, 13, -23, -18, -8, 2, -5, 12, -33, 6, 28, 10, -28, -11, 34, 4, -21, 24, -15, 21, 12, 0, -35, 26, 1, -4, 24, -27, -5, -6, -21, -16, -4, 23, 31, -25, -27, 24, -4, 7, 9, 33, 20, -11, 3, 4, -15, -21, -2, -4, -21, -15, -8, -2, 17, -7, 28, 1, 19, -11, -37, -15, -2, 3, 0, 0, 20, 6, 22, -7, 14, 16, -19, -5, 0, 6, -18, 10}
, {-1, 19, 26, 13, 3, -6, 9, -34, -8, 31, -3, -28, 6, -10, -43, 22, 27, -4, 28, -13, 7, -14, -13, -2, 13, 30, -20, 4, 1, 14, -8, -10, -10, -10, 14, 4, 49, -44, -25, 23, 17, -33, 2, 15, -19, 12, 14, 1, -27, -12, 7, 6, 15, 16, -30, 28, -18, 5, 26, 9, 9, -6, -5, 0, -10, -29, -35, 19, -13, -12, 35, 5, -15, 13, 33, -4, 10, -15, 7, 19, 8, -42, -20, 12, 23, -12, -42, 14, 6, 29, -5, -5, -6, 17, -33, -33, -45, 11, 22, -8, -28, -23, 20, -6, 1, -17, 0, -8, 21, -21, 1, -3, -30, 28, -18, -14, 12, 22, -15, -18, -11, -9, -21, 13, -1, 18, -1, -19}
, {15, 28, -18, -13, 24, 24, 11, -10, 18, -22, -10, 21, 26, -25, 25, -2, -4, -4, 16, 14, -5, 28, -29, 24, -38, 6, 22, 0, -7, -8, 1, -3, -17, 15, -3, -23, -5, 31, -3, 2, 16, 30, 18, -26, -2, -16, -18, 0, 15, 15, 8, 2, -33, -30, 41, -20, -28, 20, -28, -8, 16, -4, -5, -3, 12, -13, 2, 9, 19, -27, 4, 8, -23, 25, -18, -6, 19, -31, -4, -35, -17, -3, -13, 21, 21, -10, 31, 16, 2, -4, 3, 39, 10, 9, 18, 2, -18, -28, -27, -3, -5, 44, 16, -33, 13, -19, -20, 10, -12, 8, -3, 7, 8, 1, 15, 40, 17, -5, 32, -4, -2, 4, -20, 0, 8, -18, -2, -4}
, {-9, -18, 11, 21, 9, 9, -18, 2, -6, -2, -26, -23, 13, 14, -21, -19, 12, -33, -32, 13, 23, 16, -34, -17, -6, -10, -10, -11, 1, -21, 19, -30, 0, -28, -10, 24, 10, -42, 22, 0, 6, 8, -5, 31, 20, 14, -23, 22, -18, -23, 15, -28, -6, -30, -16, 5, -14, 16, -9, -44, -16, 18, 4, 4, -2, -14, -27, 28, -26, 7, 14, 29, 4, -34, 26, 7, -20, -36, -6, 26, 31, -9, -10, -18, -31, 15, -23, -17, 31, -22, -16, -16, 12, -33, -23, 26, -31, -21, -12, -19, -19, 17, 26, 9, 13, 4, 17, 12, 6, -20, -25, 14, 7, -23, 43, -9, 8, -8, -5, -2, 14, 19, 22, -15, -9, -30, -1, 4}
, {17, 1, -5, -7, -4, -17, -1, 1, -16, -37, 23, 17, 33, -10, 2, 6, -10, 15, 14, 2, 19, 8, 25, 32, 16, -11, 15, -6, -3, -43, -6, -25, -28, 10, 26, 12, 29, -45, -15, -1, -33, 4, -20, 12, 2, -12, -13, -6, -16, -24, 61, -16, 17, 30, 30, 35, -23, -21, 16, -4, -10, 41, 20, -38, 16, -2, -27, 0, 0, 18, -19, -9, 8, 18, 20, -6, 9, 33, -29, 3, -28, 2, -11, -4, 12, 13, 9, -35, -27, -26, 43, -9, -32, 14, -14, -14, -6, 8, -50, -3, -11, -4, 3, 24, -11, 37, -10, 0, 15, -18, -17, -39, -9, 12, 8, -25, 15, 3, 6, 32, -5, -19, 21, -13, 21, 12, 0, 12}
, {-7, 19, -10, 13, -34, 7, 20, -21, -12, 28, 34, -10, 21, -5, -6, 10, 38, 3, 17, -1, -21, 19, -6, 32, -2, 15, 15, -4, 9, -1, 7, -45, 47, 20, 23, -33, -36, 25, -27, 24, -3, -27, -27, 15, -33, -11, 7, 11, 33, -16, 3, -10, 24, 30, -4, -18, 20, 20, 1, 14, 7, -14, 20, -9, 10, -5, -3, -15, -25, 4, -27, -10, 18, -2, -18, -35, -15, -10, 19, 21, 26, -40, -23, 20, 2, 18, -32, 11, 16, 18, 4, 23, -12, -11, 6, -23, -16, 4, -7, -1, -3, -3, -12, -40, 3, 4, -3, 12, -22, 7, -3, 9, 0, -28, -40, -32, -24, 11, 11, -58, -23, -9, 0, -9, 1, 4, 3, -18}
, {20, 10, -22, -56, -9, -23, 26, -27, 7, -12, -9, 23, 14, 12, 14, -30, -30, -13, -1, -17, -14, 0, -21, 7, -1, 32, -20, 15, 13, 17, 16, 14, 4, 48, 26, -12, 19, -53, 22, 19, -14, 16, -8, -20, 4, -11, 21, -7, 3, 6, -12, -12, 1, -13, -12, -11, -7, 17, -11, -28, 12, -1, 24, 35, -24, 43, -23, 26, 9, 3, -38, 21, -1, 40, -21, -26, -43, -4, -25, 17, -58, -12, -6, -14, -20, -28, 3, 6, -23, -5, -16, -8, -9, -6, 16, 4, 2, -26, 27, 9, 25, -5, -15, 7, 18, -17, 36, -7, -5, -1, -56, -15, -28, -23, 13, 1, 2, -23, 21, -18, 11, 7, -20, 7, 17, 0, -16, 7}
, {-7, 38, 16, 6, -16, -3, 4, -9, -22, 21, 22, -18, 28, 4, -27, 8, 2, -25, 4, 0, 38, -4, 24, 4, 13, 25, 8, -21, -8, -4, -9, 28, -38, 12, -11, -7, -5, 22, 35, -23, 9, 15, 22, -29, 19, 9, 9, 7, 7, -19, -15, -9, -23, -11, -8, 15, 52, 15, -21, -10, -20, -13, -27, -23, -9, 7, 0, 27, -18, -9, 21, 6, 1, -19, -32, -6, -10, 12, 4, 19, 5, 33, 14, -28, -9, 22, -16, 17, -32, 46, 2, 22, 36, -30, 7, -1, 13, 9, 3, -20, 23, 1, 13, -28, 19, -5, 13, -5, -25, -35, -16, 11, 26, 31, 2, -3, -9, -16, -23, 18, -47, -5, -8, -8, -12, 23, -22, 19}
, {25, 38, 33, 15, 9, 9, 27, 13, -13, 14, 0, -31, 18, -25, -14, 36, 4, -46, -9, -12, -3, 26, -43, -24, -10, 6, -9, 12, -1, 18, 14, -15, -33, -17, 19, 12, -11, 15, 2, 20, 16, -9, -20, -14, -19, 12, 16, 15, -27, -4, 16, -8, -16, -7, -15, -2, -16, -8, -25, 23, 9, 12, 0, 12, -20, -20, 6, 29, -24, -12, 5, 22, 20, -19, 21, -22, 9, -8, -38, -22, 45, -12, -10, 3, 32, -5, 10, 20, -18, 10, 1, -10, 12, -31, 18, -25, -25, -20, -15, -36, -9, 0, 25, -8, 1, -46, 18, 9, 31, -32, -13, 0, -3, -14, 20, 13, -2, 21, 4, -19, -25, -20, -10, 8, 10, -28, -14, -15}
, {-2, -50, -25, 12, 35, -32, 6, 8, -16, 10, -16, -9, -9, -15, 16, 2, 30, -15, -16, 17, -39, 0, 4, 3, -17, -10, -6, 2, 27, 21, 20, -17, 13, -1, -4, 7, -13, -20, 2, 0, 13, -25, -7, -2, -7, 15, 12, 3, 11, 37, -32, -12, 4, -3, -52, -25, -10, -25, -8, -8, 27, 0, -45, -14, -12, 28, 16, -24, 13, -1, -35, 0, -30, 18, -13, -9, 19, -20, 6, -14, -16, -25, 5, 6, -1, -22, -19, -14, -8, 17, -37, -36, -20, 8, -22, -8, -2, 15, 23, 12, 14, 3, -27, 1, 12, 12, -10, -29, -6, -23, -5, -11, 20, 0, 6, -3, -20, 19, -25, -6, 16, -10, 7, -31, -4, 2, 11, -24}
, {13, 42, 22, -10, 0, -19, -3, 2, -1, 21, -11, -26, 0, -6, -7, 26, 21, 4, -11, 19, -13, 12, -2, -1, 48, 3, -8, -2, -9, -34, -10, -24, 22, -1, -21, -25, 8, 18, -1, -2, 1, -31, -44, -23, -9, 11, -7, -20, -5, -4, 3, -31, -1, -4, -7, -10, 7, -27, -20, 11, 12, 22, 0, 6, 6, 11, -4, -24, -23, -6, -6, -19, 28, -1, -38, -14, -4, 35, 20, 39, 10, 4, -15, -2, 12, -27, 33, -10, -51, -31, 15, -5, 17, -31, 18, 2, 8, 19, 4, 18, -5, -38, -7, -5, 6, 25, -2, 13, 38, -17, -14, -23, -26, 17, 19, -26, 7, 26, -22, 29, 3, 7, 15, -31, 23, 13, -17, -24}
, {33, 10, 20, -10, -3, -11, 17, 20, 25, 2, -27, 14, -23, -5, 9, -29, -26, 12, -17, 11, -14, -14, 19, -31, 12, -4, -23, 25, 38, 14, 2, -14, -21, 1, -22, 17, 15, 14, -13, 7, 0, -1, 2, -19, 11, 20, 1, -6, -12, 11, -12, -14, -26, 16, 5, -20, -29, -18, -3, -15, -9, -4, -32, 15, -34, 9, 14, -10, 1, 25, -9, -8, -32, -1, -21, 33, 5, -7, -21, -14, -33, 2, 1, -20, -13, -33, 20, 26, -37, 4, -12, -36, 18, -7, -23, 10, 15, 8, -14, 20, 5, -11, -26, 15, 12, -16, 13, 2, 23, -25, 17, -9, 6, 62, 12, -10, 36, -1, 11, 22, -12, -1, 7, 10, -6, 27, 17, 15}
, {14, 11, 2, -14, 13, -15, -21, -5, -11, 23, 2, 3, -18, -15, 25, 19, 10, -16, -16, 16, -34, 25, 4, -21, 13, -33, 24, 22, -21, -23, -11, 29, 14, 22, 1, -24, -19, 5, -27, -17, 8, -14, 0, -15, -6, 5, -6, 3, 28, 20, -14, 7, 13, 40, 14, 21, -22, -5, -13, 31, 14, 10, 28, 1, -23, -22, 4, -39, 39, 3, -5, -17, 28, -4, 32, -27, -15, 10, -9, 25, 23, -7, -36, 5, 15, -36, 32, 1, -17, -16, 24, 16, -28, -31, 9, -29, 5, -17, 21, -15, 3, -15, 26, 32, -18, -24, 16, 15, 19, -4, -31, -12, -18, -17, -23, 20, -7, -8, -6, -14, 29, 27, 40, 3, -1, -13, -29, 8}
, {3, -6, 0, -24, -32, -32, -16, -7, -18, -38, -31, -4, 19, 10, 26, 19, -22, 34, -8, 22, 0, -8, 0, -12, -27, 5, 8, 11, 27, 17, 22, -12, -3, -14, 9, -16, 20, -27, -1, 10, 9, 25, -9, 9, -29, 2, 11, 24, 21, 24, 37, 15, 20, -7, 19, 9, -34, 21, 2, -23, 19, -4, -34, 4, 24, -7, 12, 21, 1, 18, -27, -15, -26, 31, -9, 26, 15, 2, -13, -25, -9, 15, 0, 28, -5, -12, -24, -29, 28, -9, -22, -2, 4, 2, 17, 46, 0, -20, -16, 1, -10, 31, 35, 17, -12, -14, -2, 15, 19, 2, -7, -14, 24, -2, 24, -9, 2, 4, -2, -33, 9, -2, 1, -30, -30, -13, 24, -19}
, {-3, -4, -26, 13, 18, 11, -3, 1, 23, 8, -23, -21, 2, -16, -4, -2, 30, -10, -15, 11, -17, -7, -13, -21, 2, -1, 10, -25, -35, 19, -13, -25, 35, -23, 7, -19, 32, 21, 0, -29, -23, 11, 6, -23, -7, -8, -16, -2, -13, 10, -3, 22, 29, 16, 8, -21, -22, -33, 3, -17, -39, -16, 9, 17, -4, -2, -1, 16, 15, 2, -10, -10, 1, -25, -4, 14, -12, 14, 21, -15, 9, 2, 3, 26, 2, 1, -8, 10, 14, 21, -3, 37, -22, 24, 15, -8, 12, 16, -8, 5, -8, -6, 23, -9, 11, -2, 7, 36, 12, -23, 13, 1, -13, 4, 21, -20, 28, 14, 8, -20, -4, -15, 10, -10, -41, -11, -12, -34}
, {-5, 30, -25, 14, 21, 10, 23, -17, 23, -9, -19, 38, 2, 23, 0, 7, 11, 36, -8, -6, 8, -13, 14, -7, 11, -1, 14, -28, -18, -25, 7, 31, 40, -14, -11, -30, 4, 6, -22, 18, 6, 48, -14, -23, 5, 2, 20, -18, 26, 44, -31, 19, 12, -8, 9, 6, -33, -14, -7, -9, 31, -46, -25, 3, -12, 5, -10, 34, 22, -26, -20, -3, -19, 15, 15, 34, -11, -11, -11, -33, -25, 7, -12, 20, 19, -12, -16, -30, 0, -19, -13, 1, -2, -17, 24, 21, 26, -5, -15, 4, -15, -11, -2, 15, 7, -9, 23, -17, -24, -16, -10, 12, -8, -27, 40, 40, -4, 26, 17, -20, 17, 23, 0, -7, -33, -13, 18, -4}
, {23, -10, -13, -29, -16, -38, -22, -2, 18, 2, -28, -5, -26, 24, -5, 4, 7, 10, -22, 15, -10, 20, -42, -7, -12, -6, 10, 6, -26, 5, -37, -27, 10, 19, -27, -17, 4, 7, -12, 19, 27, 7, -26, -26, 8, -36, -25, -22, 39, 17, -29, 0, -10, -2, 2, -33, -20, 6, -36, 18, 25, -1, 31, -20, 1, 9, -3, -4, 4, 4, -2, -14, 20, 14, -29, 8, 1, -19, 14, 6, -21, -20, -4, -15, 8, -41, 8, 4, -6, -13, -9, 30, 31, -31, 29, -5, -2, -34, -31, 0, 11, -12, -27, 26, 25, -36, 32, -4, 10, 42, -23, 1, 4, -19, -6, 11, -9, 28, 15, -22, 28, 14, -13, 12, 1, 14, 23, -26}
, {-9, 42, 24, -31, -7, -1, 13, 25, 28, 23, 28, 31, 39, -23, -11, 4, -37, -13, 3, -30, 14, 4, -11, 2, -34, 6, -7, -13, -13, -9, -20, -16, -49, 9, -6, 18, -27, 40, 20, 4, -19, 3, 21, -21, 42, 8, 13, 13, 17, -30, -30, -21, -39, -1, -13, 3, -12, 26, -19, -11, 12, 11, -1, -15, -32, -15, -1, -12, -21, 23, 12, 18, -19, 9, 14, -18, 6, -51, -22, -1, 8, -32, 28, -1, 21, 20, -9, 12, 6, -7, 14, -32, -6, -20, 9, -6, -5, -34, -20, 11, -5, 8, -21, -14, -13, -33, -7, -5, 8, 4, -14, 3, 17, 1, 18, 25, 0, -14, 32, 29, -5, 16, -6, 6, 42, 0, 28, 0}
, {20, -23, 25, -19, -25, -7, 13, 26, -21, -23, -27, -2, -23, 18, -15, -12, -31, -9, -8, -7, -15, -16, 18, -21, -9, -9, 34, -3, 5, -11, -13, 9, 11, 42, 17, 4, -7, 5, -5, -9, -19, 14, -18, 36, 21, 1, 17, -22, -23, -19, -10, 17, -6, 3, 1, -35, -2, 39, 7, 19, -24, 17, 1, 11, 3, 11, -8, 6, 2, 1, -27, 17, -15, -11, -4, 2, 8, 38, 18, -4, -19, 22, -39, -4, 7, -23, 1, -23, 7, -6, 20, -21, -28, 15, 31, -11, 29, -21, -8, -6, 22, -13, 19, 26, 36, 15, -5, -19, -25, 23, -1, -10, -8, 6, 7, -14, 5, -11, -1, -9, 0, 12, 16, 25, 4, 11, 2, 33}
, {-22, 30, -13, 16, -26, -13, 16, 8, 12, -12, -26, 18, 4, 27, 22, -20, -1, -19, -13, -22, 19, 1, 9, 14, 5, -9, -24, -25, -12, -12, 20, 34, -3, -16, 34, 12, 6, -15, 23, 2, 4, -3, 23, 15, -28, 18, -11, 18, -8, 22, 26, -34, -12, -12, 27, 10, 13, 14, 0, -27, 11, -18, 12, -18, 2, 15, -17, 22, 24, 8, 9, 22, 14, 35, -20, -9, 7, -41, 4, -14, -24, -34, -8, 5, -16, 13, -3, 5, 2, -40, -41, -28, -12, 22, -16, 17, 10, -10, -8, -6, -11, 22, 13, 13, -16, 15, 16, -55, 5, -15, 16, 11, -8, -24, 15, 14, -23, -18, -12, -10, 34, -23, -20, -20, 25, -1, 20, 30}
}
;

#undef INPUT_SAMPLES
#undef FC_UNITS
/**
  ******************************************************************************
  * @file    fc.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version V2.0
  * @date    24 january 2023
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef _DENSE_1_H_
#define _DENSE_1_H_

#ifndef SINGLE_FILE
#include "number.h"
#include <stdint.h>
#endif

#define INPUT_SAMPLES 64
#define FC_UNITS 28

typedef int16_t dense_1_output_type[FC_UNITS];

#if 0
void dense_1(
  const number_t input[INPUT_SAMPLES], 			      // IN
	const number_t kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const number_t bias[FC_UNITS],			              // IN

	number_t output[FC_UNITS]); 			                // OUT
#endif

#undef INPUT_SAMPLES
#undef FC_UNITS

#endif//_DENSE_1_H_
/**
  ******************************************************************************
  * @file    fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifndef SINGLE_FILE
#include "dense_1.h"
#include "number.h"
#endif

#ifdef WITH_CMSIS_NN
#include "arm_nnfunctions.h"
#elif defined(WITH_NMSIS_NN)
#include "riscv_nnfunctions.h"
#endif

#define INPUT_SAMPLES 64
#define FC_UNITS 28
#define ACTIVATION_LINEAR

// For fixed point quantization
#define WEIGHTS_SCALE_FACTOR 7
#define BIASES_SCALE_FACTOR 7
#define TMP_SCALE_FACTOR 7
#define INPUT_SCALE_FACTOR 7
#define OUTPUT_SCALE_FACTOR 7
#define OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define NUMBER_T int16_t
#define LONG_NUMBER_T int32_t


static inline void dense_1(
  const NUMBER_T input[INPUT_SAMPLES], 			      // IN
	const NUMBER_T kernel[FC_UNITS][INPUT_SAMPLES],  // IN

	const NUMBER_T bias[FC_UNITS],			              // IN

	NUMBER_T output[FC_UNITS]) {			                // OUT

#if !defined(WITH_CMSIS_NN) && !defined(WITH_NMSIS_NN)
  unsigned short k, z; 
  LONG_NUMBER_T output_acc;

  for (k = 0; k < FC_UNITS; k++) { 
    output_acc = 0;
    for (z = 0; z < INPUT_SAMPLES; z++) 
      output_acc = output_acc + ((LONG_NUMBER_T)kernel[k][z] * (LONG_NUMBER_T)input[z]);

    output_acc = scale(NUMBER_T, output_acc, WEIGHTS_SCALE_FACTOR - TMP_SCALE_FACTOR, OUTPUT_ROUND_MODE);

    output_acc += scale(NUMBER_T, (LONG_NUMBER_T)bias[k], BIASES_SCALE_FACTOR - TMP_SCALE_FACTOR - INPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);


    // Activation function
#ifdef ACTIVATION_LINEAR
    // Linear (MEANS NONE)
    output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
#elif defined(ACTIVATION_RELU) || defined(ACTIVATION_RELU6)
    // ReLU
    if (output_acc < 0) {
      output[k] = 0;
    } else {
#if defined(ACTIVATION_RELU6)
      if (output_acc > scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE)) {
        output_acc = scale(NUMBER_T, 6, -(INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR), OUTPUT_ROUND_MODE);
      }
#endif
      output[k] = scale_and_clamp_to(NUMBER_T, output_acc, INPUT_SCALE_FACTOR + TMP_SCALE_FACTOR - OUTPUT_SCALE_FACTOR, OUTPUT_ROUND_MODE);
    }
#else
#error "Unsupported activation function"
#endif
  }
#else

#if BIASES_SCALE_FACTOR > WEIGHTS_SCALE_FACTOR
#error "CMSIS-NN does not support BIASES_SCALE_FACTOR larger than WEIGHTS_SCALE_FACTOR"
#endif

  static q15_t bufferA[INPUT_SAMPLES];
#ifdef WITH_CMSIS_NN
  arm_fully_connected_q15(
#elif defined(WITH_NMSIS_NN)
  riscv_fully_connected_q15(
#endif
                             (q15_t*)input,
                             (q15_t*)kernel,
                             INPUT_SAMPLES,
                             FC_UNITS,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - BIASES_SCALE_FACTOR,
                             INPUT_SCALE_FACTOR + WEIGHTS_SCALE_FACTOR - OUTPUT_SCALE_FACTOR,
                             (q15_t*)bias,
                             (q15_t*)output,
                             (q15_t*)bufferA);
#ifdef ACTIVATION_RELU
#ifdef WITH_CMSIS_NN
  arm_relu_q15((q15_t*)output, FC_UNITS);
#elif defined(WITH_NMSIS_NN)
  riscv_relu_q15((q15_t*)output, FC_UNITS);
#endif
#elif !defined(ACTIVATION_LINEAR)
#error "Unsupported activation with CMSIS-NN"
#endif


#endif
}

#undef INPUT_SAMPLES
#undef FC_UNITS
#undef ACTIVATION_LINEAR
#undef WEIGHTS_SCALE_FACTOR
#undef BIASES_SCALE_FACTOR
#undef INPUT_SCALE_FACTOR
#undef OUTPUT_SCALE_FACTOR
#undef NUMBER_T
#undef LONG_NUMBER_T
/**
  ******************************************************************************
  * @file    weights/fc.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#include <stdint.h>

#define INPUT_SAMPLES 64
#define FC_UNITS 28


const int16_t dense_1_bias[FC_UNITS] = {7, 6, 5, 0, -16, 17, -3, -4, -22, -5, -28, -7, 12, 7, 12, 3, -4, 2, 6, 2, -2, -8, 6, -2, -11, -9, 1, -7}
;

const int16_t dense_1_kernel[FC_UNITS][INPUT_SAMPLES] = {{19, -18, -50, -36, 31, -13, 35, -47, 2, -40, -4, 28, 38, -28, 8, 23, -15, -13, 7, -48, 22, 10, 13, -37, 10, -23, 24, -33, 14, 34, 24, -23, 28, -67, -6, 17, 2, -21, -29, 5, -42, -48, -21, -16, -3, 5, -28, -9, -57, 25, -24, 28, -10, 34, 24, -21, 7, -21, 6, -37, 11, -29, -43, -24}
, {20, -27, -60, -41, 18, 15, 24, -59, 10, 22, 11, -17, -6, -34, 0, -40, -54, -28, -35, -40, 23, 35, 30, 21, 12, -34, 30, -20, 8, 29, -9, 1, -27, -17, 21, 25, -49, -2, -29, -26, 25, -42, 3, -24, 18, 13, 17, -14, -23, 20, 7, -21, 27, 29, 12, -19, 26, 21, -17, 12, 18, -21, 14, -39}
, {12, -31, -18, -56, -6, 19, -30, -22, 3, 23, -13, 23, 3, -37, 29, -45, -8, 39, -12, -50, 29, 31, -6, -4, -25, -41, 24, -28, 13, -3, -3, 7, -24, 28, -3, 22, 0, 26, -28, -12, 23, 11, 32, 22, 13, 18, 17, -19, -20, 10, -18, 29, 26, -27, -39, -13, 13, -12, -28, -11, 3, 25, -50, -24}
, {23, -50, -2, 17, 12, -39, -25, -55, -3, 25, 18, 24, -37, -7, 0, 9, -19, -47, -6, -23, 33, 14, 31, 24, 10, 24, -19, -55, -8, -18, -29, -45, -8, -40, -40, 3, 21, 24, -30, -8, 18, 14, 30, -11, 15, 2, 18, 33, -10, -31, 4, 30, 22, -46, -51, -41, -30, 22, -24, -29, -34, 27, -45, 28}
, {31, 21, -31, 13, -48, -5, 36, 20, 5, -11, 22, -5, 4, -58, 27, -40, -10, -32, 20, 10, -23, 28, 35, -12, -48, -50, 25, -33, 2, 22, 16, 22, -27, 22, 14, -44, -18, -10, 19, -37, 22, -6, -3, 19, -40, 24, -7, -6, -68, 18, -51, -35, 14, 29, -60, -33, -3, 13, 25, 18, -35, -23, -51, 12}
, {18, -26, -40, 6, 20, -45, 29, -28, -35, 29, 8, -25, -22, 1, 28, -30, 4, 16, -34, -19, -11, 34, 21, 11, 7, 21, -18, -20, -27, -12, -21, -41, -23, 30, 21, 22, 3, 23, 10, -34, 19, 15, 31, 11, 18, -16, 23, -31, -30, -33, 13, -29, 2, 18, -57, 27, -21, 24, -45, 14, 15, 23, -43, 17}
, {33, 29, -64, 9, -3, 6, 26, -26, -15, -23, -49, -37, 11, -5, 27, -35, -21, -33, 23, -65, 40, 13, -36, 36, -34, 15, 19, -1, -57, -18, 20, -36, -8, 29, 32, 23, -54, 24, -6, -69, 14, 16, 28, 19, 9, -42, 22, -25, 12, 10, -43, 9, -20, -14, -37, -23, -45, 25, 13, 1, -12, -2, 0, -43}
, {31, 28, -38, 27, 17, 7, 28, -22, -1, -24, 32, -14, -23, -47, 30, -9, -45, 24, 24, 4, 33, -41, 23, 16, -15, 7, 21, -23, -1, -33, 28, -29, -35, 22, 33, -28, -43, -50, 17, 19, -13, 15, 29, -12, -22, -27, -47, 2, -18, -22, -13, 21, 4, 13, -15, 27, -5, 18, -6, -39, -26, -14, 5, -33}
, {18, -37, -41, 25, -50, 3, 6, -16, -48, -20, 28, -39, 32, -45, -54, -26, 34, 30, 33, -39, -20, -34, 35, -39, -12, 8, -22, 32, 28, -10, -34, -12, 28, -3, -40, -27, -32, 24, -54, 26, -43, -10, -8, -68, -43, -24, -52, -38, 23, -43, -35, -22, -32, -64, 17, 20, 19, 26, 28, 35, -13, -58, 14, -32}
, {25, -18, -34, 2, -16, -41, -29, -63, -55, -30, 29, 7, 34, -21, -48, -33, 33, -39, -6, 23, 29, -9, 33, -63, -55, 19, 8, -6, 36, 28, -25, -21, -32, 47, -19, -34, -52, -41, -76, -20, -54, -41, -28, -60, 8, -58, -1, 36, -33, -69, -52, -2, -42, -18, -44, 25, -21, 35, -43, 42, 39, -66, -33, 27}
, {-45, -6, 12, 22, -46, 30, -24, 7, -6, 30, -32, 29, 27, 22, -62, -47, -40, -37, 18, 9, 5, 41, 34, 3, -23, 23, -39, 12, 30, 36, 35, -17, 28, -5, 21, 8, -28, -23, -8, 23, 17, -52, 20, 12, -48, -51, -15, -9, -7, -31, -28, 9, -41, -31, -66, -60, 31, -60, -26, 22, -44, -30, 18, 26}
, {-26, 31, 5, -23, -46, 31, 14, 12, -69, -49, -20, -46, -57, -23, -17, -30, 27, 43, -53, 30, -57, -54, -23, 30, -37, -13, -33, -21, -53, -16, 34, 20, -35, 5, -17, -45, 26, -34, 20, -26, 13, -33, -33, 19, 24, 20, 22, -44, 16, -2, 26, 29, 25, -27, 18, -10, -12, 9, -23, 34, -11, 24, -36, 15}
, {30, -31, 2, 27, 27, -50, -2, 33, -20, -9, -28, -29, 10, -22, 2, 26, 30, -50, 22, -11, -40, -22, -24, -40, -15, 1, -33, 2, -47, -27, -37, 29, 4, -21, -55, -54, 21, 33, 21, 25, -12, -6, -36, -4, -49, -28, -27, -5, 32, 14, -28, 0, -26, -36, 26, 2, 31, -32, -25, -29, -40, 1, -48, 33}
, {-38, 27, 19, -3, 28, 24, -18, 14, -50, -9, -43, -30, -23, -65, -50, 11, -49, -7, -32, 26, 2, -28, 24, 17, 19, 30, -28, 13, -22, -3, 2, -44, 38, -18, 40, -6, 30, -25, 33, -18, -43, 15, -38, -50, 28, 30, -8, -39, -37, -53, 26, 31, -15, -21, 2, 19, -24, -36, 38, -41, -26, -9, -26, -13}
, {-31, -2, 25, -27, 24, 15, -17, 24, -44, -23, -18, -60, -30, 22, -47, 16, 48, -11, -27, -10, 14, -25, -42, 3, -37, -31, 23, -23, 3, -13, -10, 31, -53, -12, 27, -1, -41, 33, -34, -31, -35, 4, 8, -59, 21, 26, 5, -29, 28, -53, -14, -28, 28, -50, 23, 26, 31, 14, 24, -42, 11, -38, -10, -11}
, {7, 19, 35, 26, -15, 6, -24, -6, -68, -37, 14, -63, 8, -38, -51, -13, 0, -8, -48, 20, -27, -16, -48, 4, -22, 16, -9, -23, 14, -16, -9, 24, 24, -53, -11, 18, 36, 30, -22, 30, -37, 11, 0, -28, 23, 32, -32, 19, 26, -22, -48, 1, -38, -36, 22, 25, -40, -15, 23, -21, -19, -24, -22, -9}
, {-24, -16, -14, -12, -30, -11, -14, -18, -81, -31, 0, 2, -19, -19, 15, 27, -7, 5, -43, -21, -2, -23, -1, 13, -31, -15, -23, 47, -44, -20, 41, 8, 18, -45, -64, -44, 13, 30, 31, -51, -69, -19, 16, -7, 28, -13, 15, -42, 25, -35, 35, -27, 27, -8, 24, 26, 19, -60, 4, -9, 25, 30, 28, -46}
, {-36, -13, 20, 30, -14, -44, -30, -14, -41, -20, -52, 12, 16, 2, -43, 15, -25, 7, -58, -30, 39, -3, -44, -20, 15, 35, 33, 26, -58, 11, -19, 17, 24, -22, -54, -25, -9, -38, -26, -20, -25, -29, -26, 20, 27, 12, -11, 18, 19, -49, 24, -25, -24, -34, -4, 19, -31, 29, 39, 33, 14, -43, 21, -30}
, {-41, -52, 27, 26, 28, -27, -38, -50, -57, 28, -9, -23, -31, 22, -36, -17, 36, 20, -10, 26, -21, -37, -49, -37, -57, 14, 26, -21, -27, -40, -56, 31, -48, 36, 6, 17, 1, 15, 21, -19, 9, -45, -37, -39, 23, 26, -43, -23, 23, -24, -38, 29, -5, -2, 22, 30, -34, -11, -34, -36, -36, 23, 27, -4}
, {-73, 17, 20, -33, -52, -34, -42, -9, -1, -5, -29, -20, -24, 18, -26, -43, -30, -36, -27, 36, -54, -31, -48, 12, -45, 23, 37, -24, -3, 27, 40, -35, 10, 7, -61, 4, 34, -29, 13, -26, 15, -31, -56, 20, -11, -40, 26, -22, -29, 13, -20, 33, 29, -48, 17, 31, -10, -34, 31, 43, 10, 23, -1, -26}
, {-18, 7, 24, -9, -4, -37, -35, -30, -53, 33, 4, 38, -12, -31, -73, -38, 32, -25, -29, -24, -17, -39, -46, -6, -34, 10, 29, 3, -53, 31, -25, -1, 13, -54, -53, -8, -25, -33, 29, 1, 14, -8, -34, 25, 0, -42, -35, 0, -11, 19, 42, -12, -40, -46, 23, -6, 34, -66, 41, 30, 40, -27, 30, 12}
, {-45, 40, 13, 19, 30, 12, -36, -35, -38, 5, -12, 29, 16, 23, 14, -33, 29, -50, -36, 33, -40, 8, -24, -17, 24, -40, -1, 30, -21, -21, 11, 26, 25, -11, -17, -24, 19, 7, -32, -5, -23, -66, -1, -35, 13, -22, 22, -35, -29, -16, 15, -17, -48, -48, -15, -37, 17, 6, -24, 22, 33, 27, 23, -18}
, {-36, -34, -13, -43, 28, 31, -40, -16, 12, 5, 8, -27, -16, -47, -58, 25, 4, 38, -33, 9, -12, -36, -53, 51, 34, -54, -15, -19, 26, -16, 9, 3, 31, -16, -2, -48, 10, -37, -26, 30, -39, 14, -53, 41, -61, -14, -29, -3, 15, 27, -39, 1, -45, -17, -9, -50, -26, -49, -34, -33, -53, 17, -13, -38}
, {-8, -1, 36, -60, -32, 32, -54, 7, 19, -40, -38, -9, 14, -22, -1, 7, -3, 24, 39, -15, -59, 2, -36, -11, 14, -23, -40, -14, -38, 34, -39, -71, -40, -24, 33, -70, -36, -2, -24, -3, -41, 23, -25, 12, -87, 25, 27, -28, 20, 33, 9, -50, 25, -57, -31, -27, -35, -75, -24, -27, -6, 26, -15, 45}
, {-49, -19, -11, -63, -35, -45, -44, 28, 15, -57, 5, -23, 13, 8, -15, 28, 37, -6, -15, 15, -60, -51, 13, -57, 21, -60, -57, 30, 18, 2, -60, -64, -32, -11, 33, -44, -53, -47, 33, 40, -41, 19, -50, 36, -36, -19, 29, 31, -48, -21, -7, 0, 21, 14, 15, 11, -26, -50, 31, 19, -73, 11, -28, -27}
, {-47, -50, -32, -33, 23, -40, -35, 24, 12, 36, -34, -32, -29, 4, -57, 30, 28, -15, 8, -11, -30, -46, 6, 41, 29, -10, -56, 18, 33, 40, -37, -57, -29, -66, 11, -63, 19, 3, -17, -8, -76, 28, -45, -32, -70, -64, 21, 12, -25, 21, 11, -40, -30, -80, -18, -48, -59, 17, -21, -5, -71, -39, 27, 24}
, {-61, -41, 20, -30, -43, 21, -56, 25, 19, 20, 13, -6, -34, 13, -49, 17, -28, -12, -23, -23, -8, -45, 33, -11, 28, -35, -57, 27, -13, 20, 23, -54, 25, -38, 12, -52, 22, -44, -45, 19, -47, 23, 5, -13, -48, 26, -16, 35, -40, 17, -30, -40, 29, -50, -15, -25, 13, -9, 7, -34, -29, 19, 7, -31}
, {-37, -40, -48, -21, 31, -48, -33, -29, -6, -8, -45, -47, -48, 6, -14, 4, -48, -37, -44, 39, -29, -9, -14, 42, 15, -36, -36, 40, 27, 23, 44, -17, -26, 16, -28, 29, 24, -45, -51, -18, -23, 16, -42, 38, -14, -25, 17, -14, -51, -31, -29, -33, 8, 30, -12, -94, 36, 32, 26, 8, -41, -34, -60, 12}
}
;

#undef INPUT_SAMPLES
#undef FC_UNITS
/**
  ******************************************************************************
  * @file    model.hh
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    08 july 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MODEL_H__
#define __MODEL_H__

#ifndef SINGLE_FILE
#include "number.h"

 // InputLayer is excluded
#include "conv2d.h" // InputLayer is excluded
#include "batch_normalization.h" // InputLayer is excluded
#include "conv2d_1.h" // InputLayer is excluded
#include "batch_normalization_1.h" // InputLayer is excluded
#include "conv2d_2.h" // InputLayer is excluded
#include "batch_normalization_2.h" // InputLayer is excluded
#include "conv2d_3.h" // InputLayer is excluded
#include "flatten.h" // InputLayer is excluded
#include "dense.h" // InputLayer is excluded
#include "dense_1.h"
#endif


#define MODEL_INPUT_DIM_0 32
#define MODEL_INPUT_DIM_1 32
#define MODEL_INPUT_DIM_2 3
#define MODEL_INPUT_DIMS 32 * 32 * 3

#define MODEL_OUTPUT_SAMPLES 28

#define MODEL_INPUT_SCALE_FACTOR 7 // scale factor of InputLayer
#define MODEL_INPUT_ROUND_MODE ROUND_MODE_FLOOR
#define MODEL_INPUT_NUMBER_T int16_t
#define MODEL_INPUT_LONG_NUMBER_T int32_t

#define MODEL_OUTPUT_SCALE_FACTOR 7 // scale factor of last layer
#define MODEL_OUTPUT_ROUND_MODE ROUND_MODE_FLOOR
#define MODEL_OUTPUT_NUMBER_T int16_t
#define MODEL_OUTPUT_LONG_NUMBER_T int32_t

// node 0 is InputLayer so use its output shape as input shape of the model
// typedef  input_t[32][32][3];
typedef int16_t input_t[32][32][3];
typedef dense_1_output_type output_t;


void cnn(
  const input_t input,
  output_t output);

void reset(void);

#endif//__MODEL_H__


#ifdef __cplusplus
} // extern "C"
#endif
/**
  ******************************************************************************
  * @file    model.cc
  * @author  Pierre-Emmanuel Novac <penovac@unice.fr>, LEAT, CNRS, Université Côte d'Azur, France
  * @version 1.0.0
  * @date    24 march 2020
  * @brief   Template generating plain C code for the implementation of Convolutional Neural Networks on MCU
  */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SINGLE_FILE
#include "number.h"
#include "model.h"
// #include <chrono>

 // InputLayer is excluded
#include "conv2d.c"
#include "weights/conv2d.c" // InputLayer is excluded
#include "batch_normalization.c"
#include "weights/batch_normalization.c" // InputLayer is excluded
#include "conv2d_1.c"
#include "weights/conv2d_1.c" // InputLayer is excluded
#include "batch_normalization_1.c"
#include "weights/batch_normalization_1.c" // InputLayer is excluded
#include "conv2d_2.c"
#include "weights/conv2d_2.c" // InputLayer is excluded
#include "batch_normalization_2.c"
#include "weights/batch_normalization_2.c" // InputLayer is excluded
#include "conv2d_3.c"
#include "weights/conv2d_3.c" // InputLayer is excluded
#include "flatten.c" // InputLayer is excluded
#include "dense.c"
#include "weights/dense.c" // InputLayer is excluded
#include "dense_1.c"
#include "weights/dense_1.c"
#endif


void cnn(
  const input_t input,
  dense_1_output_type dense_1_output) {
  
  // Output array allocation
  static union {
    conv2d_output_type conv2d_output;
    conv2d_1_output_type conv2d_1_output;
    conv2d_2_output_type conv2d_2_output;
    conv2d_3_output_type conv2d_3_output;
    flatten_output_type flatten_output;
  } activations1;

  static union {
    batch_normalization_output_type batch_normalization_output;
    batch_normalization_1_output_type batch_normalization_1_output;
    batch_normalization_2_output_type batch_normalization_2_output;
    dense_output_type dense_output;
  } activations2;


// Model layers call chain 
  
  
  conv2d( // Model input is passed as model parameter
    input,
    conv2d_kernel,
    conv2d_bias,
    activations1.conv2d_output
    );
  
  
  batch_normalization(
    activations1.conv2d_output,
    batch_normalization_kernel,
    batch_normalization_bias,
    activations2.batch_normalization_output
    );
  
  
  conv2d_1(
    activations2.batch_normalization_output,
    conv2d_1_kernel,
    conv2d_1_bias,
    activations1.conv2d_1_output
    );
  
  
  batch_normalization_1(
    activations1.conv2d_1_output,
    batch_normalization_1_kernel,
    batch_normalization_1_bias,
    activations2.batch_normalization_1_output
    );
  
  
  conv2d_2(
    activations2.batch_normalization_1_output,
    conv2d_2_kernel,
    conv2d_2_bias,
    activations1.conv2d_2_output
    );
  
  
  batch_normalization_2(
    activations1.conv2d_2_output,
    batch_normalization_2_kernel,
    batch_normalization_2_bias,
    activations2.batch_normalization_2_output
    );
  
  
  conv2d_3(
    activations2.batch_normalization_2_output,
    conv2d_3_kernel,
    conv2d_3_bias,
    activations1.conv2d_3_output
    );
  
  
  flatten(
    activations1.conv2d_3_output,
    activations1.flatten_output
    );
  
  
  dense(
    activations1.flatten_output,
    dense_kernel,
    dense_bias,
    activations2.dense_output
    );
  
  
  dense_1(
    activations2.dense_output,
    dense_1_kernel,
    dense_1_bias,// Last layer uses output passed as model parameter
    dense_1_output
    );
}

#ifdef __cplusplus
} // extern "C"
#endif
