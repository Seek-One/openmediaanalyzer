#pragma once

#include <QDialog>
#include <QLineEdit>

class QStreamLinkDialog : public QDialog
{
    Q_OBJECT
public:
    QStreamLinkDialog(QWidget *parent = NULL);
    virtual ~QStreamLinkDialog();

signals:
    void accessStream(const QString& URL, const QString& username, const QString& password);

private:
    QLineEdit* m_pURLInput;
    QLineEdit* m_pUsernameInput;
    QLineEdit* m_pPasswordInput;
};