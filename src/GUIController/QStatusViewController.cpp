#include <QDebug>

#include "../GUI/QStatusView.h"
#include "../GUIModel/QDecoderModel.h"
#include "../GUIModel/QStreamModel.h"

#include "QStatusViewController.h"

QStatusViewController::QStatusViewController(QStatusView* pStatusView, QDecoderModel* pDecoderModel)
    : m_pStatusView(pStatusView), m_pDecoderModel(pDecoderModel)
{
    connect(m_pDecoderModel, &QDecoderModel::updateStatus, m_pStatusView, &QStatusView::videoStatusUpdated);
    connect(this, &QStatusViewController::openFolder, m_pStatusView, &QStatusView::folderPathSet);
    connect(this, &QStatusViewController::openStream, m_pStatusView, &QStatusView::streamLinkSet);
}

QStatusViewController::~QStatusViewController(){
}