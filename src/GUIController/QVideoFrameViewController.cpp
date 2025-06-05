#include <QDebug>

#include "../GUI/QVideoFrameView.h"
#include "../GUIModel/QDecoderModel.h"
#include "../GUIModel/QStreamModel.h"

#include "QVideoFrameViewController.h"

QVideoFrameViewController::QVideoFrameViewController(QVideoFrameView* pVideoFrameView, QDecoderModel* pDecoderModel, QStreamModel* pStreamModel)
    : m_pVideoFrameView(pVideoFrameView), m_pDecoderModel(pDecoderModel), m_pStreamModel(pStreamModel)
{
    connect(m_pDecoderModel, &QDecoderModel::updateVideoFrameViewImage, m_pVideoFrameView, &QVideoFrameView::frameLoaded);
    connect(m_pDecoderModel, &QDecoderModel::updateVideoFrameViewText, m_pVideoFrameView, &QVideoFrameView::textWritten);
    connect(m_pStreamModel, &QStreamModel::detectUnsupportedVideoCodec, m_pVideoFrameView, &QVideoFrameView::unsupportedVideoCodecDetected);
}

QVideoFrameViewController::~QVideoFrameViewController(){
}