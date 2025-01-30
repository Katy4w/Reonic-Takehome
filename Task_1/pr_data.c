#include<stdlib.h>
#include "pr_data.h"
double pr_arrivals[24] = {
0.94, 
0.94, 
0.94, 
0.94, 
0.94, 
0.94, 
0.94, 
0.94, 
2.83, 
2.83, 
5.66, 
5.66, 
5.66, 
7.55, 
7.55, 
7.55, 
10.38, 
10.38, 
10.38, 
4.72, 
4.72, 
4.72, 
0.94, 
0.94};
double pr_charging_needs[9] = {
34.31, 
4.90, 
9.80, 
11.76, 
8.82, 
11.76, 
10.78, 
4.90, 
2.94};
unsigned val_charging_needs[9] = {
0, 
5, 
10, 
20, 
30, 
50, 
100, 
200, 
300};
size_t len_charging_needs = 9;
