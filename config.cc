#include"config.h"
#include<QDebug>

Config::Config():
    ss(QSettings::IniFormat, QSettings::UserScope, "qrtorrent", "qrtorrent")
{
}
