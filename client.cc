#include "client.h"
#include<QDebug>
#include<QThread>
#include<QApplication>
#include<fstream>
#include<QDateTime>

/*void Client::sshConnect(){
    static int verbosity = SSH_LOG_PROTOCOL;
    sshs.setOption(SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    sshs.setOption(SSH_OPTIONS_HOST, conf.sshHost().toStdString().c_str());
    QString sshConf = "~/.ssh/config";
    try {
        sshs.optionsParseConfig(sshConf.toStdString().c_str());
        sshs.connect();
        sshs.userauthPublickeyAuto();
        sshc.openSession();
        Q_ASSERT(sshc.isOpen());
        emit sshConnected();
        sshc.requestShell();
        const char *echo = "echo 1";
        sshc.write(echo, strlen(echo));
        echo = "echo 2";
        sshc.write(echo, strlen(echo));
    } catch(ssh::SshException e){
        qWarning() << QString::fromStdString(e.getError());
        emit sshError(e);
    }
}

void Client::sshReconnect(){
    sshc.close();
    sshc.sendEof();
    sshs.disconnect();
    sshConnect();
}*/

Client::Client():
    QObject(),
    transport(),
    client(&transport),
    timer(this)
{
    //sshConnect();

    const std::string columns[Torrent::NB_COLUMNS+2]{"", "name",
        "d.hash=",
        "d.name=",
        "d.bytes_done=",
        "d.size_bytes=",
        "d.up.rate=",
        "d.up.total=",
        "d.down.rate=",
        "d.down.total=",
        "d.creation_date=",
        "d.custom1=",
        "d.custom=addtime",
        "d.ratio=",
        "d.is_active=",
        "d.is_hash_checking=",
        "d.is_open=",
        "d.complete=",
        "d.state="
    };
    for(const std::string &s : columns){
        fetchAllParams.add(xmlrpc_c::value_string(s));
    }

    setConnection();

    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout,
            this, &Client::fetchAll);
}

Client::~Client(){
    delete cp;
}

Torrent::Torrent(xmlrpc_c::carray fs):
    hash(QString::fromStdString(xmlrpc_c::value_string(fs[HASH]) ) ),
    name(QString::fromStdString(xmlrpc_c::value_string(fs[NAME]) ) ),
    bytes_done(xmlrpc_c::value_i8(fs[BYTES_DONE]) ),
    size_bytes(xmlrpc_c::value_i8(fs[SIZE_BYTES]) ),
    upRate(xmlrpc_c::value_i8(fs[UP_RATE]) ),
    upTotal(xmlrpc_c::value_i8(fs[UP_TOTAL]) ),
    downRate(xmlrpc_c::value_i8(fs[DOWN_RATE]) ),
    downTotal(xmlrpc_c::value_i8(fs[DOWN_TOTAL]) ),
    creation_date(xmlrpc_c::value_i8(fs[CREATION_DATE]) ),
    addtime(QString::fromStdString(xmlrpc_c::value_string(fs[Torrent::ADDTIME]) ).toLongLong() ),
    ratio(xmlrpc_c::value_i8(fs[RATIO]) ),
    is_active(xmlrpc_c::value_i8(fs[IS_ACTIVE]) == 1),
    is_hash_checking(xmlrpc_c::value_i8(fs[IS_HASH_CHECKING]) == 1),
    is_open(xmlrpc_c::value_i8(fs[IS_OPEN]) == 1),
    complete(xmlrpc_c::value_i8(fs[COMPLETE]) == 1),
    state(xmlrpc_c::value_i8(fs[STATE]) == 1)
{
    if(is_hash_checking){
        display_state = State::Checking;
    } else if(complete && is_open && state){
        display_state = State::Seeding;
    } else if(!complete && is_open && state){
        display_state = State::Downloading;
    } else if(!is_open || (is_open && !state) ){
        display_state = State::Stopped;
    }
}

QDebug &operator<<(QDebug &d, Torrent const&t){
    d << t.hash << ":" << t.name;
    return d;
}

void Client::stop(){
    stopRequested = true;
}

inline std::shared_ptr<Torrent>
Client::parse_torrent(const xmlrpc_c::value &v){
    xmlrpc_c::value_array va(v);
    auto fs = va.vectorValueValue();
    auto t = std::make_shared<Torrent>(fs);
    return t;
}

inline void Client::removeLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    size_t d = j;
    
    while(d < old_size){
        if(old[d]->hash < ts[i]->hash){
            d++;
        } else {
            if(j != d){
                emit torrentsRemoved(j, d);
                j = d;
            }

            return equalLoop(ts, old_size, new_size, j, i);
        }
    }
}

void Client::insertLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    size_t d = i;
    std::vector<std::shared_ptr<Torrent> > delta;
    
    while(d < new_size){
        if(j == old_size || old[j]->hash > ts[d]->hash){
            delta.push_back(ts[d]);
            d++;
        } else {
            if(i != d){
                emit torrentsInserted(i, delta);
                i = d;
            }

            return removeLoop(ts, old_size, new_size, j, i);
        }
    }
}

inline void Client::equalLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    while(j < old_size && i < new_size){
        if(old[j]->hash == ts[i]->hash){
            if(ts[i]->hasChanged(*old[j])){
                emit torrentChanged(i, ts[i]);
            }
            i++;
            j++;
        } else {
            return insertLoop(ts, old_size, new_size, j, i);
        }
    }
}

void Client::fetchAll(){
    Q_ASSERT(this->thread() != QApplication::instance()->thread());

    xmlrpc_c::rpcPtr c("d.multicall2", fetchAllParams);
    try {
        c->call(&client, cp);
    } catch(std::exception &e){
        qWarning() << e.what();
        rescheduleFetchAll();
        return;
    }
    Q_ASSERT(c->isFinished());

    xmlrpc_c::value_array v(xmlrpc_c::value_array(c->getResult()));
    std::vector<xmlrpc_c::value> vv = v.vectorValueValue();
    size_t j = 0;
    size_t i = 0;
    const size_t old_size = old.size();
    const size_t new_size = vv.size();

    std::vector<std::shared_ptr<Torrent> > ts;
    ts.reserve(vv.size());
    for(const xmlrpc_c::value &tv : vv){
        ts.push_back(parse_torrent(tv));
    }

    // the list must be ordered by hash for the comparison with the previous list to work
    std::sort(ts.begin(), ts.end(), [](const std::shared_ptr<Torrent> &a,
                const std::shared_ptr<Torrent> &b){
            return a->hash < b->hash;
            });
    
    insertLoop(ts, old_size, new_size, j, i);

    if(j != old_size){
        emit torrentsRemoved(j, old_size);
    } else if(i != new_size){
        emit torrentsInserted(i, {ts.begin()+i, ts.end()});
    }

    old = ts;
    rescheduleFetchAll();
}

void Client::rescheduleFetchAll(){
    if(stopRequested){
        timer.stop();
        emit finished();
    } else if(pauseRequested){
        timer.stop();
    } else {
        timer.start(interval);
    }
}

void Client::resume(){
    if(pauseRequested){
        pauseRequested = false;
        timer.start(0);
    }
}

void Client::startTorrents(QStringList hashes){
    cmdForHashes("d.start", hashes);
}

void Client::stopTorrents(QStringList hashes){
    cmdForHashes("d.stop", hashes);
}

void Client::removeTorrents(QStringList hashes, bool deleteData){
    cmdForHashes("d.erase", hashes);
}

void Client::cmdForHashes(std::string cmd, QStringList hashs)
{
    xmlrpc_c::carray cmds;
    for(const QString h : hashs){
        xmlrpc_c::carray ps = {xmlrpc_c::value_string(h.toStdString())};
        addCmd(cmds, cmd, ps);
    }
    multicall(cmds);
}

void Client::updateConnection(){
    delete cp;
    setConnection();
}

void Client::addCmd(xmlrpc_c::carray &cmds, std::string cmd, xmlrpc_c::carray &ps){
    cmds.push_back(xmlrpc_c::value_struct({
        {"methodName", xmlrpc_c::value_string(cmd)},
        {"params", xmlrpc_c::value_array(ps)}
    }));
}

// from https://stackoverflow.com/questions/15138353
void Client::loadFileInto(xmlrpc_c::cbytestring &bytes, QString filename){
    // open the file:
    std::ifstream file(filename.toStdString(), std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    bytes.reserve(fileSize);

    // read the data:
    bytes.insert(bytes.begin(),
           std::istream_iterator<unsigned char>(file),
           std::istream_iterator<unsigned char>());
}


void Client::addFiles(QString dest, QStringList files, bool start)
{
    const std::string std_dest = dest.toStdString();
    const xmlrpc_c::value_string target("");
    xmlrpc_c::carray cmds;
    const xmlrpc_c::value_string addtime(
        QString("d.custom.set=addtime,%1"). // used by ruTorrent and Flood
        arg(QDateTime::currentSecsSinceEpoch() ).
        toStdString()
        );
    int size = 0;
    
    for(const QString &filename : files){
        xmlrpc_c::cbytestring bytes;
        Client::loadFileInto(bytes, filename);

        xmlrpc_c::value_bytestring v(bytes);
        xmlrpc_c::carray params = {target, v, addtime};

        if(!dest.isEmpty()){
            params.push_back(xmlrpc_c::value_string("d.directory.set="+std_dest) );
        }

        std::string cmd;
        if(start){
            cmd = "load.raw_start";
        } else {
            cmd = "load.raw";
        }

        size += bytes.size();
        if(size > 300*1000){ // 524 kilobytes supposedly max
            qDebug() << "Add files: breaking down big multicall";
            multicall(cmds);
            cmds.clear();
            size = 0;
        }
        
        addCmd(cmds, cmd, params);
    }

    multicall(cmds);
}

bool Client::multicall(xmlrpc_c::carray cmds){
    xmlrpc_c::paramList ps;
    ps.add(xmlrpc_c::value_array(cmds));
    return call("system.multicall", ps);
}

bool Client::call(std::string cmd, xmlrpc_c::paramList ps){
    Q_ASSERT(this->thread() != QApplication::instance()->thread());
    
    xmlrpc_c::rpcPtr c(cmd, ps);
    try {
        c->call(&client, cp);
    } catch(std::exception &e){
        qWarning() << e.what();
        return false;
    }
    Q_ASSERT(c->isFinished());
    return true;
}
