#include <QDebug>
#include <QGridLayout>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QSplitter>

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
	QWidget* pWidget = createSplitView(this);
	setCentralWidget(pWidget);

    m_pStatusView = new QStatusView(pWidget);
    m_pStreamLinkDialog = new QStreamLinkDialog(pWidget);

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

QWidget* QWindowMain::createSplitView(QWidget* parent)
{
	QSplitter* pMainView = new QSplitter(parent);
	pMainView->setOrientation(Qt::Horizontal);
	pMainView->setChildrenCollapsible(false);

	m_pVideoInputView = new QVideoInputView(pMainView);
	pMainView->addWidget(m_pVideoInputView);

	auto pContentView = createContentView(pMainView);
	pMainView->addWidget(pContentView);

	auto pRightView = createRightView(pMainView);
	pMainView->addWidget(pRightView);

	//pMainView->setStretchFactor(0, 1);
	pMainView->setStretchFactor(1, 1);
	//pMainView->setStretchFactor(2, 1);

	return pMainView;
}

QWidget* QWindowMain::createContentView(QWidget* parent)
{
	auto pMainView = new QWidget(parent);

	auto pBoxLayout = new QVBoxLayout();
	pMainView->setLayout(pBoxLayout);

	m_pVideoFrameView = new QVideoFrameView(pMainView);
	pBoxLayout->addWidget(m_pVideoFrameView, 1);

	m_pTimelineView = new QTimelineView(pMainView);
	pBoxLayout->addWidget(m_pTimelineView);

	m_pStreamSettingsView = new QStreamSettingsView(pMainView);
	pBoxLayout->addWidget(m_pStreamSettingsView);

	return pMainView;
}

QWidget* QWindowMain::createRightView(QWidget* parent)
{
	auto pMainWidget = new QWidget(parent);

	auto pMainLayout = new QVBoxLayout();
	pMainWidget->setLayout(pMainLayout);

	m_pTabWidget = createTabView(pMainWidget);
	pMainLayout->addWidget(m_pTabWidget);

	m_pErrorView = new QErrorView(pMainWidget);
	pMainLayout->addWidget(m_pErrorView);

	return pMainWidget;
}

QTabWidget* QWindowMain::createTabView(QWidget* parent)
{
	auto pTabWidget = new QTabWidget(parent);

	m_pFrameInfoView = new QNALUInfoView(pTabWidget);
	pTabWidget->addTab(m_pFrameInfoView, tr("Access unit"));
	m_pVPSInfoView = new QNALUInfoView(pTabWidget);
	pTabWidget->addTab(m_pVPSInfoView, "VPS");
	m_pSPSInfoView = new QNALUInfoView(pTabWidget);
	pTabWidget->addTab(m_pSPSInfoView, "SPS");
	m_pPPSInfoView = new QNALUInfoView(pTabWidget);
	pTabWidget->addTab(m_pPPSInfoView, "PPS");

	return pTabWidget;
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

QErrorView* QWindowMain::getErrorView()
{
    return m_pErrorView;
}

QStreamSettingsView* QWindowMain::getStreamSettingsView()
{
    return m_pStreamSettingsView;
}

QStatusView* QWindowMain::getStatusView()
{
    return m_pStatusView;
}

QVideoFrameView* QWindowMain::getVideoFrameView()
{
    return m_pVideoFrameView;
}

QStreamLinkDialog* QWindowMain::getStreamLinkDialog()
{
    return m_pStreamLinkDialog;
}

void QWindowMain::errorViewToggled(QString _, QStringList minorErrors, QStringList majorErrors)
{
	if(minorErrors.isEmpty() && majorErrors.isEmpty()) {
		m_pErrorView->setVisible(false);
	}else{
		m_pErrorView->setVisible(true);
	}
}

void QWindowMain::streamSettingsViewToggled(bool visible)
{
    if(visible){
		m_pStreamSettingsView->show();
	}else {
		m_pStreamSettingsView->hide();
	}
}

void QWindowMain::closeEvent(QCloseEvent* event){
    emit stop();
}