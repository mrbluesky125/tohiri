#include "tohir.h"
#include <QSettings>
#include <QCoreApplication>
#include <QTime>
#include <QtDBus/QtDBus>
#include <QDBusArgument>

#include "amg883x.h"


TohIR::TohIR(QObject *parent) : QAbstractListModel(parent)
  ,m_rawTemperatures(64, 0.0)
{

    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    m_min = 100.0;
    m_max = -20.0;
    m_avg = 0.0;
    m_thermistor = -273.0;
    m_hotSpot = 31;

    readSettings();

    controlVdd(true);

    QThread::msleep(300);

    amg = new amg883x(0x68);
}

void TohIR::readSettings()
{
    QSettings s("harbour-tohiri", "harbour-tohiri");

    s.beginGroup("View");
    writeGradientOpacity(s.value("gradientOpacity", "0.5").toReal());
    writeUpdateRate(s.value("updateRate", 500).toInt());
    writeGranularity(s.value("granularity", "2.0").toReal());
    writeContrast(s.value("contrast", 1.0).toReal());
    s.endGroup();
}

void TohIR::saveSettings()
{
    QSettings s("harbour-tohiri", "harbour-tohiri");

    s.beginGroup("View");
    s.setValue("gradientOpacity", QString::number(m_gradientOpacity,'f',2) );
    s.setValue("updateRate", m_updateRate);
    s.setValue("granularity", m_granularity);
    s.setValue("contrast", m_contrast);
    s.endGroup();
}


TohIR::~TohIR()
{
    controlVdd(false);
}

int TohIR::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 64; //8x8 sensor size
}

QVariant TohIR::data(const QModelIndex &index, int role) const
{
    if(index.row() >= m_rawTemperatures.size())
        return QVariant();

    const qreal temperature = m_rawTemperatures.value(index.row());
    if(role == int(TemperatureRole)) {
        return QVariant::fromValue(temperature);
    }
    else if(role == int(ColorRole)) {
        return QVariant::fromValue(temperatureColor(temperature, m_min, m_max, m_avg));
    }
    else if(role == int(HotSpotRole)) {
        return QVariant::fromValue(qFuzzyCompare(temperature, m_max));
    }
    else
        return QVariant();
}

QHash<int, QByteArray> TohIR::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[TemperatureRole] = "temperature";
    roleNames[ColorRole] = "color";
    roleNames[HotSpotRole] = "hotspot";
    return roleNames;
}

/* Return git describe as string (see .pro file) */
QString TohIR::readVersion() const
{
    return QString(APPVERSION);
}

/**/
qreal TohIR::readGradientOpacity()
{
    return m_gradientOpacity;
}

/**/
void TohIR::writeGradientOpacity(qreal val)
{
    m_gradientOpacity = val;

    emit gradientOpacityChanged();
}

int TohIR::readUpdateRate()
{
    return m_updateRate;
}

void TohIR::writeUpdateRate(int val)
{
    m_updateRate = val;

    emit updateRateChanged();
}

qreal TohIR::readGranularity()
{
    return m_granularity;
}

void TohIR::writeGranularity(qreal val)
{
    m_granularity = val;

    emit granularityChanged();
}

qreal TohIR::readContrast()
{
    return m_contrast;
}

void TohIR::writeContrast(qreal val)
{
    m_contrast = val;

    emit contrastChanged();
}


/* Start IR Scan function, emit changed after completed */
void TohIR::startScan()
{
    m_rawTemperatures = amg->getTemperatureArray();
    m_min = 200.0;
    m_max = -40.0;
    m_avg = 0.0;

    /* Return color gradient array */
    for(int i=0 ; i<64 ; i++)
    {
        /* Just use whole numbers */
        const qreal tmp = m_rawTemperatures.at(i);
        m_max = qMax(m_max, tmp);
        m_min = qMin(m_min, tmp);
        m_avg = m_avg + tmp;
    }

    m_avg = m_avg/64;

    /* Get RGB values for each pixel */
    m_temperatures.clear();
    for(int i=0 ; i<64 ; i++)
        m_temperatures.append(temperatureColor(m_rawTemperatures.at(i), m_min, m_max, m_avg));

    emit scanFinished();

    emit dataChanged(index(0), index(rowCount(QModelIndex())-1));
}

/* Return thermistor temperature */
void TohIR::readThermistor()
{
    m_thermistor = amg->getThermistor();
    emit thermistorChanged(m_thermistor);
}

/* Return temperature color gradients as array */
QList<QString> TohIR::readTemperatures() const
{
    return m_temperatures;
}

/* Return minimum, average and maximum temperature of last scan */
qreal TohIR::readMinTemp() const
{
    return m_min;
}

qreal TohIR::readAvgTemp() const
{
    return m_avg;
}

qreal TohIR::readMaxTemp() const
{
    return m_max;
}

qreal TohIR::thermistor() const
{
    return m_thermistor;
}

int TohIR::readHotSpot()
{
    return m_hotSpot;
}

/* Call dbus method to save screencapture */
QString TohIR::saveScreenCapture()
{
    QString ssFilename = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
            + QDateTime::currentDateTime().toString("ddMMyy-hhmmss-zzz.png");
    QDBusMessage m = QDBusMessage::createMethodCall("org.nemomobile.lipstick",
                                                    "/org/nemomobile/lipstick/screenshot",
                                                    "",
                                                    "saveScreenshot" );

    m.setArguments(QList<QVariant>() << ssFilename);
    if (QDBusConnection::sessionBus().send(m)) {
        qDebug() << "Screenshot success to" << ssFilename;
        return ssFilename;
    }
    else {
        qDebug() << "Screenshot failed";
        return QString("Failed");
    }
}


QString TohIR::temperatureColor(qreal temp, qreal min, qreal max, qreal avg) const
{
    /* We have 61 different colors - for now */
    static const QString lookup[61] =
    {
        "#0500ff", "#0400ff", "#0300ff", "#0200ff", "#0100ff", "#0000ff",
        "#0002ff", "#0012ff", "#0022ff", "#0032ff", "#0044ff", "#0054ff",
        "#0064ff", "#0074ff", "#0084ff", "#0094ff", "#00a4ff", "#00b4ff",
        "#00c4ff", "#00d4ff", "#00e4ff", "#00fff4", "#00ffd0", "#00ffa8",
        "#00ff83", "#00ff5c", "#00ff36", "#00ff10", "#17ff00", "#3eff00",
        "#65ff00", "#8aff00", "#b0ff00", "#d7ff00", "#fdff00", "#FFfa00",
        "#FFf000", "#FFe600", "#FFdc00", "#FFd200", "#FFc800", "#FFbe00",
        "#FFb400", "#FFaa00", "#FFa000", "#FF9600", "#FF8c00", "#FF8200",
        "#FF7800", "#FF6e00", "#FF6400", "#FF5a00", "#FF5000", "#FF4600",
        "#FF3c00", "#FF3200", "#FF2800", "#FF1e00", "#FF1400", "#FF0a00",
        "#FF0000"
    };

    /* If true span is low, tweak it to around avg */
    if ((max - min) < 20.0)
    {
        max = ( ((avg + 10.0) > max) ? (avg + 10.0) : max );
        min = ( ((avg - 10.0) < min) ? (avg - 10.0) : min );
    }

    /* Adjust low end to 0, to get only positive numbers */
    qreal t = temp - min;

    /* span is 2x max or min difference to average, which is larger */
    qreal span = 2.0 * ((max - avg) > (avg - min) ? (max - avg) : (avg - min));

    /* Scale to 60 points */
    qreal x = (t * (60000.0/span))/1000.0;

    /* just to prevent segfaults, return error color (white) */
    if ( (x < 0.0) || (x > 60.0) )
        return "#FFFFFF";

    return lookup[static_cast<int>(x)]; /* Return corresponding RGB color */
}


void TohIR::controlVdd(bool state)
{
    int fd = open("/sys/devices/platform/reg-userspace-consumer.0/state", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, state ? "1" : "0", 1) != 1)
            qDebug() << "Failed to control VDD.";

        close(fd);
    }

    return;
}
