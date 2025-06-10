#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QMetaType>
#include <QDebug>

#include "QStreamLinkDialog.h"

QStreamLinkDialog::QStreamLinkDialog(QWidget* parent):
    QDialog(parent), m_pURLInput(new QComboBox(this)), m_pUsernameInput(new QLineEdit(this)), m_pPasswordInput(new QLineEdit(this))
{
    qRegisterMetaTypeStreamOperators<QList<QString>>();

    resize(700, 100);

    QVBoxLayout* pFormLayout = new QVBoxLayout(this);
    setLayout(pFormLayout);

    QWidget* pInputWidget = new QWidget(this);
    QWidget* pButtonsWidget = new QWidget(this);
    QGridLayout* pInputLayout = new QGridLayout(pInputWidget);
    QHBoxLayout* pButtonsLayout = new QHBoxLayout(pButtonsWidget);
    pInputWidget->setLayout(pInputLayout);
    pButtonsWidget->setLayout(pButtonsLayout);
    pFormLayout->addWidget(pInputWidget);
    pFormLayout->addWidget(pButtonsWidget);

    QLabel* pURLLabel = new QLabel(pInputWidget);
    QLabel* pUsernameLabel = new QLabel(pInputWidget);
    QLabel* pPasswordLabel = new QLabel(pInputWidget);
    pURLLabel->setText(tr("Stream URL"));
    pUsernameLabel->setText(tr("Username"));
    pPasswordLabel->setText(tr("Password"));
    pInputLayout->addWidget(pURLLabel, 0, 0);
    pInputLayout->addWidget(m_pURLInput, 0, 1);
    pInputLayout->addWidget(pUsernameLabel, 1, 0);
    pInputLayout->addWidget(m_pUsernameInput, 1, 1);
    pInputLayout->addWidget(pPasswordLabel, 2, 0);
    pInputLayout->addWidget(m_pPasswordInput, 2, 1);

    QPushButton* pAccessStreamButton = new QPushButton(pButtonsWidget);
    pAccessStreamButton->setText(tr("Access stream"));
    pButtonsLayout->addWidget(pAccessStreamButton);
    connect(pAccessStreamButton, &QPushButton::clicked, this, [this](){
        emit accessStream(m_URL, m_pUsernameInput->text(), m_pPasswordInput->text());
        hide();
    });

    m_pPasswordInput->setEchoMode(QLineEdit::Password);
    QSettings settings;
    QList<QString> registeredLinks = settings.value("validLinks").value<QList<QString>>();
    for(QString link : registeredLinks) m_pURLInput->addItem(link);
    m_pURLInput->setEditable(true);

    connect(m_pURLInput, &QComboBox::editTextChanged, this, [this](const QString& text){
        m_URL = text;
    });

    connect(m_pURLInput, QOverload<int>::of(&QComboBox::activated), this, [this](int index){
        m_URL = m_pURLInput->itemText(index);
    });
    m_URL = m_pURLInput->itemText(0);
}

QStreamLinkDialog::~QStreamLinkDialog(){}

void QStreamLinkDialog::validURLsUpdated(const QString& URL){
    m_pURLInput->addItem(URL);
    if(m_pURLInput->count() > 7) m_pURLInput->removeItem(0);
}