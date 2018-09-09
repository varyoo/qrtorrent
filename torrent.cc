#include "torrent.h"

Torrent::Torrent(const xmlrpc_c::carray &fs):
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
