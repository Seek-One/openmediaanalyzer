/*
 * twocc.h
 *
 *  Created on: 18 nov. 2014
 *      Author: ebeuque
 */

#ifndef TOOLKIT_CODEC_TWOCC_H_
#define TOOLKIT_CODEC_TWOCC_H_

#include <cstdint>

#ifdef __cplusplus
	extern "C" {
#endif

typedef int32_t twocc_t;

#define MAKE_TWOCC(twocc) ((twocc_t)(twocc << 16))
#define MAKE_TWOCC2(twocc, rate) ((twocc_t)((twocc << 16) + rate))

// List of standardized TwoCC
// http://wiki.multimedia.cx/index.php?title=TwoCC

// Audio codec
#define TwoCC_AudioCodec_PCM_RAW		MAKE_TWOCC(0x0001) // Non compressed PCM
#define TwoCC_AudioCodec_G711_alaw		MAKE_TWOCC(0x0006)
#define TwoCC_AudioCodec_G711_mulaw		MAKE_TWOCC(0x0007)
#define TwoCC_AudioCodec_G726_16		MAKE_TWOCC2(0x0045, 16) // ITU-T G.726 ADPCM 16 kbit/s
#define TwoCC_AudioCodec_G726_24		MAKE_TWOCC2(0x0045, 24) // ITU-T G.726 ADPCM 24 kbit/s
#define TwoCC_AudioCodec_G726_32		MAKE_TWOCC2(0x0045, 32) // ITU-T G.726 ADPCM 32 kbit/s
#define TwoCC_AudioCodec_G726_40		MAKE_TWOCC2(0x0045, 40) // ITU-T G.726 ADPCM 40 kbit/s
#define TwoCC_AudioCodec_MPEG4_AAC		MAKE_TWOCC(0xA106)

const char*
TwoCC_GetTypeName(twocc_t twocc);

const char*
TwoCC_GetMIMEType(twocc_t twocc);

twocc_t
TwoCC_FromMIMEType(const char* szMIMEType);

#ifdef __cplusplus
	}
#endif

#endif /* TOOLKIT_CODEC_TWOCC_H_ */
