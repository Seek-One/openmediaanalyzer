#pragma once

#include <QWidget>
#include <QMouseEvent>

#include "../GUIModel/QAccessUnitModel.h"

class QAccessUnitModel;

class QFrameElement : public QWidget
{
    Q_OBJECT
public:
    QFrameElement(QWidget *parent = NULL);
    ~QFrameElement();

    void setFrameElement(QSharedPointer<QAccessUnitModel> pAccessUnits);

    static inline quint64 m_maxSize = 0;
protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void enterEvent(QEvent *event);

    void leaveEvent(QEvent *event);

signals:
    void selectFrame(QSharedPointer<QAccessUnitModel> pAccessUnits);
    
public:
    QString m_text;
    QColor m_textColor;
    QColor m_barColor;

    bool m_hovered;
    bool m_selected;
    bool m_interactable;

    QSharedPointer<QAccessUnitModel> m_pAccessUnitModel;

private:
    void updateBar();
    void updateTextColor();
};