#ifndef SIMULATION
#define SIMULATION

#include "inputs.h"
#include "math_helper.h"
#include<stdlib.h>
#include "pr_data.h"

typedef struct charging_station_speed_category
{
    unsigned avbl_count; // unused parking spots
    unsigned charge_speed; // speed of charging ports
    // wait_times[i] represents the number of cars that still need ((i-clock_zero) mod length) ticks to charge.
    unsigned* wait_times; 
    size_t length; // of wait_times
    size_t clock_zero; // scans left to right through wait_times
    unsigned total_charged;
    unsigned parking_events;

} CSSC; // ==charging station speed category

typedef struct result
{
    unsigned* total_charged;
    unsigned max_observed; 
    unsigned* parking_events;
    unsigned max_observed_demand;
} Result;

void CSSC_init(CSSC* cs, unsigned charge_speed, unsigned parking_spots);
// let one tick of time pass
void CSSC_tick(CSSC* cs);

void CSSC_park_cars(CSSC* cs, unsigned wait_time_km, unsigned count);

// generate charging needs randomly based on probability table
unsigned roll_charging_needs();

// generate random amount of cars based on probability table per parking spot
// returns total charging needs of all new cars in km
void CSSC_roll_for_arrivals(CSSC* cs, double* pr_arrival, unsigned time_of_day);

void CSSC_free(CSSC* cs);
void Result_free(Result* res);

Result run_simulation(unsigned* speeds, unsigned* amounts, size_t categories);
#endif