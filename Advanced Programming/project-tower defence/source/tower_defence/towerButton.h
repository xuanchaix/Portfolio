#ifndef TOWERBUTTON_H
#define TOWERBUTTON_H
#include "tower.h"
#include <QString>
#include <QPoint>
#include <QPainter>
/*
upgrade tree 
build
*basic_tower_button --melee 1 ranged 0
(melee)
    trap_tower3
        wall_tower14--great_wall_tower15
        fortress16--stronghold17
(ranged)
    ranged_tower2
        firelock_tower4--through_arrow_tower5
        spawn_tower6--spawn_garrison7
    magic_tower8
        big_magic_tower9--great_magic_towe10
    mortar11
        big_mortar12--rocket_tower13
*/
class TowerButton{
public:
    TowerButton(int type, QPoint pos, Tower* from_tower);
    ~TowerButton();
    void paint(QPainter *painter);
    bool in_pos(QPoint target_pos);
    int get_type();
    Tower* get_from_tower();

    QPoint pos;

private:
    int type;
    QString path;
    Tower *from_tower;
    static const QSize size;

};



#endif // TOWERBUTTON_H
