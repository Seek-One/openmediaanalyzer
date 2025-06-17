find_path(AVUTIL_INCLUDE_DIR
  NAMES libavutil/avutil.h
  HINTS ${CMAKE_PREFIX_PATH} /usr/include /usr/local/include
)

find_library(AVUTIL_LIBRARY
  NAMES avutil
  HINTS ${CMAKE_PREFIX_PATH} /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AVUTIL
  REQUIRED_VARS AVUTIL_LIBRARY AVUTIL_INCLUDE_DIR
  VERSION_VAR AVUTIL_VERSION
)

mark_as_advanced(AVUTIL_INCLUDE_DIR AVUTIL_LIBRARY)

if(AVUTIL_FOUND)
  set(AVUTIL_LIBRARIES ${AVUTIL_LIBRARY})
  set(AVUTIL_INCLUDE_DIRS ${AVUTIL_INCLUDE_DIR})
endif()