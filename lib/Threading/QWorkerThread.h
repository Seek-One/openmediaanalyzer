/*
 * QWorkerThread.h
 *
 *  Created on: 26 mars 2014
 *      Author: ebeuque
 */

#ifndef QWORKERTHREAD_H_
#define QWORKERTHREAD_H_

#include <QThread>

class QWorkerObject;
class QEventLoop;


class QWorkerThread : public QThread
{
	Q_OBJECT
public:
	QWorkerThread(QWorkerObject* pWorker);
	virtual ~QWorkerThread();

	QWorkerObject* getWorker();
	bool isWorkerFinished() const;

public:
	static void execWorker(QWorkerObject* pWorker, bool bLockGUI = true);

private slots:
	void quitThread();

private:
	QWorkerObject* m_pWorker;
	bool m_bWorkerFinished;
};

#endif /* QWORKERTHREAD_H_ */
