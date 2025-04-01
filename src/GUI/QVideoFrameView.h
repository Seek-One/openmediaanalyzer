#pragma once

#include <QGroupBox>

class QVideoFrameView : public QGroupBox
{
    Q_OBJECT
public:
    QVideoFrameView(QWidget *parent = NULL);
    virtual ~QVideoFrameView();

private:

};