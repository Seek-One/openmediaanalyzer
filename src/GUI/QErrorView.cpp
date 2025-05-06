#include <QDebug>
#include <QVBoxLayout>

#include "QErrorView.h"

QErrorView::QErrorView(QWidget* parent)
    : QGroupBox(tr("Errors"), parent)
{
    m_pListWidget = new QListWidget(this);
    m_pListWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->addWidget(m_pListWidget);
    setLayout(pVBoxLayout);
    hide();
}

QErrorView::~QErrorView(){

}

void QErrorView::setErrors(QString title, QStringList minorErrors, QStringList majorErrors){
    while(m_pListWidget->takeItem(0));
    if(minorErrors.empty() && majorErrors.empty()){
        setTitle("");
        hide();
        return;
    }
    setTitle(title);
    for(QString err : majorErrors){
        QListWidgetItem* errItem = new QListWidgetItem(err);
        errItem->setForeground(Qt::red);
        m_pListWidget->addItem(errItem);
    }
    for(QString err : minorErrors){
        QListWidgetItem* errItem = new QListWidgetItem(err);
        errItem->setForeground(Qt::darkYellow);
        m_pListWidget->addItem(errItem);
    }
    show();
}

