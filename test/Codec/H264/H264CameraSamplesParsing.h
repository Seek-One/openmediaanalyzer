#ifndef CODEC_H264_SAMPLESPARSING_H_
#define CODEC_H264_SAMPLESPARSING_H_

#include "Codec/H264/H264Stream.h"


#include <QtTest>

class H264CameraSamplesParsing : public QObject {
	Q_OBJECT
public:
	H264CameraSamplesParsing(const char* szDirTestFile);

private slots:
	void test_h264AxisBitstream();
	void test_h264IQEyeBitstream();
	void test_h264Sony4kBitstream();

private:
	QByteArray loadFrame(const QDir& dirFrame, const QString& szFilename);
	void loadStream(const QString& szDirName, H264Stream& stream);

private:
	const char* m_szDirTestFile;
};

#endif // CODEC_H264_SAMPLESPARSING_H_
