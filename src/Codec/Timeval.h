/*
 * Timeval.h
 *
 *  Created on: 19 mai 2017
 *      Author: ebeuque
 */

#ifndef LIB_TOOLKIT_CORE_TIME_TIMEVAL_H_
#define LIB_TOOLKIT_CORE_TIME_TIMEVAL_H_

#include <cstdint>
#include <string>

class Timeval
{
public:
	int64_t sec;
	int64_t usec;

public:
	Timeval();
	Timeval(int64_t sec, int64_t usec);
	Timeval(const timeval& tvTime);
	Timeval(const Timeval& tvTime);
	virtual ~Timeval();

	bool isNull() const;
	void clear();

	void setTime(int64_t sec, int64_t usec);
	void setTimeFromMSec(int64_t msec);
	void setTimeFromUsec(int64_t usec);
	void setTimeFromText(const char* szTimeval);

	void setNow();
	static Timeval fromNow();

	Timeval& operator=(const timeval& tvTime);
	Timeval& operator=(const Timeval& tvTime);

	bool operator==(const Timeval& tvTime) const;
	bool operator!=(const Timeval& tvTime) const;
	bool operator<(const Timeval& tvTime) const;
	bool operator<=(const Timeval& tvTime) const;
	bool operator>(const Timeval& tvTime) const;
	bool operator>=(const Timeval& tvTime) const;

	Timeval operator+(const Timeval& tvTime) const;
	Timeval operator-(const Timeval& tvTime) const;

	void add(int64_t sec, int64_t usec);
	void add(const Timeval& tvTime);
	void add(const Timeval& tvTime1, const Timeval& tvTime2);
	void add(const Timeval& tvTime1, int64_t sec, int64_t usec);
	void addMs(int64_t msec);
	void sub(int64_t sec, int64_t usec);
	void sub(const Timeval& tvTime);
	void sub(const Timeval& tvTime1, const Timeval& tvTime2);
	static void sub(const Timeval& tvTime1, const Timeval& tvTime2, Timeval& tvResult);
	void subMs(int64_t msec);

	void mul(int m);
	void mul(const Timeval& tvTime, int m);
	void div(int d);
	void div(const Timeval& tvTime, int d);

	bool lt(int64_t sec, int64_t usec) const;
	bool lt(const Timeval& tvTime) const;
	bool le(int64_t sec, int64_t usec) const;
	bool le(const Timeval& tvTime) const;
	bool gt(int64_t sec, int64_t usec) const;
	bool gt(const Timeval& tvTime) const;
	bool ge(int64_t sec, int64_t usec) const;
	bool ge(const Timeval& tvTime) const;
	bool eq(int64_t sec, int64_t usec) const;
	bool eq(const Timeval& tvTime) const;

	void diff(const Timeval& tvTime1, const Timeval& tvTime2);

	int64_t diffMs(const Timeval& tvTime) const;
	static int64_t diffMs(const Timeval& tvTime1, const Timeval& tvTime2);

	static int checkForFreq(const Timeval& tvLast, const Timeval& tvCur, int64_t freq_ms);

	int64_t toMs() const;
	double toDbl() const;
	void toSysTimeval(timeval& tvTime) const;

	static Timeval fromString(const char* szText);
	static Timeval fromString(const std::string& szText);

	static Timeval fromISO8601(const std::string& szText);

	std::string toString() const;
	std::string toDateString(const char* szDateFormat, bool bWithUsec = true) const;

protected:
	bool bIsNull;
};

class TimevalNow : public Timeval
{
public:
	TimevalNow();
	virtual ~TimevalNow();
};

#endif /* LIB_TOOLKIT_CORE_TIME_TIMEVAL_H_ */
