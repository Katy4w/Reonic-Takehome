// Optimization ideas: 
// - calculate the ticks_till_charged of pr_charging_needs (once for each charging station speed used) ahead of time
// - calculate probability spaces for amount of cars spawned ahead of time

#include <stdio.h>
#include <stdlib.h>

// arithmatic for looping array indices. n == length
#define ADD_MOD(i, k, n)    i += k;\
                            if(i>=n){i-=n;}
#define SUB_MOD(i, k, n)    if(i<k){i+=n;}\
                            i-=k;

// inputs
#define max_need                300u // 1km
#define energy_efficiency       18u // kWh_per_100km
#define default_charging_speed  11u // kW
#define ticks_per_hour          4u
#define simulation_duration     (365u*24u) // h
#define total_ticks (ticks_per_hour*simulation_duration)

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

typedef struct charging_station_speed_category
{
    unsigned avbl_count; // unused parking spots
    unsigned charge_speed; // speed of charging ports
    // wait_times[i] represents the number of cars that still need ((i-clock_zero) mod length) ticks to charge.
    unsigned* wait_times; 
    size_t length; // of wait_times
    size_t clock_zero; // scans left to right through wait_times

} CSSC; // ==charging station speed category

typedef struct result
{
    unsigned total_charged;
    unsigned max_observed; 
    unsigned parking_events;
} Result;

#include "pr_data.c"

void CSSC_init(CSSC* cs, unsigned charge_speed, unsigned parking_spots){
    size_t max_ticks = ticks_till_charged(max_need, charge_speed); 
    cs->length = max_ticks;
    cs->charge_speed = charge_speed;
    cs->wait_times = (unsigned*)malloc(max_ticks * sizeof(unsigned));
    cs->avbl_count = parking_spots;
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
void CSSC_roll_for_arrivals(CSSC* cs, double* pr_arrival, unsigned time_of_day, Result* res){
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
    res->total_charged += total_cn;
    res->parking_events += new_cars;
}
void CSSC_free(CSSC* cs){
    free(cs->wait_times);
}

Result run_simulation(unsigned* speeds, unsigned* amounts, size_t categories){
    
    // seed random values
    srand(2001);

    // return values
    Result res;
    res.total_charged = 0; // in km for now
    res.max_observed = 0; 

    unsigned total_spots = 0; 

    //init clock
    unsigned time_of_day = 0;
    unsigned hour_progress = 0;
    
    //init parking
    CSSC* css; // only contains one element when charging speeds are the same
    css = (CSSC*)malloc(categories*sizeof(CSSC)); 

    for(unsigned i=0; i<categories; ++i){
        
        CSSC_init(&css[i], speeds[i], amounts[i]);
        total_spots += amounts[i];
    }
    
    // run simulation
    for(unsigned tick=0; tick<total_ticks; ++tick){
        unsigned spots = 0; // number of free spots

        for(unsigned i=0; i<categories; ++i){
            CSSC_roll_for_arrivals(&css[i], pr_arrivals, time_of_day, &res);
            CSSC_tick(&css[i]);
            spots += css[i].avbl_count;
        }
        spots = total_spots - spots; // now number of used spots
        if(spots > res.max_observed){
            res.max_observed = spots;
        }
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
    
    free(css);
    
    return res; 
}

Result run_with_default_params(unsigned parking_spots) {
    unsigned* speeds = malloc(sizeof(unsigned));
    unsigned* amounts = malloc(sizeof(unsigned));
    speeds[0] = default_charging_speed;
    amounts[0] = parking_spots;
    Result res = run_simulation(speeds, amounts, 1);
    free(speeds);
    free(amounts);
    return res;
}
unsigned max_dem_ob(Result* res, unsigned charging_speed){
    return res->max_observed*charging_speed;
}
unsigned max_dem_theory(Result* res, unsigned parking_spots, unsigned charging_speed){
    return parking_spots*charging_speed;
}
double concurrency_factor(Result* res, unsigned parking_spots){
    return (double)res->max_observed/(double)parking_spots;
}

int main(int argc, char *argv[])
{
    
    Result res = run_with_default_params(20);
    printf("result:! \n");
    printf("  maximum occupied at once: %d\n", res.max_observed);
    unsigned max_dem_obs = res.max_observed*default_charging_speed;
    printf("  maximum power demand: %d (should be between 77-121)\n", max_dem_obs);
    unsigned max_dem_theory = 20*default_charging_speed;
    printf("  theoretical maximum power demand: %d\n", max_dem_theory);
    printf("  concurrency factor: %f (should be between 35-55percent)\n", (double)max_dem_obs/(double)max_dem_theory);
    printf("  total charged: %d\n", res.total_charged);
    double total_consumed = res.total_charged*(double)energy_efficiency/100.0;
    printf("  total power consumed kWh: %f\n", total_consumed);
    printf("  parkings: %d\n",      res.parking_events);

    double expct_cn = 0; // Erwartungswert charging needs
    for(unsigned i; i<len_charging_needs; ++i){
        expct_cn += pr_charging_needs[i]*(double)val_charging_needs[i]/100.0;
    }
    printf("  expected charging needs: %f\n  ^^^ times parkings: %f\n", expct_cn, expct_cn*res.parking_events);
    double hours_spent_charging = res.total_charged*(double)energy_efficiency/100.0/default_charging_speed;
    double occ = hours_spent_charging/(20*simulation_duration);
    printf("  factor of time occupied: %f\n", occ);


    // between 1 and 30 chargepoints
    printf("Different concurrency factors:\n");
    for(unsigned i=1; i<31; ++i){
        Result r = run_with_default_params(i);
        printf("  %d: %f\n", i, concurrency_factor(&r, i));
    }


    //for(int q = 0; q<9;++q){printf("%d : %d\n", val_charging_needs[q], ticks_till_charged(val_charging_needs[q], 11));}
    //for(int j = 0; j<50;++j){printf("result:! %d\n", roll_charging_needs());}

    double* prs = pr_charging_needs;
    for(int i=0;i<9;++i){
        //printf("result:! %f\n", prs[i]);
    }

    

    return 0;
}