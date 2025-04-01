/*
 * FrameType.cpp
 *
 *  Created on: 19 nov. 2014
 *      Author: ebeuque
 */

#include <stdlib.h>
#include <cstring>

#include "FrameDataType.h"

FrameDataType::FrameDataType()
{
	type = Unknown;
}

FrameDataType::FrameDataType(fourcc_t fourcc)
{
	type = Video;
	this->fourcc = fourcc;
}

FrameDataType::FrameDataType(twocc_t twocc)
{
	type = Audio;
	this->twocc = twocc;
}

FrameDataType FrameDataType::fromFourCC(fourcc_t fourcc)
{
	FrameDataType dataType;;
	dataType.type = Video;
	dataType.fourcc = fourcc;
	return dataType;
}

FrameDataType FrameDataType::fromTwoCC(twocc_t twocc)
{
	FrameDataType dataType;;
	dataType.type = Audio;
	dataType.twocc = twocc;
	return dataType;
}

FrameDataType FrameDataType::fromAudioFourCC(fourcc_t fourcc)
{
	FrameDataType dataType;;
	dataType.type = Audio;
	dataType.fourcc = fourcc;
	return dataType;
}

FrameDataType FrameDataType::fromDataTypeCC(datatypecc_t datacc)
{
	FrameDataType dataType;;
	dataType.type = Data;
	dataType.datatypecc = datacc;
	return dataType;
}

bool FrameDataType::isNull() const
{
	return (type == Unknown);
}

bool FrameDataType::hasType() const
{
	return (type == Unknown);
}

bool FrameDataType::hasSubType() const
{
	switch(type){
	case Unknown:
		return false;
	case Video:
		return (fourcc != 0);
	case Audio:
		return (twocc != 0);
	case Data:
		return (datatypecc != 0);
	}
	return false;
}

bool FrameDataType::equals(const FrameDataType& other) const
{
	if(type == other.type){
		switch(type){
		case Unknown:
			return true;
		case Video:
			return (fourcc == other.fourcc);
		case Audio:
			return (twocc == other.twocc);
		case Data:
			return (datatypecc == other.datatypecc);
		}
	}
	return false;
}

FrameDataType& FrameDataType::operator= (const FrameDataType& other)
{
	type = other.type;
	switch(type){
	case Video:
		fourcc = other.fourcc;
		break;
	case Audio:
		twocc = other.twocc;
		break;
	case Data:
		datatypecc = other.datatypecc;
		break;
	default:
		break;
	}
	return (*this);
}

bool FrameDataType::operator== (const FrameDataType& other) const
{
	return equals(other);
}

bool FrameDataType::operator!= (const FrameDataType& other) const
{
	return !equals(other);
}

bool FrameDataType::isAlwaysKeyFrame() const
{
	switch(type){
	case Video:
		switch(fourcc){
		case FourCC_VideoCodec_MJPG:
		case FourCC_ImageCodec_JPEG:
			return true;
		default:
			return false;
		}
		break;
	case Audio:
		return true;
		break;
	default:
		break;
	}
	return false;
}

const char* FrameDataType::getMIMEType() const
{
	switch(type){
	case Video:
		return FourCC_GetMIMEType(fourcc);
	case Audio:
		return TwoCC_GetMIMEType(twocc);
	case Data:
		return DataTypeCC_GetMIMEType(datatypecc);
	default:
		break;
	}
	return NULL;
}

FrameDataType FrameDataType::fromMIMEType(const char* szMIME)
{
	FrameDataType dataType;
	if(strncasecmp(szMIME, "image/", 6) == 0){
		dataType.type = Video;
		dataType.fourcc = FourCC_FromMIMEType(szMIME);
	}else if(strncasecmp(szMIME, "video/", 6) == 0){
		dataType.type = Video;
		dataType.fourcc = FourCC_FromMIMEType(szMIME);
	}else if(strncasecmp(szMIME, "audio/", 6) == 0){
		dataType.type = Audio;
		dataType.twocc = TwoCC_FromMIMEType(szMIME);
	}else if(strncasecmp(szMIME, "application/", 12) == 0){
		dataType.type = Data;
		dataType.datatypecc = DataTypeCC_FromMIMEType(szMIME);
	}else if(strncasecmp(szMIME, "text/", 5) == 0){
		dataType.type = Data;
		dataType.datatypecc = DataTypeCC_FromMIMEType(szMIME);
	}
	return dataType;
}

const char* FrameDataType::getTypeName() const
{
	switch(type){
	case Video:
		return FourCC_GetTypeName(fourcc);
		break;
	case Audio:
		return TwoCC_GetTypeName(twocc);
		break;
	case Data:
		return DataTypeCC_GetTypeName(datatypecc);
		break;
	default:
		break;
	}
	return NULL;
}

bool FrameDataType::isVideo() const
{
	return type==Video;
}

bool FrameDataType::isAudio() const
{
	return type==Audio;
}

bool FrameDataType::isData() const
{
	return type==Data;
}

bool FrameDataType::isJ10Type() const
{
	switch(type){
	case Video:
		return (fourcc == FourCC_J1O_VideoCyphered);
		break;
	case Audio:
		return (fourcc == FourCC_J1O_AudioCyphered);
		break;
	case Data:
		return (datatypecc == FourCC_J1O_DataCyphered);
		break;
	default:
		break;
	}
	return false;
}
