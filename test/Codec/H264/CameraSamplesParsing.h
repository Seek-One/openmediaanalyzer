#ifndef CODECTESTCASE_H_
#define CODECTESTCASE_H_

#include <QtTest>

class CameraSamplesParsing : public QObject {
	Q_OBJECT
public:
	CameraSamplesParsing(const char* szDirTestFile);

private slots:
	void test_h264AxisBitstream();

private:
	QByteArray loadFrame(const QDir& dirFrame, const QString& szFilename);

private:
	const char* m_szDirTestFile;
};

#endif // CODECTESTCASE_H_
