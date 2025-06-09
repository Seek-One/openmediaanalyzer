#pragma once

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>

class QStreamLinkDialog : public QDialog
{
    Q_OBJECT
public:
    QStreamLinkDialog(QWidget *parent = NULL);
    virtual ~QStreamLinkDialog();

signals:
    void accessStream(const QString& URL, const QString& username, const QString& password);

public slots:
    void validURLsUpdated(const QString& URL);

private:
    QComboBox* m_pURLInput;
    QLineEdit* m_pUsernameInput;
    QLineEdit* m_pPasswordInput;

    QString m_URL;
};