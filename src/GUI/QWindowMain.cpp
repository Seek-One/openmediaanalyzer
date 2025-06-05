#include <QDebug>
#include <QGridLayout>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>

#include "QFolderView.h"
#include "QVideoFrameView.h"
#include "QTimelineView.h"
#include "QNALUInfoView.h"
#include "QErrorView.h"
#include "QStatusView.h"
#include "QStreamLinkDialog.h"

#include "QWindowMain.h"


QWindowMain::QWindowMain(QWidget* parent)
    : QMainWindow(parent)
{
    setGeometry(200, 200, 1500, 750);
    QWidget* pWidget = new QWidget(parent);
    QGridLayout* pGridLayout = new QGridLayout(pWidget);
    m_pFolderView = new QFolderView(pWidget);
    m_pVideoFrameView = new QVideoFrameView(pWidget);
    m_pTabWidget = new QTabWidget(pWidget);
    m_pVPSInfoView = new QNALUInfoView(pWidget);
    m_pSPSInfoView = new QNALUInfoView(pWidget);
    m_pPPSInfoView = new QNALUInfoView(pWidget);
    m_pFrameInfoView = new QNALUInfoView(pWidget);
    m_pTimelineView = new QTimelineView(pWidget);
    m_pErrorView = new QErrorView(pWidget);
    m_pStatusView = new QStatusView(pWidget);
    m_pStreamLinkDialog = new QStreamLinkDialog(pWidget);

    setCentralWidget(pWidget);
    
    pWidget->setLayout(pGridLayout);
    pGridLayout->addWidget(m_pFolderView, 0, 0, 3, 1);
    pGridLayout->addWidget(m_pVideoFrameView, 0, 1, 2, 2);
    m_pTabWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    pGridLayout->addWidget(m_pTabWidget, 0, 3, m_pErrorView->isVisibleTo(this) ? 2 : 3, 1);
    m_pTabWidget->addTab(m_pFrameInfoView, tr("Access unit"));
    m_pTabWidget->addTab(m_pVPSInfoView, "VPS");
    m_pTabWidget->addTab(m_pSPSInfoView, "SPS");
    m_pTabWidget->addTab(m_pPPSInfoView, "PPS");

    pGridLayout->addWidget(m_pTimelineView, 2, 1, 1, 2);
    pGridLayout->addWidget(m_pErrorView, 2, 3, 1, 1);


    QMenu *pFileMenu = menuBar()->addMenu(tr("File"));
    {
        QAction* pOpenFolderAction = new QAction(tr("Open folder..."), this);
        pOpenFolderAction->setShortcut(QKeySequence("Ctrl+O"));
        connect(pOpenFolderAction, &QAction::triggered, [this]() {
            emit openFolderClicked();
        });
        QAction* pOpenStreamAction = new QAction(tr("Open stream..."), this);
        pOpenStreamAction->setShortcut(QKeySequence("Ctrl+R"));
        connect(pOpenStreamAction, &QAction::triggered, [this]() {
            emit openStreamClicked();
            m_pStreamLinkDialog->show();
        });
        
        pFileMenu->addAction(pOpenFolderAction);
        pFileMenu->addAction(pOpenStreamAction);
    }

    QMenu *pStreamMenu = menuBar()->addMenu(tr("Stream"));
    {
        QAction* pToggleLiveContent = new QAction(tr("Live content"), this);
        pToggleLiveContent->setShortcut(QKeySequence("Ctrl+L"));
        pToggleLiveContent->setCheckable(true);
        pToggleLiveContent->setChecked(true);
        connect(pToggleLiveContent, &QAction::toggled, [this, pToggleLiveContent]() {
            emit setLiveContent(pToggleLiveContent->isChecked());
        });
        QAction* pStopStreamThread = new QAction(tr("Stop stream..."), this);
        pStopStreamThread->setShortcut(QKeySequence("Ctrl+C"));
        connect(pStopStreamThread, &QAction::triggered, [this]() {
            emit stopStreamClicked();
        });
        
        pStreamMenu->addAction(pToggleLiveContent);
        pStreamMenu->addAction(pStopStreamThread);
    }
    
    
    pWidget->show();
    
    connect(m_pTabWidget, &QTabWidget::currentChanged, this, [this](int index){
        switch(index){
            default:
            case -1: return;
            case 0:
            emit openFramesTab();
            break;
            case 1:
            emit openVPSTab();
            break;
            case 2:
            emit openSPSTab();
            break;
            case 3:
            emit openPPSTab();
            break;
        }
    });
    
    statusBar()->addPermanentWidget(m_pStatusView, 1);
}

QWindowMain::~QWindowMain()
{
}

QFolderView* QWindowMain::getFolderView(){
    return m_pFolderView;
}

QTimelineView* QWindowMain::getTimelineView(){
    return m_pTimelineView;
}

QNALUInfoView* QWindowMain::getVPSInfoView(){
    return m_pVPSInfoView;
}

QNALUInfoView* QWindowMain::getSPSInfoView(){
    return m_pSPSInfoView;
}

QNALUInfoView* QWindowMain::getPPSInfoView(){
    return m_pPPSInfoView;
}

QNALUInfoView* QWindowMain::getFrameInfoView(){
    return m_pFrameInfoView;
}

QErrorView* QWindowMain::getErrorView(){
    return m_pErrorView;
}

QStatusView* QWindowMain::getStatusView(){
    return m_pStatusView;
}

QVideoFrameView* QWindowMain::getVideoFrameView(){
    return m_pVideoFrameView;
}

QStreamLinkDialog* QWindowMain::getStreamLinkDialog(){
    return m_pStreamLinkDialog;
}

void QWindowMain::errorViewToggled(QString _, QStringList minorErrors, QStringList majorErrors){
    QGridLayout* layout = (QGridLayout*)centralWidget()->layout();
    layout->removeWidget(m_pTabWidget);
    layout->addWidget(m_pTabWidget, 0, 3, minorErrors.empty() && majorErrors.empty() ? 3 : 2, 1);
}

void QWindowMain::closeEvent(QCloseEvent* event){
    emit stop();
}