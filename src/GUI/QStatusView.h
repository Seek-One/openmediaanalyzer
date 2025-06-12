#pragma once

#include <QStandardItemModel>
#include <QLabel>

#include "../GUIModel/QDecoderModel.h"

class QStatusView : public QWidget
{
    Q_OBJECT
public:
    QStatusView(QWidget *parent = NULL);
    virtual ~QStatusView();

public slots:
    void videoStatusUpdated(QDecoderModel::StreamStatus status);
    void folderPathSet(QFileSystemModel* pModel);
    void streamLinkSet(const QString& URL);

private:
    QLabel* m_pInputSource;
    QLabel* m_pVideoStatus;
};