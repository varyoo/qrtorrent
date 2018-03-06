#include"focus.h"


focus_t::focus_t(scheduler &sched):
    s(sched)
{
    reconnect();
}

void focus_t::reconnect()
{
    in = connect(this, &focus_t::focused_out,
            &s, &scheduler::pause);
    out = connect(this, &focus_t::focused_in,
            &s, &scheduler::resume);
    Q_ASSERT(in);
    Q_ASSERT(out);
}

void focus_t::disconnect()
{
    QObject::disconnect(in);
    QObject::disconnect(out);
}

focus_t::~focus_t()
{
    disconnect();
}
