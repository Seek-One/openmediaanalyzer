/*
 * QWorkerObject.h
 *
 *  Created on: 26 mars 2014
 *      Author: ebeuque
 */

#ifndef QWORKEROBJECT_H_
#define QWORKEROBJECT_H_

#include <QObject>

/*
 *
 */
class QWorkerObject : public QObject
{
	Q_OBJECT
public:
	QWorkerObject();
	virtual ~QWorkerObject();

public:
	void setNoAutoFinish();

public slots:
	void doWork();

signals:
 	void finished();

protected:
	virtual void run() = 0;

	void notifyFinished();

private:
	bool m_bAutoFinish;
};

#endif /* QWORKEROBJECT_H_ */
