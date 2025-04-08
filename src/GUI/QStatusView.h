#pragma once

#include <QStandardItemModel>
#include <QLabel>

class QStatusView : public QWidget
{
    Q_OBJECT
public:
    QStatusView(QWidget *parent = NULL);
    virtual ~QStatusView();

public slots:
    void sizeUpdated(uint64_t size);
    void validityUpdated(uint32_t valid, uint32_t total);

private:
    QLabel* m_pStreamSize;
    QLabel* m_pFrameValidity;
};