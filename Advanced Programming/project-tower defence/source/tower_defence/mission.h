#ifndef MISSION_H
#define MISSION_H

#include <QString>
#include <QList>
#include <QPainter>
#include "route.h"

typedef struct twpos{
    int x;
    int y;
    int type;
}twpos;

class Mission{
public:
    Mission(QString mission_path);
    ~Mission();
    QString name;
    QString mission_path;
    int start_money;
    int lives;
    QString map_path;
    QList<twpos> tower_pos_list;
    QList<Route> route;
    QList<QList <int>> enemy_enter_list;//to do
    void paint_route(QPainter *painter);
};


#endif // MISSION_H
