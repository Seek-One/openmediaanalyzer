#include <QDebug>

#include "../GUI/QErrorView.h"
#include "../GUIModel/QDecoderModel.h"

#include "QErrorViewController.h"

QErrorViewController::QErrorViewController(QErrorView* pErrorView, QDecoderModel* pDecoderModel)
    : m_pErrorView(pErrorView), m_pStreamModel(pDecoderModel)
{
    connect(m_pStreamModel, &QDecoderModel::updateErrorView, m_pErrorView, &QErrorView::setErrors);
}

QErrorViewController::~QErrorViewController(){
}