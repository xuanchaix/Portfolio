#ifndef GAME_H
#define GAME_H

#include <QPainter>
#include <QPoint>
#include <QList>
#include <QElapsedTimer>
#include "tower.h"
#include "mission.h"
#include "enemy.h"
#include "towerButton.h"

enum game_state{game_running, lose_game, win_game};

class Game: public QObject{
public:
    Game(Mission *mission);
    ~Game();
    void refresh(QPainter *painter);
    QList<Tower *> get_towerlist();
    Mission* get_mission();

    int money;
    int lives;
    int state;
    bool show_tower_button;
    QList<Tower *> tower_list;
    QList<Enemy *> enemy_list;
    QList<Arrow *> arrow_list;
    QList<TowerButton *> button_list;


public slots:
    void game_update();
    void game_start();

private:
    bool enemy_all_loaded;
    Mission* g_mission;
    QElapsedTimer enemy_timer;
    void load_enemies(qint64 cur_time);
    void spawn_guard(Tower *tw);
    bool enemy_meets_melee_tower(Enemy *enemy);
    void deal_spurt_damage(int damage, int range, Enemy *target);
};


#endif // GAME_H
