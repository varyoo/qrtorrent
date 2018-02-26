#ifndef CONFIG_H
#define CONFIG_H

#include<QSettings>


class Config : public QObject {
    Q_OBJECT
public:
    Config();
    ~Config(){}
    inline QString url(){
        return ss.value("scgi_url", "").toString();
    }
    inline QString user(){
        return ss.value("user", "").toString();
    }
    inline QString password(){
        return ss.value("password", "").toString();
    }
    inline QString defaultDest(){
        return ss.value("default_destination", "").toString();
    }
    inline void setDefaultDest(QString s){
        ss.setValue("default_destination", s);
    }
    inline void setRemote(QString url, QString user, QString password){
        ss.setValue("scgi_url", url);
        ss.setValue("user", user);
        ss.setValue("password", password);
    }
    inline QString fileName(){
        return ss.fileName();
    }
private:
    QSettings ss;
};

#endif // CONFIG_H
