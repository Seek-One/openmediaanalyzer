# - Find libavcodec
# Find the native libavcodec includes and library
# This module defines
#  AVCODEC_INCLUDE_DIRS, where to find avcodec.h, etc.
#  AVCODEC_LIBRARIES, the libraries needed to use libavcodec.
#  AVCODEC_RUNTIME_LIBRARY_PATH - where to find runtime libraries (WIN32)
#  AVCODEC_FOUND, If false, do not try to use libavcodec.

if(WIN32)
	FIND_PATH(DEFAULT_AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
else(WIN32)
	FIND_PATH(DEFAULT_AVCODEC_INCLUDE_DIR libavcodec/avcodec.h PATHS /usr/local/include /usr/include)
endif(WIN32)
if(WIN32 AND DEFAULT_AVCODEC_INCLUDE_DIR)
	set(AVCODEC_RUNTIME_LIBRARY_PATH ${DEFAULT_AVCODEC_INCLUDE_DIR}/../bin)
endif(WIN32 AND DEFAULT_AVCODEC_INCLUDE_DIR)

SET(AVUTIL_NAMES ${AVUTIL_NAMES} avutil)
FIND_LIBRARY(DEFAULT_AVUTIL_LIBRARY
    NAMES ${AVUTIL_NAMES}
    PATHS /usr/lib /usr/local/lib
)
SET(AVCODEC_NAMES ${AVCODEC_NAMES} avcodec)
FIND_LIBRARY(DEFAULT_AVCODEC_LIBRARY
    NAMES ${AVCODEC_NAMES}
    PATHS /usr/lib /usr/local/lib
)

IF (DEFAULT_AVCODEC_LIBRARY AND DEFAULT_AVCODEC_INCLUDE_DIR)
    SET(AVCODEC_INCLUDE_DIRS ${DEFAULT_AVCODEC_INCLUDE_DIR})
    SET(AVCODEC_LIBRARIES ${DEFAULT_AVUTIL_LIBRARY} ${DEFAULT_AVCODEC_LIBRARY})
    SET(AVCODEC_FOUND TRUE)
ELSE (DEFAULT_AVCODEC_LIBRARY AND DEFAULT_AVCODEC_INCLUDE_DIR)
    SET(AVCODEC_FOUND FALSE)
ENDIF (DEFAULT_AVCODEC_LIBRARY AND DEFAULT_AVCODEC_INCLUDE_DIR)

# Get the version
get_filename_component(DEFAULT_AVCODEC_LIBRARY_REALPATH ${DEFAULT_AVCODEC_LIBRARY} REALPATH)
string(REGEX REPLACE ".*\\.so\\.(.*)" "\\1" AVCODEC_VERSION ${DEFAULT_AVCODEC_LIBRARY_REALPATH})
if(AVCODEC_VERSION)
    string(REGEX REPLACE "([0-9]+).*" "\\1" AVCODEC_VERSION_MAJOR ${AVCODEC_VERSION})
    string(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" AVCODEC_VERSION_MINOR ${AVCODEC_VERSION})
endif(AVCODEC_VERSION)

IF (AVCODEC_FOUND)
    IF (NOT AVCODEC_FIND_QUIETLY)
        MESSAGE(STATUS "Found libavcodec: ${AVCODEC_LIBRARIES}")
    ENDIF (NOT AVCODEC_FIND_QUIETLY)
ELSE (AVCODEC_FOUND)
    IF (AVCODEC_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find libavcodec")
     ENDIF (AVCODEC_FIND_REQUIRED)
ENDIF (AVCODEC_FOUND)

MARK_AS_ADVANCED(
  AVCODEC_LIBRARIES
  AVCODEC_INCLUDE_DIRS
)

