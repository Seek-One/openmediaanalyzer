//
// Created by ebeuque on 27/08/25.
//

#include "H26XErrors.h"

H26XError::H26XError()
{
	m_iLevel = Unknown;
}

H26XError::H26XError(const H26XError& other)
{
	m_iLevel = other.m_iLevel;
	m_szErrMsg = other.m_szErrMsg;
}

H26XError::H26XError(H26XError::Level iLevel, const std::string& szErrMsg)
{
	m_iLevel = iLevel;
	m_szErrMsg = szErrMsg;
}

H26XError::~H26XError()
{

}

H26XError::Level H26XError::level() const
{
	return m_iLevel;
}

const std::string& H26XError::message() const
{
	return m_szErrMsg;
}

H26XErrors::H26XErrors()
{

}

H26XErrors::~H26XErrors()
{

}

void H26XErrors::add(const H26XError& error)
{
	m_listErrors.add(error);
}

void H26XErrors::add(H26XError::Level iLevel, const std::string& szErrMsg)
{
	m_listErrors.add(H26XError(iLevel, szErrMsg));
}

void H26XErrors::add(const H26XErrors& errors)
{
	for (auto& error : errors.m_listErrors){
		m_listErrors.add(error);
	}
}

bool H26XErrors::empty() const
{
	return m_listErrors.empty();
}

int H26XErrors::size() const
{
	return (int)m_listErrors.size();
}

void H26XErrors::clear()
{
	m_listErrors.clear();
}

void H26XErrors::clear(int iKeptCount)
{
	do{
		int size = (int)m_listErrors.size();
		if(size > iKeptCount){
			m_listErrors.erase(m_listErrors.begin());
		}else{
			break;
		}
	}while(true);
}

void H26XErrors::dequeue()
{
	if(!m_listErrors.empty()) {
		m_listErrors.erase(m_listErrors.begin());
	}
}

H26XVector<H26XError>::iterator H26XErrors::begin()
{
	return m_listErrors.begin();
}

H26XVector<H26XError>::iterator H26XErrors::end()
{
	return m_listErrors.end();
}

H26XVector<H26XError>::const_iterator H26XErrors::begin() const
{
	return m_listErrors.begin();
}

H26XVector<H26XError>::const_iterator H26XErrors::end() const
{
	return m_listErrors.end();
}

bool H26XErrors::hasMajorErrors() const
{
	for (auto& error : m_listErrors){
		if (error.level() == H26XError::Major){
			return true;
		}
	}
	return false;
}

bool H26XErrors::hasMinorErrors() const
{
	for (auto& error : m_listErrors){
		if (error.level() == H26XError::Minor){
			return true;
		}
	}
	return false;
}