/*
 * QWorkerObjectWithStatus.cpp
 *
 *  Created on: 26 mars 2014
 *      Author: ebeuque
 */

#include "QWorkerObjectWithStatus.h"

QWorkerObjectWithStatus::QWorkerObjectWithStatus()
{
	m_bStatus = true;
}

QWorkerObjectWithStatus::~QWorkerObjectWithStatus()
{

}

bool QWorkerObjectWithStatus::status()
{
	return m_bStatus;
}

void QWorkerObjectWithStatus::setStatus(bool bStatus)
{
	m_bStatus = bStatus;
}
