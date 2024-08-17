//
//  SystemInformation.h
//  SystemMonitor
//
//  Created by Eshaan Kansagara on 8/11/24.
//

#ifndef SystemInformation_h
#define SystemInformation_h

#include <stdio.h>
#include <stdint.h>

// Function declarations
double get_cpu_usage(void);
void get_ram_usage(double *used_memory, double *total_memory);
void print_system_usage(void);



#endif /* SystemInformation_h */
