#include "mainwindow.h"
#include "game.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //感觉不硬编码关卡和地图还是不好的，可以考虑以后内置关卡编码好，外置关卡用.txt文件
    Mission *mission1 = new Mission(QString("./missions/mission1/mission.txt"));//内置：":/missionclass1/missions/mission1/mission.txt"
    Game *game = new Game(mission1);
    MainWindow w(game);
    w.show();
    return a.exec();
}
