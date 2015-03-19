#ifndef TOHIR_H
#define TOHIR_H
#include <QtCore>
#include "amg883x.h"

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
    Q_PROPERTY(QList<QString> temperatures READ readTemperatures NOTIFY scanFinished)
    Q_PROPERTY(QString version READ readVersion NOTIFY versionChanged)
    Q_PROPERTY(QString minTemp READ readMinTemp NOTIFY scanFinished)
    Q_PROPERTY(QString avgTemp READ readAvgTemp NOTIFY scanFinished)
    Q_PROPERTY(QString maxTemp READ readMaxTemp NOTIFY scanFinished)
    Q_PROPERTY(int hotSpot READ readHotSpot NOTIFY scanFinished)

    Q_PROPERTY(qreal gradientOpacity READ readGradientOpacity WRITE writeGradientOpacity(qreal) NOTIFY gradientOpacityChanged())
    Q_PROPERTY(int updateRate READ readUpdateRate WRITE writeUpdateRate() NOTIFY updateRateChanged())
    Q_PROPERTY(qreal granularity READ readGranularity WRITE writeGranularity() NOTIFY granularityChanged())
    Q_PROPERTY(qreal contrast READ readContrast WRITE writeContrast() NOTIFY contrastChanged())


public:
    explicit TohIR(QObject *parent = 0);
    virtual ~TohIR();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    QList<QString> readTemperatures() const;
    Q_INVOKABLE void startScan();
    Q_INVOKABLE QString saveScreenCapture();

    QString readVersion();
    QString readMinTemp();
    QString readAvgTemp();
    QString readMaxTemp();
    int readHotSpot();

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

    Q_INVOKABLE QString readThermistor();

signals:
    void scanFinished();
    void versionChanged();

    void gradientOpacityChanged();
    void updateRateChanged();
    void granularityChanged();
    void contrastChanged();

private:
    void controlVdd(bool state);

    QString temperatureColor(qreal temp, qreal min, qreal max, qreal avg) const;

    QVector<qreal> m_rawTemperatures;
    QList<QString> m_temperatures;

    qreal m_avg;
    qreal m_min;
    qreal m_max;
    int m_hotSpot;

    qreal m_gradientOpacity;
    int m_updateRate;
    qreal m_granularity;
    qreal m_contrast;

    amg883x* amg;
};




#endif // TOHIR_H

