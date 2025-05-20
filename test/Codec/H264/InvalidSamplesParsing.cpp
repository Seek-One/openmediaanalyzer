#include "Codec/H26X/H26XBitstreamReader.h"
#include "Codec/H264/H264SEI.h"

#include "InvalidSamplesParsing.h"

const static char g_start_code[4] = {0x00, 0x00, 0x00, 0x01};

InvalidSamplesParsing::InvalidSamplesParsing(const char* szDirTestFile)
{
	m_szDirTestFile = szDirTestFile;
}

void InvalidSamplesParsing::loadStream(const QString& szDirName, H264Stream& stream, const bool expectedPacketParsingResult){
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

void InvalidSamplesParsing::test_h264EndOfStreamBitstream()
{
	H264Stream stream;
	loadStream("h264-end-of-stream", stream, false);	
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), std::string("[NAL Header] ")+std::string(END_OF_STREAM_ERR_MSG)) != stream.majorErrors.end());
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 2);
	H264AccessUnit* pFirstAccessUnit = pAccessUnits.front();
	QVERIFY(pFirstAccessUnit->size() == 4);
	std::vector<H264NAL*> pFirstAccessUnitNALUnits = pFirstAccessUnit->getNALUnits();

	H264SPS* pSPS = (H264SPS*)pFirstAccessUnitNALUnits[0];
	QVERIFY(!pSPS->majorErrors.empty());
	QVERIFY(std::find(pSPS->majorErrors.begin(), pSPS->majorErrors.end(), std::string("[SPS] ")+std::string(END_OF_STREAM_ERR_MSG)) != pSPS->majorErrors.end());
	
	H264PPS* pPPS = (H264PPS*)pFirstAccessUnitNALUnits[1];
	QVERIFY(!pPPS->majorErrors.empty());
	QVERIFY(std::find(pPPS->majorErrors.begin(), pPPS->majorErrors.end(), std::string("[PPS] ")+std::string(END_OF_STREAM_ERR_MSG)) != pPPS->majorErrors.end());
	
	H264SEI* pSEI = (H264SEI*)pFirstAccessUnitNALUnits[2];
	QVERIFY(!pSEI->minorErrors.empty());
	QVERIFY(std::find(pSEI->minorErrors.begin(), pSEI->minorErrors.end(), "[SEI] Payload size exceeds remaining bitstream length left") != pSEI->minorErrors.end());
	
	H264Slice* pSlice = (H264Slice*)pFirstAccessUnitNALUnits[3];
	QVERIFY(!pSlice->majorErrors.empty());
	QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), std::string("[Slice] ")+std::string(END_OF_STREAM_ERR_MSG)) != pSlice->majorErrors.end());

	QVERIFY(pAccessUnits.back()->size() == 1);
	pSlice = (H264Slice*)pAccessUnits.back()->getNALUnits().front();
	QVERIFY(!pSlice->majorErrors.empty());
	QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), std::string("[Slice] ")+std::string(END_OF_STREAM_ERR_MSG)) != pSlice->majorErrors.end());
}

void InvalidSamplesParsing::test_h264AccessUnitErrorsBitstream(){
	H264Stream stream;
}

void InvalidSamplesParsing::test_h264FramesOutOfOrderBitstream(){
	H264Stream stream;
	// loadStream("h264-frames-out-of-order", stream, true);	
}
void InvalidSamplesParsing::test_h264MissingIFrameBitstream(){}
void InvalidSamplesParsing::test_h264MissingPPSBitstream(){}
void InvalidSamplesParsing::test_h264MissingSPSBitstream(){}
void InvalidSamplesParsing::test_h264OnlyVCLBitstream(){}
void InvalidSamplesParsing::test_h264SkippedFrameBitstream(){}
void InvalidSamplesParsing::test_h264SyntaxErrorsBitstream(){}
void InvalidSamplesParsing::test_h264CorruptedBitstream(){}

QByteArray InvalidSamplesParsing::loadFrame(const QDir& dirFrame, const QString& szFilename)
{
	QByteArray data;
	QFile fileFrame (dirFrame.filePath(szFilename));

	if (fileFrame.open(QFile::ReadOnly)) {
		data = fileFrame.readAll();
		data.prepend(g_start_code, 4);
		fileFrame.close();
	}

	return data;
}
