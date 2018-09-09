#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <memory>

#include "rtor/rtorrent.h"

#include <xmlrpc-c/client.hpp>


class Config : public QObject {
    Q_OBJECT

public:
    Config():
        ss(QSettings::IniFormat, QSettings::UserScope,
                "qrtorrent", "qrtorrent")
    {}

    QString url(){
        return ss.value("scgi_url", "").toString();
    }
    
    QString user(){
        return ss.value("user", "").toString();
    }
    
    QString password(){
        return ss.value("password", "").toString();
    }
    
    QString defaultDest(){
        return ss.value("default_destination", "").toString();
    }
    
    void setDefaultDest(QString s){
        ss.setValue("default_destination", s);
    }
    
    void setRemote(QString url, QString user, QString password){
        ss.setValue("scgi_url", url);
        ss.setValue("user", user);
        ss.setValue("password", password);
    }
    
    QString fileName(){
        return ss.fileName();
    }

    std::shared_ptr<rtor::client> mk_rtorrent(){
        auto url_val = url().toStdString();
        auto carriage = xmlrpc_c::carriageParm_curl0Ptr(
                new xmlrpc_c::carriageParm_curl0(url_val));

        auto user_val = user().toStdString();
        if(!user_val.empty()){
            auto pass_val = password().toStdString();
            carriage->setBasicAuth(user_val, pass_val);
        }

        return std::make_shared<rtor::client>(carriage);
    }

private:
    QSettings ss;
};

#endif // CONFIG_H
