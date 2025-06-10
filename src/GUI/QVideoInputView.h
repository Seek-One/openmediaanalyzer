#pragma once

#include <QGroupBox>
#include <QFileSystemModel>
#include <QTreeView>
#include <QTableView>
#include <QStandardItem>
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
    QTreeView* m_pFolderView;
    QTableView* m_pStreamInfoView;

    QStandardItem* m_pVideoContentBitrate;
    QStandardItem* m_pAudioContentBitrate;
    QStandardItem* m_pGlobalContentBitrate;

    QStandardItem* m_pContentType;
    QStandardItem* m_pGOVLength;
};