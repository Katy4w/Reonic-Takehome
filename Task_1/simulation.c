#include "inputs.h"
#include "math_helper.h"
#include<stdlib.h>
#include "simulation.h"
#include "pr_data.h"

void CSSC_init(CSSC* cs, unsigned charge_speed, unsigned parking_spots){
    size_t max_ticks = ticks_till_charged(max_need, charge_speed); 
    cs->length = max_ticks;
    cs->charge_speed = charge_speed;
    cs->wait_times = (unsigned*)malloc(max_ticks * sizeof(unsigned));
    cs->avbl_count = parking_spots;
    cs->total_charged = 0;
    cs->parking_events = 0;
    for(unsigned i=0; i<max_ticks; ++i){cs->wait_times[i]=0;} // nothing is parked yet
}
// let one tick of time pass
void CSSC_tick(CSSC* cs){
    cs->clock_zero++;
    if(cs->clock_zero==cs->length){cs->clock_zero=0;}

    // queue out cars that finish charging
    cs->avbl_count += cs->wait_times[cs->clock_zero];
    cs->wait_times[cs->clock_zero] = 0;
    //return freed;
}

void CSSC_park_cars(CSSC* cs, unsigned wait_time_km, unsigned count){
    unsigned t = ticks_till_charged(wait_time_km, cs->charge_speed);
    size_t i = cs->clock_zero;
    ADD_MOD(i, t, cs->length);
    cs->wait_times[i] += count;
    cs->avbl_count -= count;
}

// generate charging needs randomly based on probability table
unsigned roll_charging_needs(){
    
    double* prs = pr_charging_needs;
    unsigned* vals = val_charging_needs;
    size_t len = len_charging_needs;
    double rn = (double)rand()/(double)RAND_MAX*100.0;

    unsigned i = 0;

    while((rn>=prs[i]) && (i<len-1)){
        rn -= prs[i];
        i++;
    }
    return vals[i];
}

// generate random amount of cars based on probability table per parking spot
// returns total charging needs of all new cars in km
void CSSC_roll_for_arrivals(CSSC* cs, double* pr_arrival, unsigned time_of_day){
    unsigned total_cn = 0;
    unsigned new_cars = 0;
    
    for(int i=0; i<cs->avbl_count; ++i){ 
        double rn = (double)rand() / (double)RAND_MAX * 100.0; // random number 0 to 100
        if(rn<=pr_arrival[time_of_day]){
            unsigned cn = roll_charging_needs();
            CSSC_park_cars(cs, cn, 1); 
            total_cn += cn;
            new_cars++;
        }
    }
    cs->total_charged += total_cn;
    cs->parking_events += new_cars;
}
void CSSC_free(CSSC* cs){
    free(cs->wait_times);
}
void Result_free(Result* res){
    free(res->parking_events);
    free(res->total_charged);
}

Result run_simulation(unsigned* speeds, unsigned* amounts, size_t categories){
    
    // seed random values
    srand(random_seed);

    // return values
    Result res;
    res.max_observed = 0; // as in cars
    unsigned total_spots = 0;

    res.max_observed_demand = 0; // as in kW
    unsigned potential_demand = 0;

    //init clock
    unsigned time_of_day = 0;
    unsigned hour_progress = 0;
    
    //init parking
    CSSC* css; // only contains one element when charging speeds are the same
    css = (CSSC*)malloc(categories*sizeof(CSSC)); 

    for(unsigned i=0; i<categories; ++i){
        
        CSSC_init(&css[i], speeds[i], amounts[i]);
        total_spots += amounts[i];
        potential_demand += amounts[i]*speeds[i];
    }
    
    // run simulation
    for(unsigned tick=0; tick<total_ticks; ++tick){
        unsigned spots = 0; // number of free spots
        unsigned demand = 0; // kW not being used

        for(unsigned i=0; i<categories; ++i){
            CSSC_roll_for_arrivals(&css[i], pr_arrivals, time_of_day);
            CSSC_tick(&css[i]);

            spots += css[i].avbl_count;
            demand += css[i].avbl_count*speeds[i];
        }

        spots = total_spots - spots; // now number of used spots
        demand = potential_demand - demand; // now current power demand

        // keep track
        if(spots > res.max_observed){
            res.max_observed = spots;
        }
        if(demand > res.max_observed_demand){
            res.max_observed_demand = demand;
        }
        // left over from debugging:
        /*
        if(tick<(24*4)){
            if(hour_progress==0){printf("%d\n", time_of_day);}
            printf("oc: %d, max: %d li: ",spots, res.max_observed); 
            size_t le = css[0].length;
            for(unsigned q=0;q<le; ++q){
                if(q==css[0].clock_zero){printf("%d< ", css[0].wait_times[q]);}
                else{printf("%d, ", css[0].wait_times[q]);}
            };
            printf("\n");
        }
        */
        // inc clock
        hour_progress++;
        if(hour_progress==ticks_per_hour){
            hour_progress = 0;
            time_of_day++;
            if(time_of_day==24){
                time_of_day = 0;
            }
        }
    }
    
    // close simulation
    for(unsigned i=0; i<categories; ++i){
        CSSC_free(&(css[i]));
    }

    // copy out results data from css
    res.total_charged = malloc(categories*sizeof(unsigned)); // in km for now
    res.parking_events = malloc(categories*sizeof(unsigned));
    for(unsigned i=0;i<categories;++i){
        res.total_charged[i] = css[i].total_charged;
        res.parking_events[i] = css[i].parking_events;
    }
    free(css);
    
    return res; 
}