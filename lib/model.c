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
#include "max_pooling2d.c" // InputLayer is excluded
#include "conv2d_1.c"
#include "weights/conv2d_1.c" // InputLayer is excluded
#include "max_pooling2d_1.c" // InputLayer is excluded
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
    dense_output_type dense_output;
  } activations1;

  static union {
    max_pooling2d_output_type max_pooling2d_output;
    max_pooling2d_1_output_type max_pooling2d_1_output;
    flatten_output_type flatten_output;
  } activations2;


// Model layers call chain 
  
  
  conv2d( // Model input is passed as model parameter
    input,
    conv2d_kernel,
    conv2d_bias,
    activations1.conv2d_output
    );
  
  
  max_pooling2d(
    activations1.conv2d_output,
    activations2.max_pooling2d_output
    );
  
  
  conv2d_1(
    activations2.max_pooling2d_output,
    conv2d_1_kernel,
    conv2d_1_bias,
    activations1.conv2d_1_output
    );
  
  
  max_pooling2d_1(
    activations1.conv2d_1_output,
    activations2.max_pooling2d_1_output
    );
  
  
  flatten(
    activations2.max_pooling2d_1_output,
    activations2.flatten_output
    );
  
  
  dense(
    activations2.flatten_output,
    dense_kernel,
    dense_bias,
    activations1.dense_output
    );
  
  
  dense_1(
    activations1.dense_output,
    dense_1_kernel,
    dense_1_bias,// Last layer uses output passed as model parameter
    dense_1_output
    );
}

#ifdef __cplusplus
} // extern "C"
#endif