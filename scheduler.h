#ifndef SCHEDULER_H
#define SCHEDULER_H

#include<QTimer>
#include<functional>


class scheduler : public QObject {
    Q_OBJECT

private:
    enum state_t {
        running,
        stopped,
        paused
    };
    state_t state;
    const int interval;
    QTimer timer;

public:
    scheduler(int interval, QObject *parent = 0);
    void reschedule();
    inline const QTimer *get_timer(){
        return &timer;
    }

public slots:
    void pause();
    void resume();
    void stop();

signals:
    void finished();
};

#endif // SCHEDULER_H
