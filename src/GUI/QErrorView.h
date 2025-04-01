#pragma once

#include <QGroupBox>
#include <QListWidget>

class QErrorView : public QGroupBox
{
    Q_OBJECT
public:
    QErrorView(QWidget *parent = NULL);
    virtual ~QErrorView();

public slots:
    void setErrors(QString title, QStringList errors);

private:
    QListWidget* m_pListWidget;
};