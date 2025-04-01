/*
 * ByteBuffer.cpp
 *
 *  Created on: 15 mai 2012
 *      Author: ebeuque
 */

#include <cstring>
#include <cstdlib>

#include "ByteBuffer.h"

ByteBuffer::ByteBuffer()
{
	m_initialSize = 0;
	m_allocatedSize = 0;
	m_incrSize = 0;
	m_offset = 0;
	m_size = 0;
	m_data = NULL;
}

ByteBuffer::ByteBuffer(size_t size)
{
	m_initialSize = size;
	m_allocatedSize = size;
	m_incrSize = 0;
	m_offset = 0;
	m_size = 0;
	if(m_allocatedSize>0){
		m_data = (unsigned char*)malloc(m_allocatedSize);
	}else{
		m_data = NULL;
	}
}

ByteBuffer::ByteBuffer(const unsigned char* data, size_t size)
{
	m_initialSize = size;
	m_allocatedSize = size;
	m_incrSize = 0;
	m_offset = 0;
	m_size = size;
	if(m_allocatedSize>0){
		m_data = (unsigned char*)malloc(m_allocatedSize);
		memcpy(m_data, data, size);
	}else{
		m_data = NULL;
	}
}

ByteBuffer::ByteBuffer(unsigned char* data, size_t size)
{
	m_initialSize = size;
	m_allocatedSize = size;
	m_incrSize = 0;
	m_offset = 0;
	m_size = size;
	if(m_allocatedSize>0){
		m_data = data;
	}else{
		m_data = NULL;
	}
}

ByteBuffer::ByteBuffer(const ByteBuffer& other)
{
	m_size = other.m_size;
	m_allocatedSize = other.m_allocatedSize;
	m_initialSize = other.m_initialSize;
	m_incrSize = other.m_incrSize;
	m_offset = other.m_offset;
	if(m_allocatedSize > 0){
		m_data = (unsigned char*)malloc(m_allocatedSize);
		memcpy(m_data, other.m_data, m_allocatedSize);
	}else{
		m_data = NULL;
	}
}

ByteBuffer::~ByteBuffer()
{
	freemem();
}

void ByteBuffer::append(const ByteBuffer& buffer)
{
	append(buffer.data(), buffer.size());
}

void
ByteBuffer::append(const unsigned char* data, size_t len)
{
	unsigned char* tmp;
	if(data != NULL && len>0){
		// Check if we need to reallocate the buffer
		if(m_allocatedSize < m_size+len){
			//qDebug("Realloc (intitial=%lu) %lu<%lu+%lu=%lu", m_initialSize, m_allocsize, m_size, len, m_size+len);
			m_allocatedSize=((m_incrSize != 0 && m_incrSize >= len) ?  m_size+m_incrSize : m_size+len);
			tmp = (unsigned char*)realloc(m_data, m_allocatedSize);
			m_data = tmp;
		}
		memcpy(m_data+m_size, data, len);
		m_size+=len;
	}
}

void
ByteBuffer::append(const char* data, size_t len)
{
	append((unsigned char*) data, len);
}

void
ByteBuffer::append(char c)
{
	append(&c, 1);
}

bool ByteBuffer::read(unsigned char* data, size_t len)
{
	if(m_offset + len <= m_size){
		memcpy(data, (m_data + m_offset), len);
		m_offset+=len;
		return true;
	}
	return false;
}

void ByteBuffer::write(const ByteBuffer& buffer)
{
	write(buffer.data(), buffer.size());
}

void
ByteBuffer::write(const unsigned char* data, size_t len)
{
	unsigned char* tmp;
	if(data != NULL && len>0){
		// Check if we need to reallocate the buffer
		if(m_allocatedSize < m_offset+len){
			//TkCore::Logger::debug("Realloc (intitial=%lu) %lu<%lu+%lu=%lu", m_initialSize, m_allocatedSize, m_size, len, m_size+len);
			m_allocatedSize=((m_incrSize != 0 && m_incrSize >= len) ? m_offset+m_incrSize : m_offset+len);
			if(m_allocatedSize % 2 == 1){
				// Allocated size must be multiple of two
				m_allocatedSize++;
				tmp = (unsigned char*)realloc(m_data, m_allocatedSize);
				tmp[m_allocatedSize-1] = 0;
			}else{
				tmp = (unsigned char*)realloc(m_data, m_allocatedSize);
			}
			m_data = tmp;
		}
		memcpy(m_data+m_offset, data, len);
		if(m_size < m_offset+len){
			m_size=m_offset+len;
		}
		m_offset+=len;
	}
}

void
ByteBuffer::write(const char* data, size_t len)
{
	write((unsigned char*) data, len);
}

void
ByteBuffer::write(char c)
{
	write(&c, 1);
}

size_t ByteBuffer::offset() const
{
	return m_offset;
}

void ByteBuffer::move(size_t offset_count)
{
	m_offset+=offset_count;
}

void ByteBuffer::moveTo(size_t new_offset)
{
	m_offset = new_offset;
}

void ByteBuffer::moveBeg()
{
	m_offset = 0;
}

void ByteBuffer::moveEnd()
{
	m_offset = m_size;
}

void
ByteBuffer::clear()
{
	m_size = 0;
}

void ByteBuffer::freemem()
{
	clear();
	if(m_allocatedSize > 0){
		if(m_data != NULL){
			free(m_data);
			m_data = NULL;
		}
		m_allocatedSize = 0;
	}
}

unsigned char* ByteBuffer::dettach()
{
	unsigned char* data = m_data;

	m_data = NULL;
	m_allocatedSize = 0;
	m_size = 0;

	return data;
}

const unsigned char*
ByteBuffer::data() const
{
	return m_data;
}

size_t
ByteBuffer::size() const
{
	return m_size;
}

size_t ByteBuffer::allocatedSize() const
{
	return m_allocatedSize;
}

void ByteBuffer::setIncrementSize(size_t size)
{
	m_incrSize = size;
}

unsigned char ByteBuffer::operator[] (int idx) const
{
	return m_data[idx];
}

const unsigned char* ByteBuffer::operator+ (int idx) const
{
	return m_data + idx;
}

ByteBuffer::operator const unsigned char*()
{
	return m_data;
}

ByteBuffer::operator const char*()
{
	return (const char*)m_data;
}

ByteBuffer::operator const void*()
{
	return (const char*)m_data;
}

ByteBuffer& ByteBuffer::operator=(const ByteBuffer& other)
{
	m_size = other.m_size;
	m_allocatedSize = other.m_allocatedSize;
	m_initialSize = other.m_initialSize;
	m_incrSize = other.m_incrSize;
	m_offset = other.m_offset;
	if (m_data) {
		free(m_data);
		m_data = NULL;
	}
	if(m_allocatedSize > 0){
		m_data = (unsigned char*)malloc(m_allocatedSize);
		memcpy(m_data, other.m_data, m_allocatedSize);
	}
	return *this;
}

