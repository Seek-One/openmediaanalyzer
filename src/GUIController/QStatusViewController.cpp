#include <QDebug>

#include "../GUI/QStatusView.h"
#include "../GUIModel/QDecoderModel.h"
#include "../GUIModel/QStreamModel.h"

#include "QStatusViewController.h"

QStatusViewController::QStatusViewController(QStatusView* pStatusView, QDecoderModel* pDecoderModel)
    : m_pStatusView(pStatusView), m_pDecoderModel(pDecoderModel)
{
    connect(m_pDecoderModel, &QDecoderModel::updateStatus, m_pStatusView, &QStatusView::videoStatusUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateValidity, m_pStatusView, &QStatusView::frameValidityUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateCodedSize, m_pStatusView, &QStatusView::codedSizeUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateDecodedSize, m_pStatusView, &QStatusView::decodedSizeUpdated);
}

QStatusViewController::~QStatusViewController(){
}