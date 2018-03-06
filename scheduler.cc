#include"scheduler.h"
#include<QDebug>


scheduler::scheduler(int interval, QObject *parent):
    QObject(parent),
    state(paused),
    interval(interval),
    timer(this)
{
    timer.setSingleShot(true);
}

void scheduler::pause(){
    switch(state){
    case stopped:
    case paused:
        Q_ASSERT(false);
    case running:
        state = paused;
        timer.stop();
    }
}

void scheduler::resume(){
    switch(state){
    case stopped:
        Q_ASSERT(false);
    case running:
        // tolerated because it happens
    case paused:
        state = running;
        timer.start(0);
    }
}

void scheduler::stop(){
    switch(state){
    case stopped:
        Q_ASSERT(false);
    case paused:
    case running:
        state = stopped;
        emit finished();
    }
}

void scheduler::reschedule(){
    timer.start(interval);
}
