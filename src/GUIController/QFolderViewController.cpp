#include <QDebug>

#include "../GUI/QFolderView.h"
#include "../GUIModel/QFolderModel.h"
#include "../GUIModel/QStreamModel.h"
#include "../GUIModel/QDecoderModel.h"

#include "QFolderViewController.h"

QFolderViewController::QFolderViewController(QFolderView* pFolderView, QFolderModel* pFolderModel, QStreamModel* pStreamModel, QDecoderModel* pDecoderModel)
    : m_pFolderView(pFolderView), m_pFolderModel(pFolderModel), m_pStreamModel(pStreamModel), m_pDecoderModel(pDecoderModel)
{
    connect(m_pFolderModel, &QFolderModel::updateFolderView, m_pFolderView, &QFolderView::setFolder);
    connect(m_pStreamModel, &QStreamModel::updateFolderView, m_pFolderView, &QFolderView::setFolder);

    connect(this, &QFolderViewController::openFolder, m_pFolderModel, &QFolderModel::folderLoaded);
    connect(this, &QFolderViewController::openStream, m_pStreamModel, &QStreamModel::streamLoaded);

    connect(m_pFolderModel, &QFolderModel::loadFolderStart, m_pDecoderModel, &QDecoderModel::reset);
    connect(m_pFolderModel, &QFolderModel::loadH264File, m_pDecoderModel, &QDecoderModel::h264FileLoaded);
    connect(m_pFolderModel, &QFolderModel::loadH265File, m_pDecoderModel, &QDecoderModel::h265FileLoaded);

    connect(m_pStreamModel, &QStreamModel::loadFolderStart, m_pDecoderModel, &QDecoderModel::reset);
    connect(m_pStreamModel, &QStreamModel::loadH264File, m_pDecoderModel, &QDecoderModel::h264FileLoaded);
    connect(m_pStreamModel, &QStreamModel::loadH265File, m_pDecoderModel, &QDecoderModel::h265FileLoaded);

    connect(m_pFolderModel, &QFolderModel::loadFolderEnd, m_pDecoderModel, &QDecoderModel::folderLoaded);
}

QFolderViewController::~QFolderViewController(){
}

void QFolderViewController::folderOpened(const QString& folderPath){
    emit openFolder(folderPath);
}

void QFolderViewController::streamOpened(const QString& folderPath){
    emit openStream(folderPath);
}