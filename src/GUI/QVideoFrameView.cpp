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
    else painter.drawText(width()/4, height()/2, tr("No decoded picture for the selected access unit"));
}

void QVideoFrameView::frameLoaded(QSharedPointer<QImage> pFrameImage){
    m_pImage = pFrameImage;
    update();
}
