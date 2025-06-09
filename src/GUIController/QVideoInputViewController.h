#pragma once

#include <QObject>

class QVideoInputView;
class QFolderModel;
class QStreamModel;
class QDecoderModel;

class QVideoInputViewController : public QObject
{
    Q_OBJECT
public:
    QVideoInputViewController(QVideoInputView* pVideoInputView, QFolderModel* pFolderModel, QStreamModel* pStreamModel, QDecoderModel* pDecoderModel);

    virtual ~QVideoInputViewController();

signals:
    void openFolder(const QString& folderPath);
    void openStream(const QString& URL, const QString& username, const QString& password);

private:
    QVideoInputView* m_pVideoInputView;
    QFolderModel* m_pFolderModel;
    QStreamModel* m_pStreamModel;
    QDecoderModel* m_pDecoderModel;
};