#ifndef INPUTS
#define INPUTS

// inputs
#define max_need                300u // 1km
#define energy_efficiency       18u // kWh_per_100km
#define default_charging_speed  11u // kW
#define ticks_per_hour          4u
#define simulation_duration     (365u*24u) // h
#define random_seed             2025

#define total_ticks (ticks_per_hour*simulation_duration)

#endif