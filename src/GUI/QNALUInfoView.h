#pragma once

#include <QStandardItemModel>
#include <QTreeView>

class QNALUInfoView : public QWidget
{
    Q_OBJECT
public:
    QNALUInfoView(QWidget *parent = NULL);
    virtual ~QNALUInfoView();

public slots:
    void viewUpdated(QStandardItemModel* pModel);

private:
    QTreeView* m_pTreeView;
};