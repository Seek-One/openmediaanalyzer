#include <QFileDialog>
#include <QApplication>
#include <QDebug>

#include "../GUI/QWindowMain.h"
#include "../GUI/QNALUInfoView.h"
#include "../GUI/QStatusView.h"
#include "../GUI/QStreamLinkDialog.h"
#include "../GUIModel/QFolderModel.h"
#include "../GUIModel/QStreamModel.h"
#include "../GUIModel/QDecoderModel.h"
#include "QFolderViewController.h"
#include "QTimelineViewController.h"
#include "QErrorViewController.h"
#include "QStatusViewController.h"
#include "QVideoFrameViewController.h"

#include "QWindowMainController.h"

QWindowMainController::QWindowMainController(){
    m_pFolderModel = new QFolderModel();
    m_pStreamModel = new QStreamModel();
    m_pDecoderModel = new QDecoderModel();
    m_pWindowMain = nullptr;
    m_pFolderViewController = nullptr;
    m_pTimelineViewController = nullptr;
    m_pErrorViewController = nullptr;
    m_pStatusViewController = nullptr;
    m_pVideoFrameViewController = nullptr;
}

QWindowMainController::~QWindowMainController(){
    if(m_pFolderModel) delete m_pFolderModel;
    if(m_pStreamModel) delete m_pStreamModel;
    if(m_pFolderViewController) delete m_pFolderViewController;
    if(m_pTimelineViewController) delete m_pTimelineViewController;
    if(m_pErrorViewController) delete m_pErrorViewController;
    if(m_pStatusViewController) delete m_pStatusViewController;
    if(m_pVideoFrameViewController) delete m_pVideoFrameViewController;
}

void QWindowMainController::init(QWindowMain* pWindowMain){
    m_pWindowMain = pWindowMain;
    m_pFolderViewController = new QFolderViewController(pWindowMain->getFolderView(), m_pFolderModel, m_pStreamModel, m_pDecoderModel);
    m_pTimelineViewController = new QTimelineViewController(pWindowMain->getTimelineView(), m_pDecoderModel);
    m_pErrorViewController = new QErrorViewController(pWindowMain->getErrorView(), m_pDecoderModel);
    m_pStatusViewController = new QStatusViewController(pWindowMain->getStatusView(), m_pDecoderModel);
    m_pVideoFrameViewController = new QVideoFrameViewController(pWindowMain->getVideoFrameView(), m_pDecoderModel);

    connect(m_pWindowMain, &QWindowMain::openFolderClicked, this, &QWindowMainController::folderOpened);
    connect(m_pWindowMain, &QWindowMain::openFolderClicked, m_pStreamModel, &QStreamModel::streamStopped);
    connect(m_pWindowMain, &QWindowMain::stopStreamClicked, m_pStreamModel, &QStreamModel::streamStopped);
    connect(m_pWindowMain->getStreamLinkDialog(), &QStreamLinkDialog::accessStream ,m_pFolderViewController, &QFolderViewController::openStream);
    connect(this, &QWindowMainController::openFolder, m_pFolderViewController, &QFolderViewController::openFolder);
    connect(m_pFolderModel, &QFolderModel::loadFolderStart, m_pTimelineViewController, &QTimelineViewController::startTimeline);
    connect(m_pStreamModel, &QStreamModel::loadStreamStart, m_pTimelineViewController, &QTimelineViewController::startTimeline);
    connect(m_pWindowMain->getStatusView(), &QStatusView::setLiveContent, m_pTimelineViewController, &QTimelineViewController::setLiveContent);
    
    
    connect(m_pDecoderModel, &QDecoderModel::updateVPSInfoView, m_pWindowMain->getVPSInfoView(), &QNALUInfoView::viewUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateSPSInfoView, m_pWindowMain->getSPSInfoView(), &QNALUInfoView::viewUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updatePPSInfoView, m_pWindowMain->getPPSInfoView(), &QNALUInfoView::viewUpdated);
    connect(m_pDecoderModel, &QDecoderModel::updateFrameInfoView, m_pWindowMain->getFrameInfoView(), &QNALUInfoView::viewUpdated);
    connect(m_pWindowMain, &QWindowMain::openFramesTab, m_pDecoderModel, &QDecoderModel::framesTabOpened);
    connect(m_pWindowMain, &QWindowMain::openVPSTab, m_pDecoderModel, &QDecoderModel::vpsTabOpened);
    connect(m_pWindowMain, &QWindowMain::openSPSTab, m_pDecoderModel, &QDecoderModel::spsTabOpened);
    connect(m_pWindowMain, &QWindowMain::openPPSTab, m_pDecoderModel, &QDecoderModel::ppsTabOpened);
    connect(m_pDecoderModel, &QDecoderModel::updateErrorView, pWindowMain, &QWindowMain::errorViewToggled);

    connect(m_pWindowMain, &QWindowMain::stop, m_pStreamModel, &QStreamModel::streamStopped);

}

void QWindowMainController::folderOpened(){
    QString selectedFolder = QFileDialog::getExistingDirectory(m_pWindowMain, "Choose a folder");
    if(!selectedFolder.isEmpty()) {
        emit openFolder(selectedFolder);
    }
}
