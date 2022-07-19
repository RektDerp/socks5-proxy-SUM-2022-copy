#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "tablemodel.h"
#include <QFont>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<TableModel>("TableModel", 0, 1, "TableModel");
    qRegisterMetaType<QFont*>("const QFont*");
    QQmlApplicationEngine engine;
    TableModel model;
    //engine.rootContext()->setContextProperty("myModel", &model);

    const QUrl url(u"qrc:/ui/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
