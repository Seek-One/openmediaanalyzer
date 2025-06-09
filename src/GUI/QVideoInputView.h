#pragma once

#include <QGroupBox>
#include <QFileSystemModel>
#include <QTreeView>
#include <QLabel>

class QVideoInputView : public QGroupBox
{
    Q_OBJECT
public:
    QVideoInputView(QWidget *parent = NULL);
    virtual ~QVideoInputView();

public slots:
    void folderSet(QFileSystemModel* pModel);
    void streamSet(const QString& URL);
    void bitratesUpdated(uint64_t videoBitrate, uint64_t audioBitrate, uint64_t otherBitrate);
    void contentTypeUpdated(const QString& contentType);
    void GOVLengthUpdated(uint32_t GOVLength);
    
private:
    QTreeView* m_pTreeView;
    QWidget* m_pStreamInfoView;

    QLabel* m_pVideoContentBitrate;
    QLabel* m_pAudioContentBitrate;
    QLabel* m_pGlobalContentBitrate;

    QLabel* m_pContentType;
    QLabel* m_pGOVLength;
};