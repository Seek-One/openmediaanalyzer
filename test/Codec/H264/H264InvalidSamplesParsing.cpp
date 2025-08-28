#include "Codec/H26X/H26XBitstreamReader.h"
#include "Codec/H26X/H26XErrorsMsg.h"
#include "Codec/H264/H264SEI.h"
#include "Codec/H264/H264GOP.h"
#include "Codec/H264/H264SPS.h"
#include "Codec/H264/H264PPS.h"
#include "Codec/H264/H264Slice.h"
#include "Codec/H264/H264AccessUnit.h"

#include "H264InvalidSamplesParsing.h"

const static char g_start_code[4] = {0x00, 0x00, 0x00, 0x01};

H264InvalidSamplesParsing::H264InvalidSamplesParsing(const char* szDirTestFile)
{
	m_szDirTestFile = szDirTestFile;
}

bool H264InvalidSamplesParsing::hasError(const H26XErrors& errors, H26XError::Level level, const std::string& szErrorMsg)
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

void H264InvalidSamplesParsing::loadStream(const QString& szDirName, H264Stream& stream, const bool expectedPacketParsingResult){
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

void H264InvalidSamplesParsing::test_h264EndOfStreamBitstream()
{
	H264Stream stream;
	loadStream("h264-end-of-stream", stream, false);	
	QVERIFY(stream.getGOPs().size() == 1);
	QVERIFY(hasError(stream.errors, H26XError::Major, std::string("[NAL Header] ")+std::string(END_OF_STREAM_ERR_MSG)));
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 2);
	H264AccessUnit* pFirstAccessUnit = pAccessUnits.front();
	QVERIFY(pFirstAccessUnit->size() == 4);
	std::vector<H264NALUnit*> pFirstAccessUnitNALUnits = pFirstAccessUnit->getNALUnits();

	H264SPS* pSPS = (H264SPS*)pFirstAccessUnitNALUnits[0];
	QVERIFY(pSPS->errors.hasMajorErrors());
	QVERIFY(hasError(pSPS->errors, H26XError::Major, std::string("[SPS] ")+std::string(END_OF_STREAM_ERR_MSG)));
	
	H264PPS* pPPS = (H264PPS*)pFirstAccessUnitNALUnits[1];
	QVERIFY(pPPS->errors.hasMajorErrors());
	QVERIFY(hasError(pPPS->errors, H26XError::Major, std::string("[PPS] ")+std::string(END_OF_STREAM_ERR_MSG)));
	
	H264SEI* pSEI = (H264SEI*)pFirstAccessUnitNALUnits[2];
	QVERIFY(pSEI->errors.hasMinorErrors());
	QVERIFY(hasError(pSEI->errors, H26XError::Minor, "[SEI] Payload size exceeds remaining bitstream length left"));
	
	H264Slice* pSlice = (H264Slice*)pFirstAccessUnitNALUnits[3];
	QVERIFY(pSlice->errors.hasMajorErrors());
	QVERIFY(hasError(pSlice->errors, H26XError::Major, std::string("[Slice] ")+std::string(END_OF_STREAM_ERR_MSG)));

	QVERIFY(pAccessUnits.back()->size() == 1);
	pSlice = (H264Slice*)pAccessUnits.back()->getNALUnits().front();
	QVERIFY(pSlice->errors.hasMajorErrors());
	QVERIFY(hasError(pSlice->errors, H26XError::Major, std::string("[Slice] ")+std::string(END_OF_STREAM_ERR_MSG)));
}

void H264InvalidSamplesParsing::test_h264AccessUnitErrorsBitstream(){
	H264Stream stream;
	loadStream("h264-access-unit-errors", stream, true);
	stream.lastPacketParsed();
	QVERIFY(!stream.errors.hasMajorErrors());
	QVERIFY(!stream.errors.hasMinorErrors());
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 2);
	QVERIFY(pAccessUnits.front()->isValid());
	H264AccessUnit* pLastAccessUnit = pAccessUnits.back();
	std::vector<H264NALUnit*> pLastAccessUnitNALUnits = pLastAccessUnit->getNALUnits();
	QVERIFY(pLastAccessUnitNALUnits[0]->getNalUnitType() == H264NALUnitType::SEI);
	QVERIFY(pLastAccessUnitNALUnits[1]->getNalUnitType() == H264NALUnitType::AUD);
	QVERIFY(pLastAccessUnitNALUnits[2]->getNalUnitType() == H264NALUnitType::AUD);
	QVERIFY(pLastAccessUnitNALUnits[3]->getNalUnitType() == H264NALUnitType::NonIDRFrame);
	QVERIFY(pLastAccessUnitNALUnits[4]->getNalUnitType() == H264NALUnitType::NonIDRFrame);
	QVERIFY(pLastAccessUnit->errors.hasMinorErrors());
	std::vector<std::string> expectedAccessUnitErrors = {"Access unit delimiter not in first position", "Multiple access unit delimiters detected",
		"SEI units block is not preceding the primary coded picture", "SEI buffering period message not leading SEI unit",
		"Pictures are not ordered in ascending order of redundant_pic_cnt"}; 
	for(const std::string& expectedErr : expectedAccessUnitErrors){
		QVERIFY(hasError(pLastAccessUnit->errors, H26XError::Minor, expectedErr));
	}
}

void H264InvalidSamplesParsing::test_h264FramesOutOfOrderBitstream(){
	H264Stream stream;
	loadStream("h264-frames-out-of-order", stream, true);	
	stream.lastPacketParsed();
	QVERIFY(stream.errors.hasMajorErrors());
	QVERIFY(hasError(stream.errors, H26XError::Major, "[GOP] Out of order frames detected"));
}

void H264InvalidSamplesParsing::test_h264InvalidFrameNumsBitstream(){
	H264Stream stream;
	loadStream("h264-frames-out-of-order", stream, true);	
	stream.lastPacketParsed();
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 18);
	H264Slice* pSlice1 = pAccessUnits[11]->slice();
	QVERIFY(pSlice1 != nullptr);
	QVERIFY(pSlice1->errors.hasMinorErrors());
	QVERIFY(hasError(pSlice1->errors, H26XError::Minor, "[Slice frame number] frame_num isn't directly succeeding PrevRefFrameNum"));
	
	H264Slice* pSlice2 = pAccessUnits[12]->slice();
	QVERIFY(pSlice2 != nullptr);
	QVERIFY(pSlice2->errors.hasMinorErrors());
	QVERIFY(hasError(pSlice2->errors, H26XError::Minor, "[Slice frame number] frame_num isn't directly succeeding PrevRefFrameNum"));
	
	H264Slice* pSlice3 = pAccessUnits[13]->slice();
	QVERIFY(pSlice3 != nullptr);
	QVERIFY(pSlice3->errors.hasMinorErrors());
	QVERIFY(hasError(pSlice3->errors, H26XError::Minor, "[Slice frame number] Previous frame/field has a frame_num marked as unused"));

}

void H264InvalidSamplesParsing::test_h264MissingIFrameBitstream(){
	H264Stream stream;
	loadStream("h264-missing-iframe", stream, true);	
	stream.lastPacketParsed();
	QVERIFY(stream.errors.hasMajorErrors());
	QVERIFY(hasError(stream.errors, H26XError::Major, "[GOP] No I-frame detected"));
	std::vector<H264AccessUnit*> firstGOPUnits = stream.getGOPs().front()->getAccessUnits();
	for(const H264AccessUnit* pAccessUnit : firstGOPUnits){
		QVERIFY(pAccessUnit->errors.hasMajorErrors());
		QVERIFY(hasError(pAccessUnit->errors, H26XError::Major, "No reference I-frame"));
	}
}

void H264InvalidSamplesParsing::test_h264MissingPPSBitstream(){
	H264Stream stream;
	loadStream("h264-missing-pps", stream, true);	
	stream.lastPacketParsed();
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	for(const H264AccessUnit* pAccessUnit : pAccessUnits){
		H264Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pSlice->errors.hasMajorErrors());
		QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] reference to unknown PPS (0)"));
	}
}

void H264InvalidSamplesParsing::test_h264MissingSPSBitstream(){
	H264Stream stream;
	loadStream("h264-missing-sps", stream, true);	
	stream.lastPacketParsed();
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	H264PPS* pPPS = pAccessUnits.front()->slice()->getPPS();
	QVERIFY(hasError(pPPS->errors, H26XError::Major, "[PPS] reference to unknown SPS (0)"));
	for(const H264AccessUnit* pAccessUnit : pAccessUnits){
		H264Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pSlice->errors.hasMajorErrors());
		QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] reference to unknown SPS (0)"));
	}
}

void H264InvalidSamplesParsing::test_h264OnlyVCLBitstream(){
	H264Stream stream;
	loadStream("h264-only-vcl", stream, true);	
	stream.lastPacketParsed();
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	for(const H264AccessUnit* pAccessUnit : pAccessUnits){
		H264Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pSlice->errors.hasMajorErrors());
		QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] reference to unknown PPS (0)"));
	}
}

void H264InvalidSamplesParsing::test_h264SkippedFrameBitstream(){
	H264Stream stream;
	loadStream("h264-skipped-frame", stream, true);	
	stream.lastPacketParsed();
	const uint16_t SKIPPED_FRAME_NUM = 21;
	QVERIFY(stream.errors.hasMajorErrors());
	QVERIFY(hasError(stream.errors, H26XError::Major, "[GOP] Skipped frames detected"));
	for(H264AccessUnit* pAccessUnit : stream.getAccessUnits()) QVERIFY(pAccessUnit->frameNumber().value() != SKIPPED_FRAME_NUM);
}
void H264InvalidSamplesParsing::test_h264SyntaxErrorsBitstream(){
	H264Stream stream;
	loadStream("h264-syntax-errors", stream, true);	
	stream.lastPacketParsed();

	H264PPS* pPPS = H264PPS::PPSMap[0];
	QVERIFY(pPPS != nullptr);
	QVERIFY(pPPS->errors.hasMajorErrors());
	QVERIFY(hasError(pPPS->errors, H26XError::Major, "[PPS] Prematurely reached end of bitstream during parsing"));
	QVERIFY(pPPS->errors.hasMinorErrors());
	QVERIFY(hasError(pPPS->errors, H26XError::Minor, "[NAL Header] Picture parameter set marked as unimportant"));
	
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 3);
	H264Slice* pSlice = pAccessUnits.front()->slice();
	QVERIFY(pSlice != nullptr);
	QVERIFY(pSlice->errors.hasMinorErrors());
	QVERIFY(hasError(pSlice->errors, H26XError::Minor, "[Slice] frame_num of an IDR picture (384) should be 0"));
	
	pSlice = pAccessUnits.back()->slice();
	QVERIFY(pSlice != nullptr);
	QVERIFY(pSlice->slice_type < H264Slice::SliceType_P || pSlice->slice_type > H264Slice::SliceType_SI);
	QVERIFY(pSlice->errors.hasMajorErrors());
	QVERIFY(hasError(pSlice->errors, H26XError::Major, "[Slice] Invalid slice type"));
}

void H264InvalidSamplesParsing::test_h264CorruptedBitstream(){
	H264Stream stream;
	loadStream("h264-corrupted", stream, true);	
	stream.lastPacketParsed();
	QVERIFY(stream.errors.hasMajorErrors());
	QVERIFY(hasError(stream.errors, H26XError::Major, "Unhandled NAL type detected"));
	QVERIFY(hasError(stream.errors, H26XError::Major, "[GOP] Skipped frames detected"));
	const uint16_t SKIPPED_FRAME_NUM = 30;
	for(H264AccessUnit* pAccessUnit : stream.getAccessUnits()) QVERIFY(pAccessUnit->frameNumber().value() != SKIPPED_FRAME_NUM);
}

QByteArray H264InvalidSamplesParsing::loadFrame(const QDir& dirFrame, const QString& szFilename)
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
