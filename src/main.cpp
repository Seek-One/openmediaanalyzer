#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "GUI/QWindowMain.h"
#include "GUIController/QWindowMainController.h"

int main(int argc, char *argv[])
{
#ifdef WIN32
	// Add runtime dir in plugins search path
	QCoreApplication::addLibraryPath(".");
#endif

    QApplication app(argc, argv);
    app.setOrganizationName("seekone");
    app.setApplicationName("OpenMediaAnalyzer");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QCommandLineParser parser;
    parser.setApplicationDescription("H264/H265 analysis and error checking");
    parser.addHelpOption();
    parser.process(app);

    QTranslator translator;
    if (translator.load(QLocale("fr"), "", "", ".")) app.installTranslator(&translator);
    QWindowMain* pWindowMain = new QWindowMain();
    if(pWindowMain) pWindowMain->show();

    QWindowMainController controller;
    controller.init(pWindowMain);
    
    app.exec();

    if(pWindowMain) {
        delete pWindowMain;
        pWindowMain = nullptr;
    }
    return 0;
}