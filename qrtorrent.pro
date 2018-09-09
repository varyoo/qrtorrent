#-------------------------------------------------
#
# Project created by QtCreator 2018-02-09T18:56:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qrtorrent
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += LIBSSH_STATIC

SOURCES += \
    table.cc \
    main.cc \
    mainwindow.cc \
    connect.cc \
    adddialog.cc \
    removedialog.cc \
    tablesortmodel.cc \
    tablemodel.cc \
    rtor/rtorrent.cc \
    scheduler.cc \
    torrent_file.cc \
    files_impl.cc \
    files_daemon.cc \
    file_model.cc \
    details_dialog.cc \
    torrent_files_model.cc \
    content_view.cc \
    focus.cc \
    torrent.cc \
    move_dialog.cpp

HEADERS += \
    app.h \
    mainwindow.h \
    table.h \
    connect.h \
    config.h \
    adddialog.h \
    removedialog.h \
    tablesortmodel.h \
    tablemodel.h \
    rtor/rtorrent.h \
    scheduler.h \
    torrent_file.h \
    files_client.h \
    files_daemon.h \
    file_model.h \
    details_dialog.h \
    torrent_files_model.h \
    content_view.h \
    focus.h \
    torrent.h \
    rtor/torrent_list.h \
    rtor/torrent_list.cc \
    move_dialog.h \
    torrents_daemon.h

FORMS += \
    mainwindow.ui \
    connect.ui \
    adddialog.ui \
    removedialog.ui \
    details_dialog.ui \
    move_dialog.ui

CONFIG += c++17 \
    debug_and_release

LIBS += $$system(xmlrpc-c-config c++2 client --libs)
