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
    void streamOpened(const QString& folderPath);

signals:
    void openStream(const QString& folderPath);
    void openFolder(const QString& folderPath);

private:
    QFolderView* m_pFolderView;
    QFolderModel* m_pFolderModel;
    QStreamModel* m_pStreamModel;
    QDecoderModel* m_pDecoderModel;
};