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
    else painter.drawText(width()/4, height()/2, m_text);
}

void QVideoFrameView::frameLoaded(QSharedPointer<QImage> pFrameImage){
    m_pImage = pFrameImage;
    if(!m_pImage) m_text = tr("No decoded picture for the selected access unit");
    update();
}

void QVideoFrameView::textWritten(const QString& text){
    m_text = text;
    m_pImage = nullptr;
    update();
}

void QVideoFrameView::unsupportedVideoCodecDetected(){
    m_text = tr("Stream video format/codec not supported");
    m_pImage = nullptr;
    update();
}
