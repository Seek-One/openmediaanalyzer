#pragma once

class QFolderView;
class QFolderModel;
class QStreamModel;
class QDecoderModel;

class QFolderViewController : public QObject
{
    Q_OBJECT
public:
    QFolderViewController(QFolderView* pFolderView, QFolderModel* pFolderModel, QStreamModel* pStreamModel, QDecoderModel* pDecoderModel);

    virtual ~QFolderViewController();

    
public slots:
    void folderOpened(const QString& folderPath);
    void streamOpened();

signals:
    void openFolder(const QString& folderPath);
    void openStream();

private:
    QFolderView* m_pFolderView;
    QFolderModel* m_pFolderModel;
    QStreamModel* m_pStreamModel;
    QDecoderModel* m_pDecoderModel;
};