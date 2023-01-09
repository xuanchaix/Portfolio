#include "towerButton.h"

const QSize TowerButton::size(30, 30);

TowerButton::TowerButton(int type, QPoint pos, Tower* from_tower):
    pos(pos),
    type(type),
    from_tower(from_tower)
{
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
        case spawn_tower: path = QString(":/towers/assets/spawn_tower.png");break;
        default: path = QString("");
    }
}

TowerButton::~TowerButton(){
    from_tower = NULL;
}

void TowerButton::paint(QPainter *painter){
    painter->drawPixmap(pos.x() - size.width() / 2, pos.y() - size.height() / 2, size.width(), size.height(), path);
    QFont font;
    font.setPointSize(12);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(QRect(pos.x() - size.width() / 2, pos.y() + size.height() / 2, 30, 20), Qt::AlignHCenter, QString("%1").arg(tower_cost[type]));
}

bool TowerButton::in_pos(QPoint target_pos){
    return target_pos.x() <= pos.x() + size.width() / 2 && target_pos.x() >= pos.x() - size.width() / 2 && target_pos.y() <= pos.y() + size.height() / 2 && target_pos.y() >= pos.y() - size.height() / 2;
}

int TowerButton::get_type(){
    return type;
}

Tower* TowerButton::get_from_tower(){
    return from_tower;
}
