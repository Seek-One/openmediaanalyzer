#ifndef CODEC_H264_INVALIDSAMPLESPARSING_H_
#define CODEC_H264_INVALIDSAMPLESPARSING_H_

#include "Codec/H264/H264Stream.h"

#include <QtTest>

class H264InvalidSamplesParsing : public QObject {
	Q_OBJECT
public:
	H264InvalidSamplesParsing(const char* szDirTestFile);

private slots:
	void test_h264EndOfStreamBitstream();
	void test_h264AccessUnitErrorsBitstream();
	void test_h264FramesOutOfOrderBitstream();
	void test_h264InvalidFrameNumsBitstream();
	void test_h264MissingIFrameBitstream();
	void test_h264MissingPPSBitstream();
	void test_h264MissingSPSBitstream();
	void test_h264OnlyVCLBitstream();
	void test_h264SkippedFrameBitstream();
	void test_h264SyntaxErrorsBitstream();
	void test_h264CorruptedBitstream();

private:
	QByteArray loadFrame(const QDir& dirFrame, const QString& szFilename);
	void loadStream(const QString& szDirName, H264Stream& stream, const bool expectedPacketParsingResult);

	static bool hasError(const H26XErrors& errors, H26XError::Level level, const std::string& szErrorMsg);

private:
	const char* m_szDirTestFile;
};

#endif // CODEC_H264_INVALIDSAMPLESPARSING_H_
