#pragma once

#include <vector>
#include <QObject>
#include <QFileSystemModel>
#include <QStandardItemModel>

#include "QAccessUnitModel.h"

#define GOP_LIMIT 15
#define ERR_MSG_LIMIT 15

class QFolderView;
class H264Stream;

class QDecoderModel : public QObject
{
    Q_OBJECT
public:
    QDecoderModel();

    virtual ~QDecoderModel();

    static inline QStringList headers = {"field", "value"};
    
    static QStandardItem* modelItemFromFields(std::vector<std::string> fields, QString header);
    QStringList m_streamErrors;

signals:
    void updateTimeline(QVector<QSharedPointer<QAccessUnitModel>> frames);
    void updateTimelineUnits();
    void addTimelineUnits(QVector<QSharedPointer<QAccessUnitModel>> accessUnits);
    void removeTimelineUnits(uint32_t count);
    void updateSPSInfoView(QStandardItemModel* pModel);
    void updatePPSInfoView(QStandardItemModel* pModel);
    void updateFrameInfoView(QStandardItemModel* pModel);
    void updateErrorView(QString title, QStringList errors);

public slots:
    void reset();
    void fileLoaded(uint8_t* fileContent, quint32 fileSize);
    void frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void folderLoaded();
    void framesTabOpened();
    void spsTabOpened();
    void ppsTabOpened();

private:
    void emitFrameErrors();
    void emitSPSErrors();
    void emitPPSErrors();
    void checkForNewGOP();
    void validateCurrentGOP();
    void addStreamError(QString err);

    QSharedPointer<QAccessUnitModel> m_pSelectedFrameModel;
    QVector<QSharedPointer<QAccessUnitModel>> m_currentGOPModel;
    int m_tabIndex;
    H264Stream* m_pH264Stream;
};