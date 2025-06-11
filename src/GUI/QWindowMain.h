#pragma once

#include <QMainWindow>
#include <QTabWidget>


class QVideoInputView;
class QVideoFrameView;
class QTimelineView;
class QNALUInfoView;
class QErrorView;
class QStreamSettingsView;
class QStatusView;
class QStreamLinkDialog;

class QWindowMain : public QMainWindow
{
    Q_OBJECT

public:
    QWindowMain(QWidget* parent = 0);

    virtual ~QWindowMain();

    QVideoInputView* getVideoInputView();
    QTimelineView* getTimelineView();
    QNALUInfoView* getVPSInfoView();
    QNALUInfoView* getSPSInfoView();
    QNALUInfoView* getPPSInfoView();
    QNALUInfoView* getFrameInfoView();
    QErrorView* getErrorView();
    QStreamSettingsView* getStreamSettingsView();
    QStatusView* getStatusView();
    QVideoFrameView* getVideoFrameView();
    QStreamLinkDialog* getStreamLinkDialog();

public slots:
    void errorViewToggled(QString _, QStringList minorErrors, QStringList majorErrors);
    void streamSettingsViewToggled(bool visible);

signals:
    void openFolderClicked();
    void openStreamClicked();
    void openFramesTab();
    void openVPSTab();
    void openSPSTab();
    void openPPSTab();
    void stop();

protected:
    void closeEvent(QCloseEvent* event);

private:
    QVideoInputView* m_pVideoInputView;
    QVideoFrameView* m_pVideoFrameView;
    QTimelineView* m_pTimelineView;
    QTabWidget* m_pTabWidget;
    QNALUInfoView* m_pFrameInfoView;
    QNALUInfoView* m_pVPSInfoView;
    QNALUInfoView* m_pSPSInfoView;
    QNALUInfoView* m_pPPSInfoView;
    QErrorView* m_pErrorView;
    QStreamSettingsView* m_pStreamSettingsView;
    QStatusView* m_pStatusView;
    QStreamLinkDialog* m_pStreamLinkDialog;
};