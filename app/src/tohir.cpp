#include "tohir.h"

#include <QtCore>
#include <QtDBus>

#include "amg883x.h"
#include <unistd.h>


TohIR::TohIR(QObject *parent) : QAbstractListModel(parent)
  ,m_rawTemperatures(64, 0.0)
  ,m_colorMap(new ColorMap())
{
    m_min = 100.0;
    m_max = -20.0;
    m_avg = 0.0;
    m_thermistor = -273.0;

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
    writeGranularity(s.value("granularity", "0.5").toReal());
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

ColorMap *TohIR::colorMap() const
{
    return m_colorMap.data();
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
        return QVariant::fromValue(m_colorMap->getColor(temperature));
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
    roleNames[TemperatureRole] = "temperatureRole";
    roleNames[ColorRole] = "colorRole";
    roleNames[HotSpotRole] = "hotspotRole";
    return roleNames;
}

QVariant TohIR::getData(int row, int role) const
{
    return data(index(row), role);
}

/* Return git describe as string (see .pro file) */
QString TohIR::readVersion() const
{
    return QString(APPVERSION);
}

qreal TohIR::readGradientOpacity()
{
    return m_gradientOpacity;
}

void TohIR::writeGradientOpacity(qreal val)
{
    if(qFuzzyCompare(m_gradientOpacity, val)) return;

    m_gradientOpacity = val;
    emit gradientOpacityChanged(val);
}

int TohIR::readUpdateRate()
{
    return m_updateRate;
}

void TohIR::writeUpdateRate(int val)
{
    if(m_updateRate == val) return;
    m_updateRate = val;

    emit updateRateChanged(val);
}

qreal TohIR::readGranularity()
{
    return m_granularity;
}

void TohIR::writeGranularity(qreal val)
{
    if(qFuzzyCompare(m_granularity, val)) return;
    m_granularity = val;

    emit granularityChanged(val);
}

qreal TohIR::readContrast()
{
    return m_contrast;
}

void TohIR::writeContrast(qreal val)
{
    if(qFuzzyCompare(m_contrast, val)) return;
    m_contrast = val;

    emit contrastChanged(val);
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

    emit scanFinished();
    emit dataChanged(index(0), index(63));
}

/* Return thermistor temperature */
void TohIR::readThermistor()
{
    m_thermistor = amg->getThermistor();
    emit thermistorChanged(m_thermistor);
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

/* Call dbus method to save screencapture */
QString TohIR::saveScreenCapture()
{
    QString ssFilename = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
            + QDateTime::currentDateTime().toString("/ddMMyy-hhmmss-zzz.png");
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
