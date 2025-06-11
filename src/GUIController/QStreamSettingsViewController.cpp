#include <QDebug>

#include "../GUI/QStreamSettingsView.h"
#include "../GUIModel/QDecoderModel.h"
#include "../GUIModel/QStreamModel.h"

#include "QStreamSettingsViewController.h"

QStreamSettingsViewController::QStreamSettingsViewController(QStreamSettingsView* pStreamSettingsView, QDecoderModel* pDecoderModel, QStreamModel* pStreamModel)
    : m_pStreamSettingsView(pStreamSettingsView), m_pDecoderModel(pDecoderModel), m_pStreamModel(pStreamModel)
{
    connect(m_pStreamSettingsView, &QStreamSettingsView::toggleMemoryLimit, m_pDecoderModel, &QDecoderModel::memoryLimitToggled);
    connect(m_pStreamSettingsView, &QStreamSettingsView::toggleDurationLimit, m_pDecoderModel, &QDecoderModel::durationLimitToggled);
    connect(m_pStreamSettingsView, &QStreamSettingsView::toggleGOPCountLimit, m_pDecoderModel, &QDecoderModel::GOPCountLimitToggled);

    connect(m_pStreamSettingsView, &QStreamSettingsView::updateMemoryLimit, m_pDecoderModel, &QDecoderModel::memoryLimitUpdated);
    connect(m_pStreamSettingsView, &QStreamSettingsView::updateDurationLimit, m_pDecoderModel, &QDecoderModel::durationLimitUpdated);
    connect(m_pStreamSettingsView, &QStreamSettingsView::updateGOPCountLimit, m_pDecoderModel, &QDecoderModel::GOPCountLimitUpdated);

    connect(m_pStreamSettingsView, &QStreamSettingsView::stopStreamClicked, m_pStreamModel, &QStreamModel::streamStopped);
    connect(m_pStreamSettingsView, &QStreamSettingsView::setLiveContent, this, &QStreamSettingsViewController::setLiveContent);
    connect(m_pStreamSettingsView, &QStreamSettingsView::setLiveContent, m_pDecoderModel, &QDecoderModel::liveContentSet);
}

QStreamSettingsViewController::~QStreamSettingsViewController(){
}