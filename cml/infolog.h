#ifndef INFOLOG_H
#define INFOLOG_H

#include <QString>
#include <QDebug>

class InfoLog
{
public:
    //-----单例模式
    static InfoLog& inst() {
        static InfoLog instance;
        return instance;
    }
    InfoLog(const InfoLog&) = delete;
    InfoLog& operator=(const InfoLog&) = delete;
    //-----

    void set_debug(bool debug) {
        is_debug = debug;
    }

    void print(const QString& s) {
        if(is_debug) {
            qDebug() << s;
        }
    }




private:
    InfoLog() = default;

    bool is_debug = true;

};

#endif // INFOLOG_H
