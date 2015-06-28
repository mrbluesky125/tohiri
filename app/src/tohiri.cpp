
#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QtCore>
#include <QtQml>
#include <QtQuick>
#include <QQuickView>
#include <QGuiApplication>

#include "tohir.h"
#include "colormap.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<TohIR>("harbour.tohiri.TohIR", 1, 0, "TohIR");
    qmlRegisterType<ColorMap>("harbour.tohiri.TohIR", 1, 0, "ColorMap");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    view->setSource(SailfishApp::pathTo("qml/tohiri.qml"));
    view->show();

    return app->exec();
}

