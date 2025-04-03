#pragma once

#include <QMainWindow>
#include <QTabWidget>


class QFolderView;
class QVideoFrameView;
class QTimelineView;
class QNALUInfoView;
class QErrorView;
class QStatusView;

class QWindowMain : public QMainWindow
{
    Q_OBJECT

public:
    QWindowMain(QWidget* parent = 0);

    virtual ~QWindowMain();

    QFolderView* getFolderView();
    QTimelineView* getTimelineView();
    QNALUInfoView* getSPSInfoView();
    QNALUInfoView* getPPSInfoView();
    QNALUInfoView* getFrameInfoView();
    QErrorView* getErrorView();
    QStatusView* getStatusView();

public slots:
    void errorViewToggled(QString _, QStringList errors);

signals:
    void openFolderClicked();
    void openStreamClicked();
    void openFramesTab();
    void openSPSTab();
    void openPPSTab();
    void stop();

protected:
    void closeEvent(QCloseEvent* event);

private:
    QFolderView* m_pFolderView;
    QVideoFrameView* m_pVideoFrameView;
    QTimelineView* m_pTimelineView;
    QTabWidget* m_pTabWidget;
    QNALUInfoView* m_pFrameInfoView;
    QNALUInfoView* m_pSPSInfoView;
    QNALUInfoView* m_pPPSInfoView;
    QErrorView* m_pErrorView;
    QStatusView* m_pStatusView;
};