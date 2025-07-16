#include <QCoreApplication>
#include "mymainkafka.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyMainKafka mymain;

    return a.exec();
}
