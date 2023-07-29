
#pragma once


// NVIC_SetPriority expects priority encoded according to priority grouping
// We allocate zero bits for sub-priority, therefore we have 16 priorities to use on STM32

// can't use 0
#define NVIC_PRIO_MAX                       1
#define NVIC_PRIO_I2C_ER                    2
#define NVIC_PRIO_I2C_EV                    2
#define NVIC_PRIO_TIMER                     3
#define NVIC_PRIO_TIMER_DMA                 3
#define NVIC_PRIO_SDIO                      3
#define NVIC_PRIO_USB                       5
#define NVIC_PRIO_SERIALUART                5
#define NVIC_PRIO_VCP                       7


#if defined(AT32F43x)
    #define NVIC_PRIORITY_GROUPING NVIC_PRIORITY_GROUP_4
#else //STM32
    // Use all available bits for priority and zero bits to sub-priority
    #ifdef USE_HAL_DRIVER
        #define NVIC_PRIORITY_GROUPING NVIC_PRIORITYGROUP_4
    #else
        #define NVIC_PRIORITY_GROUPING NVIC_PriorityGroup_4
    #endif
#endif
