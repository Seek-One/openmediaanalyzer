#pragma once

#include <QObject>

class QWindowMain;
class QFolderModel;
class QStreamModel;
class QDecoderModel;
class QFolderViewController;
class QTimelineViewController;
class QErrorViewController;
class QStatusViewController;

class QWindowMainController : public QObject
{
    Q_OBJECT
public:
    QWindowMainController();

    virtual ~QWindowMainController();

    void init(QWindowMain* pWindowMain);

public slots:
    void folderOpened();
    void streamOpened();

signals:
    void openFolder(const QString& folderPath);
    void openStream(const QString& folderPath);

private:
    QWindowMain* m_pWindowMain;
    QFolderModel* m_pFolderModel;
    QStreamModel* m_pStreamModel;
    QDecoderModel* m_pDecoderModel;
    QFolderViewController* m_pFolderViewController;
    QTimelineViewController* m_pTimelineViewController;
    QErrorViewController* m_pErrorViewController;
    QStatusViewController* m_pStatusViewController;
};