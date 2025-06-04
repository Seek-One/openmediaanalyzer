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

signals:
    void openFolder(const QString& folderPath);
    void openStream(const QString& URL, const QString& username, const QString& password);

private:
    QFolderView* m_pFolderView;
    QFolderModel* m_pFolderModel;
    QStreamModel* m_pStreamModel;
    QDecoderModel* m_pDecoderModel;
};