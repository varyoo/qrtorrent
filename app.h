#include <QApplication>
#include <QDebug>

class App : public QApplication {
   Q_OBJECT

public:
    App(int &argc, char **argv) : QApplication(argc, argv){}
    bool notify(QObject *receiver, QEvent *event){
        // TODO: find a better way
        try {
            return QApplication::notify(receiver, event);
        } catch (std::exception &e) {
            qDebug() << "Exception thrown:" << e.what();
            return false;
        }
    }
};
