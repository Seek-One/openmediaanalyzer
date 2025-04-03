cmake_minimum_required(VERSION "3.13.4")
project(openmediaanalyzer CXX)
set (APPLICATION_EXE_NAME openmediaanalyzer)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Gui)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

include_directories (${PROJECT_SOURCE_DIR}/src)

set(APPLICATION_SOURCES
	src/main.cpp
    src/GUI/QErrorView.cpp
    src/GUI/QErrorView.h
    src/GUI/QFolderView.cpp
    src/GUI/QFolderView.h
    src/GUI/QFrameElement.cpp
    src/GUI/QFrameElement.h
    src/GUI/QNALUInfoView.cpp
    src/GUI/QNALUInfoView.h
    src/GUI/QStatusView.cpp
    src/GUI/QStatusView.h
    src/GUI/QTimelineView.cpp
    src/GUI/QTimelineView.h
    src/GUI/QVideoFrameView.cpp
    src/GUI/QVideoFrameView.h
    src/GUI/QWindowMain.cpp
    src/GUI/QWindowMain.h
    src/GUIController/QErrorViewController.cpp
    src/GUIController/QErrorViewController.h
    src/GUIController/QFolderViewController.cpp
    src/GUIController/QFolderViewController.h
    src/GUIController/QStatusViewController.cpp
    src/GUIController/QStatusViewController.h
    src/GUIController/QTimelineViewController.cpp
    src/GUIController/QTimelineViewController.h
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
    src/Codec/ByteBuffer.cpp
    src/Codec/ByteBuffer.h
    src/Codec/datatypecc.cpp
    src/Codec/datatypecc.h
    src/Codec/fourcc.cpp
    src/Codec/fourcc.h
    src/Codec/FrameBuffer.cpp
    src/Codec/FrameBuffer.h
    src/Codec/FrameDataType.cpp
    src/Codec/FrameDataType.h
    src/Codec/FrameMonitor.cpp
    src/Codec/FrameMonitor.h
    src/Codec/Size.cpp
    src/Codec/Size.h
    src/Codec/Timeval.cpp
    src/Codec/Timeval.h
    src/Codec/twocc.cpp
    src/Codec/twocc.h
    src/Codec/H26X/H26XBitstreamReader.cpp
    src/Codec/H26X/H26XBitstreamReader.h
    src/Codec/H26X/H26XMath.cpp
    src/Codec/H26X/H26XMath.h
    src/Codec/H26X/H26XStream.cpp
    src/Codec/H26X/H26XStream.h
    src/Codec/H26X/H26XUtils.cpp
    src/Codec/H26X/H26XUtils.h
    src/Codec/H264/H264AccessUnit.cpp
    src/Codec/H264/H264AccessUnit.h
    src/Codec/H264/H264AUD.cpp
    src/Codec/H264/H264AUD.h
    src/Codec/H264/H264BitstreamReader.cpp
    src/Codec/H264/H264BitstreamReader.h
    src/Codec/H264/H264GOP.cpp
    src/Codec/H264/H264GOP.h
    src/Codec/H264/H264NAL.cpp
    src/Codec/H264/H264NAL.h
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
    src/Codec/H264/H264Utils.cpp
    src/Codec/H264/H264Utils.h
    src/Codec/H265/H265BitstreamReader.cpp
    src/Codec/H265/H265BitstreamReader.h
    src/Codec/H265/H265NAL.cpp
    src/Codec/H265/H265NAL.h
    src/Codec/H265/H265PPS.cpp
    src/Codec/H265/H265PPS.h
    src/Codec/H265/H265ProfileTierLevel.cpp
    src/Codec/H265/H265ProfileTierLevel.h
    src/Codec/H265/H265ScalingList.cpp
    src/Codec/H265/H265ScalingList.h
    src/Codec/H265/H265ShortTermRefPicSet.cpp
    src/Codec/H265/H265ShortTermRefPicSet.h
    src/Codec/H265/H265Slice.cpp
    src/Codec/H265/H265Slice.h
    src/Codec/H265/H265SPS.cpp
    src/Codec/H265/H265SPS.h
    src/Codec/H265/H265Stream.cpp
    src/Codec/H265/H265Stream.h
    src/Codec/H265/H265Types.h
    src/Codec/H265/H265Utils.cpp
    src/Codec/H265/H265Utils.h
    src/Codec/H265/H265VPS.cpp
    src/Codec/H265/H265VPS.h
)

set(LIBRARIES
    lib/Threading/QWorkerObject.cpp
    lib/Threading/QWorkerObject.h
    lib/Threading/QWorkerObjectWithStatus.cpp
    lib/Threading/QWorkerObjectWithStatus.h
    lib/Threading/QWorkerThread.cpp
    lib/Threading/QWorkerThread.h
)

add_executable(${APPLICATION_EXE_NAME}
    ${APPLICATION_SOURCES}
    ${LIBRARIES}
)

target_link_libraries(${APPLICATION_EXE_NAME} 
    Qt5::Core 
    Qt5::Gui
    Qt5::Widgets
)

set_target_properties(${APPLICATION_EXE_NAME} PROPERTIES
    AUTOMOC ON
)