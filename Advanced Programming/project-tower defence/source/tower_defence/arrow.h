#ifndef ARROW_H
#define ARROW_H
#include <QObject>
#include <QPoint>
#include <QPainter>
#include <QSize>
#include <QPropertyAnimation>

enum arrow_type{arrow, magic_arrow, stone_arrow, shell};

class Arrow: public QObject{
    Q_OBJECT
    Q_PROPERTY(QPoint cur_pos READ get_pos WRITE set_pos);
public:
    Arrow(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type);
    ~Arrow();
    void paint(QPainter *painter);
    bool move();
    void hit();
    bool has_hitted();
    QPoint get_pos();
    int get_type();
    void set_pos(QPoint pos);
    bool is_enemy_arrow();
    void *get_enemy_target();
    void *get_tower_target();

protected:
    int type;
    int damage;
    int speed;
    QSize size;
    QPoint start_pos;
    QPoint cur_pos;
    void *target_enemy;
    void *target_tower;
    QString path;
    bool hitted;
    bool enemy_arrow;
    QPropertyAnimation *animation;

private:

};

class MagicArrow: public Arrow{
    Q_OBJECT
    Q_PROPERTY(QPoint cur_pos READ get_pos WRITE set_pos);
public:
    MagicArrow(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type);
    ~MagicArrow();
};

class StoneArrow: public Arrow{
    Q_OBJECT
    Q_PROPERTY(QPoint cur_pos READ get_pos WRITE set_pos);
public:
    StoneArrow(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type);
    ~StoneArrow();
};

class Shell: public Arrow{
    Q_OBJECT
    Q_PROPERTY(QPoint cur_pos READ get_pos WRITE set_pos);
public:
    Shell(QPoint point, void *target_enemy, void *target_tower, bool enemy_arrow, int type);
    ~Shell();
};

#endif // ARROW_H
