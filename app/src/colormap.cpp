#include "colormap.h"

void ColorMap::initialize()
{
    //wohoo - the qt animation interpolator supports all that is needed for a color table
    QVariantAnimation interpolator;
    interpolator.setStartValue(QColor("transparent"));
    interpolator.setEndValue(QColor("transparent"));
    interpolator.setEasingCurve( m_easingCurveMap.value( m_easingCurve, QEasingCurve::Linear ) );
    interpolator.setDuration(COLOR_STEPS);
    interpolator.setKeyValueAt(0.0, QColor(Qt::transparent));
    interpolator.setKeyValueAt(1.0, QColor(Qt::transparent));

    //add key values to the interpolator
    foreach(const QJsonValue& value, m_colorStops) {
        QJsonObject colorStop = value.toObject();
        qreal stopValue = colorStop.value("stop").toDouble(-1.0);
        if((stopValue < 0.0) || (stopValue > 1.0)) continue;  //invalid stop value, ignore

        interpolator.setKeyValueAt(stopValue, QColor(colorStop.value("color").toString("transparent")));
    }

    //generate color lut
    for(int index = 0; index < COLOR_STEPS; index++) {
        interpolator.setCurrentTime(index);
        m_lookupTable[index] = interpolator.currentValue().value<QColor>().name();
    }
}

ColorMap::ColorMap(QObject* parent) : QObject(parent)
  ,m_level(30.0)
  ,m_window(120.0)
{
    //init easing curve map
    m_easingCurveMap.insert("Linear", QEasingCurve::Linear);
    m_easingCurveMap.insert("InQuad", QEasingCurve::InQuad);
    m_easingCurveMap.insert("InCubic", QEasingCurve::InCubic);
    m_easingCurveMap.insert("InQuart", QEasingCurve::InQuart);
    m_easingCurveMap.insert("InQuint", QEasingCurve::InQuint);
    m_easingCurveMap.insert("InSine", QEasingCurve::InSine);
    m_easingCurveMap.insert("InExpo", QEasingCurve::InExpo);
    m_easingCurveMap.insert("InBack", QEasingCurve::InBack);
    m_easingCurveMap.insert("InBounce", QEasingCurve::InBounce);
    m_easingCurveMap.insert("InCirc", QEasingCurve::InCirc);
    m_easingCurveMap.insert("InOutBack", QEasingCurve::InOutBack);
    m_easingCurveMap.insert("InOutBack", QEasingCurve::InOutBack);
    m_easingCurveMap.insert("InOutCirc", QEasingCurve::InOutCirc);
    m_easingCurveMap.insert("InOutCubic", QEasingCurve::InOutCubic);
    m_easingCurveMap.insert("InOutExpo", QEasingCurve::InOutExpo);
    m_easingCurveMap.insert("InOutQuad", QEasingCurve::InOutQuad);
    m_easingCurveMap.insert("InOutQuart", QEasingCurve::InOutQuart);
    m_easingCurveMap.insert("InOutQuint", QEasingCurve::InOutQuint);
    m_easingCurveMap.insert("InOutSine", QEasingCurve::InOutSine);
    m_easingCurveMap.insert("InSine", QEasingCurve::InSine);
    m_easingCurveMap.insert("OutInQuart", QEasingCurve::OutInQuart);

    //add standard color table
    QByteArray rainbowTable = "["
            "{ \"stop\": 1.00, \"color\": \"red\" },"
            "{ \"stop\": 0.84, \"color\": \"orange\" },"
            "{ \"stop\": 0.68, \"color\": \"yellow\" },"
            "{ \"stop\": 0.52, \"color\": \"green\" },"
            "{ \"stop\": 0.40, \"color\": \"blue\" },"
            "{ \"stop\": 0.24, \"color\": \"indigo\" },"
            "{ \"stop\": 0.00, \"color\": \"violet\" }"
            "]";

    QJsonArray rainbowStops = QJsonDocument::fromJson(rainbowTable).array();
    setColorStops(rainbowStops);

    initialize();

    //rebuild lookup table if stops are changed
    QObject::connect(this, &ColorMap::colorStopsChanged, this, &ColorMap::initialize);
}

QString ColorMap::getColor(qreal value)
{
    const qreal m = COLOR_STEPS / window();
    const qreal offset = (level() - (window()/2.));
    int index = qRound( (value - offset) * m );
    index = qBound(0, index, COLOR_STEPS);
    return m_lookupTable.value(index, "transparent");
}

QJsonArray ColorMap::colorStops() const
{
    return m_colorStops;
}

QString ColorMap::easingCurve() const
{
    return m_easingCurve;
}

qreal ColorMap::level() const
{
    return m_level;
}

qreal ColorMap::window() const
{
    return m_window;
}

void ColorMap::setEasingCurve(QString arg)
{
    if (m_easingCurve != arg) {
        m_easingCurve = arg;
        emit easingCurveChanged(arg);
    }
}

void ColorMap::setColorStops(QJsonArray arg)
{
    if (m_colorStops != arg) {
        m_colorStops = arg;
        emit colorStopsChanged(arg);
    }
}

void ColorMap::setLevel(qreal arg)
{
    if (m_level != arg) {
        m_level = arg;
        emit levelChanged(arg);
    }
}

void ColorMap::setWindow(qreal arg)
{
    if (m_window != arg) {
        m_window = arg;
        emit windowChanged(arg);
    }
}

