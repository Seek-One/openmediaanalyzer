# - Find curl
# Find the native CURL headers and libraries.
#
# Options:
#  CURL_STATIC    - ON if curl must be used as static lib (default OFF) 
#
# This module defines:
#  CURL_INCLUDE_DIRS - where to find curl/curl.h, etc.
#  CURL_LIBRARIES    - List of libraries when using curl.
#  CURL_RUNTIME_LIBRARY_PATH - where to find runtime libraries (WIN32)
#  CURL_VERSION_STRING - Version on curl found.
#  CURL_FOUND        - True if curl found.

#=============================================================================
# Copyright 2006-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

OPTION(CURL_STATIC "on if curl is a static lib " OFF)

# Look for the header file.
FIND_PATH(CURL_INCLUDE_DIR NAMES curl/curl.h)
MARK_AS_ADVANCED(CURL_INCLUDE_DIR)

if(MSVC)
	# Search Windows SDK path (old version)
	set(MSWindows_SEARCH_PATHS
		"C:/Program Files/Microsoft SDK*/Windows/v*/Include"
		"C:/Program Files (x86)/Microsoft SDK*/Windows/v*/Include"
		"C:/Program Files/Microsoft SDKs/Windows/v7.0A/Include"
		"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Include"
	)
	find_path(MSWindows_INCLUDE_PATH windows.h PATHS ${MSWindows_SEARCH_PATHS} NO_DEFAULT_PATH)
	# Search Windows SDK path (new version)
	if(NOT MSWindows_INCLUDE_PATH)
		set(MSWindows_SEARCH_PATHS
			"C:/Program Files (x86)/Windows Kits/*/Include/*/um/"
		)
		find_path(MSWindows_INCLUDE_PATH windows.h PATHS ${MSWindows_SEARCH_PATHS} NO_DEFAULT_PATH)
	endif()
	if(MSWindows_INCLUDE_PATH)
		if(MSWindows_INCLUDE_PATH MATCHES "C:/Program Files \\(x86\\)/Windows Kits/.*")
			string(REPLACE "/Include/" "/Lib/" MSWindows_LIBRARY_PATH ${MSWindows_INCLUDE_PATH})
			if(CMAKE_CL_64)
				set(MSWindows_LIBRARY_PATH ${MSWindows_LIBRARY_PATH}/x64)
			else(CMAKE_CL_64)
				set(MSWindows_LIBRARY_PATH ${MSWindows_LIBRARY_PATH}/x86)
			endif(CMAKE_CL_64)
		else()
			set(MSWindows_LIBRARY_PATH "${MSWindows_INCLUDE_PATH}/../Lib")
		endif()
	else()
		MESSAGE (FATAL_ERROR "Cannot find Windows SDK")
	endif()
endif(MSVC)

# Look for the library.
if(CURL_STATIC)
	MESSAGE (STATUS "Requesting CURL static library")
	if (MSVC)
		ADD_DEFINITIONS(/DCURL_STATICLIB /DHTTP_ONLY /DCURL_DISABLE_LDAP)
		FIND_LIBRARY(CURL_LIBRARY NAMES curllib_static)
		FIND_LIBRARY(SSLEAY_LIBRARY NAMES ssleay32)
		FIND_LIBRARY(LIBEAY32_LIBRARY NAMES libeay32)
		# Windows specific
		FIND_LIBRARY(WS2_LIBRARY NAMES ws2_32 PATHS ${MSWindows_LIBRARY_PATH})
		FIND_LIBRARY(WLDAP32_LIBRARY NAMES wldap32 PATHS ${MSWindows_LIBRARY_PATH})
		
		if(NOT SSLEAY_LIBRARY OR NOT LIBEAY32_LIBRARY OR NOT WS2_LIBRARY OR NOT WLDAP32_LIBRARY)
			message (FATAL_ERROR "Cannot find all Curl dependencies")
		endif()
		
	else(MSVC)
		ADD_DEFINITIONS(-DCURL_STATICLIB)
		FIND_LIBRARY(CURL_LIBRARY NAMES curl)
	endif(MSVC)
else(CURL_STATIC)
	MESSAGE (STATUS "Requesting CURL dynamic library")
	if(DEBUG)
		set(CURL_LIB_PATH_SUFFIX Debug)
	else()
		set(CURL_LIB_PATH_SUFFIX Release)
	endif()
	FIND_LIBRARY(CURL_LIBRARY NAMES 
		curldll
		curl
    # Windows MSVC prebuilts:
		curllib
		libcurl_imp.lib
		libcurl
		PATH_SUFFIXES ${CURL_LIB_PATH_SUFFIX}
	)
endif(CURL_STATIC)
MARK_AS_ADVANCED(CURL_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CURL DEFAULT_MSG CURL_LIBRARY CURL_INCLUDE_DIR)

if(CURL_INCLUDE_DIR AND NOT CURL_VERSION_STRING)
  foreach(_curl_version_header curlver.h curl.h)
    if(EXISTS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}")
      file(STRINGS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}" curl_version_str REGEX "^#define[\t ]+LIBCURL_VERSION[\t ]+\".*\"")

      string(REGEX REPLACE "^#define[\t ]+LIBCURL_VERSION[\t ]+\"([^\"]*)\".*" "\\1" CURL_VERSION_STRING "${curl_version_str}")
      unset(curl_version_str)
      break()
    endif()
  endforeach()
endif()

IF(CURL_FOUND)
  SET(CURL_LIBRARIES ${CURL_LIBRARY} ${SSLEAY_LIBRARY} ${LIBEAY32_LIBRARY} ${WS2_LIBRARY} ${WLDAP32_LIBRARY})
  SET(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
  SET(CURL_RUNTIME_LIBRARY_PATH ${CURL_INCLUDE_DIR}/../bin)
ENDIF(CURL_FOUND)

UNSET(CURL_INCLUDE_DIR CACHE)
UNSET(CURL_LIBRARY CACHE)
UNSET(SSLEAY_LIBRARY CACHE)
UNSET(WS2_LIBRARY CACHE)
UNSET(LIBEAY32_LIBRARY CACHE)
UNSET(WLDAP32_LIBRARY CACHE)
