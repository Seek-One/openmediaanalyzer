#include <QDebug>
#include <QUuid>
#include <sstream>
#include <set>
#include <algorithm>

#include "../GUI/QFolderView.h"
#include "../Codec/H264/H264Stream.h"
#include "../Codec/H264/H264GOP.h"
#include "../Codec/H264/H264Slice.h"
#include "../Codec/H264/H264AUD.h"
#include "../Codec/H264/H264SEI.h"

#include "QDecoderModel.h"

QDecoderModel::QDecoderModel():
    m_pH264Stream(nullptr), m_pSelectedFrameModel(nullptr), m_tabIndex(0), m_pCodec(avcodec_find_decoder(AV_CODEC_ID_H264)), m_pSwsCtx(nullptr)
{
    if(!m_pCodec) {
        qDebug() << "Couldn't find decoder";
        return;
    }
    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    if(!m_pCodecCtx){
        qDebug() << "Couldn't allocate codec context";
        return;
    }
    if(avcodec_open2(m_pCodecCtx, m_pCodec, nullptr) < 0){
        qDebug() << "Couldn't open codec";
        return;
    }
    
}

QDecoderModel::~QDecoderModel(){
    if(m_pH264Stream)delete m_pH264Stream;
    m_streamErrors.clear();
    m_decodedFrames.clear();
    avcodec_free_context(&m_pCodecCtx);
    if(m_pSwsCtx){
        sws_freeContext(m_pSwsCtx);
        m_pSwsCtx = nullptr;
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

void buildSPSView(QDecoderModel* pStreamModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderLabels(QDecoderModel::headers);
    QStandardItem* root = model->invisibleRootItem();
    for(auto entry : H264SPS::SPSMap){
        root->appendRow(QDecoderModel::modelItemFromFields(entry.second->dump_fields(), "SPS Unit #" + QString::number(entry.first)));
    }
    emit pStreamModel->updateSPSInfoView(model);
}

void buildPPSView(QDecoderModel* pStreamModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderLabels(QDecoderModel::headers);
    QStandardItem* root = model->invisibleRootItem();
    for(auto entry : H264PPS::PPSMap){
        root->appendRow(QDecoderModel::modelItemFromFields(entry.second->dump_fields(), "PPS Unit #" + QString::number(entry.first)));
    }
    emit pStreamModel->updatePPSInfoView(model);
}

void QDecoderModel::reset(){
    if(m_pH264Stream) delete m_pH264Stream;
    m_pH264Stream = new H264Stream();
    m_streamErrors.clear();
    m_pSelectedFrameModel = nullptr;
    m_currentGOPModel.clear();
    m_decodedFrames.clear();
    frameSelected(nullptr);
    buildSPSView(this);
    buildPPSView(this);
    emit updateVideoFrameView(nullptr);
    if(m_pSwsCtx){
        sws_freeContext(m_pSwsCtx);
        m_pSwsCtx = nullptr;
    }
    avcodec_free_context(&m_pCodecCtx);
    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    if(!m_pCodecCtx){
        qDebug() << "Couldn't allocate codec context";
        return;
    }
    if(avcodec_open2(m_pCodecCtx, m_pCodec, nullptr) < 0){
        qDebug() << "Couldn't open codec";
        return;
    }
}



void QDecoderModel::fileLoaded(uint8_t* fileContent, quint32 fileSize){
    uint32_t accessUnitCountBefore = m_pH264Stream->accessUnitCount();
    uint8_t PPSUnitsBefore = H264PPS::PPSMap.size();
    uint8_t SPSUnitsBefore = H264SPS::SPSMap.size();
    m_pH264Stream->parsePacket(fileContent, fileSize);
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : m_currentGOPModel){
        auto decodedFrame = m_decodedFrames.find(pAccessUnitModel->m_id);
        bool noValidDecodedFrame = decodedFrame == m_decodedFrames.end() || decodedFrame->get() == nullptr;
        if(noValidDecodedFrame && pAccessUnitModel->m_pAccessUnit->slice()){
            m_decodedFrames[pAccessUnitModel->m_id] = QSharedPointer<QImage>(decodeSlice(pAccessUnitModel));
        }
    }
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
    for(H264GOP* pGOP : m_pH264Stream->getGOPs()){
        for(H264AccessUnit* pAccessUnit : pGOP->getAccessUnits()) pAccessUnit->validate();
    }
    if(accessUnitCountDiff == 0) emit updateTimelineUnits();
    else {
        QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitModels = QVector<QSharedPointer<QAccessUnitModel>>();
        std::list<H264AccessUnit*> pAccessUnits = m_pH264Stream->getLastAccessUnits(accessUnitCountDiff);
        for(auto itAccessUnit = pAccessUnits.begin();itAccessUnit != pAccessUnits.end();++itAccessUnit){
            QUuid id = QUuid::createUuid();
            QSharedPointer<QAccessUnitModel> pAccessUnitModel = QSharedPointer<QAccessUnitModel>(new QAccessUnitModel(*itAccessUnit, id));
            if((*itAccessUnit)->slice()) m_decodedFrames[id] = QSharedPointer<QImage>(decodeSlice(pAccessUnitModel));
            pAccessUnitModels.push_back(pAccessUnitModel);
            m_currentGOPModel.push_back(pAccessUnitModel);
            checkForNewGOP();
        }        
        emit addTimelineUnits(pAccessUnitModels);
    }

    if(H264PPS::PPSMap.size() != PPSUnitsBefore) buildPPSView(this);
    if(H264SPS::SPSMap.size() != SPSUnitsBefore) buildSPSView(this);
    switch(m_tabIndex){
        case 0:
            emitStreamErrors();
            break;
        case 1:
            emitSPSErrors();
            break;
        case 2:
            emitPPSErrors();
            break;
    }
}

QStringList errorListFromAccessUnit(const H264AccessUnit* accessUnit){
    QStringList errors;
    std::transform(accessUnit->errors.begin(), accessUnit->errors.end(), std::back_inserter(errors), [](const std::string& err){
        return err.c_str();
    });
    for(auto& NALUnit : accessUnit->NALUnits){
        std::vector<std::string> NALUnitErrors;
        switch(NALUnit->nal_unit_type){
            case H264NAL::UnitType_AUD:
                NALUnitErrors = reinterpret_cast<H264AUD*>(NALUnit.get())->errors;
                break;
            case H264NAL::UnitType_NonIDRFrame:
            case H264NAL::UnitType_IDRFrame:
                NALUnitErrors = reinterpret_cast<H264Slice*>(NALUnit.get())->errors;
                break;
            case H264NAL::UnitType_PPS:
                NALUnitErrors = reinterpret_cast<H264PPS*>(NALUnit.get())->errors;
                break;
            case H264NAL::UnitType_SPS:
                NALUnitErrors = reinterpret_cast<H264SPS*>(NALUnit.get())->errors;
                break;
            case H264NAL::UnitType_SEI:
                NALUnitErrors = reinterpret_cast<H264SEI*>(NALUnit.get())->errors;
                break;
        }
        std::transform(NALUnitErrors.begin(), NALUnitErrors.end(), std::back_inserter(errors), [](const std::string& err){
            return err.c_str();
        });
    }
    return errors;
}

void modelFromAccessUnit(QStandardItemModel* model, const H264AccessUnit* accessUnit){
    for(auto& NALUnit : accessUnit->NALUnits){
        switch(NALUnit->nal_unit_type){
            case H264NAL::UnitType_AUD:
                model->appendRow(QDecoderModel::modelItemFromFields(reinterpret_cast<H264AUD*>(NALUnit.get())->dump_fields(), "Access Unit Delimiter"));
                break;
            case H264NAL::UnitType_NonIDRFrame:
                model->appendRow(QDecoderModel::modelItemFromFields(reinterpret_cast<H264Slice*>(NALUnit.get())->dump_fields(), "Non-IDR Picture"));
                break;
            case H264NAL::UnitType_IDRFrame:
                model->appendRow(QDecoderModel::modelItemFromFields(reinterpret_cast<H264Slice*>(NALUnit.get())->dump_fields(), "IDR Picture"));
                break;
            case H264NAL::UnitType_PPS:
                model->appendRow(QDecoderModel::modelItemFromFields(reinterpret_cast<H264PPS*>(NALUnit.get())->dump_fields(), "Picture Parameter Set"));
                break;
            case H264NAL::UnitType_SPS:
                model->appendRow(QDecoderModel::modelItemFromFields(reinterpret_cast<H264SPS*>(NALUnit.get())->dump_fields(), "Sequence Parameter Set"));
                break;
            case H264NAL::UnitType_SEI:
                model->appendRow(QDecoderModel::modelItemFromFields(reinterpret_cast<H264SEI*>(NALUnit.get())->dump_fields(), "Supplemental Enhancement Information"));
                break;
        }
    }
}

void QDecoderModel::frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    QStandardItemModel* model = new QStandardItemModel(0, 2);
    model->setHorizontalHeaderLabels(QDecoderModel::headers);
    m_pSelectedFrameModel = pAccessUnitModel;
    if(pAccessUnitModel && pAccessUnitModel->m_pAccessUnit) {
        modelFromAccessUnit(model, pAccessUnitModel->m_pAccessUnit);
        if(m_tabIndex == 0) emit updateErrorView("Access unit errors", errorListFromAccessUnit(pAccessUnitModel->m_pAccessUnit));
        if(m_decodedFrames[pAccessUnitModel->m_id]) emit updateVideoFrameView(m_decodedFrames[pAccessUnitModel->m_id]);
    } else if(m_tabIndex == 0){
        emit updateErrorView("Stream errors", m_streamErrors);
    }
    emit updateFrameInfoView(model);
}

void QDecoderModel::framesTabOpened(){
    m_tabIndex = 0;
    emitStreamErrors();
}

void QDecoderModel::spsTabOpened(){
    m_tabIndex = 1;
    emitSPSErrors();
}

void QDecoderModel::ppsTabOpened(){
    m_tabIndex = 2;
    emitPPSErrors();
}

void QDecoderModel::folderLoaded(){
    validateCurrentGOP();
    emit updateTimelineUnits();
}

void QDecoderModel::emitStreamErrors(){
    if(m_pH264Stream && !m_pSelectedFrameModel) emit updateErrorView("Stream errors", m_streamErrors);
}

void QDecoderModel::emitSPSErrors(){
    QStringList errors;
    for(auto SPSEntry : H264SPS::SPSMap){
        H264SPS* pSps = SPSEntry.second;
        if(pSps->errors.empty()) continue;
        errors.push_back("SPS #" + QString::number(pSps->seq_parameter_set_id));
        std::transform(pSps->errors.begin(), pSps->errors.end(), std::back_inserter(errors), [](const std::string& err){
            return " - " + QString(err.c_str());
        });
    }
    emit updateErrorView("SPS errors", errors);
}

void QDecoderModel::emitPPSErrors(){
    QStringList errors;
    for(auto PPSEntry : H264PPS::PPSMap){
        H264PPS* pPps = PPSEntry.second;
        if(pPps->errors.empty()) continue;
        errors.push_back("PPS #" + QString::number(pPps->seq_parameter_set_id));
        std::transform(pPps->errors.begin(), pPps->errors.end(), std::back_inserter(errors), [](const std::string& err){
            return " - " + QString(err.c_str());
        });
    }
    emit updateErrorView("PPS errors", errors);
}

/* Checks if the most recent access unit is the start of a new GOP.
   If it is, moves it to a new GOP, and checks the previous GOP for 
   errors before discarding it.
 */
void QDecoderModel::checkForNewGOP(){
    if(m_currentGOPModel.size() > 1){
        QSharedPointer<QAccessUnitModel> pAccessUnitModel = m_currentGOPModel.back();
        const H264AccessUnit* lastAccessUnit = pAccessUnitModel->m_pAccessUnit;
        if(!lastAccessUnit->slice() || lastAccessUnit->slice()->slice_type != H264Slice::SliceType_I) {
            return;
        }
        m_currentGOPModel.pop_back();
        validateCurrentGOP();
        m_currentGOPModel.clear();
        m_currentGOPModel.push_back(pAccessUnitModel);
    }
}

/* Checks for structure errors in the current GOP.
 */
void QDecoderModel::validateCurrentGOP(){
    uint16_t prevFrameNumber = 0;
    bool encounteredIFrame = false;
    bool noSPSorPPS = true;
    uint16_t maxFrameNumber = 0;
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : m_currentGOPModel){        
        const H264AccessUnit* pAccessUnit = pAccessUnitModel->m_pAccessUnit;
        if(pAccessUnit->empty() || !pAccessUnit->slice()) continue;
        const H264Slice* pSlice = pAccessUnit->slice();
        if(pSlice->frame_num > maxFrameNumber) maxFrameNumber = pSlice->frame_num;
        if(pSlice->slice_type == H264Slice::SliceType_I) encounteredIFrame = true;
        // PPS & SPS check : no exploitable frame numbers if either is absent
        if(!pSlice->getPPS() || !pSlice->getSPS()){
            pAccessUnitModel->m_status = Status::REFERENCED_PPS_OR_SPS_MISSING;
            continue;
        }
        noSPSorPPS = false;
        if(!encounteredIFrame && pSlice->slice_type != H264Slice::SliceType_I) {
            pAccessUnitModel->m_status = Status::REFERENCED_IFRAME_MISSING;
            continue;
        }

        if(pSlice->frame_num < prevFrameNumber) {
            pAccessUnitModel->m_status = Status::OUT_OF_ORDER;
        }
        prevFrameNumber = pSlice->frame_num;
    }
    if(maxFrameNumber+1 != m_currentGOPModel.size() && !noSPSorPPS) addStreamError("[GOP] Skipped frames detected");
    if(!encounteredIFrame) addStreamError("[GOP] No I-frame detected");
}

void QDecoderModel::addStreamError(QString err){
    m_streamErrors.push_back(err);
    if(m_streamErrors.size() > ERR_MSG_LIMIT) m_streamErrors.pop_front();
}

// https://stackoverflow.com/questions/68048292/converting-an-avframe-to-qimage-with-conversion-of-pixel-format
QImage* QDecoderModel::getQImageFromFrame(const AVFrame* pFrame) {
    if (!m_pSwsCtx || m_frameWidth != pFrame->width || m_frameHeight != pFrame->height || m_pixelFormat != pFrame->format) {
        if (m_pSwsCtx) {
            sws_freeContext(m_pSwsCtx);
        }
        m_pSwsCtx = sws_getContext(
                        pFrame->width,
                        pFrame->height,
                        static_cast<AVPixelFormat>(pFrame->format),
                        pFrame->width,
                        pFrame->height,
                        AV_PIX_FMT_RGB24,
                        SWS_BICUBIC, nullptr, nullptr, nullptr);
        if (!m_pSwsCtx) {
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

    if (sws_scale(m_pSwsCtx,
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


QImage* QDecoderModel::decodeSlice(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    // reset context if IDR access unit ?
    if(!pAccessUnitModel->m_pAccessUnit->slice()) return nullptr;
    AVPacket* pPacket = av_packet_alloc();
    pPacket->size = pAccessUnitModel->m_pAccessUnit->size();
    pPacket->data = pAccessUnitModel->serialize();
    if(avcodec_send_packet(m_pCodecCtx, pPacket) < 0){
        qDebug() << "Couldn't send packet for decoding";
        return nullptr;
    }

    AVFrame* pFrame = av_frame_alloc();
    if(!pFrame){
        qDebug() << "Couldn't allocate frame";
        return nullptr;
    }

    int receivedFrame = avcodec_receive_frame(m_pCodecCtx, pFrame);
    if(receivedFrame == AVERROR(EAGAIN) || receivedFrame == AVERROR_EOF || receivedFrame < 0){
        qDebug() << "Error when receiving frame :" << receivedFrame;
        return nullptr;
    }
    av_packet_free(&pPacket);
    QImage* pImage = getQImageFromFrame(pFrame);
    av_frame_free(&pFrame); 
    avformat_network_deinit();
    return pImage;
}