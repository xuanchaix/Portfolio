#include "game.h"
#include <QtDebug>
#include <QTimer>
#include <QRandomGenerator>
#include <QDateTime>

Game::Game(Mission *mission):
    money(mission->start_money),
    lives(mission->lives),
    g_mission(mission)
{
    for(auto i: mission->tower_pos_list){
        tower_list.push_back(new Tower(QPoint(i.x, i.y), i.type));
    }
    show_tower_button = false;
    enemy_all_loaded = false;
    state = game_running;
    QTimer *qt = new QTimer(this);
    connect(qt, &QTimer::timeout, [this]{
        money += 1;
    });
    qt->start(1000);
    enemy_timer.start();
}

Game::~Game(){
    delete g_mission;
    g_mission = NULL;
    tower_list.clear();
    enemy_list.clear();
    arrow_list.clear();
};

void Game::refresh(QPainter *painter){
    if(state == game_running){
        //draw money and lives
        //painter->save();
        painter->drawPixmap(0, 0, 800, 600, g_mission->map_path);
        QFont font;
        font.setPointSize(18);
        painter->setFont(font);
        painter->setPen(Qt::red);
        painter->drawText(QRect(300, 10, 300, 200), QString("lives: %1").arg(lives));
        painter->setPen(Qt::darkYellow);
        painter->drawText(QRect(450, 10, 300, 200), QString("money: %1").arg(money));

        //draw towers and enemies
        for(auto i: tower_list){
            i->paint(painter);
        }
        for(auto i: tower_list){
            i->paint_hp(painter);
        }
        for(auto i: tower_list){
            i->paint_range(painter);
        }
        //g_mission->paint_route(painter);
        for(auto i: enemy_list){
            i->paint(painter);
        }
        for(auto i: enemy_list){
            i->paint_hp(painter);
        }
        for(auto i: enemy_list){
            i->paint_state(painter);
        }
        //draw arrows
        for(auto i: arrow_list){
            i->paint(painter);
        }

        if(show_tower_button){
            for(auto i:button_list){
                i->paint(painter);
            }
        }

    }
    else if(state == lose_game){
        QFont font;
        font.setPointSize(30);
        painter->setFont(font);
        painter->setPen(Qt::black);
        painter->drawText(QRect(240, 200, 300, 200), QString("很遗憾，你输了"));
        font.setPointSize(20);
        painter->setFont(font);
        painter->drawText(QRect(250, 300, 300, 200), QString("重新开始"));
        painter->drawText(QRect(450, 300, 300, 200), QString("退出"));
    }
    else if(state == win_game){
        QFont font;
        font.setPointSize(30);
        painter->setFont(font);
        painter->setPen(Qt::black);
        painter->drawText(QRect(240, 200, 300, 200), QString("恭喜你，你赢了"));
        font.setPointSize(20);
        painter->setFont(font);
        painter->drawText(QRect(250, 300, 300, 200), QString("重新开始"));
        painter->drawText(QRect(450, 300, 300, 200), QString("退出"));
    }
    painter->end();

}

Mission* Game::get_mission(){
    return g_mission;
}

QList<Tower *> Game::get_towerlist(){
    return tower_list;
}

void Game::game_start(){
}

void Game::game_update(){//controls the game
    for(auto i: tower_list){
        //tower died?
        if(i->get_hp() <= 0 || i->upgraded()){
            for(auto j :enemy_list){
                if(j->block_tower == i){
                    j->block_tower = NULL;
                    j->not_blocked();
                }
                if(j->is_ranged() && j->target_tower == i){
                    j->target_tower = NULL;
                    j->not_aiming();
                }
            }
            for(auto j:arrow_list){
                if(j->get_tower_target() == i){
                    delete j;
                    arrow_list.removeOne(j);
                }
            }
            if(!(i->send_arrow_list.empty())){
                for(auto j: i->send_arrow_list){
                    if(j != NULL && enemy_list.contains(j->get_enemy_target())){
                        arrow_list.push_back(j);
                    }
                }
                i->send_arrow_list.clear();
            }
            if(!(i->upgraded()) && i->get_type() != guard_tower){
                Tower *tw;
                if(i->is_melee()){
                    tw = new Tower(i->get_position(), melee_cons);
                }
                else{
                    tw = new Tower(i->get_position(), ranged_cons);
                }
                tower_list.push_back(tw);
            }
            delete i;
            tower_list.removeOne(i);
        }
        //ranged tower choose target
        else if(i->is_ranged()){
            i->choose_target(enemy_list);
            if(!(i->send_arrow_list.empty())){
                for(auto j: i->send_arrow_list){
                    if(j != NULL && enemy_list.contains(j->get_enemy_target())){
                        arrow_list.push_back(j);
                    }
                }
                i->send_arrow_list.clear();
            }
        }
        //spawn guard
        if(i->get_type() == spawn_tower || i->get_type() == spawn_garrison){
            if(i->can_spawn){
                i->can_spawn = false;
                spawn_guard(i);
            }
        }
    }
    for(auto i :enemy_list){
        i->sub_hitted();
        //enemy enter base?
        if(!(i->move())){
            --lives;
            delete i;
            enemy_list.removeOne(i);
        }
        //enemy died?
        else if(i->get_hp() <= 0){
            for(auto j:tower_list){
                if(j->block_enemy_list.contains(i)){
                    j->block_enemy_list.removeOne(i);
                }
                else if(j->target_enemy == i){
                    j->target_enemy = NULL;
                }
            }
            for(auto j:arrow_list){
                if(j->get_enemy_target() == i){
                    delete j;
                    arrow_list.removeOne(j);
                }
            }
            for(auto j:enemy_list){
                if(j->heal_target == i){
                    j->heal_target = NULL;
                }
            }
            if(!(i->send_arrow_list.empty())){
                for(auto j: i->send_arrow_list){
                    if(j != NULL && tower_list.contains(j->get_tower_target())){
                        arrow_list.push_back(j);
                    }
                }
                i->send_arrow_list.clear();
            }
            money += i->get_reward();
            delete i;
            enemy_list.removeOne(i);
        }
        else{
            //enemy blocked?
            enemy_meets_melee_tower(i);
            //ranged enemy
            if(i->is_ranged()){
                if(i->is_super_ranged()){
                    for(auto t:tower_list){
                        if(t->get_type() != melee_cons && t->get_type() != ranged_cons && t->get_type() != trap_tower){
                            i->choose_target(t);
                        }
                    }
                }
                else{
                    for(auto t:tower_list){
                        if(t->is_melee() && t->get_type() != melee_cons && t->get_type() != ranged_cons && t->get_type() != trap_tower){
                            i->choose_target(t);
                        }
                    }
                }
                if(!(i->send_arrow_list.empty())){
                    for(auto j: i->send_arrow_list){
                        if(j != NULL && tower_list.contains(j->get_tower_target())){
                            arrow_list.push_back(j);
                        }
                    }
                    i->send_arrow_list.clear();
                }
            }
            if(i->is_healer()){
                i->choose_heal_target(enemy_list);
            }
            if(i->is_buffer()){
                i->buff(enemy_list);
            }
            if(i->is_buffed() && QDateTime::currentDateTime().toMSecsSinceEpoch() - i->get_bufftime() > 3000){
                i->not_buffed();
            }
        }

    }
    //arrow hit?
    for(auto i: arrow_list){
        if(i->has_hitted()){
            if(i->get_type() == shell && !(i->is_enemy_arrow())){
                deal_spurt_damage(1, 60, (Enemy *)i->get_enemy_target());
            }
            delete i;
            arrow_list.removeOne(i);
        }
        else if(!(i->is_enemy_arrow()) && !enemy_list.contains(i->get_enemy_target())){
            //qDebug() << "arrowbug!";
            delete i;
            arrow_list.removeOne(i);
        }
        else if((i->is_enemy_arrow()) && !tower_list.contains(i->get_tower_target())){
            //qDebug() << "arrowbug!";
            delete i;
            arrow_list.removeOne(i);
        }
    }
    load_enemies(enemy_timer.elapsed());
    if(enemy_list.empty() && enemy_all_loaded){
        QTimer* qt1 = new QTimer(this);
        connect(qt1, &QTimer::timeout, [=]() mutable{
            state = win_game;
        });
        qt1->start(3000);
        qt1->setSingleShot(true);
    }
    if(lives <= 0){
        state = lose_game;
    }
}

void Game::deal_spurt_damage(int damage, int range, Enemy *target){
    for(auto i: enemy_list){
        int dif_x = target->get_position().x() - i->get_position().x();
        int dif_y = target->get_position().y() - i->get_position().y();
        if(!(i->is_fly()) && dif_x * dif_x + dif_y * dif_y <= range * range && i != target){
            i->get_attacked(damage);
        }
    }
}

void Game::spawn_guard(Tower *tw){
    if(tw->target_enemy){
        Tower *g_tw = new GuardTower(tw->target_enemy->get_position(), guard_tower);
        tower_list.push_back(g_tw);
    }
}

bool Game::enemy_meets_melee_tower(Enemy *enemy){
    QPoint enemy_pos = enemy->get_position();
    if(!tower_list.contains(enemy->block_tower)){
        enemy->block_tower = NULL;
        enemy->not_blocked();
    }
    if(!tower_list.contains(enemy->target_tower)){
        enemy->target_tower = NULL;
        enemy->not_aiming();
    }
    for(auto i :tower_list){
        if(i->is_melee()){
            QPoint tower_pos = i->get_position();
            if((enemy->is_bellicose() || i->get_type() == great_wall_tower || i->get_type() == stronghold)
                && ((enemy_pos.x() - tower_pos.x()) * (enemy_pos.x() - tower_pos.x()) + (enemy_pos.y() - tower_pos.y()) * (enemy_pos.y() - tower_pos.y())) <= 400
                && i->block_enemy_list.size() < i->block_enemy_num
                && enemy->is_blocked() == false
                && i->get_type() != trap_tower){
                i->block_enemy_list.push_back(enemy);
                enemy->get_blocked();
                enemy->block_tower = i;
                return true;
            }
            else if(i->get_type() == trap_tower
                    && ((enemy_pos.x() - tower_pos.x()) * (enemy_pos.x() - tower_pos.x()) + (enemy_pos.y() - tower_pos.y()) * (enemy_pos.y() - tower_pos.y())) <= 2500
                    && !(i->block_enemy_list.contains(enemy))){
                i->block_enemy_list.push_back(enemy);
                return true;
            }
            else if(i->get_type() == trap_tower
                    && ((enemy_pos.x() - tower_pos.x()) * (enemy_pos.x() - tower_pos.x()) + (enemy_pos.y() - tower_pos.y()) * (enemy_pos.y() - tower_pos.y())) > 2500
                    && i->block_enemy_list.contains(enemy)){
                i->block_enemy_list.removeOne(enemy);
                return false;
            }
        }
    }
    return false;
}

void Game::load_enemies(qint64 cur_time){
    /*
    QTimer* qt1 = new QTimer(this);
    qt1->start(2100);//2100
    int num = 1;
    int i = 0;
    connect(qt1, &QTimer::timeout, [=]() mutable{
        if(num <= 30){//30
            Bandit *b = new Bandit(g_mission->route.at(i));
            enemy_list.push_back(b);
            i = (i + 1) % g_mission->route.size();
            ++num;
        }
    });

    QTimer* qt2 = new QTimer(this);
    qt2->start(7800);
    connect(qt2, &QTimer::timeout, [=]() mutable{
        if(num <= 8){//8
            Scout *s = new Scout(g_mission->route.at(i));
            enemy_list.push_back(s);
            i = (i + 1) % g_mission->route.size();
            ++num;
        }
    });

    QTimer* qt3 = new QTimer(this);
    qt3->start(6600);
    connect(qt3, &QTimer::timeout, [=]() mutable{
        if(num <= 10){//10
            FlyingMan *f = new FlyingMan(g_mission->route.at(i));
            enemy_list.push_back(f);
            i = (i + 1) % g_mission->route.size();
            ++num;
        }
    });

    QTimer* qt4 = new QTimer(this);
    qt4->start(7600);
    connect(qt4, &QTimer::timeout, [=]() mutable{
        if(num <= 8){//8
            Archer *a = new Archer(g_mission->route.at(i));
            enemy_list.push_back(a);
            i = (i + 1) % g_mission->route.size();
            ++num;
        }
    });

    QTimer* qt5 = new QTimer(this);
    qt5->start(17390);//17390
    connect(qt5, &QTimer::timeout, [=]() mutable{
        if(num <= 3){//4
            Catapult *c = new Catapult(g_mission->route.at(i));
            enemy_list.push_back(c);
            i = (i + 1) % g_mission->route.size();
            ++num;
        }
    });

    QTimer* qt6 = new QTimer(this);
    qt6->start(14530);//14530
    connect(qt6, &QTimer::timeout, [=]() mutable{
        if(num <= 3){//3
            Shaman *s = new Shaman(g_mission->route.at(i));
            enemy_list.push_back(s);
            i = (i + 1) % g_mission->route.size();
            ++num;
        }
    });

    QTimer* qt7 = new QTimer(this);
    qt7->start(23000);//23000
    connect(qt7, &QTimer::timeout, [=]() mutable{
        if(num <= 1){//1
            General *g = new General(g_mission->route.at(i));
            enemy_list.push_back(g);
            i = (i + 1) % g_mission->route.size();
            ++num;
        }
    });*/
    int j = 0;
    enemy_all_loaded = true;
    for(int i = 0; i < g_mission->enemy_enter_list.size(); i++){
        if(!g_mission->enemy_enter_list[i].isEmpty()){
            enemy_all_loaded = false;
            if(g_mission->enemy_enter_list[i][0] < (int)cur_time){
                switch (i) {
                    case bandit: {
                        Bandit *b = new Bandit(g_mission->route.at(j));
                        enemy_list.push_back(b);
                        j = (j + 1) % g_mission->route.size();
                        break;
                        }
                    case scout: {
                        Scout *s = new Scout(g_mission->route.at(j));
                        enemy_list.push_back(s);
                        j = (j + 1) % g_mission->route.size();
                        break;
                        }
                    case flyingman:{
                        FlyingMan *f = new FlyingMan(g_mission->route.at(j));
                        enemy_list.push_back(f);
                        j = (j + 1) % g_mission->route.size();
                        break;
                    }
                    case archer:{
                        Archer *a = new Archer(g_mission->route.at(j));
                        enemy_list.push_back(a);
                        j = (j + 1) % g_mission->route.size();
                        break;
                    }
                    case catapult:{
                        Catapult *c = new Catapult(g_mission->route.at(j));
                        enemy_list.push_back(c);
                        j = (j + 1) % g_mission->route.size();
                        break;
                    }
                    case shaman:{
                        Shaman *s = new Shaman(g_mission->route.at(j));
                        enemy_list.push_back(s);
                        j = (j + 1) % g_mission->route.size();
                        break;
                    }
                    case general:{
                        General *g = new General(g_mission->route.at(j));
                        enemy_list.push_back(g);
                        j = (j + 1) % g_mission->route.size();
                        break;
                    }
                    default:{break;}
                }
                g_mission->enemy_enter_list[i].pop_front();
            }
        }
    }
}
