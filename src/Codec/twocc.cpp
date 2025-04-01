/*
 * twocc.cpp
 *
 *  Created on: 16 janv. 2015
 *      Author: ebeuque
 */

#include <cstring>

#include "twocc.h"

const char*
TwoCC_GetTypeName(twocc_t twocc)
{
	switch(twocc){
	case TwoCC_AudioCodec_G711_alaw: return "G.711 a-law";
	case TwoCC_AudioCodec_G711_mulaw: return "G.711 mu-law";
	case TwoCC_AudioCodec_G726_24: return "G.726-24";
	case TwoCC_AudioCodec_G726_32: return "G.726-32";
	case TwoCC_AudioCodec_MPEG4_AAC: return "MPEG4 AAC";
	}
	return nullptr;
}

const char*
TwoCC_GetMIMEType(twocc_t twocc)
{
	switch(twocc){
	case TwoCC_AudioCodec_G711_alaw: return "audio/pcma";
	case TwoCC_AudioCodec_G711_mulaw: return "audio/pcmu";
	case TwoCC_AudioCodec_G726_24: return "audio/g726-24";
	case TwoCC_AudioCodec_G726_32: return "audio/g726-32";
	case TwoCC_AudioCodec_MPEG4_AAC: return "audio/mpeg4-generic";
	}
	return nullptr;
}

twocc_t
TwoCC_FromMIMEType(const char* szMIMEType)
{
	if(strncasecmp(szMIMEType, "audio/", 6) == 0){
		if(strncasecmp(szMIMEType+6, "pcma", 4) == 0){
			return TwoCC_AudioCodec_G711_alaw;
		}
		if(strncasecmp(szMIMEType+6, "pcmu", 4) == 0){
			return TwoCC_AudioCodec_G711_mulaw;
		}
		if(strncasecmp(szMIMEType+6, "g726-24", 7) == 0){
			return TwoCC_AudioCodec_G726_24;
		}
		if(strncasecmp(szMIMEType+6, "g726-32", 7) == 0){
			return TwoCC_AudioCodec_G726_32;
		}
		if(strncasecmp(szMIMEType+6, "mpeg4-generic", 13) == 0){
			return TwoCC_AudioCodec_MPEG4_AAC;
		}
	}

	return 0;
}
