#include <QFileDialog>
#include <QDebug>

#include "../../lib/Threading/QWorkerThread.h"
#include "../GUI/QWindowMain.h"
#include "../GUI/QNALUInfoView.h"
#include "../GUIModel/QFolderModel.h"
#include "../GUIModel/QStreamModel.h"
#include "../GUIModel/QDecoderModel.h"
#include "QFolderViewController.h"
#include "QTimelineViewController.h"
#include "QErrorViewController.h"

#include "QWindowMainController.h"

QWindowMainController::QWindowMainController(){
    m_pFolderModel = new QFolderModel();
    m_pStreamModel = new QStreamModel();
    m_pDecoderModel = new QDecoderModel();
    m_pWindowMain = nullptr;
    m_pFolderViewController = nullptr;
    m_pTimelineViewController = nullptr;
    m_pErrorViewController = nullptr;
}

QWindowMainController::~QWindowMainController(){
    if(m_pFolderModel) delete m_pFolderModel;
    if(m_pStreamModel) delete m_pStreamModel;
    if(m_pFolderViewController) delete m_pFolderViewController;
    if(m_pTimelineViewController) delete m_pTimelineViewController;
    if(m_pErrorViewController) delete m_pErrorViewController;
}

void QWindowMainController::init(QWindowMain* pWindowMain){
    m_pWindowMain = pWindowMain;
    m_pFolderViewController = new QFolderViewController(pWindowMain->getFolderView(), m_pFolderModel, m_pStreamModel, m_pDecoderModel);
    m_pTimelineViewController = new QTimelineViewController(pWindowMain->getTimelineView(), m_pDecoderModel);
    m_pErrorViewController = new QErrorViewController(pWindowMain->getErrorView(), m_pDecoderModel);

    connect(m_pWindowMain, &QWindowMain::openFolderClicked, this, &QWindowMainController::folderOpened);
    connect(m_pWindowMain, &QWindowMain::openStreamClicked, this, &QWindowMainController::streamOpened);
    connect(this, &QWindowMainController::openFolder, m_pFolderViewController, &QFolderViewController::folderOpened);
    connect(this, &QWindowMainController::openStream, m_pFolderViewController, &QFolderViewController::streamOpened);
    connect(m_pFolderModel, &QFolderModel::loadFolderStart, m_pTimelineViewController, &QTimelineViewController::timelineStarted);
    connect(m_pStreamModel, &QStreamModel::loadFolderStart, m_pTimelineViewController, &QTimelineViewController::timelineStarted);
    
    
    connect(m_pDecoderModel, &QDecoderModel::updateSPSInfoView, m_pWindowMain->getSPSInfoView(), &QNALUInfoView::viewUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updatePPSInfoView, m_pWindowMain->getPPSInfoView(), &QNALUInfoView::viewUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateFrameInfoView, m_pWindowMain->getFrameInfoView(), &QNALUInfoView::viewUpdated);
    connect(m_pWindowMain, &QWindowMain::openFramesTab, m_pDecoderModel, &QDecoderModel::framesTabOpened);
    connect(m_pWindowMain, &QWindowMain::openSPSTab, m_pDecoderModel, &QDecoderModel::spsTabOpened);
    connect(m_pWindowMain, &QWindowMain::openPPSTab, m_pDecoderModel, &QDecoderModel::ppsTabOpened);
    connect(m_pDecoderModel, &QDecoderModel::updateErrorView, pWindowMain, &QWindowMain::errorViewToggled);

    connect(m_pWindowMain, &QWindowMain::stop, m_pStreamModel, &QStreamModel::stopRunning);
    QWorkerThread::execWorker(m_pStreamModel, false);
}

void QWindowMainController::folderOpened(){
    QString selectedFolder = QFileDialog::getExistingDirectory(m_pWindowMain, "Choose a folder");
    if(!selectedFolder.isEmpty()) {
        m_pStreamModel->stopProcessing();
        emit openFolder(selectedFolder);
    }
}

void QWindowMainController::streamOpened(){
    QString selectedFolder = QFileDialog::getExistingDirectory(m_pWindowMain, "Choose a folder");
    if(!selectedFolder.isEmpty()) {
        m_pStreamModel->stopProcessing();
        emit openStream(selectedFolder);
    }
}

