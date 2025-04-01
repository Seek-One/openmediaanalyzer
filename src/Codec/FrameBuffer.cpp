/*
 * FrameBuffer.cpp
 *
 *  Created on: 28 févr. 2012
 *      Author: ebeuque
 */

#include "FrameBuffer.h"

#include <stdlib.h>
#include <string.h>

#define FRAME_ORIGIN_ALL FRAME_ORIGIN_ACQUISITION | FRAME_ORIGIN_IMAGE_PROCESSING | FRAME_ORIGIN_RECORDING_FILE

//#define DEBUG_FRAME_OBJECT FRAME_ORIGIN_IMAGE_PROCESSING
//#define DEBUG_FRAME_OBJECT FRAME_ORIGIN_ALL
//#define DEBUG_FRAME_OBJECT FRAME_ORIGIN_RECORDING_FILE

#ifdef DEBUG_FRAME_OBJECT
#endif

#include "FrameMonitor.h"

FrameBuffer::FrameBuffer(const FrameDataType& datatype, size_t size, FreeBufferFunc freeFunc)
{
	m_datatype = datatype;
	m_iFrameFlags = 0;

#ifdef DEBUG_FRAME_OBJECT
	/*
	if((m_origin & DEBUG_FRAME_OBJECT) > 0){
		qDebug("*** FrameObject::construct 0x%X (initial size %lu bytes) ***", this, size);
	}*/
	Logger::debug("*** FrameBuffer::construct %p (initial size %lu bytes) ***", this, (unsigned long)size);
#endif
	m_initialSize = size;
	m_allocsize = size;
	m_size = 0;
	if(m_allocsize>0){
		m_data = (unsigned char*)malloc(m_allocsize);
	}else{
		m_data = NULL;
	}
	m_constdata = NULL;
	m_freeFunc = freeFunc;
}

FrameBuffer::FrameBuffer(const FrameDataType& datatype, size_t size, unsigned char* data, FreeBufferFunc freeFunc)
{
	m_datatype = datatype;
	m_iFrameFlags = 0;

#ifdef DEBUG_FRAME_OBJECT
	/*
	if((m_origin & DEBUG_FRAME_OBJECT) > 0){
		qDebug("*** FrameObject::construct2 0x%X (initial size %lu bytes) ***", this, size);
	}*/
	Logger::debug("*** FrameBuffer::construct2 %p (initial size %lu bytes) ***", this, (unsigned long)size);
#endif

	m_initialSize = size;
	m_allocsize = size;
	m_size = size;
	if(m_allocsize>0){
		m_data = data;
	}else{
		m_data = NULL;
	}
	m_constdata = NULL;
	m_freeFunc = freeFunc;
}

FrameBuffer::FrameBuffer(const FrameDataType& datatype, size_t size, const unsigned char* data)
{
	m_datatype = datatype;
	m_iFrameFlags = 0;

#ifdef DEBUG_FRAME_OBJECT
	/*
	if((m_origin & DEBUG_FRAME_OBJECT) > 0){
		qDebug("*** FrameObject::construct2 0x%X (initial size %lu bytes) ***", this, size);
	}*/
	Logger::debug("*** FrameBuffer::construct2 %p (initial size %lu bytes) ***", this, (unsigned long)size);
#endif

	m_initialSize = size;
	m_size = size;
	m_allocsize = size;
	m_data = NULL;
	m_constdata = data;
	m_freeFunc = NULL;
}

FrameBuffer::FrameBuffer(const FrameBuffer& other)
{
	m_datatype = other.m_datatype;
	m_iFrameFlags = other.m_iFrameFlags;

#ifdef DEBUG_FRAME_OBJECT
	/*
	if((m_origin & DEBUG_FRAME_OBJECT) > 0){
		qDebug("*** FrameObject::construct2 0x%X (initial size %lu bytes) ***", this, size);
	}*/
	Logger::debug("*** FrameBuffer::construct3 %p (initial size %lu bytes) ***", this, (unsigned long)size);
#endif

	m_initialSize = other.m_initialSize;
	m_size = other.m_size;
	m_allocsize = other.m_allocsize;
	if(other.m_data){
		m_data = (unsigned char*) malloc(other.m_size * sizeof(unsigned char));
		memcpy(m_data, other.m_data, other.m_size);
	}
	else if(other.m_constdata){
		m_data = (unsigned char*) malloc(other.m_size * sizeof(unsigned char));
		memcpy(m_data, other.m_constdata, other.m_size);
	}
	else{
		m_data = NULL;
	}
	m_constdata = NULL;
	m_freeFunc = NULL;
}

FrameBuffer::~FrameBuffer()
{
#ifdef DEBUG_FRAME_OBJECT
	/*
	if((m_origin & DEBUG_FRAME_OBJECT) > 0){
		qDebug("*** FrameObject::destruct 0x%X (initial size %lu bytes) ***", this, m_initialSize);
	}*/
	Logger::debug("*** FrameBuffer::destruct %p (initial size %lu bytes) ***", this, (unsigned long)m_initialSize);
#endif

	if(m_allocsize > 0 && m_data){
		if(m_freeFunc){
			m_freeFunc(m_data);
		}else{
			free(m_data);
		}
		m_data = NULL;
	}
}

const FrameDataType&
FrameBuffer::getDataType() const
{
	return m_datatype;
}

void FrameBuffer::setDataType(const FrameDataType& dataType)
{
	m_datatype = dataType;
}

bool FrameBuffer::isAlwaysKeyFrameDataType() const
{
	return m_datatype.isAlwaysKeyFrame();
}

void FrameBuffer::setPresentationTime(int iTvSec, int iTvUsec)
{
	m_tvPresentationTime.setTime(iTvSec, iTvUsec);
}

void FrameBuffer::setPresentationTime(const Timeval& timeval)
{
	m_tvPresentationTime = timeval;
}

const Timeval& FrameBuffer::getPresentationTime() const
{
	return m_tvPresentationTime;
}

bool FrameBuffer::isKeyFrame() const
{
	return (m_datatype.isAlwaysKeyFrame() ? true : (m_iFrameFlags & KeyFrame ? true : false));
}

void FrameBuffer::setIsKeyFrame(bool bKeyFrame)
{
	if(bKeyFrame){
		m_iFrameFlags |= KeyFrame;
	}else{
		m_iFrameFlags &= ~KeyFrame;
	}
}

bool FrameBuffer::isDuplicateFrame() const
{
	return (m_iFrameFlags & DuplicateFrame ? true : false);
}

void FrameBuffer::setDuplicateFrame(bool bDuplicate)
{
	if(bDuplicate){
		m_iFrameFlags |= DuplicateFrame;
	}else{
		m_iFrameFlags &= ~DuplicateFrame;
	}
}

void
FrameBuffer::appendData(const unsigned char* data, size_t len)
{
	unsigned char* tmp;
	// Check if we need to reallocate the buffer
	if(m_allocsize < m_size+len){
		if(m_data){
			//qDebug("Realloc (intitial=%lu) %lu<%lu+%lu=%lu", m_initialSize, m_allocsize, m_size, len, m_size+len);
			tmp = (unsigned char*)realloc(m_data, m_size+len);
			m_data = tmp;
		}else{
			m_data = (unsigned char*)malloc(len);
		}
		m_allocsize+=len;
	}
	memcpy(m_data+m_size, data, len);
	m_size+=len;
}


const unsigned char*
FrameBuffer::getData() const
{
	if(m_constdata){
		return m_constdata;
	}
	return m_data;
}

size_t
FrameBuffer::getDataSize() const
{
	return m_size;
}
