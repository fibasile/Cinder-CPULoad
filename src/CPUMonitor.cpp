//
//  CPUMonitor.cpp
//  CPULoad
//
//  Created by Fiore Basile on 17/04/13.
//
//
#include "CPUMonitor.h"
#include <stdio.h>


using namespace std;

CPUMonitor::CPUMonitor() :
m_lastLoad(0),m_prevCpuInfo(NULL),m_numPrevCpuInfo(0U),m_cpuInfo(NULL),m_numCpuInfo(0U),m_numCPUs(0)
{
    int mib[2U] = { CTL_HW, HW_NCPU };
    size_t sizeOfNumCPUs = sizeof(m_numCPUs);
    int status = sysctl(mib, 2U, &m_numCPUs, &sizeOfNumCPUs, NULL, 0U);
    if (status)
        m_numCPUs = 1;
    poll();

}

float CPUMonitor::getLoad(){
    return m_lastLoad;
}

void CPUMonitor::poll() {
    natural_t numCPUsU = 0U;
    float average = 0;
    kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numCPUsU, &m_cpuInfo, &m_numCpuInfo);
    if(err == KERN_SUCCESS) {
        for(unsigned i = 0U; i < m_numCPUs; ++i) {
            float inUse=0, total=0;
            if(m_prevCpuInfo) {
                inUse = (
                         (m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER]   - m_prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER])
                         + (m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM] - m_prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM])
                         + (m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE]   - m_prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE])
                         );
                total = inUse + (m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE] - m_prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE]);
            } else {
                inUse = m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER] + m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM] + m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE];
                total = inUse + m_cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE];
            }
            
            
//            cout << "Core: " << i << " Usage " << inUse/total << "\n";
            
            average += inUse/total;
            
            
        }
        m_lastLoad = average / m_numCPUs;
        
        if(m_prevCpuInfo) {
            size_t prevCpuInfoSize = sizeof(integer_t) * m_numPrevCpuInfo;
            vm_deallocate(mach_task_self(), (vm_address_t)m_prevCpuInfo, prevCpuInfoSize);
        }
        
        m_prevCpuInfo = m_cpuInfo;
        m_numPrevCpuInfo = m_numCpuInfo;
        
        m_cpuInfo = NULL;
        m_numCpuInfo = 0U;
        
        
    } else {
        // error
        cout << "Error reading cpu load";
        m_lastLoad = 0;
    }
}

CPUMonitor::~CPUMonitor() {
    if(m_prevCpuInfo) {
        size_t prevCpuInfoSize = sizeof(integer_t) * m_numPrevCpuInfo;
        vm_deallocate(mach_task_self(), (vm_address_t)m_prevCpuInfo, prevCpuInfoSize);
    }

}


