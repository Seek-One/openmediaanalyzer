/*
 * FrameType.h
 *
 *  Created on: 18 nov. 2014
 *      Author: ebeuque
 */

#ifndef TOOLKIT_CODEC_FRAMETYPE_H_
#define TOOLKIT_CODEC_FRAMETYPE_H_

#include "fourcc.h" // Video type identifier
#include "twocc.h" // Audio type identifier
#include "datatypecc.h" // Text type identifier

class FrameDataType
{
public:
	enum Type {
		Unknown = 0,
		Video = 1,
		Audio = 2,
		Data = 3,
	};

	FrameDataType();
	FrameDataType(fourcc_t fourcc);
	FrameDataType(twocc_t twocc);

	static FrameDataType fromFourCC(fourcc_t fourcc);
	static FrameDataType fromTwoCC(twocc_t twocc);
	static FrameDataType fromAudioFourCC(fourcc_t fourcc);
	static FrameDataType fromDataTypeCC(datatypecc_t datacc);

	bool isNull() const;

	bool hasType() const;
	bool hasSubType() const;

	bool equals(const FrameDataType& other) const;

	FrameDataType& operator= (const FrameDataType& other);
	bool operator== (const FrameDataType& other) const;
	bool operator!= (const FrameDataType& other) const;

	bool isAlwaysKeyFrame() const; // Tell if the data type is a stream where all frame are key frame
	const char* getMIMEType() const;
	static FrameDataType fromMIMEType(const char* szMIME);
	const char* getTypeName() const;

	bool isVideo() const;
	bool isAudio() const;
	bool isData() const;
	bool isJ10Type() const;

public:
	Type type;
	union {
		fourcc_t fourcc;
		twocc_t twocc;
		datatypecc_t datatypecc;
	};
};

#endif /* TOOLKIT_CODEC_FRAMETYPE_H_ */
