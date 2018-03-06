#ifndef FOCUS_H
#define FOCUS_H

#include"scheduler.h"
#include<QMetaObject>


class focus_t : public QObject {
    Q_OBJECT
    scheduler &s;
    QMetaObject::Connection in;
    QMetaObject::Connection out;

public:
    focus_t(scheduler &s);
    ~focus_t();
    inline void focus_in(){
        emit focused_in();
    }
    inline void focus_out(){
        emit focused_out();
    }
    void reconnect();
    void disconnect();

signals:
    void focused_in();
    void focused_out();
};

#endif // FOCUS_H
