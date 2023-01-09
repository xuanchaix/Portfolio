#ifndef TOWER_H
#define TOWER_H

#include <QPoint>
#include <QString>
#include <QSize>
#include <QPainter>
#include <QtDebug>
#include <QTimer>
#include <QBrush>
#include "enemy.h"
#include "arrow.h"
/*
       tower tree
--melee 1 ranged 0
(melee)
    trap_tower14
        wall_tower3--great_wall_tower15
        fortress16--stronghold17
(ranged)
    ranged_tower2
        firelock_tower4--through_arrow_tower5
        spawn_tower6--spawn_garrison7
    magic_tower8
        big_magic_tower9--great_magic_tower10
    mortar11
        big_mortar12--rocket_tower13
        */
/*
 * 添加防御塔：绘制防御塔图片（在Tower构造函数和towerbutton中加入），在towet.h中声明，添加cost函数（也许可以多态？还没实现），添加防御塔cost（下方数组和.cpp文件头部）
 * 添加构造函数和析构函数，定义防御塔行为，新建arrow类等
 * 在mainwindow.cpp和towerbutton.h(.cpp)中添加防御塔升级（见上表）
*/
enum tower_type{ranged_cons, melee_cons, ranged_tower, wall_tower, firelock_tower, through_arrow_tower, spawn_tower, spawn_garrison,
               magic_tower, big_magic_tower, great_magic_tower, mortar, big_mortar, rocket_tower, trap_tower, great_wall_tower,
               fortress, stronghold, guard_tower};

const int tower_cost[30] = {0, 0, 150, 100, 100, 100, 50, 75, 150, 120, 150, 150, 150, 150, 50, 100, 75, 50};

class Tower: public QObject{
public:
    Tower(QPoint pos, int type);
    ~Tower();
    QPoint get_position();
    QPoint get_leftupperposition();
    bool in_position(QPoint pos_in);
    QString get_path();
    int get_type();
    int get_hp();
    void get_attacked(int damage);
    void update(QString upath, int urange, int uattack, int uattack_speed, int utype);
    void paint(QPainter *painter);
    void paint_hp(QPainter *painter);
    bool is_melee();
    bool is_ranged();
    static int get_cost();
    void is_upgrade();
    bool upgraded();

    //melee
    QList<Enemy *> block_enemy_list;//only useful for melee towers;
    int block_enemy_num;

    //ranged
    bool show_range;
    void paint_range(QPainter *painter);
    int enemy_distance(QPoint enemy_pos);
    void choose_target(QList<Enemy *> enemy_list);
    QList<Arrow *> send_arrow_list;
    Enemy *target_enemy;

    //spawn tower
    bool can_spawn;


protected:
    QPoint pos;
    QString path;
    static const QSize size;
    int range;
    int attack;
    int attack_speed;
    int type;
    static const int cost;
    int max_hp;
    int cur_hp;
    bool melee;
    bool ranged;
    bool is_upgraded;
};

class TrapTower: public Tower{
public:
    TrapTower(QPoint pos, int type);
    ~TrapTower();
    static int get_cost();

protected:
    int hp;
    static const int cost;

protected slots:
    void melee_attack();
};

class WallTower: public TrapTower{
public:
    WallTower(QPoint pos, int type);
    ~WallTower();
    static int get_cost();


protected:
    int hp;
    static const int cost;

protected slots:
    //void melee_attack();

private:

};

class GreatWallTower: public WallTower{
public:
    GreatWallTower(QPoint pos, int curr_hp, int type);
    ~GreatWallTower();
    static int get_cost();

protected:
    int hp;
    static const int cost;
protected slots:
    void ranged_attack();
};

class RangedTower: public Tower{
public:
    RangedTower(QPoint pos, int type);
    ~RangedTower();
    static int get_cost();


protected slots:
    void ranged_attack();

protected:
    static const int cost;

};

class MagicTower: public RangedTower{
public:
    MagicTower(QPoint pos, int type);
    ~MagicTower();
    static int get_cost();

protected:
    static const int cost;

};

class Mortar: public RangedTower{
public:
    Mortar(QPoint pos, int type);
    ~Mortar();
    static int get_cost();

protected:
    static const int cost;
};

class Fortress: public TrapTower{
public:
    Fortress(QPoint pos, int type);
    ~Fortress();
    static int get_cost();


protected:
    int hp;
    static const int cost;

};

class GuardTower: public TrapTower{
public:
    //衍生塔
    GuardTower(QPoint pos, int type);
    ~GuardTower();

protected:
    int hp;

};

class SpawnTower: public RangedTower{
public:
    SpawnTower(QPoint pos, int curr_hp, int type);
    ~SpawnTower();
    static int get_cost();
protected:
    static const int cost;

};

#endif // TOWER_H
