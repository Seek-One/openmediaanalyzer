/*
 * datatypecc.h
 *
 *  Created on: 22 mars 2016
 *      Author: lgruber
 */

#ifndef DATATYPECC_H_
#define DATATYPECC_H_

#include <cstdint>

#ifdef __cplusplus
	extern "C" {
#endif

typedef int32_t datatypecc_t;

#define MAKE_DATATYPECC(datatypecc) ((datatypecc_t)(datatypecc << 16))

// Generic Data Type
#define DataTypeCC_Application_Pdf		MAKE_DATATYPECC(0x0001)
#define DataTypeCC_Application_Json		MAKE_DATATYPECC(0x0002)
#define DataTypeCC_Application_Xml		MAKE_DATATYPECC(0x0003)
#define DataTypeCC_Application_Onvif	MAKE_DATATYPECC(0x0004)

#define DataTypeCC_Text_Plain			MAKE_DATATYPECC(0x1000)
#define DataTypeCC_Text_Xml				MAKE_DATATYPECC(0x1001)

const char*
DataTypeCC_GetTypeName(datatypecc_t datatypecc);

const char*
DataTypeCC_GetMIMEType(datatypecc_t datatypecc);

datatypecc_t
DataTypeCC_FromMIMEType(const char* szMIMEType);

#ifdef __cplusplus
	}
#endif

#endif /* DATATYPECC_H_ */
