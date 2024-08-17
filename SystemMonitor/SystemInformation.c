#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/mach.h>
#include <mach/mach_host.h>

double get_cpu_usage(void) {
    // Getting CPU statistics using mach library by Apple
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    host_cpu_load_info_data_t cpu_load;
    
    static unsigned long long prev_user = 0, prev_system = 0, prev_idle = 0, prev_nice = 0;
    
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpu_load, &count) != KERN_SUCCESS) {
        fprintf(stderr, "Error getting CPU usage\n");
        return -1.0;
    }
    
    // CPU usage is divided among these 4 processes
    unsigned long long user = cpu_load.cpu_ticks[CPU_STATE_USER]; // Time spent executing user code
    unsigned long long system = cpu_load.cpu_ticks[CPU_STATE_SYSTEM]; // Time spent executing kernel code
    unsigned long long idle = cpu_load.cpu_ticks[CPU_STATE_IDLE]; // Time the CPU was idle
    unsigned long long nice = cpu_load.cpu_ticks[CPU_STATE_NICE]; // Time spent executing user code with low priority
    
    // Calculate total CPU time among all 4 processes
    unsigned long long total = (user - prev_user) + (system - prev_system) + (idle - prev_idle) + (nice - prev_nice);
    // Sum of total ticks spent on actual work (user and system tasks) / total
    double cpu_usage = ((user - prev_user) + (system - prev_system)) / (double)total * 100.0;
    
    // The usage is calculated by the (current - past). Set current to prev to calculate percentage in next round
    prev_user = user;
    prev_system = system;
    prev_idle = idle;
    prev_nice = nice;
    
    // Return usage as a percentage
    return cpu_usage;
}


double get_memory_usage(void) {
    
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics64_data_t vm_stats;

    mach_port = mach_host_self();

    // Calculate size of memory page in bytes
    if (host_page_size(mach_port, &page_size) != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get page size.\n");
        return -1;
    }

    // Gives information about the system's memory usage. Tells us how many pages of memory are in use, how many are free, etc.
    // Basically the virtual mem (vm) statistics
    count = HOST_VM_INFO64_COUNT;
    if (host_statistics64(mach_port, HOST_VM_INFO64, (host_info64_t)&vm_stats, &count) != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get VM statistics.\n");
        return -1;
    }

    // int64_t free_memory = vm_stats.free_count * page_size; // Memory that is currently not in use by anything
    int64_t active_memory = vm_stats.active_count * page_size; // Memory that is actively being used by applications or the system
    int64_t wired_memory = vm_stats.wire_count * page_size; // Used by OS or critical apps - memory that islocked in RAM
    int64_t used_memory = active_memory + wired_memory; // Amount of memory used by system + user tasks

    int64_t total_memory = (int64_t)vm_stats.wire_count * page_size +
                           (int64_t)vm_stats.active_count * page_size +
                           (int64_t)vm_stats.inactive_count * page_size +
                           (int64_t)vm_stats.free_count * page_size;

    // Get the percentage
    double memory_usage = ((double)used_memory / (double)total_memory) * 100.0;

    return memory_usage;
}

double get_disk_usage(void) {
    char line[1024]; // Larger buffer to safely hold the output line
    FILE *fp = popen("df -H / | awk 'NR==2 {print $3, $2}'", "r"); // Directly extract used and total space
    if (fp == NULL) {
        printf("Failed to run command\n");
        return -1;
    }

    double usedGB = 0.0, totalGB = 0.0;
    if (fgets(line, sizeof(line), fp) != NULL) {
        // Parse the output to get used and total space
        sscanf(line, "%lf %lf", &usedGB, &totalGB);
    }

    pclose(fp);

    // Calculate and return the used disk space percentage
    if (totalGB == 0) {
        return 0; // To avoid division by zero
    }
    return (usedGB / totalGB) * 100;
}
