#pragma once

#include <QGroupBox>
#include <QFileSystemModel>
#include <QTreeView>

class QFolderView : public QGroupBox
{
    Q_OBJECT
public:
    QFolderView(QWidget *parent = NULL);
    virtual ~QFolderView();

public slots:
    void setFolder(QFileSystemModel* pModel);
    
private:
    QTreeView* m_pTreeView;
};