/*
 * QWorkerObjectWithStatus.h
 *
 *  Created on: 26 mars 2014
 *      Author: ebeuque
 */

#ifndef QWORKEROBJECTWITHSTATUS_H_
#define QWORKEROBJECTWITHSTATUS_H_

#include "QWorkerObject.h"

/*
 *
 */
class QWorkerObjectWithStatus : public QWorkerObject {
public:
	QWorkerObjectWithStatus();
	virtual ~QWorkerObjectWithStatus();

	bool status();
	void setStatus(bool bStatus);

private:
	bool m_bStatus;
};

#endif /* QWORKEROBJECTWITHSTATUS_H_ */
