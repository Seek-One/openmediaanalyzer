/*
 * FrameMonitor.h
 *
 *  Created on: 8 juin 2016
 *      Author: ebeuque
 */

#ifndef LIB_TOOLKIT_CODEC_FRAMEMONITOR_H_
#define LIB_TOOLKIT_CODEC_FRAMEMONITOR_H_

#include <stdlib.h>

#include <cstring>

typedef void (*FrameMonitorFunc)(int type, int count, int64_t size);


class FrameMonitor
{
public:
	static void setMonitorFunc(FrameMonitorFunc func);

	static void monitor(int type, int count, int64_t size);

private:
	static FrameMonitorFunc m_funcMonitor;
};

#endif /* LIB_TOOLKIT_CODEC_FRAMEMONITOR_H_ */
