#include <QCoreApplication>
#include "mymain.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyMain mymain;

    return a.exec();
}
