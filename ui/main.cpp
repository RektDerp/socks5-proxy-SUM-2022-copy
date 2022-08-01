#include "sessionmodel.h"
#include "sortfiltersessionmodel.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QIcon>

int main(int argc, char *argv[])
{
    using namespace proxy::ui;
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon("icon.png"));
    qmlRegisterType<SessionModel>("SessionModel", 0, 1, "SessionModel");
    qmlRegisterType<SortFilterSessionModel>("SortFilterSessionModel", 0, 1, "SortFilterSessionModel");
    QQmlApplicationEngine engine;
    const QUrl url(QString("qrc:/resources/main/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
