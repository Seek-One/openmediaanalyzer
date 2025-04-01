#include "QVideoFrameView.h"

QVideoFrameView::QVideoFrameView(QWidget* parent)
    : QGroupBox("Video preview", parent)
{
    show();
}

QVideoFrameView::~QVideoFrameView(){

}
