#ifndef ROUTE_H
#define ROUTE_H
#include <QPoint>
#include <QList>
#include <QPainter>

class Route{
public:
    Route(QList<QPoint> waypoint_in);
    ~Route();
    QList<QPoint> waypoint;

    void paint(QPainter *painter);
private:

};


#endif // ROUTE_H
