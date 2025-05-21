#ifndef CODEC_H265_SAMPLESPARSING_H_
#define CODEC_H265_SAMPLESPARSING_H_

#include "Codec/H265/H265Stream.h"


#include <QtTest>

class H265CameraSamplesParsing : public QObject {
	Q_OBJECT
public:
	H265CameraSamplesParsing(const char* szDirTestFile);

private slots:
	void test_h265DahuaBitstream();

private:
	QByteArray loadFrame(const QDir& dirFrame, const QString& szFilename);
	void loadStream(const QString& szDirName, H265Stream& stream);

private:
	const char* m_szDirTestFile;
};

#endif // CODEC_H265_SAMPLESPARSING_H_
