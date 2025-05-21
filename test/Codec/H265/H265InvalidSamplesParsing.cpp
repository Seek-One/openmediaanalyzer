#include "Codec/H26X/H26XBitstreamReader.h"
#include "Codec/H265/H265AccessUnit.h"
#include "Codec/H265/H265SEI.h"

#include "H265InvalidSamplesParsing.h"

const static char g_start_code[4] = {0x00, 0x00, 0x00, 0x01};

H265InvalidSamplesParsing::H265InvalidSamplesParsing(const char* szDirTestFile)
{
	m_szDirTestFile = szDirTestFile;
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

void H265InvalidSamplesParsing::test_h265FramesOutOfOrderBitstream(){
	H265Stream stream;
	loadStream("h265-out-of-order", stream, true);
	stream.lastPacketParsed();
	QVERIFY(!stream.majorErrors.empty());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "[GOP] Out of order frames detected") != stream.majorErrors.end());
}
void H265InvalidSamplesParsing::test_h265MissingIFrameBitstream(){
	H265Stream stream;
	loadStream("h265-missing-i-frame", stream, true);
	stream.lastPacketParsed();
	QVERIFY(!stream.majorErrors.empty());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "[GOP] No I-frame detected") != stream.majorErrors.end());
}

void H265InvalidSamplesParsing::test_h265MissingPPSBitstream(){
	H265Stream stream;
	loadStream("h265-missing-pps", stream, true);
	stream.lastPacketParsed();
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	for(const H265AccessUnit* pAccessUnit : pAccessUnits){
		H265Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(!pSlice->majorErrors.empty());
		QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] reference to unknown PPS (0)") != pSlice->majorErrors.end());
	}
}
void H265InvalidSamplesParsing::test_h265MissingSPSBitstream(){
	H265Stream stream;
	loadStream("h265-missing-sps", stream, true);
	stream.lastPacketParsed();
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	H265PPS* pPPS = pAccessUnits.front()->slice()->getPPS();
	QVERIFY(std::find(pPPS->majorErrors.begin(), pPPS->majorErrors.end(), "[PPS] reference to unknown SPS (0)") != pPPS->majorErrors.end());
	for(const H265AccessUnit* pAccessUnit : pAccessUnits){
		H265Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(!pSlice->majorErrors.empty());
		QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] reference to unknown SPS (0)") != pSlice->majorErrors.end());
	}
}
void H265InvalidSamplesParsing::test_h265MissingVPSBitstream(){
	H265Stream stream;
	loadStream("h265-missing-vps", stream, true);
	stream.lastPacketParsed();
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	H265SPS* pSPS = pAccessUnits.front()->slice()->getSPS();
	QVERIFY(std::find(pSPS->majorErrors.begin(), pSPS->majorErrors.end(), "[SPS] reference to unknown VPS (0)") != pSPS->majorErrors.end());
	for(const H265AccessUnit* pAccessUnit : pAccessUnits){
		H265Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(!pSlice->majorErrors.empty());
		QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] reference to unknown VPS (0)") != pSlice->majorErrors.end());
	}

}
void H265InvalidSamplesParsing::test_h265SkippedFrameBitstream(){
	H265Stream stream;
	loadStream("h265-skipped-frame", stream, true);
	stream.lastPacketParsed();
	const uint16_t SKIPPED_FRAME_NUM = 12;
	QVERIFY(!stream.majorErrors.empty());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "[GOP] Skipped frames detected") != stream.majorErrors.end());
	for(H265AccessUnit* pAccessUnit : stream.getAccessUnits()) QVERIFY(pAccessUnit->frameNumber().value() != SKIPPED_FRAME_NUM);
}

void H265InvalidSamplesParsing::test_h265EndOfStreamBitstream()
{
	H265Stream stream;
	loadStream("h265-end-of-stream", stream, true);	
	QVERIFY(stream.getGOPs().size() == 1);
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), std::string("[NAL Header] ")+std::string(END_OF_STREAM_ERR_MSG)) != stream.majorErrors.end());
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 2);
	H265AccessUnit* pFirstAccessUnit = pAccessUnits.front();
	QVERIFY(pFirstAccessUnit->size() == 4);
	std::vector<H265NAL*> pFirstAccessUnitNALUnits = pFirstAccessUnit->getNALUnits();

	H265SPS* pSPS = (H265SPS*)pFirstAccessUnitNALUnits[0];
	QVERIFY(!pSPS->majorErrors.empty());
	QVERIFY(std::find(pSPS->majorErrors.begin(), pSPS->majorErrors.end(), std::string("[SPS] ")+std::string(END_OF_STREAM_ERR_MSG)) != pSPS->majorErrors.end());
	
	H265PPS* pPPS = (H265PPS*)pFirstAccessUnitNALUnits[1];
	QVERIFY(!pPPS->majorErrors.empty());
	QVERIFY(std::find(pPPS->majorErrors.begin(), pPPS->majorErrors.end(), std::string("[PPS] ")+std::string(END_OF_STREAM_ERR_MSG)) != pPPS->majorErrors.end());
	
	H265SEI* pSEI = (H265SEI*)pFirstAccessUnitNALUnits[2];
	QVERIFY(!pSEI->minorErrors.empty());
	QVERIFY(std::find(pSEI->minorErrors.begin(), pSEI->minorErrors.end(), "[SEI] Payload size exceeds remaining bitstream length left") != pSEI->minorErrors.end());
	
	H265Slice* pSlice = (H265Slice*)pFirstAccessUnitNALUnits[3];
	QVERIFY(!pSlice->majorErrors.empty());
	QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] reference to unknown VPS (0)") != pSlice->majorErrors.end());

	QVERIFY(pAccessUnits.back()->size() == 1);
	pSlice = (H265Slice*)pAccessUnits.back()->getNALUnits().front();
	QVERIFY(!pSlice->majorErrors.empty());
	QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), std::string("[Slice] ")+std::string(END_OF_STREAM_ERR_MSG)) != pSlice->majorErrors.end());
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
