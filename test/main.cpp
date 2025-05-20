#include <QtTest>

#include "Codec/H264/CameraSamplesParsing.h"
#include "Codec/H264/InvalidSamplesParsing.h"

int main(int argc, char**argv)
{
	QCoreApplication app(argc, argv);

	int iRes = 0;

	if(iRes == 0){
		CameraSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	if(iRes == 0){
		InvalidSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	return (iRes == 0) ? 0 : 1;
}