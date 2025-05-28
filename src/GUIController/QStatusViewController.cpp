#include <QDebug>

#include "../GUI/QStatusView.h"
#include "../GUIModel/QDecoderModel.h"

#include "QStatusViewController.h"

QStatusViewController::QStatusViewController(QStatusView* pStatusView, QDecoderModel* pDecoderModel)
    : m_pStatusView(pStatusView), m_pDecoderModel(pDecoderModel)
{
    connect(m_pDecoderModel, &QDecoderModel::updateSize, m_pStatusView, &QStatusView::sizeUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateValidity, m_pStatusView, &QStatusView::frameValidityUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateStatus, m_pStatusView, &QStatusView::streamStatusUpdated);
    connect(m_pStatusView, &QStatusView::setLiveContent, m_pDecoderModel, &QDecoderModel::setLiveContent);
}

QStatusViewController::~QStatusViewController(){
}