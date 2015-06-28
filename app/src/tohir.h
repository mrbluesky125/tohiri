#ifndef TOHIR_H
#define TOHIR_H

#include <QtCore>
#include "amg883x.h"
#include "colormap.h"

class TohIR : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ItemDataRole {
        TemperatureRole = Qt::UserRole +1,
        ColorRole,
        HotSpotRole
    };

private:
    Q_ENUMS(ItemDataRole)
    Q_PROPERTY(QString version READ readVersion NOTIFY versionChanged)
    Q_PROPERTY(qreal minTemp READ readMinTemp NOTIFY scanFinished)
    Q_PROPERTY(qreal avgTemp READ readAvgTemp NOTIFY scanFinished)
    Q_PROPERTY(qreal maxTemp READ readMaxTemp NOTIFY scanFinished)
    Q_PROPERTY(qreal thermistor READ thermistor NOTIFY thermistorChanged)

    Q_PROPERTY(qreal gradientOpacity READ readGradientOpacity WRITE writeGradientOpacity NOTIFY gradientOpacityChanged)
    Q_PROPERTY(int updateRate READ readUpdateRate WRITE writeUpdateRate NOTIFY updateRateChanged)
    Q_PROPERTY(qreal granularity READ readGranularity WRITE writeGranularity NOTIFY granularityChanged)
    Q_PROPERTY(qreal contrast READ readContrast WRITE writeContrast NOTIFY contrastChanged)
    Q_PROPERTY(ColorMap* colorMap READ colorMap NOTIFY colorMapChanged)

public:
    explicit TohIR(QObject *parent = 0);
    virtual ~TohIR();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE QVariant getData(int row, int role) const;

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void readThermistor();
    Q_INVOKABLE QString saveScreenCapture();

    QString readVersion() const;
    qreal readMinTemp() const;
    qreal readAvgTemp() const;
    qreal readMaxTemp() const;
    qreal thermistor() const;

    qreal readGradientOpacity();
    void writeGradientOpacity(qreal val);

    int readUpdateRate();
    void writeUpdateRate(int val);

    qreal readGranularity();
    void writeGranularity(qreal val);

    qreal readContrast();
    void writeContrast(qreal val);

    Q_INVOKABLE void readSettings();
    Q_INVOKABLE void saveSettings();

    ColorMap *colorMap() const;

signals:
    void scanFinished();
    void thermistorChanged(qreal);
    void versionChanged();

    void gradientOpacityChanged(qreal);
    void updateRateChanged(int);
    void granularityChanged(qreal);
    void contrastChanged(qreal);

    void colorMapChanged(QObject* arg);

private:
    void controlVdd(bool state);

    QVector<qreal> m_rawTemperatures;

    qreal m_avg;
    qreal m_min;
    qreal m_max;
    qreal m_thermistor;

    qreal m_gradientOpacity;
    int m_updateRate;
    qreal m_granularity;
    qreal m_contrast;

    amg883x* amg;
    QScopedPointer<ColorMap> m_colorMap;
};




#endif // TOHIR_H

