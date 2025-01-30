// Optimization ideas: 
// - calculate the ticks_till_charged of pr_charging_needs (once for each charging station speed used) ahead of time
// - calculate probability spaces for amount of cars spawned ahead of time

#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"

typedef struct outputs
{   
    // same as in 'struct result'
    unsigned total_charged;

    unsigned max_observed; 
    unsigned total_spaces;

    unsigned max_observed_demand; // in kW
    unsigned max_theoretical_demand; 

    unsigned parking_events; // total cars parked
    double total_power_consumed; // in kWh
    double concurrency_factor;
    double time_occupied; // in %
    double average_charging_needs;

} Outputs;


Outputs process_results(Result*res, unsigned* charging_speeds, unsigned* parking_spots, unsigned categories){
    Outputs outs;

// calculate general values

    // copy over from Result struct
    outs.max_observed = res->max_observed;
    outs.max_observed_demand = res->max_observed_demand;

    // calculations
    outs.average_charging_needs = 0;
    for(unsigned i=0; i<len_charging_needs; ++i){
        outs.average_charging_needs += pr_charging_needs[i]*(double)val_charging_needs[i]/100.0;
    }

// calculate speed specific values

    outs.total_charged = 0;
    outs.parking_events = 0;

    outs.total_spaces = 0;
    outs.max_theoretical_demand = 0;
    double hours_spent_charging = 0.0;

    // all acronyms are for specific CSSC
    for(unsigned i=0; i<categories; ++i){
        
        // copy over from Result struct
        unsigned tc = res->total_charged[i];
        unsigned pe = res->parking_events[i];
        unsigned ts = parking_spots[i];
        unsigned mtd = parking_spots[i]*charging_speeds[i];
        
        // calculations
        double tpc = (double)(tc*energy_efficiency)/100.0; // total power consumed
        hours_spent_charging += tpc/(double)charging_speeds[i];
        
        outs.max_theoretical_demand += mtd;
        outs.total_spaces += ts;
        outs.total_charged += tc;
        outs.parking_events += pe;
    }
    outs.time_occupied += hours_spent_charging/(double)(outs.total_spaces*simulation_duration);
    outs.total_power_consumed = (double)(outs.total_charged*energy_efficiency)/100.0;
    outs.concurrency_factor = (double)outs.max_observed_demand/(double)outs.max_theoretical_demand;

    return outs;
}

Outputs run_with_default_params(unsigned parking_spots) {

    unsigned* speeds = malloc(sizeof(unsigned));
    unsigned* amounts = malloc(sizeof(unsigned));
    speeds[0] = default_charging_speed;
    amounts[0] = parking_spots;
    Result res = run_simulation(speeds, amounts, 1);
    Outputs outs = process_results(&res, speeds, amounts, 1);
    free(speeds);
    free(amounts);
    Result_free(&res);
    return outs;
}

double concurrency_factor(Result* res, unsigned parking_spots){
    return (double)res->max_observed/(double)parking_spots;
}

void print_outputs(Outputs* outs, unsigned expectations){
    printf("  The most EVs parked at once was: %d\n", outs->max_observed);
    printf("  This corresponds to a power demand of: %dkW", outs->max_observed_demand);
    if(expectations==1){printf(" (should be between 77-121)");}
    printf("\n");
    printf("  The theoretical maximum power demand is: %dkW\n", outs->max_theoretical_demand);
    printf("  This corresponds to a concurrency factor of: %f%%", outs->concurrency_factor);
    if(expectations==1){printf(" (should be between 35-55percent)");}
    printf("\n");
    printf("  The total amount charged was: %dkm\n", outs->total_charged);
    printf("  With a fuel efficiency of %dkWh/100km, this totals to: %fkWh\n", energy_efficiency, outs->total_power_consumed);
    printf("  In total: %d EVs parked there in a timespan of %d days\n", outs->parking_events, simulation_duration/24u);
    printf("  Given the probability data, we expect a car to have to charge: %fkm\n", outs->average_charging_needs);
    printf("    Multiply this by the number of cars parked, and the result: %fkm\n", outs->average_charging_needs*outs->parking_events);
    printf("    should be around %dkm.\n", outs->total_charged);
    printf("  In total, charging stations where occupied about: %f%% of the simulation.\n", outs->time_occupied*100);
}
int main(int argc, char *argv[])
{
    // run with default parameters
    {
        Outputs outs = run_with_default_params(20);
        printf("\nResults for a default simulation of %d parking spaces that charge at a rate of %dkW each.\n", 20, default_charging_speed);
        print_outputs(&outs, 1);
    }

    // run for between 1 and 30 chargepoints
    {
        printf("\nDifferent concurrency factors depending on the number of charging stations:\n");
        for(unsigned i=1; i<31; ++i){
            Outputs outs = run_with_default_params(i);
            printf("  %d: %f\n", i, outs.concurrency_factor);
        }
    }
    // example of how to run with charging stations that charge at different speeds:
    printf("\nResults for a simulation of 5, 3, and 1 parking spaces that charge at a rates of 11, 22 and 55 respectively.\n");
    {   
        unsigned* speeds = malloc(3*sizeof(unsigned));
        unsigned* amounts = malloc(3*sizeof(unsigned));
        speeds[0] = 11;
        amounts[0] = 5;
        speeds[1] = 22;
        amounts[1] = 3;
        speeds[2] = 50;
        amounts[2] = 1;
        Result res = run_simulation(speeds, amounts, 3);
        Outputs outs = process_results(&res, speeds, amounts, 3);
        free(speeds);
        free(amounts);
        Result_free(&res);

        print_outputs(&outs, 0);
    }

    return 0;
}

// left over from debugging:
//for(int q = 0; q<9;++q){printf("%d : %d\n", val_charging_needs[q], ticks_till_charged(val_charging_needs[q], 11));}
//for(int j = 0; j<50;++j){printf("result:! %d\n", roll_charging_needs());}