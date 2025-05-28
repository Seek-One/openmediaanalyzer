#include <QDebug>

#include "../GUI/QTimelineView.h"
#include "../GUIModel/QDecoderModel.h"

#include "QTimelineViewController.h"

QTimelineViewController::QTimelineViewController(QTimelineView* pTimelineView, QDecoderModel* pDecoderModel)
    : m_pTimelineView(pTimelineView), m_pDecoderModel(pDecoderModel)
{
    connect(this, &QTimelineViewController::startTimeline, m_pTimelineView, &QTimelineView::resetTimeline);
    connect(m_pDecoderModel, &QDecoderModel::addTimelineUnits, m_pTimelineView, &QTimelineView::accessUnitsAdded);
    connect(m_pDecoderModel, &QDecoderModel::removeTimelineUnits, m_pTimelineView, &QTimelineView::accessUnitsRemoved);
    connect(m_pDecoderModel, &QDecoderModel::updateTimelineUnits, m_pTimelineView, &QTimelineView::accessUnitsUpdated);
    connect(m_pTimelineView, &QTimelineView::selectAccessUnit, this, &QTimelineViewController::frameSelected);

    connect(this, &QTimelineViewController::selectFrame, m_pDecoderModel, &QDecoderModel::frameSelected);
    connect(m_pTimelineView, &QTimelineView::deleteFrame, m_pDecoderModel, &QDecoderModel::frameDeleted);

    connect(this, &QTimelineViewController::setLiveContent, m_pTimelineView, &QTimelineView::liveContentSet);
}

QTimelineViewController::~QTimelineViewController(){
}

void QTimelineViewController::frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnits){
    emit selectFrame(pAccessUnits);
}

void QTimelineViewController::timelineStarted(){
    emit startTimeline();
}

void QTimelineViewController::liveContentSet(bool val){
    emit setLiveContent(val);
}