#pragma once

#include <QWidget>
#include <QMouseEvent>

class QAccessUnitModel;

class QAccessUnitElement : public QWidget
{
    Q_OBJECT
public:
    QAccessUnitElement(QWidget *parent = NULL);
    ~QAccessUnitElement();
    void setAccessUnitElement(QSharedPointer<QAccessUnitModel> pAccessUnits);

    static inline quint64 m_maxSize = 0;
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);

signals:
    void selectAccessUnit(QSharedPointer<QAccessUnitModel> pAccessUnits);
    
public:
    QString m_text;
    QColor m_barColor;

    bool m_hovered;
    bool m_selected;

    QSharedPointer<QAccessUnitModel> m_pAccessUnitModel;

private:
    void updateBar();
    void updateBarColor();
};