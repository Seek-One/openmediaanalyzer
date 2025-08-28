##############################
# CMake
##############################

cmake_minimum_required(VERSION "3.13.4")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
set(CMAKE_CXX_STANDARD 17)

##############################
# Project data
##############################

set(APPLICATION_VENDOR_NAME "seekone")
set(APPLICATION_VENDOR_DOMAIN "www.seek-one.fr")
set(APPLICATION_NAME "OpenMediaAnalyzer")
set(APPLICATION_VERSION_MAJOR 1)
set(APPLICATION_VERSION_MINOR 0)
set(APPLICATION_VERSION_PATCH 0)

if(NOT DEFINED APPLICATION_VERSION_BUILD)
	set(APPLICATION_VERSION_BUILD "")
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(APPLICATION_VERSION_BUILD ${APPLICATION_VERSION_BUILD}dbg)
endif()

set(APPLICATION_VERSION ${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}${APPLICATION_VERSION_BUILD})
set(APPLICATION_CONTACTMAIL "contact@seek-one.fr")
set(APPLICATION_CONTACT "Contact Seek-One <${APPLICATION_CONTACTMAIL}>")
set(APPLICATION_PACKAGE_VENDOR "seekone")
set(APPLICATION_PACKAGE_NAME "openmediaanalyzer")
set(APPLICATION_EXE_NAME openmediaanalyzer)

project(openmediaanalyzer CXX)
set(CMAKE_VERBOSE_MAKEFILE OFF)

##############################
# CMake modules
##############################

include(CMakeModules/ProjectCommonConfig.cmake)
include(CMakeModules/ProjectCompilerChecks.cmake)

##############################
# Library
##############################

# Compiler
set(WITH_COMPILER_LIBRARIES ON CACHE BOOL "Enable build with compiler libraries")
set(WITH_PTHREAD ON CACHE BOOL "Enable build with pthread library")

# GUI
set(WITH_QT ON CACHE BOOL "Enable build with qt library")
set(QT6_COMPONENTS Core Gui Widgets Test)

if(WIN32)
	set(QT_PLATFORMS_COMPONENTS qwindows qminimal)
elseif(APPLE)
	set(QT_PLATFORMS_COMPONENTS qcocoa)
else()
	set(QT_PLATFORMS_COMPONENTS qxcb qminimal)
endif()
set(QT_IMAGEFORMATS_COMPONENTS qjpeg)
if(APPLE)
	set(QT_PLATFORMS_COMPONENTS qcocoa)
endif()
if(WIN32)
	set(QT_PLATFORMS_COMPONENTS qwindows)
endif()

set(WITH_AVCODEC ON CACHE BOOL "Enable build with avcodec library")
set(WITH_AVFORMAT ON CACHE BOOL "Enable build with avformat library")
set(WITH_SWSCALE ON CACHE BOOL "Enable build with swscale library")
set(WITH_CURL ON CACHE BOOL "Enable build with libcurl")
set(WITH_OPENSSL ON BOOL "Enable build with openssl library")

# Define output variable
set (PACKAGE_DEPS)
if(WITH_AUTONOMOUS_PACKAGE_BUILD)
	set(AUTONOMOUS_PACKAGE_BUILD ON CACHE BOOL "Enable build of autonomous package without dependencies")
endif()

include(CMakeModules/ProjectIncludeLibraries.cmake)
include(CMakeModules/ProductImageUtils.cmake)

####################
# Config file
####################

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)
add_definitions(-DHAVE_CONFIG_H)
configure_file (
	"${PROJECT_SOURCE_DIR}/version.h.in"
	"${PROJECT_BINARY_DIR}/version.h"
)
add_definitions(-DHAVE_VERSION_H)
include_directories (${PROJECT_BINARY_DIR})

####################
# Add sources
####################

include_directories (${PROJECT_SOURCE_DIR}/src)
    
set(APPLICATION_SOURCES
	src/main.cpp
    src/GUI/QAccessUnitElement.cpp
    src/GUI/QAccessUnitElement.h
    src/GUI/QErrorView.cpp
    src/GUI/QErrorView.h
    src/GUI/QNALUInfoView.cpp
    src/GUI/QNALUInfoView.h
    src/GUI/QStatusView.cpp
    src/GUI/QStatusView.h
    src/GUI/QStreamSettingsView.cpp
    src/GUI/QStreamSettingsView.h
    src/GUI/QStreamLinkDialog.cpp
    src/GUI/QStreamLinkDialog.h
    src/GUI/QTimelineCounterElement.cpp
    src/GUI/QTimelineCounterElement.h
    src/GUI/QTimelineView.cpp
    src/GUI/QTimelineView.h
    src/GUI/QVideoFrameView.cpp
    src/GUI/QVideoFrameView.h
    src/GUI/QVideoInputView.cpp
    src/GUI/QVideoInputView.h
    src/GUI/QWindowMain.cpp
    src/GUI/QWindowMain.h
    src/GUIController/QErrorViewController.cpp
    src/GUIController/QErrorViewController.h
	src/GUIController/QH26XDumpObject.cpp
	src/GUIController/QH26XDumpObject.h
    src/GUIController/QVideoInputViewController.cpp
    src/GUIController/QVideoInputViewController.h
    src/GUIController/QStatusViewController.cpp
    src/GUIController/QStatusViewController.h
    src/GUIController/QStreamSettingsViewController.cpp
    src/GUIController/QStreamSettingsViewController.h
    src/GUIController/QTimelineViewController.cpp
    src/GUIController/QTimelineViewController.h
    src/GUIController/QVideoFrameViewController.cpp
    src/GUIController/QVideoFrameViewController.h
    src/GUIController/QWindowMainController.cpp
    src/GUIController/QWindowMainController.h
    src/GUIModel/QAccessUnitModel.cpp
    src/GUIModel/QAccessUnitModel.h
    src/GUIModel/QDecoderModel.cpp
    src/GUIModel/QDecoderModel.h
    src/GUIModel/QFolderModel.cpp
    src/GUIModel/QFolderModel.h
    src/GUIModel/QStreamModel.cpp
    src/GUIModel/QStreamModel.h
)

set(CODEC_SOURCES
    src/Codec/H26X/H26XBitstreamReader.cpp
    src/Codec/H26X/H26XBitstreamReader.h
	src/Codec/H26X/H26XDumpObject.h
	src/Codec/H26X/H26XDumpObject.cpp
	src/Codec/H26X/H26XErrors.h
	src/Codec/H26X/H26XErrors.cpp
	src/Codec/H26X/H26XErrorsMsg.h
    src/Codec/H26X/H26XMath.cpp
    src/Codec/H26X/H26XMath.h
	src/Codec/H26X/H26XNALHeader.cpp
	src/Codec/H26X/H26XNALHeader.h
	src/Codec/H26X/H26XNALPayload.cpp
	src/Codec/H26X/H26XNALPayload.h
	src/Codec/H26X/H26XNALUnit.cpp
	src/Codec/H26X/H26XNALUnit.h
    src/Codec/H26X/H26XStream.cpp
    src/Codec/H26X/H26XStream.h
	src/Codec/H26X/H26XTypes.h
    src/Codec/H26X/H26XUtils.cpp
    src/Codec/H26X/H26XUtils.h
	src/Codec/H26X/H26XVector.h
    src/Codec/H264/H264AccessUnit.cpp
    src/Codec/H264/H264AccessUnit.h
    src/Codec/H264/H264AUD.cpp
    src/Codec/H264/H264AUD.h
    src/Codec/H264/H264BitstreamReader.cpp
    src/Codec/H264/H264BitstreamReader.h
    src/Codec/H264/H264GOP.cpp
    src/Codec/H264/H264GOP.h
	src/Codec/H264/H264NALHeader.cpp
	src/Codec/H264/H264NALHeader.h
	src/Codec/H264/H264NALUnit.cpp
	src/Codec/H264/H264NALUnit.h
	src/Codec/H264/H264NALUnitType.cpp
	src/Codec/H264/H264NALUnitType.h
    src/Codec/H264/H264PPS.cpp
    src/Codec/H264/H264PPS.h
    src/Codec/H264/H264Slice.cpp
    src/Codec/H264/H264Slice.h
    src/Codec/H264/H264SPS.cpp
    src/Codec/H264/H264SPS.h
    src/Codec/H264/H264SEI.cpp
    src/Codec/H264/H264SEI.h
    src/Codec/H264/H264Stream.cpp
    src/Codec/H264/H264Stream.h
    src/Codec/H265/H265AccessUnit.cpp
    src/Codec/H265/H265AccessUnit.h
    src/Codec/H265/H265BitstreamReader.cpp
    src/Codec/H265/H265BitstreamReader.h
    src/Codec/H265/H265GOP.cpp
    src/Codec/H265/H265GOP.h
    src/Codec/H265/H265HrdParameters.cpp
    src/Codec/H265/H265HrdParameters.h
	src/Codec/H265/H265NALHeader.cpp
	src/Codec/H265/H265NALHeader.h
	src/Codec/H265/H265NALUnit.cpp
	src/Codec/H265/H265NALUnit.h
	src/Codec/H265/H265NALUnitType.cpp
	src/Codec/H265/H265NALUnitType.h
    src/Codec/H265/H265PPS.cpp
    src/Codec/H265/H265PPS.h
    src/Codec/H265/H265ProfileTierLevel.cpp
    src/Codec/H265/H265ProfileTierLevel.h
    src/Codec/H265/H265ScalingList.cpp
    src/Codec/H265/H265ScalingList.h
    src/Codec/H265/H265SEI.cpp
    src/Codec/H265/H265SEI.h
    src/Codec/H265/H265ShortTermRefPicSet.cpp
    src/Codec/H265/H265ShortTermRefPicSet.h
    src/Codec/H265/H265Slice.cpp
    src/Codec/H265/H265Slice.h
    src/Codec/H265/H265SPS.cpp
    src/Codec/H265/H265SPS.h
    src/Codec/H265/H265Stream.cpp
    src/Codec/H265/H265Stream.h
    src/Codec/H265/H265Types.h
    src/Codec/H265/H265Utils.h
    src/Codec/H265/H265VPS.cpp
    src/Codec/H265/H265VPS.h
)

set(STREAM_SOURCES
    src/Stream/HTTPClient.cpp
    src/Stream/HTTPClient.h
    src/Stream/RTSPClient.cpp
    src/Stream/RTSPClient.h
    src/Stream/StreamClient.cpp
    src/Stream/StreamClient.h
)

set(APPLICATION_SOURCES ${APPLICATION_SOURCES} ${CODEC_SOURCES})
set(APPLICATION_SOURCES ${APPLICATION_SOURCES} ${STREAM_SOURCES})


set(APPLICATION_MOC_HEADERS
    src/GUI/QAccessUnitElement.h
    src/GUI/QErrorView.h
    src/GUI/QNALUInfoView.h
    src/GUI/QStatusView.h
    src/GUI/QStreamSettingsView.h
    src/GUI/QStreamLinkDialog.h
    src/GUI/QTimelineCounterElement.h
    src/GUI/QTimelineView.h
    src/GUI/QVideoFrameView.h
    src/GUI/QVideoInputView.h
    src/GUI/QWindowMain.h
    src/GUIController/QErrorViewController.h
    src/GUIController/QVideoInputViewController.h
    src/GUIController/QStatusViewController.h
    src/GUIController/QStreamSettingsViewController.h
    src/GUIController/QTimelineViewController.h
    src/GUIController/QVideoFrameViewController.h
    src/GUIController/QWindowMainController.h
    src/GUIModel/QAccessUnitModel.h
    src/GUIModel/QDecoderModel.h
    src/GUIModel/QFolderModel.h
    src/GUIModel/QStreamModel.h
)

qtx_wrap_cpp(
    APPLICATION_MOC_OUTFILES
	${APPLICATION_MOC_HEADERS}
)
    
set (I18N_FILES
    src/GUI/QAccessUnitElement.cpp
    src/GUI/QErrorView.cpp
    src/GUI/QVideoInputView.cpp
    src/GUI/QStatusView.cpp
    src/GUI/QStreamLinkDialog.cpp
    src/GUI/QTimelineView.cpp
    src/GUI/QVideoFrameView.cpp
    src/GUI/QWindowMain.cpp
    src/GUIModel/QDecoderModel.cpp
    src/GUIModel/QStreamModel.cpp
)
set(TS_FILES
    ts/fr.ts
)
    
option (UPDATE_TRANSLATIONS "Update source translation ts/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)" ON)
if (UPDATE_TRANSLATIONS)
	qtx_create_translation(qm_FILES ${TS_FILES} ${I18N_FILES} OPTIONS -locations none)
else (UPDATE_TRANSLATIONS)
	qtx_add_translation(qm_FILES ${TS_FILES})
endif (UPDATE_TRANSLATIONS)

add_custom_target(update_ts ALL DEPENDS ${qm_FILES})

####################
# Resource files
####################

# Configure translation file
configure_file (
	"${PROJECT_SOURCE_DIR}/ts.qrc.in"
	"${PROJECT_BINARY_DIR}/ts.qrc"
)

if (WIN32)
	set (QT_TS_FILES
		"${QT_TRANSLATIONS_DIR}/qt_fr.qm"
	)
	INSTALL(FILES ${QT_TS_FILES} DESTINATION ${INSTALL_PATH_SHARE}/translations)

	if(NOT QT_USE_VERSION LESS 5)
		set (QT_TS_FILES
			"${QT_TRANSLATIONS_DIR}/qt_en.qm"
			"${QT_TRANSLATIONS_DIR}/qtbase_en.qm"
			"${QT_TRANSLATIONS_DIR}/qtbase_fr.qm"
		)
		INSTALL(FILES ${QT_TS_FILES} DESTINATION ${INSTALL_PATH_SHARE}/translations)
	endif()
endif()

# Resources
configure_file (
	"${PROJECT_SOURCE_DIR}/data/logos/icon.rc.in"
	"${PROJECT_BINARY_DIR}/data/logos/icon.rc"
)
configure_file (
	"${PROJECT_SOURCE_DIR}/data/app.qrc.in"
	"${PROJECT_BINARY_DIR}/data/app.qrc"
)

qtx_add_resources(RCS_OUTFILES ${QRC_FILES})
qtx_add_resources(RCS_TS_OUTFILES ${QRC_TS_FILES})

IF(WIN32)
	set (RC_FILES
		"${PROJECT_BINARY_DIR}/data/logos/icon.rc"
		"platforms/win32/versioninfo.rc"
	)
	file(COPY ${PROJECT_SOURCE_DIR}/data/logos/${APPLICATION_PACKAGE_NAME}.ico DESTINATION ${PROJECT_BINARY_DIR}/data/logos/)
	# Fix resource compiler with MinGW
	if(MINGW)
		set(CMAKE_RC_COMPILER_INIT windres)
		ENABLE_LANGUAGE(RC)
		SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff -i <SOURCE> -o <OBJECT>")
	endif(MINGW)
endif(WIN32)

# Install degfault structure files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/structure_files DESTINATION ${INSTALL_PATH_SHARE})

####################
# RPATH
####################

if(UNIX AND NOT APPLE)
	# Default library path for debian
	set(CMAKE_INSTALL_RPATH "/usr/${INSTALL_PATH_LIB}")
	# Define RPATH instead of RUNPATH
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")
endif(UNIX AND NOT APPLE)


####################
# Create executable
####################

## Remove the output console
IF(WIN32)
	SET(GUI_TYPE WIN32)
ENDIF(WIN32)

# Console is useful in Debug mode
if(CMAKE_BUILD_TYPE MATCHES Debug)
	SET(GUI_TYPE "")
endif()

add_executable (${APPLICATION_EXE_NAME}
	${GUI_TYPE}
	${RC_FILES}
	${APPLICATION_SOURCES}
	${APPLICATION_MOC_OUTFILES}
	${RCS_OUTFILES}
	${RCS_TS_OUTFILES}
	${qm_FILES}
)
include_directories(${INSTALL_PATH_LIB})
target_link_libraries (${APPLICATION_EXE_NAME}
    ${PTHREAD_LIBRARIES}
    ${QT_LIBRARIES}
    ${QT_QTMAIN_LIBRARY}
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)

install(TARGETS ${APPLICATION_EXE_NAME} DESTINATION ${INSTALL_PATH_BIN})

####################
# Testing
####################

SET (TEST_SRC
    test/main.cpp
    test/Codec/H264/H264ValidSamplesParsing.cpp
    test/Codec/H264/H264ValidSamplesParsing.h
    test/Codec/H264/H264InvalidSamplesParsing.cpp
    test/Codec/H264/H264InvalidSamplesParsing.h
    test/Codec/H265/H265ValidSamplesParsing.cpp
    test/Codec/H265/H265ValidSamplesParsing.h
    test/Codec/H265/H265InvalidSamplesParsing.cpp
    test/Codec/H265/H265InvalidSamplesParsing.h
)

set (TEST_EXE ${APPLICATION_EXE_NAME}-testsuite)
add_executable (${TEST_EXE} ${TEST_SRC} ${CODEC_SOURCES} ${UNITTEST_MOC_OUTFILES})

set_target_properties(${TEST_EXE} PROPERTIES 
    COMPILE_FLAGS -DUNITTEST
    AUTOMOC ON
)

target_link_libraries (${TEST_EXE}
    ${QT_LIBRARIES}
	${QT_QTMAIN_LIBRARY}
)
install(TARGETS ${TEST_EXE} DESTINATION ${INSTALL_PATH_BIN})

############################
# Common files installation
############################

# Documentation file
set(DOCFILES_EXT "")
if(WIN32)
	set(DOCFILES_EXT ".txt")
endif(WIN32)

# README file
configure_file (
	"${PROJECT_SOURCE_DIR}/README.md"
	"${PROJECT_BINARY_DIR}/README${DOCFILES_EXT}"
	NEWLINE_STYLE ${AUTO_NEWLINE_STYLE}
)
install(FILES "${PROJECT_BINARY_DIR}/README${DOCFILES_EXT}" DESTINATION ${INSTALL_PATH_DOC})

# COPYING file
configure_file (
	"${PROJECT_SOURCE_DIR}/COPYING"
	"${PROJECT_BINARY_DIR}/COPYING${DOCFILES_EXT}"
	NEWLINE_STYLE ${AUTO_NEWLINE_STYLE}
)
# ChangeLog file
set(CHANGELOG "ChangeLog")
install(FILES "${PROJECT_SOURCE_DIR}/${CHANGELOG}" DESTINATION ${INSTALL_PATH_DOC} RENAME ChangeLog${DOCFILES_EXT})

# Install icons theme
#install(DIRECTORY ${PROJECT_BINARY_DIR}/data/theme/icons/
#	DESTINATION ${INSTALL_PATH_SHARE}/icons_themes/
#	PATTERN ".svn" EXCLUDE
#	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
#)

# Desktop file
configure_file (
	"${PROJECT_SOURCE_DIR}/data/desktop.in"
	"${PROJECT_BINARY_DIR}/data/${APPLICATION_PACKAGE_NAME}.desktop"
)
if(UNIX AND NOT WIN32)
	install(FILES ${PROJECT_BINARY_DIR}/data/${APPLICATION_PACKAGE_NAME}.desktop DESTINATION ${INSTALL_PATH_APPLICATIONS})
endif(UNIX AND NOT WIN32)

# Application icons
if(UNIX AND NOT WIN32)
	file(GLOB LIST_ICONS RELATIVE ${PROJECT_SOURCE_DIR}/data/logos ${PROJECT_SOURCE_DIR}/data/logos/*${APPLICATION_PACKAGE_NAME}.png)
	foreach(ICON ${LIST_ICONS})
		set(ICON_DST "")
		if(${ICON} MATCHES "_[^_]*${APPLICATION_PACKAGE_NAME}.png")
			STRING(REGEX REPLACE "_[^_]*${APPLICATION_PACKAGE_NAME}.png" "" ICON_DST ${ICON})
			STRING(REPLACE "_" "/" ICON_DST ${ICON_DST})
			set(ICON_DST ${INSTALL_PATH_ICONS}/${ICON_DST})
		else()
			set(ICON_DST ${INSTALL_PATH_ICONS})
		endif()
		install(FILES ${PROJECT_SOURCE_DIR}/data/logos/${ICON}
			DESTINATION ${ICON_DST} RENAME ${APPLICATION_PACKAGE_NAME}.png
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
		)
	endforeach(ICON)
endif(UNIX AND NOT WIN32)

#################
# Build package
#################

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_PATH_BIN})
INCLUDE(InstallRequiredSystemLibraries)

# CPack confile file
configure_file(${PROJECT_SOURCE_DIR}/CPackOptions.cmake.in
	${PROJECT_BINARY_DIR}/CPackOptions.cmake
	@ONLY)
set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/CPackOptions.cmake)

if(WIN32)
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		SET(CPACK_GENERATOR ZIP)
	else()
		SET(CPACK_GENERATOR ZIP NSIS)
	endif()
elseif(APPLE)
	configure_file(${PROJECT_SOURCE_DIR}/platforms/osx/Info.plist.in
		${PROJECT_BINARY_DIR}/platforms/osx/Info.plist
		@ONLY)
	configure_file(${PROJECT_SOURCE_DIR}/platforms/osx/run.sh.in
		${PROJECT_BINARY_DIR}/platforms/osx/${APPLICATION_NAME}
		@ONLY)
	SET(CPACK_GENERATOR Bundle)
else()
	SET(CPACK_GENERATOR TGZ DEB)
endif()

SET(CPACK_PACKAGE_NAME ${APPLICATION_PACKAGE_NAME})
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${APPLICATION_DESCRIPTION_SUMMARY})
SET(CPACK_PACKAGE_VENDOR ${APPLICATION_VENDOR_NAME})
SET(CPACK_PACKAGE_CONTACT ${APPLICATION_CONTACT})
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_BINARY_DIR}/README${DOCFILES_EXT}")
SET(CPACK_PACKAGE_VERSION_MAJOR ${APPLICATION_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${APPLICATION_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${APPLICATION_VERSION_PATCH}${APPLICATION_VERSION_BUILD})
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/COPYING${DOCFILES_EXT}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${APPLICATION_PACKAGE_NAME}) # Useful for registry key and NSIS auto uninstaller

if(CMAKE_BUILD_TYPE MATCHES Debug)
	SET(CPACK_STRIP_FILES FALSE)
	SET(CPACK_SOURCE_STRIP_FILES FALSE)
else()
	SET(CPACK_STRIP_FILES TRUE)
	SET(CPACK_SOURCE_STRIP_FILES TRUE)
endif()

if(WIN32)
	# NSIS
	SET(CPACK_NSIS_DISPLAY_NAME "${APPLICATION_NAME}")
elseif(APPLE)
	if (NOT CPACK_SYSTEM_NAME)
		IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
			SET(CPACK_SYSTEM_NAME amd64)
		ELSEIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
			SET(CPACK_SYSTEM_NAME i386)
		ENDIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
	endif()

	SET(CPACK_BUNDLE_NAME "${APPLICATION_NAME}")
else()
	# DEB instruction files
	configure_file (
		"${PROJECT_SOURCE_DIR}/platforms/debian/postinst.in"
		"${PROJECT_BINARY_DIR}/platforms/debian/postinst"
	)
	configure_file (
		"${PROJECT_SOURCE_DIR}/platforms/debian/prerm.in"
		"${PROJECT_BINARY_DIR}/platforms/debian/prerm"
	)

	if (NOT CPACK_SYSTEM_NAME)
		IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
			SET(CPACK_SYSTEM_NAME amd64)
		ELSEIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
			SET(CPACK_SYSTEM_NAME i386)
		ENDIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
	endif()

	JOINLIST(PACKAGE_DEPS ", " DEBIAN_DEPS)

	SET(CPACK_DEBIAN_PACKAGE_NAME ${APPLICATION_PACKAGE_NAME})
	SET(CPACK_DEBIAN_PACKAGE_VERSION ${APPLICATION_VERSION})
	SET(CPACK_DEBIAN_PACKAGE_DEPENDS ${DEBIAN_DEPS})
	SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_BINARY_DIR}/platforms/debian/postinst;${PROJECT_BINARY_DIR}/platforms/debian/prerm;")
	SET(CPACK_DEBIAN_PACKAGE_SECTION "misc")
	# SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${DEB_ARCH})
endif()
SET(CPACK_PACKAGE_EXECUTABLES "${APPLICATION_EXE_NAME}" "${APPLICATION_NAME}")
include(CPack)