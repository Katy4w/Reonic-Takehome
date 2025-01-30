#include "inputs.h"
#include "math_helper.h"
// arithmatic for looping array indices. n == length
#define ADD_MOD(i, k, n)    i += k;\
                            if(i>=n){i-=n;}
#define SUB_MOD(i, k, n)    if(i<k){i+=n;}\
                            i-=k;

// calculates ceil(a / b)
//b!=0
unsigned ceil_div(unsigned a, unsigned b) { 
    return (a+b-1)/b;
}
// how many ticks will an EV need until it's done charging.
unsigned ticks_till_charged(unsigned km, unsigned charging_speed){
    if( km==0 ){return 1;} // <- mathematically wrong but required for this program

    //              (km * kWh/100km         * ticks/h     ) / (kW/100)               = ticks
    return ceil_div((km * energy_efficiency * ticks_per_hour), charging_speed*100u);
    // we multiply charging speed by 100 to cancel out the energy efficiency being 100 times too big.
}