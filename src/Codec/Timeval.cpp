/*
 * Timeval.cpp
 *
 *  Created on: 19 mai 2017
 *      Author: ebeuque
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctime>

#include <cstring>
#include <sys/time.h>
#include "Timeval.h"

#ifdef WIN32
#define timegm _mkgmtime
#endif

Timeval::Timeval()
{
	bIsNull = true;
	sec = 0;
	usec = 0;
}

Timeval::Timeval(int64_t _sec, int64_t _usec)
{
	bIsNull = false;
	sec = _sec;
	usec = _usec;
}

Timeval::Timeval(const timeval& tvTime)
{
	bIsNull = false;
	sec = tvTime.tv_sec;
	usec = tvTime.tv_usec;
}

Timeval::Timeval(const Timeval& tvTime)
{
	bIsNull = tvTime.bIsNull;
	sec = tvTime.sec;
	usec = tvTime.usec;
}

Timeval::~Timeval()
{

}

bool Timeval::isNull() const
{
	return bIsNull;
}

void Timeval::clear()
{
	bIsNull = true;
	sec = 0;
	usec = 0;
}

void Timeval::setTime(int64_t _sec, int64_t _usec)
{
	bIsNull = false;
	sec = _sec;
	usec = _usec;
}

void Timeval::setTimeFromMSec(int64_t _msec)
{
	bIsNull = false;
	sec = _msec/1000;
	usec = _msec%1000;
}

void Timeval::setTimeFromUsec(int64_t _usec)
{
	bIsNull = false;
	sec = _usec/1000000;
	usec = _usec%1000000;
}

void Timeval::setTimeFromText(const char* szTimeval)
{
	if(!szTimeval){
		return;
	}
    int iRes;
    long lTmp1 = 0;
    long lTmp2 = 0;
	iRes = sscanf(szTimeval, "%ld,%06ld", &lTmp1, &lTmp2);
	if(iRes == 2 || iRes == 1){
		bIsNull = false;
		sec = lTmp1;
		usec = lTmp2;
	}
}

void Timeval::setNow()
{
	timeval tvNow;
	gettimeofday(&tvNow, NULL);
	bIsNull = false;
	sec = tvNow.tv_sec;
	usec = tvNow.tv_usec;
}

Timeval Timeval::fromNow()
{
	timeval tvNow;
	gettimeofday(&tvNow, NULL);
	return Timeval(tvNow.tv_sec, tvNow.tv_usec);
}

Timeval& Timeval::operator=(const timeval& tvTime)
{
	bIsNull = false;
	sec = tvTime.tv_sec;
	usec = tvTime.tv_usec;
	return *this;
}

Timeval& Timeval::operator=(const Timeval& tvTime)
{
	bIsNull = tvTime.bIsNull;
	sec = tvTime.sec;
	usec = tvTime.usec;
	return *this;
}

bool Timeval::operator==(const Timeval& tvTime) const
{
	return (bIsNull == tvTime.bIsNull) && (sec == tvTime.sec) && (usec == tvTime.usec);
}

bool Timeval::operator!=(const Timeval& tvTime) const
{
	return (bIsNull != tvTime.bIsNull) || (sec != tvTime.sec) || (usec != tvTime.usec);
}

bool Timeval::operator<(const Timeval& tvTime) const
{
	if(bIsNull){
		return !tvTime.bIsNull;
	}else if(tvTime.bIsNull){
		return false;
	}else{
		if(sec < tvTime.sec){
			return true;
		}else if(sec == tvTime.sec){
			return usec < tvTime.usec;
		}
	}
	return false;
}

bool Timeval::operator<=(const Timeval& tvTime) const
{
	if(bIsNull){
		return true;
	}else if(tvTime.bIsNull){
		return false;
	}else{
		if(sec < tvTime.sec){
			return true;
		}else if(sec == tvTime.sec){
			return usec <= tvTime.usec;
		}
	}
	return false;
}

bool Timeval::operator>(const Timeval& tvTime) const
{
	if(bIsNull){
		return false;
	}else if(tvTime.bIsNull){
		return !bIsNull;;
	}else{
		if(sec > tvTime.sec){
			return true;
		}else if(sec == tvTime.sec){
			return usec > tvTime.usec;
		}
	}
	return false;
}

bool Timeval::operator>=(const Timeval& tvTime) const
{
	if(bIsNull){
		return tvTime.bIsNull;
	}else if(tvTime.bIsNull){
		return true;
	}else{
		if(sec > tvTime.sec){
			return true;
		}else if(sec == tvTime.sec){
			return usec >= tvTime.usec;
		}
	}
	return false;
}

Timeval Timeval::operator+(const Timeval& tvTime) const
{
	Timeval tvResult = *this;
	tvResult.add(tvTime);
	return tvResult;
}

Timeval Timeval::operator-(const Timeval& tvTime) const
{
	Timeval tvResult = *this;
	tvResult.sub(tvTime);
	return tvResult;
}

void Timeval::add(int64_t _sec, int64_t _usec)
{
	int64_t rsec;
	int64_t rusec;

	rsec = sec + _sec;
	rusec = usec + _usec;
	if (rusec >= 1000000){
		rsec++;
		rusec -= 1000000;
	}

	sec = rsec;
	usec = rusec;
}

void Timeval::add(const Timeval& tvTime)
{
	add(tvTime.sec, tvTime.usec);
}

void Timeval::add(const Timeval& tvTime1, const Timeval& tvTime2)
{
	*this = tvTime1;
	add(tvTime2);
}

void Timeval::add(const Timeval& tvTime1, int64_t sec, int64_t usec)
{
	*this = tvTime1;
	add(sec, usec);
}

void Timeval::addMs(int64_t msec)
{
	int64_t sec = msec / 1000;
	int64_t usec = (msec % 1000) * 1000;
	add(sec, usec);
}

void Timeval::sub(int64_t _sec, int64_t _usec)
{
	int64_t rsec;
	int64_t rusec;

	rsec = sec - _sec;
	rusec = usec - _usec;
	if (rusec < 0){
		rsec--;
		rusec += 1000000;
	}

	sec = rsec;
	usec = rusec;
}

void Timeval::sub(const Timeval& tvTime)
{
	sub(tvTime.sec, tvTime.usec);
}

void Timeval::sub(const Timeval& tvTime1, const Timeval& tvTime2)
{
	*this = tvTime1;
	sub(tvTime2);
}

void Timeval::sub(const Timeval& tvTime1, const Timeval& tvTime2, Timeval& tvResult)
{
	tvResult = tvTime1;
	tvResult.sub(tvTime2);
}

void Timeval::subMs(int64_t msec)
{
	int64_t sec = msec / 1000;
	int64_t usec = (msec % 1000) * 1000;
	sub(sec, usec);
}

void Timeval::mul(int m)
{
	mul(*this, m);
}

void Timeval::mul(const Timeval& tvTime, int m)
{
	int64_t u = tvTime.usec * m;
	sec = tvTime.sec * m + u / 1000000;
	usec = u % 1000000;
	bIsNull = false;
}

void Timeval::div(int d)
{
	div(*this, d);
}

void Timeval::div(const Timeval& tvTime, int d)
{
	int64_t s = tvTime.sec;
	sec = s / d;
	usec = (tvTime.usec + 1000000 * ( s % d ) ) / d;
	bIsNull = false;
}

bool Timeval::lt(int64_t _sec, int64_t _usec) const
{
	if(bIsNull){
		return true;
	}else{
		if(sec < _sec){
			return true;
		}else if(sec == _sec){
			return usec < _usec;
		}
	}
	return false;
}

bool Timeval::lt(const Timeval& tvTime) const
{
	return lt(tvTime.sec, tvTime.usec);
}

bool Timeval::le(int64_t _sec, int64_t _usec) const
{
	if(bIsNull){
		return true;
	}else{
		if(sec < _sec){
			return true;
		}else if(sec == _sec){
			return usec <= _usec;
		}
	}
	return false;
}

bool Timeval::le(const Timeval& tvTime) const
{
	return le(tvTime.sec, tvTime.usec);
}

bool Timeval::gt(int64_t _sec, int64_t _usec) const
{
	if(bIsNull){
		return false;
	}else{
		if(sec > _sec){
			return true;
		}else if(sec == _sec){
			return usec > _usec;
		}
	}
	return false;
}

bool Timeval::gt(const Timeval& tvTime) const
{
	return gt(tvTime.sec, tvTime.usec);
}

bool Timeval::ge(int64_t _sec, int64_t _usec) const
{
	if(bIsNull){
		return false;
	}else{
		if(sec > _sec){
			return true;
		}else if(sec == _sec){
			return usec >= _usec;
		}
	}
	return false;
}

bool Timeval::ge(const Timeval& tvTime) const
{
	return ge(tvTime.sec, tvTime.usec);
}

bool Timeval::eq(int64_t _sec, int64_t _usec) const
{
	if(bIsNull){
		return false;
	}
	return (sec == _sec) && (usec == _usec);
}

bool Timeval::eq(const Timeval& tvTime) const
{
	return eq(tvTime.sec, tvTime.usec);
}

void Timeval::diff(const Timeval& tvTime1, const Timeval& tvTime2)
{
	sub(tvTime1, tvTime2, *this);
}

int64_t Timeval::diffMs(const Timeval& tvTime) const
{
	return diffMs(*this, tvTime);
}

int64_t Timeval::diffMs(const Timeval& tvTime1, const Timeval& tvTime2)
{
	Timeval tvResult = tvTime1;
	tvResult.sub(tvTime2);
	return tvResult.toMs();
}

int Timeval::checkForFreq(const Timeval& tvLast, const Timeval& tvCur, int64_t freq_ms)
{
	// Timer set
	Timeval tvFreq;
	Timeval tvTmp;
	tvFreq.setTime(freq_ms/1000, freq_ms%1000*1000);

	tvTmp.add(tvLast, tvFreq);

	// Check time is passed
	if(tvTmp <= tvCur){
		return 1;
	}
	// Check if the next frame will match
	tvTmp.add(tvFreq);
	if(tvTmp <= tvCur){
		return 1;
	}

	return 0;
}

int64_t Timeval::toMs() const
{
	return (sec*1000) + (usec/1000);
}

double Timeval::toDbl() const
{
	return ((double)sec)+(((double)usec)*0.000001);
}

void Timeval::toSysTimeval(timeval& tvTime) const
{
#ifdef MSVC
	tvTime.tv_sec = (long)sec;
	tvTime.tv_usec = (long)usec;
#else
	tvTime.tv_sec = sec;
	tvTime.tv_usec = usec;
#endif
}

Timeval Timeval::fromString(const char* szText)
{
	const char* szSep = strchr(szText, ',');

	int64_t iSec = 0;
	int64_t iUsec = 0;

	if(szSep){
		iSec = strtoul(szText, NULL, 10);
		szSep++;
		iUsec = strtoul(szSep, NULL, 10);
	}else{
		iSec = strtoul(szText, NULL, 10);
	}

	return Timeval(iSec, iUsec);
}

Timeval Timeval::fromString(const std::string& szText)
{
	return fromString(szText.c_str());
}

std::string Timeval::toString() const
{
	if(bIsNull){
		return std::string();
	}
	char buffer[50];
	std::snprintf(buffer, sizeof(buffer), "%ld,%06ld", (long)sec, (long)usec);
	return std::string(buffer);
}

Timeval Timeval::fromISO8601(const std::string& szText)
{
	size_t iLen = szText.length();
	if (iLen < 20){
		return Timeval();
	}
	const char* szInput = szText.c_str();

	size_t iPos = 0;

	std::tm time = { 0 };
	time.tm_year = atoi(&szInput[0]) - 1900;
	time.tm_mon = atoi(&szInput[5]) - 1;
	time.tm_mday = atoi(&szInput[8]);
	time.tm_hour = atoi(&szInput[11]);
	time.tm_min = atoi(&szInput[14]);
	time.tm_sec = atoi(&szInput[17]);
	time.tm_isdst = 0;
	iPos += 19;

	int millis = 0;
	if((iLen-iPos)>0 && szInput[iPos] == '.'){
		millis = atoi(&szInput[iPos+1]);
		iPos += 3;
	}

	if((iLen-iPos)>0 && szInput[iPos] == 'Z'){

	}

	int tzHours = 0;
	int tzMinutes = 0;
	if((iLen-iPos)>0 && szInput[iPos] == '+'){
		tzHours -= atoi(&szInput[iPos+1]);
		iPos += 3;
		tzMinutes -= atoi(&szInput[iPos+1]);
		iPos += 2;
	}
	if((iLen-iPos)>0 && szInput[iPos] == '-'){
		tzHours += atoi(&szInput[iPos+1]);
		iPos += 3;
		tzMinutes += atoi(&szInput[iPos+1]);
		iPos += 2;
	}

	int64_t iTimeSec = timegm(&time);
	iTimeSec += (tzHours*3600);
	iTimeSec += (tzMinutes*60);

	return Timeval(iTimeSec, millis*1000);
}

std::string Timeval::toDateString(const char* szDateFormat, bool bWithUsec) const
{
	size_t iBufSize = strlen(szDateFormat) + 64; // Add a buffer large enough
	char* szBuf = (char*)malloc(iBufSize);
	if(bWithUsec){
		timeval tvTmp;
		toSysTimeval(tvTmp);
		char tmbuf[64];
		struct tm time_tm;
		time_t tmpTime = tvTmp.tv_sec;
		memcpy(&time_tm, localtime(&tmpTime), sizeof(struct tm));
		strftime(tmbuf, sizeof(tmbuf), szDateFormat, &time_tm);
		snprintf(szBuf, iBufSize, "%s,%06ld", tmbuf, (long)tvTmp.tv_usec);
	}else{
		struct tm time_tm;
		memcpy(&time_tm, localtime(&sec), sizeof(struct tm));
		strftime(szBuf, iBufSize, szDateFormat, &time_tm);
	}
	return std::string(szBuf);
}

TimevalNow::TimevalNow()
{
	timeval tvNow;
	gettimeofday(&tvNow, NULL);
	bIsNull = false;
	sec = tvNow.tv_sec;
	usec = tvNow.tv_usec;
}

TimevalNow::~TimevalNow()
{

}
