//
//  SystemInformation.c
//  SystemMonitor
//
//  Created by Eshaan Kansagara on 8/11/24.
//

#include "SystemInformation.h"
#include <mach/mach.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>

// Function to get CPU usage
double get_cpu_usage(void) {
    host_cpu_load_info_data_t cpu_info;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    
    kern_return_t kr = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpu_info, &count);
    if (kr != KERN_SUCCESS) {
        return -1.0;
    }
    
    uint32_t user = cpu_info.cpu_ticks[CPU_STATE_USER];
    uint32_t system = cpu_info.cpu_ticks[CPU_STATE_SYSTEM];
    uint32_t idle = cpu_info.cpu_ticks[CPU_STATE_IDLE];
    
    uint32_t total = user + system + idle;
    
    return ((double)(user + system) / (double)total) * 100.0;
}

// Function to get RAM usage
void get_ram_usage(double *used_memory, double *total_memory) {
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics64_data_t vm_stats;
    
    mach_port = mach_host_self();
    count = sizeof(vm_stats) / sizeof(natural_t);
    if (host_page_size(mach_port, &page_size) != KERN_SUCCESS) {
        *used_memory = -1;
        *total_memory = -1;
        return;
    }
    
    if (host_statistics64(mach_port, HOST_VM_INFO64, (host_info64_t)&vm_stats, &count) != KERN_SUCCESS) {
        *used_memory = -1;
        *total_memory = -1;
        return;
    }
    
    *used_memory = (double)(vm_stats.active_count + vm_stats.inactive_count + vm_stats.wire_count) * (double)page_size / (1024 * 1024 * 1024);
    *total_memory = *used_memory + (double)vm_stats.free_count * (double)page_size / (1024 * 1024 * 1024);
}

// Function to print CPU and RAM usage
void print_system_usage(void) {
    double cpu_usage = get_cpu_usage();
    double used_memory, total_memory;
    
    get_ram_usage(&used_memory, &total_memory);
    
    printf("CPU Usage: %.2f%%\n", cpu_usage);
    printf("Used RAM: %.2f GB\n", used_memory);
    printf("Total RAM: %.2f GB\n", total_memory);
}
