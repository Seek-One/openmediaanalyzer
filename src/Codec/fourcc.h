/*
 * fourcc.h
 *
 *  Created on: 17 oct. 2012
 *      Author: ebeuque
 */

#ifndef TOOLKIT_CODEC_FOURCC_H_
#define TOOLKIT_CODEC_FOURCC_H_

#include <cstdint>

#ifdef __cplusplus
	extern "C" {
#endif

typedef uint32_t fourcc_t;

#define MAKE_FOURCC( a, b, c, d ) \
	((fourcc_t)( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
			| ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) ))

#define FourCC_Unknown				((fourcc_t)0)

/* Video codec */
// http://www.fourcc.org/codecs.php
#define FourCC_VideoCodec_MJPG		MAKE_FOURCC('M','J','P','G')
#define FourCC_VideoCodec_MxPEG		MAKE_FOURCC('M','X','P','G')
#define FourCC_VideoCodec_H263		MAKE_FOURCC('H','2','6','3') // H.263 1996
#define FourCC_VideoCodec_H263P		MAKE_FOURCC('I','L','V','R') // ITU-T H.263+ 1998
#define FourCC_VideoCodec_H264		MAKE_FOURCC('H','2','6','4')
#define FourCC_VideoCodec_H265		MAKE_FOURCC('H','E','V','C')
#define FourCC_VideoCodec_MPEG4		MAKE_FOURCC('M','P','G','4')
#define FourCC_VideoCodec_HKH4		MAKE_FOURCC('H','K','H','4')
#define FourCC_VideoCodec_VP8		MAKE_FOURCC('V','P','8','0') // https://www.webmproject.org/docs/container/#video-codec
#define FourCC_VideoCodec_VP9		MAKE_FOURCC('V','P','9','0') // https://www.webmproject.org/docs/container/#video-codec

// Jet1oeil cyphered data
#define FourCC_J1O_VideoCyphered	MAKE_FOURCC('J','1','V','C')
#define FourCC_J1O_AudioCyphered	MAKE_FOURCC('J','1','A','C')
#define FourCC_J1O_DataCyphered		MAKE_FOURCC('J','1','D','C')

/* Image codec (compressed) */
#define FourCC_ImageCodec_JPEG		MAKE_FOURCC('J','P','E','G')

/* Pixmap format */
/* Planar YUV 4:2:0 Y:U:V full scale */
#define FourCC_PixmapCodec_J420		MAKE_FOURCC('J','4','2','0')
/* Planar YUV 4:2:2 Y:U:V full scale */
#define FourCC_PixmapCodec_J422     MAKE_FOURCC('J','4','2','2')
/* Planar YUV 4:4:0 Y:U:V full scale */
#define FourCC_PixmapCodec_J440     MAKE_FOURCC('J','4','4','0')
/* Planar YUV 4:4:4 Y:U:V full scale */
#define FourCC_PixmapCodec_J444     MAKE_FOURCC('J','4','4','4')
/* 8 bits grey */
#define FourCC_PixmapCodec_GREY     MAKE_FOURCC('G','R','E','Y')
/* 24 bits RGB */
#define FourCC_PixmapCodec_RGB24    MAKE_FOURCC('R','V','2','4')
/* 24 bits RGB padded to 32 bits */
#define FourCC_PixmapCodec_RGB32    MAKE_FOURCC('R','V','3','2')

const char*
FourCC_GetTypeName(fourcc_t fourcc);

const char*
FourCC_GetMIMEType(fourcc_t fourcc);

fourcc_t
FourCC_FromMIMEType(const char* szMIMEType);

#ifdef __cplusplus
	}
#endif


#endif /* TOOLKIT_CODEC_FOURCC_H_ */
