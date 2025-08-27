//
// Created by ebeuque on 27/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H26XERRORS_H
#define TOOLKIT_CODEC_UTILS_H26XERRORS_H

#include <string>
#include <iterator>

#include "H26XVector.h"

class H26XError
{
public:
	enum Level {
		Unknown,
		Minor,
		Major
	};

	H26XError();
	H26XError(const H26XError& other);
	H26XError(H26XError::Level iLevel, const std::string& szErrMsg);
	virtual ~H26XError();

	H26XError::Level level() const;
	const std::string& message() const;

private:
	H26XError::Level m_iLevel;
	std::string m_szErrMsg;
};

class H26XErrors
{
public:
	typedef typename H26XVector<H26XError>::iterator iterator;
	typedef typename H26XVector<H26XError>::const_iterator const_iterator;

public:
	H26XErrors();
	virtual ~H26XErrors();

	void add(const H26XError& error);
	void add(H26XError::Level iLevel, const std::string& szErrMsg);
	void add(const H26XErrors& errors);

	bool empty() const;
	int size() const;

	void dequeue();

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	void clear();
	void clear(int iKeptCount);

	bool hasMajorErrors() const;
	bool hasMinorErrors() const;

private:
	H26XVector<H26XError> m_listErrors;
};


#endif //OPENMEDIAANALYZER_H26XERRORS_H
