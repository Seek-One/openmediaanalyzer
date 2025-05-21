#include <QtTest>

#include "Codec/H264/H264ValidSamplesParsing.h"
#include "Codec/H264/H264InvalidSamplesParsing.h"
#include "Codec/H265/H265ValidSamplesParsing.h"
#include "Codec/H265/H265InvalidSamplesParsing.h"

int main(int argc, char**argv)
{
	QCoreApplication app(argc, argv);

	int iRes = 0;

	if(iRes == 0){
		H264ValidSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	if(iRes == 0){
		H264InvalidSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	if(iRes == 0){
		H265ValidSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	if(iRes == 0){
		H265InvalidSamplesParsing testCase(".");
		iRes = QTest::qExec(&testCase, argc, argv);
	}

	return (iRes == 0) ? 0 : 1;
}