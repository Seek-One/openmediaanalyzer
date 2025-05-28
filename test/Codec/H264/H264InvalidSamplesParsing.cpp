#include "Codec/H26X/H26XBitstreamReader.h"
#include "Codec/H264/H264SEI.h"
#include "Codec/H264/H264GOP.h"

#include "H264InvalidSamplesParsing.h"

const static char g_start_code[4] = {0x00, 0x00, 0x00, 0x01};

H264InvalidSamplesParsing::H264InvalidSamplesParsing(const char* szDirTestFile)
{
	m_szDirTestFile = szDirTestFile;
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

void H264InvalidSamplesParsing::test_h264AccessUnitErrorsBitstream(){
	H264Stream stream;
	loadStream("h264-access-unit-errors", stream, true);
	stream.lastPacketParsed();
	QVERIFY(stream.majorErrors.empty());
	QVERIFY(stream.minorErrors.empty());
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 2);
	QVERIFY(pAccessUnits.front()->isValid());
	H264AccessUnit* pLastAccessUnit = pAccessUnits.back();
	std::vector<H264NAL*> pLastAccessUnitNALUnits = pLastAccessUnit->getNALUnits();
	QVERIFY(pLastAccessUnitNALUnits[0]->nal_unit_type == H264NAL::UnitType_SEI);
	QVERIFY(pLastAccessUnitNALUnits[1]->nal_unit_type == H264NAL::UnitType_AUD);
	QVERIFY(pLastAccessUnitNALUnits[2]->nal_unit_type == H264NAL::UnitType_AUD);
	QVERIFY(pLastAccessUnitNALUnits[3]->nal_unit_type == H264NAL::UnitType_NonIDRFrame);
	QVERIFY(pLastAccessUnitNALUnits[4]->nal_unit_type == H264NAL::UnitType_NonIDRFrame);
	QVERIFY(!pLastAccessUnit->minorErrors.empty());
	std::vector<std::string> expectedAccessUnitErrors = {"Access unit delimiter not in first position", "Multiple access unit delimiters detected",
		"SEI units block is not preceding the primary coded picture", "SEI buffering period message not leading SEI unit",
		"Pictures are not ordered in ascending order of redundant_pic_cnt"}; 
	for(const std::string& expectedErr : expectedAccessUnitErrors){
		QVERIFY(std::find(pLastAccessUnit->minorErrors.begin(), pLastAccessUnit->minorErrors.end(), expectedErr) != pLastAccessUnit->minorErrors.end());
	}
}

void H264InvalidSamplesParsing::test_h264FramesOutOfOrderBitstream(){
	H264Stream stream;
	loadStream("h264-frames-out-of-order", stream, true);	
	stream.lastPacketParsed();
	QVERIFY(!stream.majorErrors.empty());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "[GOP] Out of order frames detected") != stream.majorErrors.end());
}

void H264InvalidSamplesParsing::test_h264InvalidFrameNumsBitstream(){
	H264Stream stream;
	loadStream("h264-frames-out-of-order", stream, true);	
	stream.lastPacketParsed();
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 18);
	H264Slice* pSlice1 = pAccessUnits[11]->slice();
	QVERIFY(pSlice1 != nullptr);
	QVERIFY(!pSlice1->minorErrors.empty());
	QVERIFY(std::find(pSlice1->minorErrors.begin(), pSlice1->minorErrors.end(), "[Slice frame number] frame_num isn't directly succeeding PrevRefFrameNum") != pSlice1->minorErrors.end());	
	
	H264Slice* pSlice2 = pAccessUnits[12]->slice();
	QVERIFY(pSlice2 != nullptr);
	QVERIFY(!pSlice2->minorErrors.empty());	
	QVERIFY(std::find(pSlice2->minorErrors.begin(), pSlice2->minorErrors.end(), "[Slice frame number] frame_num isn't directly succeeding PrevRefFrameNum") != pSlice2->minorErrors.end());	
	
	H264Slice* pSlice3 = pAccessUnits[13]->slice();
	QVERIFY(pSlice3 != nullptr);
	QVERIFY(!pSlice3->minorErrors.empty());	
	QVERIFY(std::find(pSlice3->minorErrors.begin(), pSlice3->minorErrors.end(), "[Slice frame number] Previous frame/field has a frame_num marked as unused") != pSlice2->minorErrors.end());	

}

void H264InvalidSamplesParsing::test_h264MissingIFrameBitstream(){
	H264Stream stream;
	loadStream("h264-missing-iframe", stream, true);	
	stream.lastPacketParsed();
	QVERIFY(!stream.majorErrors.empty());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "[GOP] No I-frame detected") != stream.majorErrors.end());
	std::vector<H264AccessUnit*> firstGOPUnits = stream.getGOPs().front()->getAccessUnits();
	for(const H264AccessUnit* pAccessUnit : firstGOPUnits){
		QVERIFY(!pAccessUnit->majorErrors.empty());
		QVERIFY(std::find(pAccessUnit->majorErrors.begin(), pAccessUnit->majorErrors.end(), "No reference I-frame") != pAccessUnit->majorErrors.end());
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
		QVERIFY(!pSlice->majorErrors.empty());
		QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] reference to unknown PPS (0)") != pSlice->majorErrors.end());
	}
}

void H264InvalidSamplesParsing::test_h264MissingSPSBitstream(){
	H264Stream stream;
	loadStream("h264-missing-sps", stream, true);	
	stream.lastPacketParsed();
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	H264PPS* pPPS = pAccessUnits.front()->slice()->getPPS();
	QVERIFY(std::find(pPPS->majorErrors.begin(), pPPS->majorErrors.end(), "[PPS] reference to unknown SPS (0)") != pPPS->majorErrors.end());
	for(const H264AccessUnit* pAccessUnit : pAccessUnits){
		H264Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(!pSlice->majorErrors.empty());
		QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] reference to unknown SPS (0)") != pSlice->majorErrors.end());
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
		QVERIFY(!pSlice->majorErrors.empty());
		QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] reference to unknown PPS (0)") != pSlice->majorErrors.end());
	}
}

void H264InvalidSamplesParsing::test_h264SkippedFrameBitstream(){
	H264Stream stream;
	loadStream("h264-skipped-frame", stream, true);	
	stream.lastPacketParsed();
	const uint16_t SKIPPED_FRAME_NUM = 21;
	QVERIFY(!stream.majorErrors.empty());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "[GOP] Skipped frames detected") != stream.majorErrors.end());
	for(H264AccessUnit* pAccessUnit : stream.getAccessUnits()) QVERIFY(pAccessUnit->frameNumber().value() != SKIPPED_FRAME_NUM);
}
void H264InvalidSamplesParsing::test_h264SyntaxErrorsBitstream(){
	H264Stream stream;
	loadStream("h264-syntax-errors", stream, true);	
	stream.lastPacketParsed();

	H264PPS* pPPS = H264PPS::PPSMap[0];
	QVERIFY(pPPS != nullptr);
	QVERIFY(!pPPS->majorErrors.empty());
	QVERIFY(std::find(pPPS->majorErrors.begin(), pPPS->majorErrors.end(), "[PPS] Prematurely reached end of bitstream during parsing") != pPPS->majorErrors.end());
	QVERIFY(!pPPS->minorErrors.empty());
	QVERIFY(std::find(pPPS->minorErrors.begin(), pPPS->minorErrors.end(), "[NAL Header] Picture parameter set marked as unimportant") != pPPS->minorErrors.end());
	
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 3);
	H264Slice* pSlice = pAccessUnits.front()->slice();
	QVERIFY(pSlice != nullptr);
	QVERIFY(!pSlice->minorErrors.empty());
	QVERIFY(std::find(pSlice->minorErrors.begin(), pSlice->minorErrors.end(), "[Slice] frame_num of an IDR picture (384) should be 0") != pSlice->minorErrors.end());
	
	pSlice = pAccessUnits.back()->slice();
	QVERIFY(pSlice != nullptr);
	QVERIFY(pSlice->slice_type < H264Slice::SliceType_P || pSlice->slice_type > H264Slice::SliceType_SI);
	QVERIFY(!pSlice->majorErrors.empty());
	QVERIFY(std::find(pSlice->majorErrors.begin(), pSlice->majorErrors.end(), "[Slice] Invalid slice type") != pSlice->majorErrors.end());
}

void H264InvalidSamplesParsing::test_h264CorruptedBitstream(){
	H264Stream stream;
	loadStream("h264-corrupted", stream, true);	
	stream.lastPacketParsed();
	QVERIFY(!stream.majorErrors.empty());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "Unhandled NAL type detected") != stream.majorErrors.end());
	QVERIFY(std::find(stream.majorErrors.begin(), stream.majorErrors.end(), "[GOP] Skipped frames detected") != stream.majorErrors.end());
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
