#include <QDebug>
#include <QVBoxLayout>

#include "QErrorView.h"

QErrorView::QErrorView(QWidget* parent)
    : QGroupBox("Errors", parent)
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

void QErrorView::setErrors(QString title, QStringList errors){
    while(m_pListWidget->takeItem(0));
    if(errors.empty()){
        setTitle("");
        hide();
        return;
    }
    setTitle(title);
    m_pListWidget->addItems(errors);
    show();
}

