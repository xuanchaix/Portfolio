#ifndef ENEMY_H
#define ENEMY_H
#include "route.h"
#include "arrow.h"
#include <QTimer>
#include <QtDebug>
#include <QRect>
#include <QVector2D>

enum enemy_type{enemy, bandit, scout, flyingman, archer, catapult, shaman, general};

class Enemy: public QObject{
public:
    Enemy(Route route_in, QString path);
    ~Enemy();

    void paint(QPainter *painter);
    void paint_hp(QPainter *painter);
    void paint_state(QPainter *painter);
    bool move();
    QPoint get_position();
    void get_attacked(int damage);
    int get_hp();
    int get_reward();
    bool is_blocked();
    bool is_bellicose();
    bool is_fly();
    void get_blocked();
    void not_blocked();
    void not_aiming();
    void not_buffed();
    int get_index_toute();
    bool choose_target(void* tower);
    void choose_heal_target(QList<Enemy *> enemy_list);
    void buff(QList<Enemy *> enemy_list);
    bool is_ranged();
    bool is_super_ranged();
    bool is_buffer();
    bool is_healer();
    bool is_buffed();
    void sub_hitted();
    int get_bufftime();
    int cal_distance(QPoint tower_pos);
    void *block_tower;
    void *target_tower;
    Enemy *heal_target;
    QList <Arrow *> send_arrow_list;

protected:
    //properties
    int attack;
    double speed;
    int attack_speed;
    int max_hp;
    int cur_hp;
    int reward; //击杀赏金
    int range; //攻击、治疗、buff范围
    QSize size;
    Route route;
    QString path;

    //condition
    int index_route;
    QPoint pos;
    bool blocked;
    bool aiming;
    bool healing;
    bool stop;
    int is_hitted; //被击中了，闪烁
    bool buffed; //被buff了，近战攻击+1，持续3秒
    int buff_time;

    //abilities
    bool fly; //不被近战塔拦截
    bool bellicose; //主动攻击近战塔
    bool ranged; //可以远程攻击
    bool super_ranged; //可以远程攻击远程塔
    bool buffer; //可以加buff
    bool healer; //可以治疗


    void melee_attack();
    void ranged_attack();
    void heal();
};

class Bandit: public Enemy{
public:
    Bandit(Route route_in);
    ~Bandit();
};

class Scout: public Enemy{
public:
    Scout(Route route_in);
    ~Scout();
};

class FlyingMan: public Enemy{
public:
    FlyingMan(Route route_in);
    ~FlyingMan();
};

class Archer: public Enemy{
public:
    Archer(Route route_in);
    ~Archer();
};

class Catapult: public Enemy{
public:
    Catapult(Route route_in);
    ~Catapult();
};

class Shaman: public Enemy{
public:
    Shaman(Route route_in);
    ~Shaman();
};

class General: public Enemy{
public:
    General(Route route_in);
    ~General();
};
#endif // ENEMY_H
