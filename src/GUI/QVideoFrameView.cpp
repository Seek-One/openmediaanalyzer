#include <QPainter>

#include "QVideoFrameView.h"

QVideoFrameView::QVideoFrameView(QWidget* parent)
    : QGroupBox(tr("Video preview"), parent), m_pImage(nullptr)
{
    m_pWidget = new QWidget(this);
    show();
}

QVideoFrameView::~QVideoFrameView(){
}

void QVideoFrameView::paintEvent(QPaintEvent* pEvent){
    QPainter painter(this);
    if(m_pImage) painter.drawImage(rect(), *m_pImage);
}

void QVideoFrameView::frameLoaded(QSharedPointer<QImage> pFrameImage){
    m_pImage = pFrameImage;
    update();
}
