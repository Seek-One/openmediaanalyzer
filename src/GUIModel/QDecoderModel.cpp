#include <QDebug>
#include <QUuid>
#include <sstream>
#include <set>
#include <algorithm>

#include "../GUI/QVideoInputView.h"
#include "../Codec/H264/H264Stream.h"
#include "../Codec/H264/H264GOP.h"
#include "../Codec/H264/H264Slice.h"
#include "../Codec/H264/H264AUD.h"
#include "../Codec/H264/H264SEI.h"
#include "../Codec/H265/H265Stream.h"
#include "../Codec/H265/H265AccessUnit.h"

#include "QDecoderModel.h"

QDecoderModel::QDecoderModel():
    m_pH264Stream(nullptr), m_pH265Stream(nullptr), m_pSelectedFrameModel(nullptr), m_tabIndex(0), 
    m_pH264Codec(avcodec_find_decoder(AV_CODEC_ID_H264)), m_pH264SwsCtx(nullptr),
    m_pH265Codec(avcodec_find_decoder(AV_CODEC_ID_H265)), m_pH265SwsCtx(nullptr),
    m_liveContent(true)
{
    if(!m_pH264Codec) {
        qDebug() << "Couldn't find H264 decoder";
        return;
    }
    if(!m_pH265Codec) {
        qDebug() << "Couldn't find H265 decoder";
        return;
    }
    m_pH264CodecCtx = avcodec_alloc_context3(m_pH264Codec);
    if(!m_pH264CodecCtx){
        qDebug() << "Couldn't allocate H264 codec context";
        return;
    }
    m_pH265CodecCtx = avcodec_alloc_context3(m_pH265Codec);
    if(!m_pH264CodecCtx){
        qDebug() << "Couldn't allocate H265 codec context";
        return;
    }
    if(avcodec_open2(m_pH264CodecCtx, m_pH264Codec, nullptr) < 0){
        qDebug() << "Couldn't open H264 codec";
        return;
    }
    if(avcodec_open2(m_pH265CodecCtx, m_pH265Codec, nullptr) < 0){
        qDebug() << "Couldn't open H265 codec";
        return;
    }
}

QDecoderModel::~QDecoderModel(){
    if(m_pH264Stream) delete m_pH264Stream;
    if(m_pH265Stream) delete m_pH265Stream;
    m_minorStreamErrors.clear();
    m_majorStreamErrors.clear();
    m_decodedFrames.clear();
    while(!m_requestedFrames.empty()) m_requestedFrames.pop();
    avcodec_free_context(&m_pH264CodecCtx);
    if(m_pH264SwsCtx){
        sws_freeContext(m_pH264SwsCtx);
        m_pH264SwsCtx = nullptr;
    }
    avcodec_free_context(&m_pH265CodecCtx);
    if(m_pH265SwsCtx){
        sws_freeContext(m_pH265SwsCtx);
        m_pH265SwsCtx = nullptr;
    }
}

QStandardItem* QDecoderModel::modelItemFromFields(std::vector<std::string> fields, QString header){
    QStandardItem* headerItem = new QStandardItem(header);
    for(std::string field: fields){
        QList<QStandardItem*> row;
        std::stringstream fieldstream(field);
        std::string fieldElement;
        while(getline(fieldstream, fieldElement, ':')) row.push_back(new QStandardItem(fieldElement.c_str()));
        headerItem->appendRow(row);
    }
    return headerItem;
}

void buildH264SPSView(QDecoderModel* pDecoderModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderItem(0, new QStandardItem(QDecoderModel::tr("field")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QDecoderModel::tr("value")));
    QStandardItem* root = model->invisibleRootItem();
    for(auto entry : H264SPS::SPSMap){
        root->appendRow(QDecoderModel::modelItemFromFields(entry.second->dump_fields(), QDecoderModel::tr("SPS Unit #") + QString::number(entry.first)));
    }
    emit pDecoderModel->updateSPSInfoView(model);
}

void buildH264PPSView(QDecoderModel* pDecoderModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderItem(0, new QStandardItem(QDecoderModel::tr("field")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QDecoderModel::tr("value")));
    QStandardItem* root = model->invisibleRootItem();
    for(auto entry : H264PPS::PPSMap){
        root->appendRow(QDecoderModel::modelItemFromFields(entry.second->dump_fields(), QDecoderModel::tr("PPS Unit #") + QString::number(entry.first)));
    }
    emit pDecoderModel->updatePPSInfoView(model);
}

void buildVPSView(QDecoderModel* pDecoderModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderItem(0, new QStandardItem(QDecoderModel::tr("field")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QDecoderModel::tr("value")));
    QStandardItem* root = model->invisibleRootItem();
    for(auto entry : H265VPS::VPSMap){
        root->appendRow(QDecoderModel::modelItemFromFields(entry.second->dump_fields(), QDecoderModel::tr("VPS Unit #") + QString::number(entry.first)));
    }
    emit pDecoderModel->updateVPSInfoView(model);
}

void buildH265SPSView(QDecoderModel* pDecoderModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderItem(0, new QStandardItem(QDecoderModel::tr("field")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QDecoderModel::tr("value")));
    QStandardItem* root = model->invisibleRootItem();
    for(auto entry : H265SPS::SPSMap){
        root->appendRow(QDecoderModel::modelItemFromFields(entry.second->dump_fields(), QDecoderModel::tr("SPS Unit #") + QString::number(entry.first)));
    }
    emit pDecoderModel->updateSPSInfoView(model);
}

void buildH265PPSView(QDecoderModel* pDecoderModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderItem(0, new QStandardItem(QDecoderModel::tr("field")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QDecoderModel::tr("value")));
    QStandardItem* root = model->invisibleRootItem();
    for(auto entry : H265PPS::PPSMap){
        root->appendRow(QDecoderModel::modelItemFromFields(entry.second->dump_fields(), QDecoderModel::tr("PPS Unit #") + QString::number(entry.first)));
    }
    emit pDecoderModel->updatePPSInfoView(model);
}

void QDecoderModel::reset(){
    if(m_pH264Stream) delete m_pH264Stream;
    m_pH264Stream = new H264Stream();
    if(m_pH265Stream) delete m_pH265Stream;
    m_pH265Stream = new H265Stream();
    m_minorStreamErrors.clear();
    m_majorStreamErrors.clear();
    m_pSelectedFrameModel = nullptr;
    m_currentGOPModel.clear();
    m_decodedFrames.clear();
    while(!m_requestedFrames.empty()) m_requestedFrames.pop();
    frameSelected(nullptr);
    buildH264SPSView(this);
    buildH264PPSView(this);
    emit updateSize(0);
    emit updateValidity(0, 0);
    emit updateStatus(StreamStatus_NoStream);
    emit updateVideoFrameViewText("");
    if(m_pH264SwsCtx){
        sws_freeContext(m_pH264SwsCtx);
        m_pH264SwsCtx = nullptr;
    }
    avcodec_free_context(&m_pH264CodecCtx);
    m_pH264CodecCtx = avcodec_alloc_context3(m_pH264Codec);
    if(!m_pH264CodecCtx){
        qDebug() << "Couldn't allocate codec context";
        return;
    }
    if(avcodec_open2(m_pH264CodecCtx, m_pH264Codec, nullptr) < 0){
        qDebug() << "Couldn't open codec";
        return;
    }
    if(m_pH265SwsCtx){
        sws_freeContext(m_pH265SwsCtx);
        m_pH265SwsCtx = nullptr;
    }
    avcodec_free_context(&m_pH265CodecCtx);
    m_pH265CodecCtx = avcodec_alloc_context3(m_pH265Codec);
    if(!m_pH265CodecCtx){
        qDebug() << "Couldn't allocate codec context";
        return;
    }
    if(avcodec_open2(m_pH265CodecCtx, m_pH265Codec, nullptr) < 0){
        qDebug() << "Couldn't open codec";
        return;
    }
}

void QDecoderModel::h264FileLoaded(uint8_t* fileContent, quint32 fileSize){
    uint32_t accessUnitCountBefore = m_pH264Stream->accessUnitCount();
    uint8_t PPSUnitsBefore = H264PPS::PPSMap.size();
    uint8_t SPSUnitsBefore = H264SPS::SPSMap.size();
    m_pH264Stream->parsePacket(fileContent, fileSize);
    decodeCurrentH264GOP();
    delete[] fileContent;
    uint32_t accessUnitCountDiff = m_pH264Stream->accessUnitCount() - accessUnitCountBefore;
    std::deque<H264GOP*> GOPs = m_pH264Stream->getGOPs(); 
    if(GOPs.size() > GOP_LIMIT){
        // try to remove GOPs preceding the first GOP with an IDR first
        bool foundIDR = false;
        for(int i = 1;i < GOPs.size()-1;++i){
            if(GOPs[i]->hasIDR){
                foundIDR = true;
                emit removeTimelineUnits(m_pH264Stream->popFrontGOPs(i));
                break;
            }
        }
        // if no IDR GOPs are found, remove half of the max capacity
        if(!foundIDR) emit removeTimelineUnits(m_pH264Stream->popFrontGOPs(GOP_LIMIT/2));
    }
    checkForNewGOP();
    m_minorStreamErrors.clear();
    m_majorStreamErrors.clear();
    std::transform(m_pH264Stream->minorErrors.begin(), m_pH264Stream->minorErrors.end(), std::back_inserter(m_minorStreamErrors), [](const std::string& err){
        return QString(err.c_str());
    });
    std::transform(m_pH264Stream->majorErrors.begin(), m_pH264Stream->majorErrors.end(), std::back_inserter(m_majorStreamErrors), [](const std::string& err){
        return QString(err.c_str());
    });

    if(accessUnitCountDiff == 0) emit updateTimelineUnits();
    else {
        QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitModels = QVector<QSharedPointer<QAccessUnitModel>>();
        std::list<H264AccessUnit*> pAccessUnits = m_pH264Stream->getLastAccessUnits(accessUnitCountDiff);
        for(auto itAccessUnit = pAccessUnits.begin();itAccessUnit != pAccessUnits.end();++itAccessUnit){
            QUuid id = QUuid::createUuid();
            QSharedPointer<QAccessUnitModel> pAccessUnitModel = QSharedPointer<QAccessUnitModel>(new QAccessUnitModel(*itAccessUnit, id));
            if((*itAccessUnit)->decodable) decodeH264Slice(pAccessUnitModel);
            pAccessUnitModels.push_back(pAccessUnitModel);
            m_currentGOPModel.push_back(pAccessUnitModel);
            checkForNewGOP();
        }        
        emit addTimelineUnits(pAccessUnitModels);
    }

    if(H264PPS::PPSMap.size() != PPSUnitsBefore) buildH264PPSView(this);
    if(H264SPS::SPSMap.size() != SPSUnitsBefore) buildH264SPSView(this);
    switch(m_tabIndex){
        case 0:
            emitStreamErrors();
            break;
        case 1:
            // No VPS units in H264
            break;
        case 2:
            emitH264SPSErrors();
            break;
        case 3:
            emitH264PPSErrors();
            break;
    }
    updateH264StatusBar();
}

void QDecoderModel::h265FileLoaded(uint8_t* fileContent, quint32 fileSize){
    uint32_t accessUnitCountBefore = m_pH265Stream->accessUnitCount();
    uint8_t PPSUnitsBefore = H265PPS::PPSMap.size();
    uint8_t SPSUnitsBefore = H265SPS::SPSMap.size();
    uint8_t VPSUnitsBefore = H265VPS::VPSMap.size();
    m_pH265Stream->parsePacket(fileContent, fileSize);
    decodeCurrentH265GOP();
    delete[] fileContent;
    uint32_t accessUnitCountDiff = m_pH265Stream->accessUnitCount() - accessUnitCountBefore;
    std::deque<H265GOP*> GOPs = m_pH265Stream->getGOPs(); 
    if(GOPs.size() > GOP_LIMIT){
        // try to remove GOPs preceding the first GOP with an IDR first
        bool foundIDR = false;
        for(int i = 1;i < GOPs.size()-1;++i){
            if(GOPs[i]->hasIDR){
                foundIDR = true;
                emit removeTimelineUnits(m_pH265Stream->popFrontGOPs(i));
                break;
            }
        }
        // if no IDR GOPs are found, remove half of the max capacity
        if(!foundIDR) emit removeTimelineUnits(m_pH265Stream->popFrontGOPs(GOP_LIMIT/2));
    }
    checkForNewGOP();
    m_minorStreamErrors.clear();
    m_majorStreamErrors.clear();
    std::transform(m_pH265Stream->minorErrors.begin(), m_pH265Stream->minorErrors.end(), std::back_inserter(m_minorStreamErrors), [](const std::string& err){
        return QString(err.c_str());
    });
    std::transform(m_pH265Stream->majorErrors.begin(), m_pH265Stream->majorErrors.end(), std::back_inserter(m_majorStreamErrors), [](const std::string& err){
        return QString(err.c_str());
    });
    if(accessUnitCountDiff == 0) emit updateTimelineUnits();
    else {
        QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitModels = QVector<QSharedPointer<QAccessUnitModel>>();
        std::list<H265AccessUnit*> pAccessUnits = m_pH265Stream->getLastAccessUnits(accessUnitCountDiff);
        QVector<QSharedPointer<QAccessUnitModel>> pDecodableAccessUnitModels, pSortedAccessUnitModelsToDecode;
        for(H265AccessUnit* pAccessUnit : pAccessUnits){
            QUuid id = QUuid::createUuid();
            QSharedPointer<QAccessUnitModel> pAccessUnitModel = QSharedPointer<QAccessUnitModel>(new QAccessUnitModel(pAccessUnit, id));
            pSortedAccessUnitModelsToDecode.push_back(pAccessUnitModel);
            if(pAccessUnit->decodable) pDecodableAccessUnitModels.push_back(pAccessUnitModel);
            pAccessUnitModels.push_back(pAccessUnitModel);
            m_currentGOPModel.push_back(pAccessUnitModel);
            checkForNewGOP();
        }
        std::sort(pSortedAccessUnitModelsToDecode.begin(), pSortedAccessUnitModelsToDecode.end(), [](QSharedPointer<QAccessUnitModel> lhs, QSharedPointer<QAccessUnitModel> rhs){
            if(!lhs->m_displayedFrameNum.has_value()) return true;
            if(!rhs->m_displayedFrameNum.has_value()) return false;
            return lhs->m_displayedFrameNum.value() < rhs->m_displayedFrameNum.value();
        });
        for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pSortedAccessUnitModelsToDecode) m_requestedFrames.push(pAccessUnitModel);
        for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pDecodableAccessUnitModels) decodeH265Slice(pAccessUnitModel);
        emit addTimelineUnits(pAccessUnitModels);
    }

    if(H265PPS::PPSMap.size() != PPSUnitsBefore) buildH265PPSView(this);
    if(H265SPS::SPSMap.size() != SPSUnitsBefore) buildH265SPSView(this);
    if(H265VPS::VPSMap.size() != VPSUnitsBefore) buildVPSView(this);
    switch(m_tabIndex){
        case 0:
            emitStreamErrors();
            break;
        case 1:
            emitVPSErrors();
            break;
        case 2:
            emitH265SPSErrors();
            break;
        case 3:
            emitH265PPSErrors();
            break;
    }
    updateH265StatusBar();
}

QStringList minorErrorListFromAccessUnit(const std::variant<const H264AccessUnit*, const H265AccessUnit*> accessUnit){
    QStringList errors;
    if(std::holds_alternative<const H264AccessUnit*>(accessUnit)){
        const H264AccessUnit* h264AccessUnit = std::get<const H264AccessUnit*>(accessUnit);
        std::transform(h264AccessUnit->minorErrors.begin(), h264AccessUnit->minorErrors.end(), std::back_inserter(errors), [](const std::string& err){
            return err.c_str();
        });
        for(auto& NALUnit : h264AccessUnit->getNALUnits()){
            std::transform(NALUnit->minorErrors.begin(), NALUnit->minorErrors.end(), std::back_inserter(errors), [](const std::string& err){
                return err.c_str();
            });
        }
    } else if(std::holds_alternative<const H265AccessUnit*>(accessUnit)){
        const H265AccessUnit* h265AccessUnit = std::get<const H265AccessUnit*>(accessUnit);
        std::transform(h265AccessUnit->minorErrors.begin(), h265AccessUnit->minorErrors.end(), std::back_inserter(errors), [](const std::string& err){
            return err.c_str();
        });
        for(auto& NALUnit : h265AccessUnit->getNALUnits()){
            std::transform(NALUnit->minorErrors.begin(), NALUnit->minorErrors.end(), std::back_inserter(errors), [](const std::string& err){
                return err.c_str();
            });
        }
    }
    return errors;
}

QStringList majorErrorListFromAccessUnit(const std::variant<const H264AccessUnit*, const H265AccessUnit*> accessUnit){
    QStringList errors;
    if(std::holds_alternative<const H264AccessUnit*>(accessUnit)){
        const H264AccessUnit* h264AccessUnit = std::get<const H264AccessUnit*>(accessUnit);
        std::transform(h264AccessUnit->majorErrors.begin(), h264AccessUnit->majorErrors.end(), std::back_inserter(errors), [](const std::string& err){
            return err.c_str();
        });
        for(auto& NALUnit : h264AccessUnit->getNALUnits()){
            std::transform(NALUnit->majorErrors.begin(), NALUnit->majorErrors.end(), std::back_inserter(errors), [](const std::string& err){
                return err.c_str();
            });
        }
    } else if(std::holds_alternative<const H265AccessUnit*>(accessUnit)){
        const H265AccessUnit* h265AccessUnit = std::get<const H265AccessUnit*>(accessUnit);
        std::transform(h265AccessUnit->majorErrors.begin(), h265AccessUnit->majorErrors.end(), std::back_inserter(errors), [](const std::string& err){
            return err.c_str();
        });
        for(auto& NALUnit : h265AccessUnit->getNALUnits()){
            std::transform(NALUnit->majorErrors.begin(), NALUnit->majorErrors.end(), std::back_inserter(errors), [](const std::string& err){
                return err.c_str();
            });
        }
    }
    return errors;
}

void modelFromAccessUnit(QStandardItemModel* model, const std::variant<const H264AccessUnit*, const H265AccessUnit*> accessUnit){
    if(std::holds_alternative<const H264AccessUnit*>(accessUnit)){
        for(auto& NALUnit : std::get<const H264AccessUnit*>(accessUnit)->NALUnits){
            switch(NALUnit->nal_unit_type){
                case H264NAL::UnitType_AUD:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Access Unit Delimiter")));
                    break;
                case H264NAL::UnitType_NonIDRFrame:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Non-IDR Picture")));
                    break;
                case H264NAL::UnitType_IDRFrame:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("IDR Picture")));
                    break;
                case H264NAL::UnitType_PPS:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Picture Parameter Set")));
                    break;
                case H264NAL::UnitType_SPS:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Sequence Parameter Set")));
                    break;
                case H264NAL::UnitType_SEI:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Supplemental Enhancement Information")));
                    break;
            }
        }
    } else  if(std::holds_alternative<const H265AccessUnit*>(accessUnit)){
        for(auto& NALUnit : std::get<const H265AccessUnit*>(accessUnit)->NALUnits){
            switch(NALUnit->nal_unit_type){
                case H265NAL::UnitType_TRAIL_N:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Trailing picture (N)")));
                    break;
                case H265NAL::UnitType_TRAIL_R:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Trailing picture (R)")));
                    break;
                case H265NAL::UnitType_TSA_N:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Temporal sub-layer access picture (N)")));
                    break;
                case H265NAL::UnitType_TSA_R:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Temporal sub-layer access picture (R)")));
                    break;
                case H265NAL::UnitType_STSA_N:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Step-wise temporal sub-layer access picture (N)")));
                    break;
                case H265NAL::UnitType_STSA_R:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Step-wise temporal sub-layer access picture (R)")));
                    break;
                case H265NAL::UnitType_RADL_N:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Random access decodable leading picture (N)")));
                    break;
                case H265NAL::UnitType_RADL_R:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Random access decodable leading picture (R)")));
                    break;
                case H265NAL::UnitType_RASL_N:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Random access skipped leading picture (N)")));
                    break;
                case H265NAL::UnitType_RASL_R:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Random access skipped leading picture (R)")));
                    break;
                case H265NAL::UnitType_BLA_W_LP:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Broken link access picture (with LP)")));
                    break;
                case H265NAL::UnitType_BLA_W_RADL:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Broken link access picture (with RADL pictures)")));
                    break;
                case H265NAL::UnitType_BLA_N_LP:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Broken link access picture (no LP)")));
                    break;
                case H265NAL::UnitType_IDR_W_RADL:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Instantaneous decoding refresh picture (with RADL pictures)")));
                    break;
                case H265NAL::UnitType_IDR_N_LP:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Instantaneous decoding refresh picture (no LP)")));
                    break;
                case H265NAL::UnitType_CRA_NUT:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Clean random access picture")));
                    break;
                case H265NAL::UnitType_VPS:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Video parameter set")));
                    break;
                case H265NAL::UnitType_SPS:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Sequence parameter set")));
                    break;
                case H265NAL::UnitType_PPS:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Picture parameter set")));
                    break;
                case H265NAL::UnitType_SEI_PREFIX:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Prefix supplemental enhancement information")));
                    break;
                case H265NAL::UnitType_SEI_SUFFIX:
                    model->appendRow(QDecoderModel::modelItemFromFields(NALUnit->dump_fields(), QDecoderModel::tr("Suffix supplemental enhancement information")));
                    break;
            }
        }
    } 
}

void QDecoderModel::frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("field")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("value")));
    m_pSelectedFrameModel = pAccessUnitModel;
    if(pAccessUnitModel) {
        modelFromAccessUnit(model, pAccessUnitModel->m_pAccessUnit);
        if(m_tabIndex == 0) emit updateErrorView(tr("Access unit errors"), minorErrorListFromAccessUnit(pAccessUnitModel->m_pAccessUnit), majorErrorListFromAccessUnit(pAccessUnitModel->m_pAccessUnit));
        emit updateVideoFrameViewImage(m_decodedFrames[pAccessUnitModel->m_id]);
    } else if(m_tabIndex == 0){
        emit updateErrorView(tr("Stream errors"), m_minorStreamErrors, m_majorStreamErrors);
    }
    emit updateFrameInfoView(model);
}

void QDecoderModel::framesTabOpened(){
    m_tabIndex = 0;
    if(m_pSelectedFrameModel) emit updateErrorView(tr("Access unit errors"), minorErrorListFromAccessUnit(m_pSelectedFrameModel->m_pAccessUnit), majorErrorListFromAccessUnit(m_pSelectedFrameModel->m_pAccessUnit));
    else emitStreamErrors();
}

void QDecoderModel::vpsTabOpened(){
    m_tabIndex = 1;
    if(m_pH265Stream && !m_pH265Stream->getGOPs().empty()) emitVPSErrors();
}

void QDecoderModel::spsTabOpened(){
    m_tabIndex = 2;
    if(m_pH264Stream &&!m_pH264Stream->getGOPs().empty()) emitH264SPSErrors();
    else if(m_pH265Stream && !m_pH265Stream->getGOPs().empty()) emitH265SPSErrors();
}

void QDecoderModel::ppsTabOpened(){
    m_tabIndex = 3;
    if(m_pH264Stream && !m_pH264Stream->getGOPs().empty()) emitH264PPSErrors();
    else if(m_pH265Stream &&!m_pH265Stream->getGOPs().empty()) emitH265PPSErrors();
}

void QDecoderModel::frameDeleted(QUuid id){
    if(m_decodedFrames[id]) m_decodedFrames.remove(id);
}

void QDecoderModel::liveContentSet(bool val){
    m_liveContent = val;
}

void QDecoderModel::folderLoaded(){
    m_minorStreamErrors.clear();
    m_majorStreamErrors.clear();

    std::deque<H264GOP*> h264GOPs = m_pH264Stream->getGOPs();
    std::deque<H265GOP*> h265GOPs = m_pH265Stream->getGOPs();
    if(!h264GOPs.empty()){
        m_pH264Stream->lastPacketParsed();
        std::transform(m_pH264Stream->minorErrors.begin(), m_pH264Stream->minorErrors.end(), std::back_inserter(m_minorStreamErrors), [](const std::string& err){
            return QString(err.c_str());
        });
        std::transform(m_pH264Stream->majorErrors.begin(), m_pH264Stream->majorErrors.end(), std::back_inserter(m_majorStreamErrors), [](const std::string& err){
            return QString(err.c_str());
        });
        decodeCurrentH264GOP();
        updateH264StatusBarValidity();
        updateH264StatusBarStatus();
    } else if(!h265GOPs.empty()){
        m_pH265Stream->lastPacketParsed();
        std::transform(m_pH265Stream->minorErrors.begin(), m_pH265Stream->minorErrors.end(), std::back_inserter(m_minorStreamErrors), [](const std::string& err){
            return QString(err.c_str());
        });
        std::transform(m_pH265Stream->majorErrors.begin(), m_pH265Stream->majorErrors.end(), std::back_inserter(m_majorStreamErrors), [](const std::string& err){
            return QString(err.c_str());
        });
        decodeCurrentH265GOP();
        updateH265StatusBarValidity();
        updateH265StatusBarStatus();
    }
    validateCurrentGOP();
    if(m_pSelectedFrameModel) emit updateErrorView(tr("Access unit errors"), minorErrorListFromAccessUnit(m_pSelectedFrameModel->m_pAccessUnit), majorErrorListFromAccessUnit(m_pSelectedFrameModel->m_pAccessUnit));
    else emitStreamErrors();
    emit updateTimelineUnits();
}

void QDecoderModel::emitStreamErrors(){
    if(!m_pSelectedFrameModel) emit updateErrorView("Stream errors", m_minorStreamErrors, m_majorStreamErrors);
}

void QDecoderModel::emitH264SPSErrors(){
    QStringList minorErrors, majorErrors;
    for(auto SPSEntry : H264SPS::SPSMap){
        H264SPS* pSps = SPSEntry.second;
        if(!pSps->minorErrors.empty()){
            minorErrors.push_back(tr("SPS #") + QString::number(pSps->seq_parameter_set_id));
            std::transform(pSps->minorErrors.begin(), pSps->minorErrors.end(), std::back_inserter(minorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
        if(!pSps->majorErrors.empty()){
            majorErrors.push_back(tr("SPS #") + QString::number(pSps->seq_parameter_set_id));
            std::transform(pSps->majorErrors.begin(), pSps->majorErrors.end(), std::back_inserter(majorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
    }
    emit updateErrorView(tr("SPS errors"), minorErrors, majorErrors);
}

void QDecoderModel::emitH264PPSErrors(){
    QStringList minorErrors, majorErrors;
    for(auto PPSEntry : H264PPS::PPSMap){
        H264PPS* pPps = PPSEntry.second;
        if(!pPps->minorErrors.empty()){
            minorErrors.push_back("PPS #" + QString::number(pPps->pic_parameter_set_id));
            std::transform(pPps->minorErrors.begin(), pPps->minorErrors.end(), std::back_inserter(minorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
        if(!pPps->majorErrors.empty()){
            majorErrors.push_back("PPS #" + QString::number(pPps->pic_parameter_set_id));
            std::transform(pPps->majorErrors.begin(), pPps->majorErrors.end(), std::back_inserter(majorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
    }
    emit updateErrorView(tr("PPS errors"), minorErrors, majorErrors);
}

void QDecoderModel::emitVPSErrors(){
    QStringList minorErrors, majorErrors;
    for(auto VPSEntry : H265VPS::VPSMap){
        H265VPS* pVps = VPSEntry.second;
        if(!pVps->minorErrors.empty()){
            minorErrors.push_back(tr("VPS #") + QString::number(pVps->vps_video_parameter_set_id));
            std::transform(pVps->minorErrors.begin(), pVps->minorErrors.end(), std::back_inserter(minorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
        if(!pVps->majorErrors.empty()){
            majorErrors.push_back(tr("VPS #") + QString::number(pVps->vps_video_parameter_set_id));
            std::transform(pVps->majorErrors.begin(), pVps->majorErrors.end(), std::back_inserter(majorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
    }
    emit updateErrorView(tr("VPS errors"), minorErrors, majorErrors);
}
void QDecoderModel::emitH265SPSErrors(){
    QStringList minorErrors, majorErrors;
    for(auto SPSEntry : H265SPS::SPSMap){
        H265SPS* pSps = SPSEntry.second;
        if(!pSps->minorErrors.empty()){
            minorErrors.push_back(tr("SPS #") + QString::number(pSps->sps_seq_parameter_set_id));
            std::transform(pSps->minorErrors.begin(), pSps->minorErrors.end(), std::back_inserter(minorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
        if(!pSps->majorErrors.empty()){
            majorErrors.push_back(tr("SPS #") + QString::number(pSps->sps_seq_parameter_set_id));
            std::transform(pSps->majorErrors.begin(), pSps->majorErrors.end(), std::back_inserter(majorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
    }
    emit updateErrorView(tr("SPS errors"), minorErrors, majorErrors);
}

void QDecoderModel::emitH265PPSErrors(){
    QStringList minorErrors, majorErrors;
    for(auto PPSEntry : H265PPS::PPSMap){
        H265PPS* pPps = PPSEntry.second;
        if(!pPps->minorErrors.empty()) {
            minorErrors.push_back(tr("PPS #") + QString::number(pPps->pps_pic_parameter_set_id));
            std::transform(pPps->minorErrors.begin(), pPps->minorErrors.end(), std::back_inserter(minorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
        if(!pPps->majorErrors.empty()) {
            majorErrors.push_back(tr("PPS #") + QString::number(pPps->pps_pic_parameter_set_id));
            std::transform(pPps->majorErrors.begin(), pPps->majorErrors.end(), std::back_inserter(majorErrors), [](const std::string& err){
                return " - " + QString(err.c_str());
            });
        }
    }
    emit updateErrorView(tr("PPS errors"), minorErrors, majorErrors);
}

/* Checks if the most recent access unit is the start of a new GOP.
   If it is, moves it to a new GOP, and checks the previous GOP for 
   errors before discarding it.
 */
void QDecoderModel::checkForNewGOP(){
    if(m_currentGOPModel.size() > 1){
        QSharedPointer<QAccessUnitModel> pAccessUnitModel = m_currentGOPModel.back();
        if(pAccessUnitModel->isH264()){
            const H264AccessUnit* lastAccessUnit = std::get<const H264AccessUnit*>(pAccessUnitModel->m_pAccessUnit);
            if(!lastAccessUnit->slice() || lastAccessUnit->slice()->nal_unit_type != H264NAL::UnitType_IDRFrame) {
                return;
            }
        } else if(pAccessUnitModel->isH265()){
            const H265AccessUnit* lastAccessUnit = std::get<const H265AccessUnit*>(pAccessUnitModel->m_pAccessUnit);
            if(!lastAccessUnit->slice() || !lastAccessUnit->slice()->isIDR()) {
                return;
            }
        }
        m_currentGOPModel.pop_back();
        validateCurrentGOP();
        m_currentGOPModel.clear();
        m_currentGOPModel.push_back(pAccessUnitModel);
    }
}

void QDecoderModel::decodeCurrentH264GOP(){
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : m_currentGOPModel){
        auto decodedFrame = m_decodedFrames.find(pAccessUnitModel->m_id);
        bool noValidDecodedFrame = decodedFrame == m_decodedFrames.end() || decodedFrame->get() == nullptr;
        if(noValidDecodedFrame && std::get<const H264AccessUnit*>(pAccessUnitModel->m_pAccessUnit)->decodable){
            decodeH264Slice(pAccessUnitModel);
        }
    }
}

void QDecoderModel::decodeCurrentH265GOP(){
    QVector<QSharedPointer<QAccessUnitModel>> pDecodableAccessUnitModels;
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : m_currentGOPModel){
        if(std::get<const H265AccessUnit*>(pAccessUnitModel->m_pAccessUnit)->hasMajorErrors()) continue;
        if(!m_decodedFrames[pAccessUnitModel->m_id] && std::get<const H265AccessUnit*>(pAccessUnitModel->m_pAccessUnit)->decodable) pDecodableAccessUnitModels.push_back(pAccessUnitModel);
    }
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pDecodableAccessUnitModels) decodeH265Slice(pAccessUnitModel);
}

/* Checks for structure errors in the current GOP.
 */
void QDecoderModel::validateCurrentGOP(){
    if(m_currentGOPModel.empty()) return;
    if(m_currentGOPModel.front()->isH264()){
        validateH264GOPFrames();
    } else if(m_currentGOPModel.front()->isH265()){
        validateH265GOPFrames();
    }
}

void QDecoderModel::validateH264GOPFrames(){
    uint16_t prevFrameNumber = 0;
    bool encounteredIFrame = false;
    bool noSPSorPPS = true;
    uint16_t maxFrameNumber = 0;
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : m_currentGOPModel){        
        const H264AccessUnit* pAccessUnit = std::get<const H264AccessUnit*>(pAccessUnitModel->m_pAccessUnit);
        if(pAccessUnit->empty() || !pAccessUnit->slice()) continue;
        const H264Slice* pSlice = pAccessUnit->slice();
        if(pSlice->frame_num > maxFrameNumber) maxFrameNumber = pSlice->frame_num;
        if(pSlice->slice_type == H264Slice::SliceType_I) encounteredIFrame = true;
        // PPS & SPS check : no exploitable frame numbers if either is absent
        if(!pSlice->getPPS() || !pSlice->getSPS()){
            pAccessUnitModel->m_status = QAccessUnitModel::Status_REFERENCED_PPS_OR_SPS_MISSING;
            continue;
        }
        noSPSorPPS = false;
        if(pAccessUnit->hasMajorErrors()){
            pAccessUnitModel->m_status = QAccessUnitModel::Status_INVALID_STRUCTURE;
            continue;
        }
        if(!encounteredIFrame && pSlice->slice_type != H264Slice::SliceType_I) {
            pAccessUnitModel->m_status = QAccessUnitModel::Status_REFERENCED_IFRAME_MISSING;
            continue;
        }
        if(pSlice->frame_num < prevFrameNumber && (prevFrameNumber + 1)%pSlice->getSPS()->MaxFrameNumber != pSlice->frame_num) {
            pAccessUnitModel->m_status = QAccessUnitModel::Status_OUT_OF_ORDER;
        }
        prevFrameNumber = pSlice->frame_num;
        if(pAccessUnit->hasMinorErrors() && pAccessUnitModel->m_status == QAccessUnitModel::Status_OK){
            pAccessUnitModel->m_status = QAccessUnitModel::Status_NON_CONFORMING;
        }
    }
}

void QDecoderModel::validateH265GOPFrames(){
    uint16_t prevFrameNumber = 0;
    bool encounteredIFrame = false;
    bool noSPSorPPS = true;
    uint16_t maxFrameNumber = 0;
    uint16_t lastNonBFrameNumber = 0;
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : m_currentGOPModel){        
        const H265AccessUnit* pAccessUnit = std::get<const H265AccessUnit*>(pAccessUnitModel->m_pAccessUnit);
        if(pAccessUnit->empty() || !pAccessUnit->slice()) continue;
        const H265Slice* pSlice = pAccessUnit->slice();
        if(pSlice->slice_pic_order_cnt_lsb > maxFrameNumber) maxFrameNumber = pSlice->slice_pic_order_cnt_lsb;
        if(pSlice->slice_type == H265Slice::SliceType_I) encounteredIFrame = true;
        // PPS & SPS check : no exploitable frame numbers if either is absent
        if(!pSlice->getPPS() || !pSlice->getSPS() || !pSlice->getVPS()){
            pAccessUnitModel->m_status = QAccessUnitModel::Status_REFERENCED_PPS_OR_SPS_MISSING;
            continue;
        }
        noSPSorPPS = false;
        if(!encounteredIFrame && pSlice->slice_type != H265Slice::SliceType_I) {
            pAccessUnitModel->m_status = QAccessUnitModel::Status_REFERENCED_IFRAME_MISSING;
            continue;
        }
        if(pAccessUnit->hasMajorErrors()){
            pAccessUnitModel->m_status = QAccessUnitModel::Status_INVALID_STRUCTURE;
            continue;
        }
        if(pSlice->slice_type == H265Slice::SliceType_B){
            if(pSlice->slice_pic_order_cnt_lsb > lastNonBFrameNumber) pAccessUnitModel->m_status = QAccessUnitModel::Status_OUT_OF_ORDER;
        } else {
            lastNonBFrameNumber = pSlice->slice_pic_order_cnt_lsb;
            if(pSlice->slice_pic_order_cnt_lsb < prevFrameNumber && (prevFrameNumber + 1)%pSlice->getSPS()->computeMaxFrameNumber() != pSlice->slice_pic_order_cnt_lsb) {
                pAccessUnitModel->m_status = QAccessUnitModel::Status_OUT_OF_ORDER;
            }
        }
        
        prevFrameNumber = pSlice->slice_pic_order_cnt_lsb;
        if(pAccessUnit->hasMinorErrors() && pAccessUnitModel->m_status == QAccessUnitModel::Status_OK){
            pAccessUnitModel->m_status = QAccessUnitModel::Status_NON_CONFORMING;
        }
    }
}

void QDecoderModel::updateH264StatusBarSize(){
    std::deque<H264GOP*> GOPs = m_pH264Stream->getGOPs();
    uint64_t size = std::accumulate(GOPs.begin(), GOPs.end(), 0, [](uint64_t acc, const H264GOP* GOP){
        return acc + GOP->byteSize();
    });
    emit updateSize(size);
}

void QDecoderModel::updateH264StatusBarValidity(){
    std::deque<H264GOP*> GOPs = m_pH264Stream->getGOPs();
    uint32_t valid = std::accumulate(GOPs.begin(), GOPs.end(), 0, [](uint32_t acc, const H264GOP* GOP){
        std::vector<H264AccessUnit*> pAccessUnits = GOP->getAccessUnits();
        return acc + std::accumulate(pAccessUnits.begin(), pAccessUnits.end(), 0, [](uint32_t accAU, const H264AccessUnit* pAccessUnit){
            return accAU + (pAccessUnit->isValid() ? 1 : 0);
        });
    });
    uint32_t total = std::accumulate(GOPs.begin(), GOPs.end(), 0, [](uint32_t acc, const H264GOP* GOP){
        return acc + GOP->size();
    });
    emit updateValidity(valid, total);
}

void QDecoderModel::updateH264StatusBarStatus(){
    std::deque<H264GOP*> GOPs = m_pH264Stream->getGOPs();
    if(!m_majorStreamErrors.empty() || std::any_of(GOPs.begin(), GOPs.end(), [](H264GOP* pGOP){
        return pGOP->hasMajorErrors();
    })) emit updateStatus(StreamStatus::StreamStatus_Damaged);
    else if(!m_minorStreamErrors.empty() || std::any_of(GOPs.begin(), GOPs.end(), [](H264GOP* pGOP){
        return pGOP->hasMinorErrors();
    })) emit updateStatus(StreamStatus::StreamStatus_NonConformant);
    else updateStatus(StreamStatus::StreamStatus_OK);
}

void QDecoderModel::updateH264StatusBar(){
    updateH264StatusBarSize();
    updateH264StatusBarValidity();
    updateH264StatusBarStatus();
}

void QDecoderModel::updateH265StatusBarSize(){
    std::deque<H265GOP*> GOPs = m_pH265Stream->getGOPs();
    uint64_t size = std::accumulate(GOPs.begin(), GOPs.end(), 0, [](uint64_t acc, const H265GOP* GOP){
        return acc + GOP->byteSize();
    });
    emit updateSize(size);
}

void QDecoderModel::updateH265StatusBarValidity(){
    std::deque<H265GOP*> GOPs = m_pH265Stream->getGOPs();
    uint32_t valid = std::accumulate(GOPs.begin(), GOPs.end(), 0, [](uint32_t acc, const H265GOP* GOP){
        std::vector<H265AccessUnit*> pAccessUnits = GOP->getAccessUnits();
        return acc + std::accumulate(pAccessUnits.begin(), pAccessUnits.end(), 0, [](uint32_t accAU, const H265AccessUnit* pAccessUnit){
            return accAU + (pAccessUnit->isValid() ? 1 : 0);
        });
    });
    uint32_t total = std::accumulate(GOPs.begin(), GOPs.end(), 0, [](uint32_t acc, const H265GOP* GOP){
        return acc + GOP->size();
    });
    emit updateValidity(valid, total);
}

void QDecoderModel::updateH265StatusBarStatus(){
    std::deque<H265GOP*> GOPs = m_pH265Stream->getGOPs();
    if(!m_majorStreamErrors.empty() || std::any_of(GOPs.begin(), GOPs.end(), [](H265GOP* pGOP){
        return pGOP->hasMajorErrors();
    })) emit updateStatus(StreamStatus::StreamStatus_Damaged);
    else if(!m_minorStreamErrors.empty() || std::any_of(GOPs.begin(), GOPs.end(), [](H265GOP* pGOP){
        return pGOP->hasMinorErrors();
    })) emit updateStatus(StreamStatus::StreamStatus_NonConformant);
    else updateStatus(StreamStatus::StreamStatus_OK);
}

void QDecoderModel::updateH265StatusBar(){
    updateH265StatusBarSize();
    updateH265StatusBarValidity();
    updateH265StatusBarStatus();
}

// https://stackoverflow.com/questions/68048292/converting-an-avframe-to-qimage-with-conversion-of-pixel-format
QImage* QDecoderModel::getQImageFromH264Frame(const AVFrame* pFrame) {
    if (!m_pH264SwsCtx || m_frameWidth != pFrame->width || m_frameHeight != pFrame->height || m_pixelFormat != pFrame->format) {
        if (m_pH264SwsCtx) {
            sws_freeContext(m_pH264SwsCtx);
        }
        m_pH264SwsCtx = sws_getContext(
                        pFrame->width,
                        pFrame->height,
                        static_cast<AVPixelFormat>(pFrame->format),
                        pFrame->width,
                        pFrame->height,
                        AV_PIX_FMT_RGB24,
                        SWS_BICUBIC, nullptr, nullptr, nullptr);
        if (!m_pH264SwsCtx) {
            qDebug() << "Failed to create sws context";
            return nullptr;
        }
        m_frameWidth = pFrame->width;
        m_frameHeight = pFrame->height;
        m_pixelFormat = pFrame->format;
    }

    int imgBytesSize = 3 * pFrame->width * pFrame->height;
    unsigned char* rgbBuffer = nullptr;
    rgbBuffer = static_cast<unsigned char*>(malloc(imgBytesSize));
    if (!rgbBuffer) {
        qDebug() << "Error allocating buffer for frame conversion";
        return nullptr;
    }

    uint8_t* rgbData[1] = { rgbBuffer };
    int rgbLinesizes[1] = { 3 * pFrame->width };

    if (sws_scale(m_pH264SwsCtx,
                  pFrame->data,
                  pFrame->linesize, 0,
                  pFrame->height,
                  rgbData,
                  rgbLinesizes) != pFrame->height) {
        qDebug() << "Error changing frame color range";
        return nullptr;
    }

    QImage* pImage = new QImage(rgbBuffer, pFrame->width, pFrame->height, QImage::Format_RGB888, [](void* data) {
        free(data);
    }, rgbBuffer);

    rgbBuffer = nullptr;
    return pImage;
}

QImage* QDecoderModel::getQImageFromH265Frame(const AVFrame* pFrame) {
    if (!m_pH265SwsCtx || m_frameWidth != pFrame->width || m_frameHeight != pFrame->height || m_pixelFormat != pFrame->format) {
        if (m_pH265SwsCtx) {
            sws_freeContext(m_pH265SwsCtx);
        }
        m_pH265SwsCtx = sws_getContext(
                        pFrame->width,
                        pFrame->height,
                        static_cast<AVPixelFormat>(pFrame->format),
                        pFrame->width,
                        pFrame->height,
                        AV_PIX_FMT_RGB24,
                        SWS_BICUBIC, nullptr, nullptr, nullptr);
        if (!m_pH265SwsCtx) {
            qDebug() << "Failed to create sws context";
            return nullptr;
        }
        m_frameWidth = pFrame->width;
        m_frameHeight = pFrame->height;
        m_pixelFormat = pFrame->format;
    }

    int imgBytesSize = 3 * pFrame->width * pFrame->height;
    unsigned char* rgbBuffer = nullptr;
    rgbBuffer = static_cast<unsigned char*>(malloc(imgBytesSize));
    if (!rgbBuffer) {
        qDebug() << "Error allocating buffer for frame conversion";
        return nullptr;
    }

    uint8_t* rgbData[1] = { rgbBuffer };
    int rgbLinesizes[1] = { 3 * pFrame->width };

    if (sws_scale(m_pH265SwsCtx,
                  pFrame->data,
                  pFrame->linesize, 0,
                  pFrame->height,
                  rgbData,
                  rgbLinesizes) != pFrame->height) {
        qDebug() << "Error changing frame color range";
        return nullptr;
    }

    QImage* pImage = new QImage(rgbBuffer, pFrame->width, pFrame->height, QImage::Format_RGB888, [](void* data) {
        free(data);
    }, rgbBuffer);

    rgbBuffer = nullptr;
    return pImage;
}



void QDecoderModel::decodeH264Slice(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    if(!std::get<const H264AccessUnit*>(pAccessUnitModel->m_pAccessUnit)->slice()) return;
    AVPacket* pPacket = av_packet_alloc();
    pPacket->size = std::get<const H264AccessUnit*>(pAccessUnitModel->m_pAccessUnit)->byteSize();
    pPacket->data = pAccessUnitModel->serialize();
    if(avcodec_send_packet(m_pH264CodecCtx, pPacket) < 0){
        qDebug() << "Couldn't send packet for decoding";
        return;
    }
    av_packet_free(&pPacket);
    AVFrame* pFrame = av_frame_alloc();
    if(!pFrame){
        qDebug() << "Couldn't allocate frame";
        return;
    }
    m_requestedFrames.push(pAccessUnitModel);


    int receivedFrame = avcodec_receive_frame(m_pH264CodecCtx, pFrame);
    while(receivedFrame == 0 && !m_requestedFrames.empty()){
        while(std::get<const H264AccessUnit*>(m_requestedFrames.front()->m_pAccessUnit)->hasMajorErrors()) m_requestedFrames.pop();
        m_decodedFrames[m_requestedFrames.front()->m_id] = QSharedPointer<QImage>(getQImageFromH264Frame(pFrame));
        if(m_liveContent) emit updateVideoFrameViewImage(m_decodedFrames[m_requestedFrames.front()->m_id] );
        m_requestedFrames.pop();
        receivedFrame = avcodec_receive_frame(m_pH265CodecCtx, pFrame);
    }
    av_frame_free(&pFrame); 
    avformat_network_deinit();
}

void QDecoderModel::decodeH265Slice(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    if(!std::get<const H265AccessUnit*>(pAccessUnitModel->m_pAccessUnit)->slice() || pAccessUnitModel->decoded) return;
    AVPacket* pPacket = av_packet_alloc();
    pPacket->size = std::get<const H265AccessUnit*>(pAccessUnitModel->m_pAccessUnit)->byteSize();
    pPacket->data = pAccessUnitModel->serialize();
    if(avcodec_send_packet(m_pH265CodecCtx, pPacket) < 0){
        qDebug() << "Couldn't send packet for decoding";
        return;
    }
    av_packet_free(&pPacket);
    AVFrame* pFrame = av_frame_alloc();
    if(!pFrame){
        qDebug() << "Couldn't allocate frame";
        return;
    }
    
    int receivedFrame = avcodec_receive_frame(m_pH265CodecCtx, pFrame);
    while(receivedFrame == 0){
        while(std::get<const H265AccessUnit*>(m_requestedFrames.front()->m_pAccessUnit)->hasMajorErrors()) m_requestedFrames.pop();
        m_decodedFrames[m_requestedFrames.front()->m_id] = QSharedPointer<QImage>(getQImageFromH265Frame(pFrame));
        if(m_liveContent) emit updateVideoFrameViewImage(m_decodedFrames[m_requestedFrames.front()->m_id] );
        m_requestedFrames.pop();
        receivedFrame = avcodec_receive_frame(m_pH265CodecCtx, pFrame);
    }
    pAccessUnitModel->decoded = true;
    av_frame_free(&pFrame); 
    avformat_network_deinit();

}