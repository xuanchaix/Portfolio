#include "arrow.h"
#include "tower.h"
#include "enemy.h"

Arrow::Arrow(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type=arrow):
    type(type),
    start_pos(point),
    target_enemy((void *)target_enemy),
    target_tower((void *)target_tower),
    enemy_arrow(enemy_arrow)
{
    damage = 1;
    speed = 250;
    hitted = false;
    size = QSize(20, 10);
    switch (type) {
        case arrow: path = QString(":/arrows/assets/arrow1.png");break;
        case magic_arrow: path = QString(":/arrows/assets/arrow2.png");break;
        case stone_arrow: path = QString(":/arrows/assets/arrow3.png");break;
        case shell: path = QString(":/arrows/assets/arrow4.png");break;
        default: path = QString("");
    }
}

Arrow::~Arrow(){
    target_enemy = NULL;
    target_tower = NULL;
    delete animation;
    animation = NULL;
}

void Arrow::paint(QPainter *painter){
    painter->drawPixmap(cur_pos.x(), cur_pos.y(), size.width(), size.height(), path);
}



bool Arrow::move(){
    if(target_enemy != NULL || target_tower != NULL){
        QPoint target;
        if(!enemy_arrow){
            target = ((Enemy *)target_enemy)->get_position();
        }
        else{
            target = ((Tower *)target_tower)->get_position();
        }
        //QVector2D *qv2d = new QVector2D(target - cur_pos);
        //qv2d->normalize();
        //QPoint p = qv2d->toPoint();
        //int x = sqrt(p.x() * p.x() + p.y() * p.y());
        //p = QPoint(p.x() / x, p.y() / x);
        //cur_pos += p * speed;
        //delete qv2d;
        animation = new QPropertyAnimation(this, "cur_pos");
        animation->setDuration(speed);
        animation->setStartValue(start_pos);
        animation->setEndValue(target);
        //animation->setEasingCurve(QEasingCurve::OutInQuad);
        animation->start();
        connect(animation, &QPropertyAnimation::finished, [=]() {
            hit();
            hitted = true;
        });
        //if(cur_pos.x() <= target.x() + 5 && cur_pos.x() >= target.x() - 5 && cur_pos.y() <= target.y() + 5 && cur_pos.y() >= target.y() - 5){
        //    return true;
        //}
        //else{
        //    return false;
        //}
    }

    return false;
}

void Arrow::hit(){
    if((int)enemy_arrow == 0){
        //qDebug() << (int)enemy_arrow;
        ((Enemy *)target_enemy)->get_attacked(damage);
    }
    else if((int)enemy_arrow == 1){
        ((Tower *)target_tower)->get_attacked(damage);
    }
    else{
        qDebug() << "arrow bug!!";
        delete this;
    }
}

bool Arrow::is_enemy_arrow(){
    return enemy_arrow;
}

void * Arrow::get_enemy_target(){
    return target_enemy;
}

void * Arrow::get_tower_target(){
    return target_tower;
}

bool Arrow::has_hitted(){
    return hitted;
}

QPoint Arrow::get_pos(){
    return cur_pos;
}

int Arrow::get_type(){
    return type;
}

void Arrow::set_pos(QPoint pos){
    cur_pos = pos;
}

MagicArrow::MagicArrow(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type=magic_arrow):Arrow(point, target_enemy, target_tower, enemy_arrow, type)
{
    damage = 3;
    speed = 400;
    hitted = false;
    size = QSize(15, 15);
}

MagicArrow::~MagicArrow(){
    target_enemy = NULL;
    target_tower = NULL;
    delete animation;
    animation = NULL;
}

StoneArrow::StoneArrow(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type=stone_arrow):Arrow(point, target_enemy, target_tower, enemy_arrow, type)
{
    damage = 2;
    speed = 850;
    hitted = false;
    size = QSize(15, 15);
}

StoneArrow::~StoneArrow(){
    target_enemy = NULL;
    target_tower = NULL;
    delete animation;
    animation = NULL;
}

Shell::Shell(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type=shell):Arrow(point, target_enemy, target_tower, enemy_arrow, type)
{
    damage = 3;
    speed = 600;
    hitted = false;
    size = QSize(15, 15);
}

Shell::~Shell(){
    target_enemy = NULL;
    target_tower = NULL;
    delete animation;
    animation = NULL;
}

