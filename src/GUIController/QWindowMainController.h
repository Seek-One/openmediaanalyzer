#pragma once

#include <QObject>

class QWindowMain;
class QFolderModel;
class QStreamModel;
class QDecoderModel;
class QVideoInputViewController;
class QTimelineViewController;
class QErrorViewController;
class QStatusViewController;
class QVideoFrameViewController;

class QWindowMainController : public QObject
{
    Q_OBJECT
public:
    QWindowMainController();

    virtual ~QWindowMainController();

    void init(QWindowMain* pWindowMain);

public slots:
    void folderOpened();

signals:
    void openFolder(const QString& folderPath);
    void openStream();

private:
    QWindowMain* m_pWindowMain;
    QFolderModel* m_pFolderModel;
    QStreamModel* m_pStreamModel;
    QDecoderModel* m_pDecoderModel;
    QVideoInputViewController* m_pVideoInputViewController;
    QTimelineViewController* m_pTimelineViewController;
    QErrorViewController* m_pErrorViewController;
    QStatusViewController* m_pStatusViewController;
    QVideoFrameViewController* m_pVideoFrameViewController;
};