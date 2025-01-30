#ifndef MATH_HELPER
#define MATH_HELPER

#include "inputs.h"

// arithmatic for looping array indices. n == length
#define ADD_MOD(i, k, n)    i += k;\
                            if(i>=n){i-=n;}
#define SUB_MOD(i, k, n)    if(i<k){i+=n;}\
                            i-=k;

// calculates ceil(a / b)
//b!=0
unsigned ceil_div(unsigned a, unsigned b);
// how many ticks will an EV need until it's done charging.
unsigned ticks_till_charged(unsigned km, unsigned charging_speed);

#endif