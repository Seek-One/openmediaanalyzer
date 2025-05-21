#include <QtTest>

#include "Codec/H264/H264CameraSamplesParsing.h"
#include "Codec/H264/H264InvalidSamplesParsing.h"
#include "Codec/H265/H265CameraSamplesParsing.h"

int main(int argc, char**argv)
{
	QCoreApplication app(argc, argv);

	int iRes = 0;

	if(iRes == 0){
		H264CameraSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	if(iRes == 0){
		H264InvalidSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	if(iRes == 0){
		H265CameraSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	return (iRes == 0) ? 0 : 1;
}