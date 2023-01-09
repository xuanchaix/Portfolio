#include "tower.h"

const QSize Tower::size(50, 50);
const int Tower::cost = 0;
const int WallTower::cost = tower_cost[wall_tower];
const int RangedTower::cost = tower_cost[ranged_tower];
const int TrapTower::cost = tower_cost[trap_tower];
const int Mortar::cost = tower_cost[mortar];
const int MagicTower::cost = tower_cost[magic_tower];
const int Fortress::cost = tower_cost[fortress];
const int GreatWallTower::cost = tower_cost[great_wall_tower];
const int SpawnTower::cost = tower_cost[spawn_tower];



Tower::Tower(QPoint pos, int type):
    pos(pos),
    type(type)
{
    attack = 0;
    range = 0;
    attack_speed = 0;
    show_range = false;
    melee = false;
    ranged = false;

    //melee tower
    block_enemy_num = 0;
    max_hp = 1;
    cur_hp = 1;

    //ranged tower
    target_enemy = NULL;
    //spawn
    can_spawn = false;
    //qDebug() << type;
    is_upgraded = false;
    switch (type) {
        case ranged_cons: path = QString(":/towers/assets/construction.bmp");break;
        case melee_cons: path = QString(":/towers/assets/construction.bmp");break;
        case ranged_tower: path = QString(":/towers/assets/tower1.bmp");break;
        case wall_tower: path = QString(":/towers/assets/tower2.bmp");break;
        case magic_tower: path = QString(":/towers/assets/magic_tower.png");break;
        case mortar: path = QString(":/towers/assets/mortar.png");break;
        case trap_tower: path = QString(":/towers/assets/trap_tower.png");break;
        case fortress: path = QString(":/towers/assets/fortress_tower.png");break;
        case great_wall_tower: path = QString(":/towers/assets/great_wall_tower.png");break;
        case guard_tower: path = QString(":/towers/assets/guard_tower.png");break;
        case spawn_tower: path = QString(":/towers/assets/spawn_tower.png");break;
        default: path = QString("");
    }
}

Tower::~Tower(){
    block_enemy_list.clear();
    send_arrow_list.clear();
    target_enemy = NULL;

}

QPoint Tower::get_position(){
    return pos;
}

QString Tower::get_path(){
    return path;
}

QPoint Tower::get_leftupperposition(){
    QPoint p;
    p.setX(pos.x() - size.width() / 2);
    p.setY(pos.y() - size.height() / 2);
    return p;
}

void Tower::paint(QPainter *painter){
    //painter->save();
    if(type == ranged_tower && path != QString(":/towers/assets/tower1.bmp"))qDebug() << path;
    painter->drawPixmap(pos.x() - size.width() / 2, pos.y() - size.height() / 2, size.width(), size.height(), path);
}

void Tower::paint_hp(QPainter *painter){
    if(type != melee_cons && type != ranged_cons && type != trap_tower){
        QPoint startpoint = pos - QPoint(size.width() / 2, size.height() * 3 / 2 - 20);
        painter->setBrush(Qt::red);
        painter->setPen(Qt::red);
        painter->drawRect(QRect(startpoint, QSize(size.width(), 3)));

        painter->setBrush(Qt::green);
        painter->setPen(Qt::green);
        painter->drawRect(QRect(startpoint, QSize((double)size.width() * cur_hp / max_hp, 3)));
    }
}

void Tower::paint_range(QPainter *painter){
    //painter->save();
    if(range && show_range){
        painter->setBrush(QBrush());
        painter->setPen(Qt::red);
        painter->drawEllipse(get_position(), range, range);
    }
}

bool Tower::in_position(QPoint pos_in){
    //qDebug() << pos_in.x() << pos_in.y() << pos.x() << pos.y();
    return pos_in.x() > pos.x() - size.width() / 2 && pos_in.x() < pos.x() + size.width() / 2 && pos_in.y() > pos.y() - size.height() / 2 && pos_in.y() < pos.y() + size.height() / 2;
}

int Tower::get_type(){
    return type;
}

int Tower::get_hp(){
    return cur_hp;
}

void Tower::update(QString upath, int urange, int uattack, int uattack_speed, int utype){
    path = upath;
    range = urange;
    attack = uattack;
    attack_speed = uattack_speed;
    type = utype;
}

bool Tower::is_melee(){
    return melee;
}

bool Tower::is_ranged(){
    return ranged;
}

void Tower::is_upgrade(){
    is_upgraded = true;
}

bool Tower::upgraded(){
    return is_upgraded;
}

int Tower::get_cost(){
    return cost;
}

int RangedTower::get_cost(){
    return cost;
}

int WallTower::get_cost(){
    return cost;
}

int MagicTower::get_cost(){
    return cost;
}

int TrapTower::get_cost(){
    return cost;
}

int Mortar::get_cost(){
    return cost;
}

int Fortress::get_cost(){
    return cost;
}

int GreatWallTower::get_cost(){
    return cost;
}

int SpawnTower::get_cost(){
    return cost;
}

void Tower::get_attacked(int damage){
    cur_hp -= damage;
}

void Tower::choose_target(QList<Enemy *> enemy_list){
    int max_index_route = 0;
    Enemy *temp_target = target_enemy;
    if(!enemy_list.empty()){
        for(auto i: enemy_list){
            if(i->is_fly() && type == mortar){
                continue;
            }
            int dis = enemy_distance(i->get_position());
            if(dis <= range * range){//in range
                if(i->get_index_toute() > max_index_route){//moved farest
                    max_index_route = i->get_index_toute();
                    if(!temp_target || dis > enemy_distance(temp_target->get_position())){//max distance
                        temp_target = i;
                    }
                }
                //else if((*i)->get_index_toute() == max_index_route){
                //    if(!temp_target || dis > enemy_distance(temp_target->get_position())){
                //        temp_target = (*i);
                //    }
                //}
            }
        }
        if(temp_target && enemy_distance(temp_target->get_position()) > range * range){
            temp_target = NULL;
        }
    }
    else{
        temp_target = NULL;
    }
    if(target_enemy != temp_target){
        target_enemy = temp_target;
        send_arrow_list.clear();
    }
    if(!enemy_list.contains(target_enemy)){
        target_enemy = NULL;
        send_arrow_list.clear();
    }
}

int Tower::enemy_distance(QPoint enemy_pos){
    return (enemy_pos.x() - pos.x()) * (enemy_pos.x() - pos.x()) + (enemy_pos.y() - pos.y()) * (enemy_pos.y() - pos.y());
}

TrapTower::TrapTower(QPoint pos, int type=trap_tower):Tower(pos, type){
    attack = 1;
    range = 50;
    attack_speed = 1000;
    show_range = false;
    melee = true;
    block_enemy_num = 1000000;
    max_hp = 10;
    cur_hp = 10;
    if(type == trap_tower){
        QTimer *qt = new QTimer(this);
        connect(qt, &QTimer::timeout, [this]{
            melee_attack();
        });
        qt->start(attack_speed);
    }
    //delete qt;
}

TrapTower::~TrapTower(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}


WallTower::WallTower(QPoint pos, int type=wall_tower):TrapTower(pos, type){
    attack = 1;// QString(":/towers/assets/tower2"),
    range = 0;
    attack_speed = 1000;
    show_range = false;
    melee = true;
    block_enemy_num = 5;
    max_hp = 40;
    cur_hp = 40;
    if(type == wall_tower){
        QTimer *qt = new QTimer(this);
        connect(qt, &QTimer::timeout, [this]{
            melee_attack();
        });
        qt->start(attack_speed);
    }
    //delete qt;
}

WallTower::~WallTower(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}

GreatWallTower::GreatWallTower(QPoint pos, int curr_hp, int type=wall_tower):WallTower(pos, type){
    attack = 1;
    range = 150;
    int range_attack_speed = 1000;
    attack_speed = 1000;
    show_range = false;
    melee = true;
    ranged = true;
    block_enemy_num = 10;
    max_hp = 80;
    cur_hp = curr_hp + 40;
    if(type == great_wall_tower){
        QTimer *qt1 = new QTimer(this);
        connect(qt1, &QTimer::timeout, [this]{
            melee_attack();
        });
        qt1->start(attack_speed);
        QTimer *qt2 = new QTimer(this);
        connect(qt2, &QTimer::timeout, [this]{
            ranged_attack();
        });
        qt2->start(range_attack_speed);
    }
    //delete qt;
}

GreatWallTower::~GreatWallTower(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}

void GreatWallTower::ranged_attack(){
    if(type == great_wall_tower && target_enemy){
        Arrow *arrow = new Arrow(get_position(), (void *)target_enemy, NULL, false, 0);
        send_arrow_list.push_back(arrow);
        arrow->move();
    }
}

void TrapTower::melee_attack(){
    //qDebug() << block_enemy_list.size();
    if(!block_enemy_list.empty()){
        for(auto it :block_enemy_list){
            if(!(it->is_fly())){
                it->get_attacked(attack);
                if(it->get_hp() <= 0){
                    block_enemy_list.removeOne(it);
                }
            }
        }
    }
}

RangedTower::RangedTower(QPoint pos, int type=ranged_tower):Tower(pos, type){
    //attack = 1;
    range = 200;
    attack_speed = 600;
    ranged = true;
    show_range = false;
    max_hp = 10;
    cur_hp = 10;
    //arrow = new Arrow(QString(":/arrows/assets/arrow1.png"), get_position());
    if(type == ranged_tower){
        QTimer *qt = new QTimer(this);
        connect(qt, &QTimer::timeout, [this]{
            ranged_attack();
        });
        qt->start(attack_speed);
    }
    //delete qt;
}

RangedTower::~RangedTower(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}

void RangedTower::ranged_attack(){
    if(type == ranged_tower && target_enemy){
        Arrow *arrow = new Arrow(get_position(), (void *)target_enemy, NULL, false, 0);
        send_arrow_list.push_back(arrow);
        arrow->move();
    }
    else if(type == magic_tower && target_enemy){
        Arrow *arrow = new MagicArrow(get_position(), (void *)target_enemy, NULL, false, magic_arrow);
        send_arrow_list.push_back(arrow);
        arrow->move();
    }
    else if(type == mortar && target_enemy){
        Arrow *arrow = new Shell(get_position(), (void *)target_enemy, NULL, false, shell);
        send_arrow_list.push_back(arrow);
        arrow->move();
    }
}

MagicTower::MagicTower(QPoint pos, int type=magic_tower):RangedTower(pos, type){
    //attack = 1;
    range = 150;
    attack_speed = 1500;
    ranged = true;
    show_range = false;
    max_hp = 10;
    cur_hp = 10;
    if(type == magic_tower){
        QTimer *qt = new QTimer(this);
        connect(qt, &QTimer::timeout, [this]{
            ranged_attack();
        });
        qt->start(attack_speed);
    }
}

MagicTower::~MagicTower(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}

Mortar::Mortar(QPoint pos, int type=mortar):RangedTower(pos, type){
    //attack = 1;
    range = 300;
    attack_speed = 2000;
    ranged = true;
    show_range = false;
    max_hp = 10;
    cur_hp = 10;
    if(type == mortar){
        QTimer *qt = new QTimer(this);
        connect(qt, &QTimer::timeout, [this]{
            ranged_attack();
        });
        qt->start(attack_speed);
    }
}

Mortar::~Mortar(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}

Fortress::Fortress(QPoint pos, int type=fortress):TrapTower(pos, type){
    attack = 0;
    range = 0;
    attack_speed = 0;
    show_range = false;
    melee = true;
    block_enemy_num = 10;
    max_hp = 80;
    cur_hp = 80;
}

Fortress::~Fortress(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}

GuardTower::GuardTower(QPoint pos, int type=guard_tower):TrapTower(pos, type){
    attack = 1;
    range = 0;
    attack_speed = 1000;
    show_range = false;
    melee = true;
    block_enemy_num = 2;
    max_hp = 10;
    cur_hp = 10;
    if(type == guard_tower){
        QTimer *qt = new QTimer(this);
        connect(qt, &QTimer::timeout, [this]{
            melee_attack();
        });
        qt->start(attack_speed);
    }
}

GuardTower::~GuardTower(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}

SpawnTower::SpawnTower(QPoint pos, int curr_hp, int type=wall_tower):RangedTower(pos, type){
    //attack = 1;
    range = 150;
    attack_speed = 5000;
    show_range = false;
    ranged = true;
    max_hp = 20;
    cur_hp = curr_hp + 10;
    can_spawn = true;
    if(type == spawn_tower){
        QTimer *qt = new QTimer(this);
        connect(qt, &QTimer::timeout, [this]{
            can_spawn = true;
        });
        qt->start(attack_speed);
    }
    //delete qt;
}


SpawnTower::~SpawnTower(){
    target_enemy = NULL;
    block_enemy_list.clear();
    send_arrow_list.clear();
}
