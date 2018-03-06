#ifndef FILES_DEAMON_H
#define FILES_DEAMON_H


class files_deamon : public QObject
{ Q_OBJECT
public:
    files_deamon(QObject *parent, rtorrent&, const QString &hash);
private:
    const QString hash;
    Config conf{};
    rtorrent &rtor;
};

#endif // FILES_DEAMON_H
