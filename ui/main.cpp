#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "sessionmodel.h"
#include "sortfiltersessionmodel.h"
#include <QFont>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<SessionModel>("SessionModel", 0, 1, "SessionModel");
    qmlRegisterType<SortFilterSessionModel>("SortFilterSessionModel", 0, 1, "SortFilterSessionModel");
    qRegisterMetaType<QFont*>("const QFont*");
    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/resources/main/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
