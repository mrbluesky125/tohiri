#ifndef FPGA_COLOR_MAP_H
#define FPGA_COLOR_MAP_H

#include <QtCore>
#include <QtGui>

class ColorMap : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString easingCurve READ easingCurve WRITE setEasingCurve NOTIFY easingCurveChanged)
    Q_PROPERTY(QJsonArray colorStops READ colorStops WRITE setColorStops NOTIFY colorStopsChanged)
    Q_PROPERTY(qreal level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(qreal window READ window WRITE setWindow NOTIFY windowChanged)

    QMap<int, QString> m_lookupTable;
    QMap<QString, QEasingCurve::Type> m_easingCurveMap;
    QString m_easingCurve;
    QJsonArray m_colorStops;
    qreal m_level;
    qreal m_window;

    static const int COLOR_STEPS = 255;

public:
    ColorMap(QObject* parent = 0);

    Q_INVOKABLE QString getColor(qreal value);

    QJsonArray colorStops() const;
    QString easingCurve() const;
    qreal level() const;
    qreal window() const;

public slots:
    void initialize();

    void setEasingCurve(QString arg);
    void setColorStops(QJsonArray arg);
    void setLevel(qreal arg);
    void setWindow(qreal arg);

signals:
    void easingCurveChanged(QString arg);
    void colorStopsChanged(QJsonArray arg);
    void levelChanged(qreal arg);
    void windowChanged(qreal arg);
};

#endif
