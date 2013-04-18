//
//  CPUMonitor.h
//  CPULoad
//
//  Created by Fiore Basile on 17/04/13.
//
//

#ifndef __CPULoad__CPUMonitor__
#define __CPULoad__CPUMonitor__

#include <iostream>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>


class CPUMonitor {
    float m_lastLoad;
    //Additional members
    processor_info_array_t m_cpuInfo, m_prevCpuInfo;
    mach_msg_type_number_t m_numCpuInfo, m_numPrevCpuInfo;
    unsigned m_numCPUs;
public:
    CPUMonitor();
    virtual ~CPUMonitor();
    float getLoad();
    void poll();
};



#endif /* defined(__CPULoad__CPUMonitor__) */
