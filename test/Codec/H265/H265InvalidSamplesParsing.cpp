#include "Codec/H26X/H26XBitstreamReader.h"
#include "Codec/H26X/H26XErrorsMsg.h"
#include "Codec/H265/H265GOP.h"
#include "Codec/H265/H265AccessUnit.h"
#include "Codec/H265/H265SPS.h"
#include "Codec/H265/H265PPS.h"
#include "Codec/H265/H265Slice.h"
#include "Codec/H265/H265SEI.h"

#include "H265InvalidSamplesParsing.h"

const static char g_start_code[4] = {0x00, 0x00, 0x00, 0x01};

H265InvalidSamplesParsing::H265InvalidSamplesParsing(const char* szDirTestFile)
{
	m_szDirTestFile = szDirTestFile;
}

bool H265InvalidSamplesParsing::hasError(const H26XErrors& errors, H26XError::Level level, const std::string& szErrorMsg)
{
	for(const H26XError& error : errors)
	{
		if((level == H26XError::Unknown) || (error.level() == level)){
			if(error.message() == szErrorMsg){
				return true;
			}
		}
	}
	return false;
}

void H265InvalidSamplesParsing::loadStream(const QString& szDirName, H265Stream& stream, const bool expectedPacketParsingResult){
	QDir dirFrame = QDir(QString("%0/stream-samples/%1").arg(m_szDirTestFile, szDirName));

	QStringList listFrame = dirFrame.entryList(QDir::Files, QDir::Name);
	QByteArray bitstream;
	for (int i = 0; i < listFrame.size(); ++i){
		const QString& szFileFrame = listFrame[i];
		QByteArray data = loadFrame(dirFrame, szFileFrame);
		QVERIFY(data.size() > 0);
		bitstream.append(data);
	}
	QVERIFY(stream.parsePacket((uint8_t*)bitstream.data(), bitstream.size()) == expectedPacketParsingResult);
}

void H265InvalidSamplesParsing::test_h265FramesOutOfOrderBitstream()
{
	H265Stream stream;
	loadStream("h265-out-of-order", stream, true);
	stream.lastPacketParsed();
	QVERIFY(stream.errors.hasMajorErrors());
	QVERIFY(hasError(stream.errors, H26XError::Major, "[GOP] Out of order frames detected"));
}

void H265InvalidSamplesParsing::test_h265MissingIFrameBitstream()
{
	H265Stream stream;
	loadStream("h265-missing-i-frame", stream, true);
	stream.lastPacketParsed();
	QVERIFY(stream.errors.hasMajorErrors());
	QVERIFY(hasError(stream.errors, H26XError::Major, "[GOP] No I-frame detected"));
	std::vector<H265AccessUnit*> firstGOPUnits = stream.getGOPs().front()->getAccessUnits();
	for(const H265AccessUnit* pAccessUnit : firstGOPUnits){
		QVERIFY(pAccessUnit->errors.hasMajorErrors());
		QVERIFY(hasError(pAccessUnit->errors, H26XError::Major, "No reference I-frame"));
	}
}

void H265InvalidSamplesParsing::test_h265MissingPPSBitstream(){
	H265Stream stream;
	loadStream("h265-missing-pps", stream, true);
	stream.lastPacketParsed();
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	for(const H265AccessUnit* pAccessUnit : pAccessUnits){
		H265Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pAccessUnit->errors.hasMajorErrors());
		QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] reference to unknown PPS (0)"));
	}
}
void H265InvalidSamplesParsing::test_h265MissingSPSBitstream(){
	H265Stream stream;
	loadStream("h265-missing-sps", stream, true);
	stream.lastPacketParsed();
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	H265PPS* pPPS = pAccessUnits.front()->slice()->getPPS();
	QVERIFY(hasError(pPPS->errors, H26XError::Major, "[PPS] reference to unknown SPS (0)"));
	for(const H265AccessUnit* pAccessUnit : pAccessUnits){
		H265Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pSlice->errors.hasMajorErrors());
		QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] reference to unknown SPS (0)"));
	}
}
void H265InvalidSamplesParsing::test_h265MissingVPSBitstream(){
	H265Stream stream;
	loadStream("h265-missing-vps", stream, true);
	stream.lastPacketParsed();
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	H265SPS* pSPS = pAccessUnits.front()->slice()->getSPS();
	QVERIFY(hasError(pSPS->errors, H26XError::Major, "[SPS] reference to unknown VPS (0)"));
	for(const H265AccessUnit* pAccessUnit : pAccessUnits){
		H265Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pSlice->errors.hasMajorErrors());
		QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] reference to unknown VPS (0)"));
	}

}
void H265InvalidSamplesParsing::test_h265SkippedFrameBitstream(){
	H265Stream stream;
	loadStream("h265-skipped-frame", stream, true);
	stream.lastPacketParsed();
	const uint16_t SKIPPED_FRAME_NUM = 12;
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	for(H265AccessUnit* pAccessUnit : pAccessUnits){
		QVERIFY(pAccessUnit->frameNumber().value() != SKIPPED_FRAME_NUM);
	}
	H265Slice* postSkippedFrameSlice = pAccessUnits[SKIPPED_FRAME_NUM]->slice(); // POC #13
	QVERIFY(postSkippedFrameSlice != nullptr);
	QVERIFY(postSkippedFrameSlice->errors.hasMajorErrors());
	QVERIFY(hasError(postSkippedFrameSlice->errors, H26XError::Major, "[Slice] Missing reference frames : [12]"));
}

void H265InvalidSamplesParsing::test_h265EndOfStreamBitstream()
{
	H265Stream stream;
	loadStream("h265-end-of-stream", stream, true);	
	QVERIFY(stream.getGOPs().size() == 1);
	QVERIFY(hasError(stream.errors, H26XError::Major, std::string("[NAL Header] ")+std::string(END_OF_STREAM_ERR_MSG)));
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 2);
	H265AccessUnit* pFirstAccessUnit = pAccessUnits.front();
	QVERIFY(pFirstAccessUnit->size() == 4);
	std::vector<H265NALUnit*> pFirstAccessUnitNALUnits = pFirstAccessUnit->getNALUnits();

	H265SPS* pSPS = (H265SPS*)pFirstAccessUnitNALUnits[0];
	QVERIFY(pSPS->errors.hasMajorErrors());
	QVERIFY(hasError(pSPS->errors, H26XError::Major, std::string("[SPS] ")+std::string(END_OF_STREAM_ERR_MSG)));
	
	H265PPS* pPPS = (H265PPS*)pFirstAccessUnitNALUnits[1];
	QVERIFY(pPPS->errors.hasMajorErrors());
	QVERIFY(hasError(pPPS->errors, H26XError::Major, std::string("[PPS] ")+std::string(END_OF_STREAM_ERR_MSG)));
	
	H265SEI* pSEI = (H265SEI*)pFirstAccessUnitNALUnits[2];
	QVERIFY(!pSEI->errors.hasMinorErrors());
	QVERIFY(hasError(pSEI->errors, H26XError::Minor, "[SEI] Payload size exceeds remaining bitstream length left"));
	
	H265Slice* pSlice = (H265Slice*)pFirstAccessUnitNALUnits[3];
	QVERIFY(pSlice->errors.hasMajorErrors());
	QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] reference to unknown VPS (0)"));

	QVERIFY(pAccessUnits.back()->size() == 1);
	pSlice = (H265Slice*)pAccessUnits.back()->getNALUnits().front();
	QVERIFY(!pSlice->errors.hasMajorErrors());
	QVERIFY(hasError(pSlice->errors, H26XError::Major, std::string("[Slice] ")+std::string(END_OF_STREAM_ERR_MSG)));
}


QByteArray H265InvalidSamplesParsing::loadFrame(const QDir& dirFrame, const QString& szFilename)
{
	QByteArray data;
	QFile fileFrame (dirFrame.filePath(szFilename));

	if (fileFrame.open(QFile::ReadOnly)) {
		data = fileFrame.readAll();
		if(memcmp(data, g_start_code, 4) != 0) data.prepend(g_start_code, 4);
		fileFrame.close();
	}

	return data;
}
