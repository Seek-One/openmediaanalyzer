#include <QDebug>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>


#include "QStreamSettingsView.h"

QStreamSettingsView::QStreamSettingsView(QWidget* parent)
    : QGroupBox(tr("Stream settings"), parent), m_pLiveContentCheckbox(new QCheckBox(tr("Live content"), this))
{
    QVBoxLayout* pStreamSettingsLayout = new QVBoxLayout(this);
    setLayout(pStreamSettingsLayout);

    QWidget* pDiscardStrategyContainer = new QWidget(this);
    QWidget* pStreamControlContainer = new QWidget(this);
    pStreamSettingsLayout->addWidget(pDiscardStrategyContainer);
    pStreamSettingsLayout->addWidget(pStreamControlContainer);

    QGridLayout* pDiscardStrategyLayout = new QGridLayout(pDiscardStrategyContainer);
    pDiscardStrategyContainer->setLayout(pDiscardStrategyLayout);


    QCheckBox* pMemoryLimitCheckbox = new QCheckBox(tr("Encoded Stream Memory limit"), pDiscardStrategyContainer);
    QCheckBox* pDurationLimitCheckbox = new QCheckBox(tr("Fixed video duration"), pDiscardStrategyContainer);
    QCheckBox* pGOPCountLimitCheckbox = new QCheckBox(tr("Fixed GOP count"), pDiscardStrategyContainer);
    QSpinBox* pMemoryLimitSpinbox = new QSpinBox(pDiscardStrategyContainer);
    QSpinBox* pDurationLimitSpinbox = new QSpinBox(pDiscardStrategyContainer);
    QSpinBox* pGOPCountLimitSpinbox = new QSpinBox(pDiscardStrategyContainer);
    pDiscardStrategyLayout->addWidget(pMemoryLimitCheckbox, 0, 0);
    pDiscardStrategyLayout->addWidget(pDurationLimitCheckbox, 1, 0);
    pDiscardStrategyLayout->addWidget(pGOPCountLimitCheckbox, 2, 0);
    pDiscardStrategyLayout->addWidget(pMemoryLimitSpinbox, 0, 1);
    pDiscardStrategyLayout->addWidget(pDurationLimitSpinbox, 1, 1);
    pDiscardStrategyLayout->addWidget(pGOPCountLimitSpinbox, 2, 1);

    connect(pMemoryLimitCheckbox, &QCheckBox::toggled, this, [this, pMemoryLimitSpinbox](bool checked){
        pMemoryLimitSpinbox->setEnabled(checked);
        emit toggleMemoryLimit(checked);
    });
    connect(pDurationLimitCheckbox, &QCheckBox::toggled, this, [this, pDurationLimitSpinbox](bool checked){
        pDurationLimitSpinbox->setEnabled(checked);
        emit toggleDurationLimit(checked);
    });
    connect(pGOPCountLimitCheckbox, &QCheckBox::toggled, this, [this, pGOPCountLimitSpinbox](bool checked){
        pGOPCountLimitSpinbox->setEnabled(checked);
        emit toggleGOPCountLimit(checked);
    });

    connect(pMemoryLimitSpinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &QStreamSettingsView::updateMemoryLimit);
    connect(pDurationLimitSpinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &QStreamSettingsView::updateDurationLimit);
    connect(pGOPCountLimitSpinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &QStreamSettingsView::updateGOPCountLimit);

    pMemoryLimitSpinbox->setRange(MEMORY_LIMIT_MIN, MEMORY_LIMIT_MAX);
    pMemoryLimitSpinbox->setSingleStep(MEMORY_LIMIT_STEP);
    pMemoryLimitSpinbox->setSuffix(tr("MB"));
    pDurationLimitSpinbox->setRange(DURATION_LIMIT_MIN, DURATION_LIMIT_MAX);
    pDurationLimitSpinbox->setSuffix(tr(" minutes"));
    pGOPCountLimitSpinbox->setRange(GOP_COUNT_LIMIT_MIN, GOP_COUNT_LIMIT_MAX);

    pMemoryLimitCheckbox->setChecked(MEMORY_LIMIT_SET_BY_DEFAULT);
    pMemoryLimitSpinbox->setValue(MEMORY_LIMIT_DEFAULT_VALUE);
    pDurationLimitSpinbox->setEnabled(DURATION_LIMIT_SET_BY_DEFAULT);
    pDurationLimitSpinbox->setValue(DURATION_LIMIT_DEFAULT_VALUE);
    pGOPCountLimitSpinbox->setEnabled(GOP_COUNT_LIMIT_SET_BY_DEFAULT);
    pGOPCountLimitSpinbox->setValue(GOP_COUNT_LIMIT_DEFAUT_VALUE);
    
    QHBoxLayout* pStreamControlLayout = new QHBoxLayout(pStreamControlContainer);
    pStreamControlContainer->setLayout(pStreamControlLayout);

    QPushButton* pStopStreamButton = new QPushButton(tr("Stop stream"), pStreamControlContainer);
    pStreamControlLayout->addWidget(m_pLiveContentCheckbox);
    pStreamControlLayout->addWidget(pStopStreamButton);

    connect(m_pLiveContentCheckbox, &QCheckBox::toggled, this, [this](bool checked) {
            emit setLiveContent(checked);
    });
    connect(pStopStreamButton, &QAbstractButton::clicked, [this]() {
        emit stopStreamClicked();
    });

    m_pLiveContentCheckbox->setChecked(true);

    hide();
}

QStreamSettingsView::~QStreamSettingsView(){

}

void QStreamSettingsView::liveContentSet(bool activated){
    m_pLiveContentCheckbox->setChecked(activated);
}

