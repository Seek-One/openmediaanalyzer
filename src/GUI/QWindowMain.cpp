#include <QDebug>
#include <QGridLayout>
#include <QMenuBar>
#include <QAction>

#include "QFolderView.h"
#include "QVideoFrameView.h"
#include "QTimelineView.h"
#include "QNALUInfoView.h"
#include "QErrorView.h"

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
    m_pSPSInfoView = new QNALUInfoView(pWidget);
    m_pPPSInfoView = new QNALUInfoView(pWidget);
    m_pFrameInfoView = new QNALUInfoView(pWidget);
    m_pTimelineView = new QTimelineView(pWidget);
    m_pErrorView = new QErrorView(pWidget);

    setCentralWidget(pWidget);
    
    pWidget->setLayout(pGridLayout);
    pGridLayout->addWidget(m_pFolderView, 0, 0, 3, 1);
    pGridLayout->addWidget(m_pVideoFrameView, 0, 1, 2, 2);
    m_pTabWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    pGridLayout->addWidget(m_pTabWidget, 0, 3, m_pErrorView->isVisibleTo(this) ? 2 : 3, 1);
    m_pTabWidget->addTab(m_pFrameInfoView, "Access unit");
    m_pTabWidget->addTab(m_pSPSInfoView, "SPS");
    m_pTabWidget->addTab(m_pPPSInfoView, "PPS");

    pGridLayout->addWidget(m_pTimelineView, 2, 1, 1, 2);
    pGridLayout->addWidget(m_pErrorView, 2, 3, 1, 1);


    QMenu *pFileMenu = menuBar()->addMenu(tr("&File"));
    {
        QAction* pOpenFolderAction = new QAction(tr("&Open folder..."), this);
        pOpenFolderAction->setShortcut(QKeySequence("Ctrl+O"));
        connect(pOpenFolderAction, &QAction::triggered, [this]() {
            emit openFolderClicked();
        });
        QAction* pOpenStreamAction = new QAction(tr("&Open \"stream\"..."), this);
        connect(pOpenStreamAction, &QAction::triggered, [this]() {
            emit openStreamClicked();
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
                emit openSPSTab();
                break;
            case 2:
                emit openPPSTab();
                break;
        }
    });
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

QVideoFrameView* QWindowMain::getVideoFrameView(){
    return m_pVideoFrameView;
}

void QWindowMain::errorViewToggled(QString _, QStringList errors){
    QGridLayout* layout = (QGridLayout*)centralWidget()->layout();
    layout->removeWidget(m_pTabWidget);
    layout->addWidget(m_pTabWidget, 0, 3, errors.empty() ? 3 : 2, 1);
}

void QWindowMain::closeEvent(QCloseEvent* event){
    emit stop();
}