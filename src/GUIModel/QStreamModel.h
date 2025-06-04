#pragma once

#include <QObject>
#include <QFileSystemModel>

#include "QAccessUnitModel.h"

class QFolderView;
class QStreamWorker;

class QStreamModel : public QObject
{
    Q_OBJECT
public:
    QStreamModel();
    
    virtual ~QStreamModel();
    
signals:
    void updateFolderView(QFileSystemModel* model);
    void loadH264File(uint8_t* fileContent, quint32 fileSize);
    void loadH265File(uint8_t* fileContent, quint32 fileSize);
    void loadStreamStart();
    void stopProcessing();

public slots:
    void streamLoaded(const QString& URL, const QString& username, const QString& password);
    void streamStopped();
    
private:
    QThread* m_pThread;
    QStreamWorker* m_pWorker;
};

class QStreamWorker : public QObject {
    Q_OBJECT
public:
    QStreamWorker(const QString& URL, const QString& username, const QString& password);
    ~QStreamWorker();
    
    QVector<uint8_t> m_buffer;
public slots:
    void process();
    void streamStopped();
signals:
    void loadH264File(uint8_t* fileContent, quint32 fileSize);
    void loadH265File(uint8_t* fileContent, quint32 fileSize);
    void finished();
    void error();
private:
    bool m_running;
    const QString& m_URL;
    const QString& m_username;
    const QString& m_password;
};