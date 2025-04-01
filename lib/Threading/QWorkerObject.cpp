/*
 * QWorkerObject.cpp
 *
 *  Created on: 26 mars 2014
 *      Author: ebeuque
 */

#include "QWorkerObject.h"

QWorkerObject::QWorkerObject()
{
	m_bAutoFinish = true;
}

QWorkerObject::~QWorkerObject()
{

}

void QWorkerObject::setNoAutoFinish()
{
	m_bAutoFinish = false;
}

void QWorkerObject::doWork()
{
	run();
	if(m_bAutoFinish){
		notifyFinished();
	}
}

void QWorkerObject::notifyFinished()
{
	emit finished();
}
