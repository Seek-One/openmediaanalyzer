#include <QDebug>

#include "../GUI/QVideoInputView.h"
#include "../GUIModel/QFolderModel.h"
#include "../GUIModel/QStreamModel.h"
#include "../GUIModel/QDecoderModel.h"

#include "QVideoInputViewController.h"

QVideoInputViewController::QVideoInputViewController(QVideoInputView* pVideoInputView, QFolderModel* pFolderModel, QStreamModel* pStreamModel, QDecoderModel* pDecoderModel)
    : m_pVideoInputView(pVideoInputView), m_pFolderModel(pFolderModel), m_pStreamModel(pStreamModel), m_pDecoderModel(pDecoderModel)
{
    connect(m_pFolderModel, &QFolderModel::updateVideoInputView, m_pVideoInputView, &QVideoInputView::folderSet);
    connect(this, &QVideoInputViewController::openStream, m_pVideoInputView, &QVideoInputView::streamSet);
    
    connect(this, &QVideoInputViewController::openFolder, m_pFolderModel, &QFolderModel::folderLoaded);
    connect(this, &QVideoInputViewController::openStream, m_pStreamModel, &QStreamModel::streamLoaded);
    
    connect(m_pFolderModel, &QFolderModel::loadFolderStart, m_pDecoderModel, &QDecoderModel::reset);
    connect(m_pFolderModel, &QFolderModel::loadH264File, m_pDecoderModel, &QDecoderModel::h264FileLoaded);
    connect(m_pFolderModel, &QFolderModel::loadH265File, m_pDecoderModel, &QDecoderModel::h265FileLoaded);
    
    connect(m_pStreamModel, &QStreamModel::loadStreamStart, m_pDecoderModel, &QDecoderModel::reset);
    connect(m_pStreamModel, &QStreamModel::loadH264Packet, m_pDecoderModel, &QDecoderModel::h264PacketLoaded);
    connect(m_pStreamModel, &QStreamModel::loadH265Packet, m_pDecoderModel, &QDecoderModel::h265PacketLoaded);

    connect(m_pStreamModel, &QStreamModel::updateStatusBitrates, m_pVideoInputView, &QVideoInputView::bitratesUpdated);
    connect(m_pStreamModel, &QStreamModel::updateContentType, m_pVideoInputView, &QVideoInputView::contentTypeUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateGOVLength, m_pVideoInputView, &QVideoInputView::GOVLengthUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateValidity, m_pVideoInputView, &QVideoInputView::frameValidityUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateCodedSize, m_pVideoInputView, &QVideoInputView::codedSizeUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateDecodedSize, m_pVideoInputView, &QVideoInputView::decodedSizeUpdated);

    connect(m_pFolderModel, &QFolderModel::loadFolderEnd, m_pDecoderModel, &QDecoderModel::folderLoaded);
}

QVideoInputViewController::~QVideoInputViewController(){
}
