/*
 * FrameMonitor.cpp
 *
 *  Created on: 8 juin 2016
 *      Author: ebeuque
 */

#include "FrameMonitor.h"

FrameMonitorFunc FrameMonitor::m_funcMonitor = NULL;


void FrameMonitor::setMonitorFunc(FrameMonitorFunc func)
{
	m_funcMonitor = func;
}

void FrameMonitor::monitor(int type, int count, int64_t size)
{
	if(m_funcMonitor){
		m_funcMonitor(type, count, size);
	}
}

