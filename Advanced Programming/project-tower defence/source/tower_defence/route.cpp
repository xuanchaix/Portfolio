#include "route.h"

Route::Route(QList<QPoint> waypoint_in){
    waypoint = waypoint_in;
}

Route::~Route(){
}

void Route::paint(QPainter *painter){
    //painter->save();
    painter->setPen(Qt::gray);
    for(auto i = waypoint.begin(); i != waypoint.end() - 1; i++){
        painter->drawLine(*i, *(i + 1));
    }
}
