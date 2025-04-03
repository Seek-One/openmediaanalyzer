#pragma once

#include <QGroupBox>
#include <QImage>

class QVideoFrameView : public QGroupBox
{
    Q_OBJECT
public:
    QVideoFrameView(QWidget *parent = NULL);
    virtual ~QVideoFrameView();

public slots:
    void frameLoaded(QSharedPointer<QImage> pFrameImage);

protected:
    void paintEvent(QPaintEvent* pEvent) override;

private:
    QWidget* m_pWidget;
    QSharedPointer<QImage> m_pImage;
};