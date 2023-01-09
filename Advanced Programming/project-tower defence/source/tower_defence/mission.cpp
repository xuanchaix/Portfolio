#include "mission.h"
#include <QFile>
#include <QtDebug>
#include <QRegularExpression>

Mission::Mission(QString mission_path){
    this->mission_path = mission_path;
    QFile mission_file(mission_path);
    if(!mission_file.exists()){
        qDebug() << "mission doesn't exist!";
    }
    if(!mission_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "can't open mission!";
    }
    QTextStream mission_text(&mission_file);
    QString m_line;
    while(!mission_text.atEnd()){
        m_line = mission_text.readLine();
        //qDebug() << m_line;
        if(m_line.contains("name")){
            m_line = m_line.simplified();
            m_line.remove(QRegularExpression("\\s"));
            name = m_line.mid(5, -1);
            //qDebug() << "name :" << name;
        }
        if(m_line.contains("start_money")){
            m_line = m_line.simplified();
            m_line.remove(QRegularExpression("\\s"));
            start_money = m_line.mid(12, -1).toInt();
            //qDebug() << "start_money :" << start_money;
        }
        if(m_line.contains("start_lives")){
            m_line = m_line.simplified();
            m_line.remove(QRegularExpression("\\s"));
            lives = m_line.mid(12, -1).toInt();
            //qDebug() << "start_lives :" << lives;
        }
        if(m_line.contains("map_path")){
            m_line = m_line.simplified();
            m_line.remove(QRegularExpression("\\s"));
            map_path = m_line.mid(9, -1);
            //qDebug() << "map_path :" << map_path;
        }
        if(m_line.contains("tower_pos_list")){
            while(m_line != "}"){
                m_line = mission_text.readLine();
                m_line = m_line.simplified();
                m_line.remove(QRegularExpression("\\s"));
                if(m_line == "}"){
                    break;
                }
                QStringList tp_lst = m_line.split(",");
                twpos tw_pos;
                tw_pos.x = tp_lst.at(0).mid(1, -1).toInt();
                tw_pos.y = tp_lst.at(1).toInt();
                tw_pos.type = tp_lst.at(2).mid(0, tp_lst.at(2).size() - 1).toInt();
                tower_pos_list.push_back(tw_pos);
            }
        }
        if(m_line.contains("route_in")){
            m_line = mission_text.readLine();
            m_line = m_line.simplified();
            m_line.remove(QRegularExpression("\\s"));
            while(m_line == "{"){
                QList<QPoint> *waypointin = new QList<QPoint>;
                while(true){
                    m_line = mission_text.readLine();
                    m_line = m_line.simplified();
                    m_line.remove(QRegularExpression("\\s"));
                    if(m_line == "}"){
                        m_line = mission_text.readLine();
                        m_line = m_line.simplified();
                        m_line.remove(QRegularExpression("\\s"));
                        break;
                    }
                    QStringList tp_lst = m_line.split(",");
                    QPoint waypoint;
                    waypoint.setX(tp_lst.at(0).mid(1, -1).toInt());
                    waypoint.setY(tp_lst.at(1).mid(0, tp_lst.at(1).size() - 1).toInt());
                    //qDebug() << waypoint;
                    waypointin->push_back(waypoint);
                }
                route.push_back(Route(*waypointin));
            }
        }
        if(m_line.contains("enemy_enter_list")){
            m_line = mission_text.readLine();
            m_line = m_line.simplified();
            m_line.remove(QRegularExpression("\\s"));
            int enemy_count = 1;
            enemy_enter_list.push_back(QList<int>());
            while(m_line == "{"){
                enemy_enter_list.push_back(QList<int>());
                while(true){
                    m_line = mission_text.readLine();
                    m_line = m_line.simplified();
                    m_line.remove(QRegularExpression("\\s"));
                    if(m_line == "}"){
                        m_line = mission_text.readLine();
                        m_line = m_line.simplified();
                        m_line.remove(QRegularExpression("\\s"));
                        break;
                    }
                    enemy_enter_list[enemy_count].push_back(m_line.toInt());
                }
                ++enemy_count;
            }
        }
    }
}

Mission::~Mission(){
}

void Mission::paint_route(QPainter *painter){
    for(auto i:route){
        i.paint(painter);
    }
}
