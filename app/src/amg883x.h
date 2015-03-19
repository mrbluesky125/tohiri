/*
The MIT License (MIT)

Copyright (c) 2014 Kimmo Lindholm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef amg883X_H
#define amg883X_H
#include <QObject>
#include <QString>
#include "driverBase.h"

namespace
{

const unsigned char amgThermistorRegister = 0x0e;
const unsigned char amgTemperatureRegister = 0x80;
const unsigned char amgAverageRegister = 0x07;
const unsigned char amgReservedRegister1F = 0x1f;

}

class amg883x : public DriverBase
{
    Q_OBJECT
public:
    explicit amg883x(unsigned char amg_address);
    ~amg883x();

    void init();

    qreal getThermistor();
    QVector<qreal> getTemperatureArray();



private:
    unsigned char amgAddress;
};

#endif // amg883X_H
