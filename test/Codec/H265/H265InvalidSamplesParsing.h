#ifndef CODEC_H265_INVALIDSAMPLESPARSING_H_
#define CODEC_H265_INVALIDSAMPLESPARSING_H_

#include "Codec/H265/H265Stream.h"

#include <QtTest>

class H265InvalidSamplesParsing : public QObject {
	Q_OBJECT
public:
	H265InvalidSamplesParsing(const char* szDirTestFile);

private slots:
	void test_h265FramesOutOfOrderBitstream();
	void test_h265MissingIFrameBitstream();
	void test_h265MissingPPSBitstream();
	void test_h265MissingSPSBitstream();
	void test_h265MissingVPSBitstream();
	void test_h265SkippedFrameBitstream();
	void test_h265EndOfStreamBitstream();

private:
	QByteArray loadFrame(const QDir& dirFrame, const QString& szFilename);
	void loadStream(const QString& szDirName, H265Stream& stream, const bool expectedPacketParsingResult);

	static bool hasError(const H26XErrors& errors, H26XError::Level level, const std::string& szErrorMsg);

private:
	const char* m_szDirTestFile;
};

#endif // CODEC_H265_INVALIDSAMPLESPARSING_H_
