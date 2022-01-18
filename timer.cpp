#include "timer.h"
#include <QDebug>
#include <QLCDNumber>


Timer::Timer(QLCDNumber *_lcd, QTime _initial):
    lcd(_lcd),timer(new QTimer(lcd)), time(_initial) {
    connect(timer, &QTimer::timeout, this, &Timer::displayUpdate);
    lcd->setSegmentStyle(QLCDNumber::Filled);
}

Timer::~Timer() {}

void Timer::start() {
    qDebug() << "timer start.";
    lcd->display(getTimeString());
    timer->start(1000);
}

void Timer::pause() {
    timer->stop();
}

void Timer::resume() {
    timer->start(1000);
}

void Timer::restart() {
    time = QTime(0, 0, 0);
    start();
}

QString Timer::getTimeString() {
    return time.toString("mm:ss");
}

void Timer::displayUpdate() {
    time = time.addSecs(1);
    lcd->display(time.toString("mm:ss"));
}


