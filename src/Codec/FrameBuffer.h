/*
 * FrameBuffer.h
 *
 *  Created on: 28 févr. 2012
 *      Author: ebeuque
 */

#ifndef TOOLKIT_CODEC_FRAMEBUFFER_H_
#define TOOLKIT_CODEC_FRAMEBUFFER_H_


#include "Timeval.h"

#include "FrameDataType.h"

typedef void (*FreeBufferFunc)(unsigned char* data);

/*
 * A Frame is a data transmission unit or network packet that includes frame synchronization information.
 */
class FrameBuffer
{
public:
	// Constructor. Initialize the frame and allocate the buffer with the given size.
	FrameBuffer(const FrameDataType& datatype, size_t size, FreeBufferFunc freeFunc = NULL);

	// Constructor. Initialize the frame and attach the buffer with the given size.
	// data will be freed when no longer needed.
	FrameBuffer(const FrameDataType& datatype, size_t size, unsigned char* data, FreeBufferFunc freeFunc = NULL);

	// Constructor. Initialize the frame and attach the buffer with the given size.
	// data will be not be freed when no longer needed.
	explicit FrameBuffer(const FrameDataType& datatype, size_t size, const unsigned char* data);

	FrameBuffer(const FrameBuffer& other);

	virtual ~FrameBuffer();

	///////////////////////////////
	// Extra data of frame buffer
	///////////////////////////////

	// Return the fourcc/twocc code for the data represented in the buffer
	const FrameDataType& getDataType() const;
	void setDataType(const FrameDataType& datatype);
	bool isAlwaysKeyFrameDataType() const;

	// Time to use as presentation time (used for the frame synchronization)
	void setPresentationTime(int iTvSec, int iTvUsec);
	void setPresentationTime(const Timeval& timeval);
	const Timeval& getPresentationTime() const;

	// Set/tell is the frame is a key frame. Default value is true.
	bool isKeyFrame() const;
	void setIsKeyFrame(bool bKeyFrame);

	// Set/tell the frame duplicate another frame and may be ignored for some process.
	bool isDuplicateFrame() const;
	void setDuplicateFrame(bool bDuplicate);

	////////////////////////////////////////////
	// Buffer manipulation  of the frame buffer
	////////////////////////////////////////////

	// Append byte data to the frame buffer growing it if necessary
	void appendData(const unsigned char* data, size_t len);

	// Get buffer and buffer size
	const unsigned char* getData() const;
	size_t getDataSize() const;

private:
	enum FrameFlags {
		KeyFrame = 0x01,
		DuplicateFrame = 0x02,
	};

	// Data type
	FrameDataType m_datatype;
	// Presentation time of the frame
	Timeval m_tvPresentationTime;
	// Flags of the frame
	int m_iFrameFlags;

	// Data buffer information
	unsigned char* m_data;
	const unsigned char* m_constdata;
	size_t m_size;
	size_t m_allocsize;
	size_t m_initialSize;

	FreeBufferFunc m_freeFunc;
};

#endif /* TOOLKIT_CODEC_FRAMEBUFFER_H_ */
