#ifndef SETTING_H_
#define SETTING_H_

// Select only ONE type of anonymity
// #define CCA2_SIGNATURE
#define CPA_SIGNATURE

// if PRE_COMP is defined values are pre computated and stored on the drone
// #define PRE_COMP // if selected pre-comp version is used

#ifdef PRE_COMP
  #define FLIGHT_TIME 5 // number of pre-comp.
#endif

#endif