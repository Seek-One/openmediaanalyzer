#include <QApplication>
#include <QSettings>
#include <QTranslator>

#include "GUI/QWindowMain.h"
#include "GUIController/QWindowMainController.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    app.setOrganizationName("seekone");
    app.setApplicationName("OpenMediaAnalyzer");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    // QTranslator translator;
    // if (translator.load(QLocale("fr"), "", "", "ts")) app.installTranslator(&translator);
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