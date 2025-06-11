#include <QDebug>
#include <QGridLayout>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>

#include "QVideoInputView.h"
#include "QVideoFrameView.h"
#include "QTimelineView.h"
#include "QNALUInfoView.h"
#include "QErrorView.h"
#include "QStreamSettingsView.h"
#include "QStatusView.h"
#include "QStreamLinkDialog.h"

#include "QWindowMain.h"


QWindowMain::QWindowMain(QWidget* parent)
    : QMainWindow(parent)
{
    const quint16 WINDOW_WIDTH = 1500;
    const quint16 WINDOW_HEIGHT = 750;
    setGeometry(200, 200, WINDOW_WIDTH, WINDOW_HEIGHT);
    setMinimumSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    QWidget* pWidget = new QWidget(parent);
    QGridLayout* pGridLayout = new QGridLayout(pWidget);
    m_pVideoInputView = new QVideoInputView(pWidget);
    m_pVideoFrameView = new QVideoFrameView(pWidget);
    m_pTabWidget = new QTabWidget(pWidget);
    m_pVPSInfoView = new QNALUInfoView(pWidget);
    m_pSPSInfoView = new QNALUInfoView(pWidget);
    m_pPPSInfoView = new QNALUInfoView(pWidget);
    m_pFrameInfoView = new QNALUInfoView(pWidget);
    m_pTimelineView = new QTimelineView(pWidget);
    m_pErrorView = new QErrorView(pWidget);
    m_pStreamSettingsView = new QStreamSettingsView(pWidget);
    m_pStatusView = new QStatusView(pWidget);
    m_pStreamLinkDialog = new QStreamLinkDialog(pWidget);

    setCentralWidget(pWidget);
    
    pWidget->setLayout(pGridLayout);
    pGridLayout->addWidget(m_pVideoInputView, 0, 0, m_pStreamSettingsView->isVisibleTo(this) ? 2 : 3, 1);
    pGridLayout->addWidget(m_pVideoFrameView, 0, 1, 2, 2);
    m_pTabWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    pGridLayout->addWidget(m_pTabWidget, 0, 3, m_pErrorView->isVisibleTo(this) ? 2 : 3, 1);
    m_pTabWidget->addTab(m_pFrameInfoView, tr("Access unit"));
    m_pTabWidget->addTab(m_pVPSInfoView, "VPS");
    m_pTabWidget->addTab(m_pSPSInfoView, "SPS");
    m_pTabWidget->addTab(m_pPPSInfoView, "PPS");

    pGridLayout->addWidget(m_pTimelineView, 2, 1, 1, 2);
    pGridLayout->addWidget(m_pStreamSettingsView, 2, 0, 1, 1);
    pGridLayout->addWidget(m_pErrorView, 2, 3, 1, 1);

    m_pVideoInputView->setMinimumWidth(WINDOW_WIDTH/5);
    m_pTabWidget->setMinimumWidth(WINDOW_WIDTH/5);


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

QVideoInputView* QWindowMain::getVideoInputView(){
    return m_pVideoInputView;
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

QStreamSettingsView* QWindowMain::getStreamSettingsView(){
    return m_pStreamSettingsView;
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

void QWindowMain::streamSettingsViewToggled(bool visible){
    if(visible) m_pStreamSettingsView->show();
    else m_pStreamSettingsView->hide();
    QGridLayout* layout = (QGridLayout*)centralWidget()->layout();
    layout->removeWidget(m_pVideoInputView);
    layout->addWidget(m_pVideoInputView, 0, 0, visible ? 2 : 3, 1);
}

void QWindowMain::closeEvent(QCloseEvent* event){
    emit stop();
}