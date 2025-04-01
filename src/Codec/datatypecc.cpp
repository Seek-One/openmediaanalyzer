/*
 * datatypecc.c
 *
 *  Created on: 22 mars 2016
 *      Author: lgruber
 */

#include <cstring>

#include "datatypecc.h"

const char*
DataTypeCC_GetTypeName(datatypecc_t datatypecc)
{
	switch(datatypecc){
	case DataTypeCC_Application_Pdf:
		return "PDF";
	case DataTypeCC_Application_Json:
		return "JSON";
	case DataTypeCC_Application_Xml:
	case DataTypeCC_Text_Xml:
		return "XML";
	case DataTypeCC_Application_Onvif:
		return "ONVIF";
	case DataTypeCC_Text_Plain:
		return "Text";
	}
	return NULL;
}

const char*
DataTypeCC_GetMIMEType(datatypecc_t datatypecc)
{
	switch(datatypecc){
	case DataTypeCC_Application_Pdf:
		return "application/pdf";
	case DataTypeCC_Application_Json:
		return "application/json";
	case DataTypeCC_Application_Xml:
	case DataTypeCC_Text_Xml:
		return "application/xml";
	case DataTypeCC_Application_Onvif:
		return "application/VND.ONVIF.METADATA";
	case DataTypeCC_Text_Plain:
		return "text/plain";
	}
	return NULL;
}

datatypecc_t
DataTypeCC_FromMIMEType(const char* szMIMEType)
{
	if(strncasecmp(szMIMEType, "application/", 12) == 0){
		if(strncasecmp(szMIMEType+12, "pdf", 3) == 0){
			return DataTypeCC_Application_Pdf;
		}
		if(strncasecmp(szMIMEType+12, "json", 4) == 0){
			return DataTypeCC_Application_Json;
		}
		if(strncasecmp(szMIMEType+12, "xml", 3) == 0){
			return DataTypeCC_Application_Xml;
		}
		if(strncasecmp(szMIMEType+12, "VND.ONVIF.METADATA", 18) == 0){
			return DataTypeCC_Application_Onvif;
		}
	}else if(strncasecmp(szMIMEType, "text/", 5) == 0){
		if(strncasecmp(szMIMEType+5, "plain", 5) == 0){
			return DataTypeCC_Text_Plain;
		}
		if(strncasecmp(szMIMEType+5, "xml", 3) == 0){
			return DataTypeCC_Text_Xml;
		}
	}

	return 0;
}


