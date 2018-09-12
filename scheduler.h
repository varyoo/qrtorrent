#ifndef SCHEDULER2_H
#define SCHEDULER2_H

#include <QTimer>
#include <QDebug>


class scheduler : public QObject {
    Q_OBJECT

private:
    bool run;
    int interval;
    QTimer timer;

public:
    scheduler(int interval):
        QObject(nullptr),
        run(false),
        interval(interval),
        timer(this)
    {
        timer.setSingleShot(true);
        connect(&timer, &QTimer::timeout,
                this, &scheduler::timeout);
    }

private slots:
    void timeout(){
        emit run_fetch();

        if(run){
            timer.start(interval);
        }
    }

public slots:
    void stop(){
        run = false;
    }

    void start(){
        if(!run){
            timer.start(0);
            run = true;
        }
    }

    void finish(){
        timer.stop();
        emit finished();
    }

signals:
    void run_fetch();
    void finished();
};

#endif // SCHEDULER2_H
