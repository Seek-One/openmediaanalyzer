#include <QDebug>

#include "../GUI/QVideoFrameView.h"
#include "../GUIModel/QDecoderModel.h"

#include "QVideoFrameViewController.h"

QVideoFrameViewController::QVideoFrameViewController(QVideoFrameView* pVideoFrameView, QDecoderModel* pDecoderModel)
    : m_pVideoFrameView(pVideoFrameView), m_pDecoderModel(pDecoderModel)
{
    connect(m_pDecoderModel, &QDecoderModel::updateVideoFrameView, m_pVideoFrameView, &QVideoFrameView::frameLoaded);
}

QVideoFrameViewController::~QVideoFrameViewController(){
}