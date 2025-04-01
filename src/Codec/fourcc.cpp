/*
 * fourcc.cpp
 *
 *  Created on: 17 oct. 2012
 *      Author: ebeuque
 */

#include <cstring>

#include "fourcc.h"

const char*
FourCC_GetTypeName(fourcc_t fourcc)
{
	switch(fourcc){
	case FourCC_ImageCodec_JPEG: return "JPEG";
	case FourCC_VideoCodec_MJPG: return "MJPEG";
	case FourCC_VideoCodec_H264: return "H.264";
	case FourCC_VideoCodec_H265: return "H.265";
	case FourCC_VideoCodec_MxPEG: return "MxPEG";
	case FourCC_VideoCodec_VP8: return "VP8";
	case FourCC_VideoCodec_VP9: return "VP9";
	}
	return NULL;
}

const char*
FourCC_GetMIMEType(fourcc_t fourcc)
{
	switch(fourcc){
	case FourCC_ImageCodec_JPEG: return "image/jpeg";
	case FourCC_VideoCodec_MJPG: return "video/x-motion-jpeg";
	case FourCC_VideoCodec_H264: return "video/H264";
	case FourCC_VideoCodec_H265: return "video/H265";
	case FourCC_VideoCodec_MxPEG: return "video/MxPEG";
	case FourCC_VideoCodec_VP8: return "video/VP8"; // https://www.iana.org/assignments/media-types/media-types.xhtml#video
	case FourCC_VideoCodec_VP9: return "video/VP9";
	}
	return NULL;
}

fourcc_t
FourCC_FromMIMEType(const char* szMIMEType)
{
	if(strncasecmp(szMIMEType, "image/", 6) == 0){
		if(strncasecmp(szMIMEType+6, "jpeg", 4) == 0){
			return FourCC_ImageCodec_JPEG;
		}
		if(strncasecmp(szMIMEType+6, "pjpeg", 5) == 0){
			return FourCC_ImageCodec_JPEG;
		}
	}
	if(strncasecmp(szMIMEType, "video/", 6) == 0){
		if(strncasecmp(szMIMEType+6, "H264", 4) == 0){
			return FourCC_VideoCodec_H264;
		}
		if(strncasecmp(szMIMEType+6, "H265", 4) == 0){
			return FourCC_VideoCodec_H265;
		}
		if(strncasecmp(szMIMEType+6, "MxPEG", 5) == 0){
			return FourCC_VideoCodec_MxPEG;
		}
		if(strncasecmp(szMIMEType+6, "x-motion-jpeg", 13) == 0){
			return FourCC_VideoCodec_MJPG;
		}
		if(strncasecmp(szMIMEType+6, "VP8", 3) == 0){
			return FourCC_VideoCodec_VP9;
		}
		if(strncasecmp(szMIMEType+6, "VP9", 3) == 0){
			return FourCC_VideoCodec_VP9;
		}
	}

	return 0;
}
