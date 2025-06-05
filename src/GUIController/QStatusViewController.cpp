#include <QDebug>

#include "../GUI/QStatusView.h"
#include "../GUIModel/QDecoderModel.h"
#include "../GUIModel/QStreamModel.h"

#include "QStatusViewController.h"

QStatusViewController::QStatusViewController(QStatusView* pStatusView, QDecoderModel* pDecoderModel, QStreamModel* pStreamModel)
    : m_pStatusView(pStatusView), m_pDecoderModel(pDecoderModel), m_pStreamModel(pStreamModel)
{
    connect(m_pDecoderModel, &QDecoderModel::updateSize, m_pStatusView, &QStatusView::sizeUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateValidity, m_pStatusView, &QStatusView::frameValidityUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateStatus, m_pStatusView, &QStatusView::videoStatusUpdated);

    connect(m_pStreamModel, &QStreamModel::updateStatusBitrates, m_pStatusView, &QStatusView::bitratesUpdated);
}

QStatusViewController::~QStatusViewController(){
}