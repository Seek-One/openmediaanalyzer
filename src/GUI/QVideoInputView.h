#pragma once

#include <QGroupBox>
#include <QFileSystemModel>
#include <QTreeView>

class QVideoInputView : public QGroupBox
{
    Q_OBJECT
public:
    QVideoInputView(QWidget *parent = NULL);
    virtual ~QVideoInputView();

public slots:
    void folderSet(QFileSystemModel* pModel);
    void streamSet(const QString& URL);
    
private:
    QTreeView* m_pTreeView;
};