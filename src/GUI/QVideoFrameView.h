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
    void textWritten(const QString& text);
    void unsupportedVideoCodecDetected();

protected:
    void paintEvent(QPaintEvent* pEvent) override;

private:
    QWidget* m_pWidget;
    QSharedPointer<QImage> m_pImage;
    QString m_text;
};