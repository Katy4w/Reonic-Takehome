#include <stdio.h>

#define INC_MOD(i, n)   i++;\
                        if(i==n){i=0;}
#define DEC_MOD(i, n)   if(i==0){i=n;}\
                        i--;
#define ADD_MOD(i, k, n)    i += k;\
                            if(i>=n){i-=n;}
#define SUB_MOD(i, k, n)    if(i<k){i+=n;}\
                            i-=k;
#define CEIL_DIV(a,b) (a+b-1)/b // notably returns 1 when a is 0

#define max_need            300 // 1km
#define energy_efficiency   18/100 // kWh_per_1km
#define charging_speed      11 // kW
#define ticks_per_hour      4
#define simulation_duration 365*24 // h
#define total_ticks ticks_per_hour*simulation_duration

#define ticks_till_charged(km) CEIL_DIV((km * energy_efficiency * ticks_per_hour), charging_speed)

typedef struct charging_station_speed_category
{
    unsigned avbl_count;
    unsigned* wait_times;
    unsigned speed_kW;
    size_t length;
    size_t clock_zero;
} CSSC; // ==charging station speed category

double pr_arrivals[24]; 
double pr_charging_needs[2][];

void CSSC_init(CSSC* cs){
    size_t max_ticks = ticks_till_charged(max_need); 
    cs->length = max_ticks;
    cs->wait_times = (unsigned*)(max_ticks * sizeof(unsigned));
}
void CSSC_tick(CSSC* cs){
    INC_MOD(cs->clock_zero, cs->length);

    // queue out cars that finish charging
    cs->avbl_count += cs->wait_times[cs->clock_zero];
    cs->wait_times[cs->clock_zero] = 0;
}
void CSSC_add_cars(CSSC* cs, unsigned wait_time, unsigned count){
    unsigned t = ticks_till_charged(wait_time);
    size_t i = ADD_MOD(cs->clock_zero, t, cs->length);
    cs->wait_times[i] += count;
}
unsigned CSSC_spawn_cars(CSSC* cs); // return to use in counting total number of parkings

int main(int argc, char *argv[])
{
    int i = 5;
    int n = 10;
    ADD_MOD(i,7,n);
    //printf("%d\n", i);
    SUB_MOD(i,11,n);
    //printf("%d\n", i);

    // do i remember how to do arrays?
    int arr[5] = {0,1,2,3,4};
    //printf("%d\n", arr[4]);

    unsigned z = 0;
    printf("%d\n", ticks_till_charged(max_need));

    printf("argc = %d\n", argc);
    for (int ndx = 0; ndx != argc; ++ndx)
        printf("argv[%d] --> %s\n", ndx, argv[ndx]);
    printf("argv[argc] = %p\n", (void*)argv[argc]);
}