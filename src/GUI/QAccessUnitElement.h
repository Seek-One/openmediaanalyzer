#pragma once

#include <QWidget>
#include <QMouseEvent>

#include "../GUIModel/QAccessUnitModel.h"

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

    void enterEvent(QEvent *event);

    void leaveEvent(QEvent *event);

public slots:
    void accessUnitSelected();

signals:
    void selectAccessUnit(QSharedPointer<QAccessUnitModel> pAccessUnits);
    
public:
    QString m_text;
    QColor m_barColor;

    bool m_hovered;
    bool m_selected;
    bool m_interactable;

    QSharedPointer<QAccessUnitModel> m_pAccessUnitModel;

private:
    void updateBar();
    void updateBarColor();
};