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
#include "max_pooling2d.h" // InputLayer is excluded
#include "conv2d_1.h" // InputLayer is excluded
#include "max_pooling2d_1.h" // InputLayer is excluded
#include "flatten.h" // InputLayer is excluded
#include "dense.h" // InputLayer is excluded
#include "dense_1.h"
#endif


#define MODEL_INPUT_DIM_0 32
#define MODEL_INPUT_DIM_1 32
#define MODEL_INPUT_DIM_2 3
#define MODEL_INPUT_DIMS 32 * 32 * 3

#define MODEL_OUTPUT_SAMPLES 28

#define MODEL_INPUT_SCALE_FACTOR 0 // scale factor of InputLayer
#define MODEL_INPUT_ROUND_MODE ROUND_MODE_NONE
#define MODEL_INPUT_NUMBER_T float
#define MODEL_INPUT_LONG_NUMBER_T float

#define MODEL_OUTPUT_SCALE_FACTOR 0 // scale factor of last layer
#define MODEL_OUTPUT_ROUND_MODE ROUND_MODE_NONE
#define MODEL_OUTPUT_NUMBER_T float
#define MODEL_OUTPUT_LONG_NUMBER_T float

// node 0 is InputLayer so use its output shape as input shape of the model
// typedef  input_t[32][32][3];
typedef float input_t[32][32][3];
typedef dense_1_output_type output_t;


void cnn(
  const input_t input,
  output_t output);

void reset(void);

#endif//__MODEL_H__


#ifdef __cplusplus
} // extern "C"
#endif