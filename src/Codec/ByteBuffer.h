/*
 * ByteBuffer.h
 *
 *  Created on: 15 mai 2012
 *      Author: ebeuque
 */

#ifndef TOOLKIT_CORE_BYTEBUFFER_H_
#define TOOLKIT_CORE_BYTEBUFFER_H_

#include <cstdint>
#include <cstddef>

/*
 *
 */
class ByteBuffer {
public:
	ByteBuffer();
	ByteBuffer(size_t size);
	ByteBuffer(const unsigned char* data, size_t size);
	ByteBuffer(unsigned char* data, size_t size);
	ByteBuffer(const ByteBuffer& other);
	virtual ~ByteBuffer();

	void append(const ByteBuffer& buffer);
	void append(const unsigned char* data, size_t len);
	void append(const char* data, size_t len);
	void append(char c);
	bool read(unsigned char* data, size_t len);
	void write(const ByteBuffer& buffer);
	void write(const unsigned char* data, size_t len);
	void write(const char* data, size_t len);
	void write(char c);
	void clear();
	void freemem();
	unsigned char* dettach();

	size_t offset() const;
	void move(size_t offset_count);
	void moveTo(size_t new_offset);
	void moveBeg();
	void moveEnd();

	const unsigned char* data() const;
	size_t size() const;

	size_t allocatedSize() const;
	void setIncrementSize(size_t size);

	unsigned char operator[] (int idx) const;
	const unsigned char* operator+ (int idx) const;
	operator const unsigned char* ();
	operator const char* ();
	operator const void* ();
	ByteBuffer& operator=(const ByteBuffer& other);

private:
	unsigned char* m_data; // data
	size_t m_size;
	size_t m_allocatedSize;
	size_t m_initialSize;
	size_t m_incrSize;
	size_t m_offset;
};

#endif /* TOOLKIT_CORE_BYTEBUFFER_H_ */
