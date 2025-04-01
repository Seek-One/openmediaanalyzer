/*
 * QWorkerThread.cpp
 *
 *  Created on: 26 mars 2014
 *      Author: ebeuque
 */

#include <QEventLoop>

#include "QWorkerObject.h"
#include "QWorkerThread.h"

QWorkerThread::QWorkerThread(QWorkerObject* pWorker)
{
	m_bWorkerFinished = false;
	m_pWorker = pWorker;

	m_pWorker->moveToThread(this);
	connect(this, SIGNAL(started()), pWorker, SLOT(doWork()));
	connect(pWorker, SIGNAL(finished()), this, SLOT(quitThread()));
}

QWorkerThread::~QWorkerThread()
{

}

QWorkerObject* QWorkerThread::getWorker()
{
	return m_pWorker;
}

void QWorkerThread::quitThread()
{
	m_bWorkerFinished = true;
	quit();
}

void QWorkerThread::execWorker(QWorkerObject* pWorker, bool bLockGUI)
{
	QEventLoop eventLoop;
	QWorkerThread thread(pWorker);
	connect(&thread, SIGNAL(finished()), &eventLoop, SLOT(quit()));

	thread.start();

	if(bLockGUI){
		eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
	}else{
		eventLoop.exec();
	}

	if(!thread.m_bWorkerFinished){
		// Event loop may be stopped by exit event, so we manually stop the thread
		thread.exit(-1);
	}

	thread.wait();
}
