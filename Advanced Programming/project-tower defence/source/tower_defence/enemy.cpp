#include "enemy.h"
#include "tower.h"
#include <QDateTime>
Enemy::Enemy(Route route_in, QString path):
    route(route_in),
    path(path)
{
    index_route = 1;
    pos = route.waypoint[0];
    blocked = false;
    aiming = false;
    healing = false;
    reward = 0;
    fly = false;
    bellicose = true;
    healer = false;
    buffer = false;
    buffed = false;
    ranged = false;
    super_ranged = false;
    stop = false;
    is_hitted = 0;
    buff_time = 0;
}

Enemy::~Enemy(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}

bool Enemy::is_blocked(){
    return blocked;
}

void Enemy::get_blocked(){
    blocked = true;
}

void Enemy::not_blocked(){
    blocked = false;
}

void Enemy::melee_attack(){
    if(blocked && block_tower){
        if(buffed){
            ((Tower *)block_tower)->get_attacked(attack + 1);
        }
        else{
            ((Tower *)block_tower)->get_attacked(attack);
        }
    }
}

void Enemy::ranged_attack(){
    if(aiming && !super_ranged){
        Arrow *arrow = new Arrow(get_position(), NULL, target_tower, true, 0);
        send_arrow_list.push_back(arrow);
        arrow->move();
    }
    else if(aiming && super_ranged){
        Arrow *arrow = new StoneArrow(get_position(), NULL, target_tower, true, stone_arrow);
        send_arrow_list.push_back(arrow);
        arrow->move();
    }
}

void Enemy::heal(){
    if(healing && heal_target != NULL){
        heal_target->get_attacked(-2);
    }
    else if(heal_target == NULL){
        healing = false;
    }
}

void Enemy::choose_heal_target(QList<Enemy *> enemy_list){
    if(heal_target == NULL){
        healing = false;
    }
    else if(!healing){
        for(auto i:enemy_list){
            if(cal_distance(i->get_position()) <= range * range){
                heal_target = i;
                healing = true;
            }
        }
    }
    else{
        for(auto i:enemy_list){
            if(cal_distance(i->get_position()) <= range * range && i->get_hp() < heal_target->get_hp()){
                heal_target = i;
            }
        }
    }
}

void Enemy::buff(QList<Enemy *> enemy_list){
    for(auto i:enemy_list){
        if(cal_distance(i->get_position()) <= range * range && i != this){
            i->buffed = true;
            i->buff_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
        }
    }
}

int Enemy::get_bufftime(){
    return buff_time;
}

void Enemy::not_aiming(){
    aiming = false;
}

void Enemy::not_buffed(){
    buffed = false;
}

void Enemy::paint(QPainter *painter){
    //painter->save();
    //QImage img(path);
    //painter->drawImage(QPoint(pos.x() - size.width() / 2, pos.y() - size.height() / 2), img);
    if(!is_hitted){
        painter->drawPixmap(pos.x() - size.width() / 2, pos.y() - size.height() / 2, size.width(), size.height(), path);
    }
}

void Enemy::paint_hp(QPainter *painter){
    //painter->save();
    QPoint startpoint = pos - QPoint(size.width() / 2, size.height() - 6);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::red);
    painter->drawRect(QRect(startpoint, QSize(size.width(), 2)));

    painter->setBrush(Qt::green);
    painter->setPen(Qt::green);
    painter->drawRect(QRect(startpoint, QSize((double)size.width() * cur_hp / max_hp, 2)));
}

void Enemy::paint_state(QPainter *painter){
    if(buffed){
        QFont font;
        font.setPointSize(6);
        painter->setFont(font);
        painter->setPen(Qt::blue);
        painter->drawText(QRect(pos.x() - size.width() / 2 + 30, pos.y() - size.height() / 2, 40, 10), QString("buffed"));
    }
}

bool Enemy::move(){
    //follow the route
    if(pos.x() <= route.waypoint.at(index_route).x() + 1 && pos.x() >= route.waypoint.at(index_route).x() - 1 && pos.y() <= route.waypoint.at(index_route).y() + 1 && pos.y() >= route.waypoint.at(index_route).y() - 1){
        if(index_route < route.waypoint.size() - 1){
            ++index_route;
        }
        else{
            return false;
        }
    }
    //whether bolcked by melee tower
    if(!blocked && !aiming && !stop){
        //move
        QPoint target = route.waypoint.at(index_route);
        QVector2D *qv2d = new QVector2D(target - pos);
        qv2d->normalize();
        pos += qv2d->toPoint() * speed;
        delete qv2d;
    }
    return true;
}

int Enemy::get_index_toute(){
    return index_route;
}

QPoint Enemy::get_position(){
    //return QPoint(pos.x() - size.width() / 2, pos.y() - size.height() / 2);
    return pos;
}

void Enemy::get_attacked(int damage){
    cur_hp -= damage;
    if(cur_hp > max_hp){
        cur_hp = max_hp;
    }
    if(damage > 0){
        is_hitted = 2;
    }
}

void Enemy::sub_hitted(){
    if(is_hitted > 0){
        --is_hitted;
    }
}

int Enemy::get_hp(){
    return cur_hp;
}

int Enemy::get_reward(){
    return reward;
}

bool Enemy::is_bellicose(){
    return bellicose;
}

bool Enemy::is_fly(){
    return fly;
}

bool Enemy::is_ranged(){
    return ranged;
}

bool Enemy::is_super_ranged(){
    return super_ranged;
}

bool Enemy::is_buffer(){
    return buffer;
}

bool Enemy::is_healer(){
    return healer;
}

bool Enemy::is_buffed(){
    return buffed;
}

bool Enemy::choose_target(void *tower){
    int dis = cal_distance(((Tower *)tower)->get_position());
    if(!aiming){
        if(dis < range * range){
            target_tower = tower;
            aiming = true;
            return true;
        }
    }
    else if(((Tower *)target_tower)->is_melee() && super_ranged){
        if(dis < range * range && !((Tower *)tower)->is_melee()){
            target_tower = tower;
            aiming = true;
            return true;
        }
    }
    return false;
}

int Enemy::cal_distance(QPoint tower_pos){
    return (tower_pos.x() - pos.x()) * (tower_pos.x() - pos.x()) + (tower_pos.y() - pos.y()) * (tower_pos.y() - pos.y());
}

Bandit::Bandit(Route route_in):Enemy(route_in, QString(""))
{
    attack = 1;
    speed = 1;
    attack_speed = 1000;
    max_hp = 10;
    cur_hp = 10;
    reward = 10;
    fly = false;
    bellicose = true;
    ranged = false;
    range = 0;
    super_ranged = false;
    buffer = false;
    size = QSize(30, 40);
    path = QString(":/enemies/assets/enemy1.png");

    QTimer *qt = new QTimer(this);
    connect(qt, &QTimer::timeout, [this]{
        melee_attack();
    });
    qt->start(attack_speed);

    //delete qt;
}

Bandit::~Bandit(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}

Scout::Scout(Route route_in):Enemy(route_in, QString("")){
    attack = 0;
    speed = 1;
    attack_speed = 0;
    max_hp = 7;
    cur_hp = 7;
    reward = 15;
    fly = false;
    bellicose = false;
    ranged = false;
    range = 0;
    super_ranged = false;
    buffer = false;
    size = QSize(30, 40);
    path = QString(":/enemies/assets/enemy2.png");

}

Scout::~Scout(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}

FlyingMan::FlyingMan(Route route_in):Enemy(route_in, QString("")){
    attack = 1;
    speed = 2;
    attack_speed = 1000;
    max_hp = 7;
    cur_hp = 7;
    reward = 15;
    fly = true;
    bellicose = true;
    ranged = false;
    range = 0;
    super_ranged = false;
    buffer = false;
    size = QSize(40, 30);
    path = QString(":/enemies/assets/enemy3.png");

    QTimer *qt = new QTimer(this);
    connect(qt, &QTimer::timeout, [this]{
        melee_attack();
    });
    qt->start(attack_speed);

}

FlyingMan::~FlyingMan(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}

Archer::Archer(Route route_in):Enemy(route_in, QString("")){
    attack = 1;
    speed = 1;
    attack_speed = 1000;
    max_hp = 10;
    cur_hp = 10;
    reward = 15;
    fly = false;
    bellicose = true;
    ranged = true;
    range = 80;
    super_ranged = false;
    buffer = false;
    size = QSize(30, 40);
    path = QString(":/enemies/assets/enemy5.png");

    QTimer *qt1 = new QTimer(this);
    connect(qt1, &QTimer::timeout, [this]{
        if(blocked){
            melee_attack();
        }
    });
    qt1->start(attack_speed);

    QTimer *qt2 = new QTimer(this);
    connect(qt2, &QTimer::timeout, [this]{
        if(!blocked){
            ranged_attack();
        }
    });
    qt2->start(attack_speed);
}

Archer::~Archer(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}

Catapult::Catapult(Route route_in):Enemy(route_in, QString("")){
    attack = 0;
    speed = 1;
    attack_speed = 1500;
    max_hp = 10;
    cur_hp = 10;
    reward = 20;
    fly = false;
    bellicose = false;
    ranged = true;
    range = 120;
    super_ranged = true;
    buffer = false;
    size = QSize(40, 30);
    path = QString(":/enemies/assets/enemy6.png");

    QTimer *qt2 = new QTimer(this);
    connect(qt2, &QTimer::timeout, [this]{
        if(!blocked){
            ranged_attack();
        }
    });
    qt2->start(attack_speed);
}

Catapult::~Catapult(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}

Shaman::Shaman(Route route_in):Enemy(route_in, QString("")){
    attack = 1;
    speed = 1;
    attack_speed = 1000;
    max_hp = 10;
    cur_hp = 10;
    reward = 20;
    fly = false;
    bellicose = true;
    ranged = false;
    range = 150;
    super_ranged = false;
    buffer = false;
    healer = true;
    size = QSize(30, 40);
    path = QString(":/enemies/assets/enemy4.png");

    QTimer *qt1 = new QTimer(this);
    connect(qt1, &QTimer::timeout, [this]{
        melee_attack();
    });
    qt1->start(attack_speed);

    QTimer *qt2 = new QTimer(this);
    connect(qt2, &QTimer::timeout, [this]{
        heal();
        stop = !stop;
    });
    qt2->start(1000);
}

Shaman::~Shaman(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}

General::General(Route route_in):Enemy(route_in, QString("")){
    attack = 2;
    speed = 1;
    attack_speed = 1000;
    max_hp = 30;
    cur_hp = 30;
    reward = 30;
    fly = false;
    bellicose = true;
    ranged = false;
    range = 120;
    super_ranged = false;
    buffer = true;
    healer = false;
    size = QSize(30, 40);
    path = QString(":/enemies/assets/enemy7.png");

    QTimer *qt1 = new QTimer(this);
    connect(qt1, &QTimer::timeout, [this]{
        melee_attack();
    });
    qt1->start(attack_speed);
}

General::~General(){
    block_tower = NULL;
    target_tower = NULL;
    heal_target = NULL;
    send_arrow_list.clear();
}
