#ifndef GRAPH_H
#define GRAPH_H
#include <QPoint>
#include <QPainter>
#include <QDebug>
#include <QFont>

enum class graphType{line, triangle, rectangle, circle, ellipse, text, multiple_graph};

class Graph{
public:
    Graph(QPoint start, QPoint end);
    void set_end_pos(QPoint end);
    void set_start_pos(QPoint start);
    QPoint get_start_pos();
    QPoint get_end_pos();
    virtual void set_edit();
    void set_move_all();
    bool is_edit();
    bool is_move_all();
    graphType get_type();
    virtual void paint(QPainter* qp) = 0;
    virtual bool in_shape(QPoint pos) = 0;
    virtual void begin_move(QPoint pos) = 0;
    virtual void move(QPoint start_pos, QPoint end_pos) = 0;
    virtual void stop_move() = 0;
    virtual void normalize() = 0;
    virtual void translate_all_points(QPoint qp) = 0;

    virtual Graph* clone() = 0;

protected:
    graphType type;
    QPoint start;
    QPoint end;
    bool is_editing;
    bool move_start;
    bool move_end;
    bool move_all;
};


class Line: public Graph{
public:
    Line(QPoint start, QPoint end);
    virtual void paint(QPainter* qp) override;
    virtual bool in_shape(QPoint pos) override;
    virtual void begin_move(QPoint pos) override;
    virtual Line* clone() override;
    virtual void move(QPoint start_pos, QPoint end_pos) override;
    virtual void stop_move() override;
    virtual void normalize() override;
    virtual void translate_all_points(QPoint qp) override;

};


class Triangle: public Graph{
public:
    Triangle(QPoint start, QPoint end);
    virtual void paint(QPainter* qp) override;
    virtual bool in_shape(QPoint pos) override;
    virtual void begin_move(QPoint pos) override;
    virtual Triangle* clone() override;
    virtual void move(QPoint start_pos, QPoint end_pos) override;
    virtual void stop_move() override;
    virtual void normalize() override;
    virtual void translate_all_points(QPoint qp) override;

private:
    QPoint the_other;
    bool move_the_other;
};


class Rectangle: public Graph{
public:
    Rectangle(QPoint start, QPoint end);
    virtual void paint(QPainter* qp) override;
    virtual bool in_shape(QPoint pos) override;
    virtual void begin_move(QPoint pos) override;
    virtual Rectangle* clone() override;
    virtual void move(QPoint start_pos, QPoint end_pos) override;
    virtual void stop_move() override;
    virtual void normalize() override;
    virtual void translate_all_points(QPoint qp) override;

};


class Ellipse: public Graph{
public:
    Ellipse(QPoint start, QPoint end);
    virtual void paint(QPainter* qp) override;
    virtual bool in_shape(QPoint pos) override;
    virtual void begin_move(QPoint pos) override;
    virtual Ellipse* clone() override;
    virtual void move(QPoint start_pos, QPoint end_pos) override;
    virtual void stop_move() override;
    virtual void normalize() override;
    virtual void translate_all_points(QPoint qp) override;

private:
    QPoint the_other;
    bool move_the_other;
};


class Circle: public Ellipse{
public:
    Circle(QPoint start, QPoint end);
    virtual void paint(QPainter* qp) override;
    virtual bool in_shape(QPoint pos) override;
    virtual void begin_move(QPoint pos) override;
    virtual Circle* clone() override;
    virtual void move(QPoint start_pos, QPoint end_pos) override;
    virtual void stop_move() override;
    virtual void normalize() override;
    virtual void translate_all_points(QPoint qp) override;
};


class Multiple_Graph: public Graph{
public:
    static Graph* get_instance();
    static Graph* build_instance(QList<Graph *> graph_list);
    static void clear_instance();
    void add_graph(Graph * graph);
    QList<Graph *> get_graph_list();
    Graph* delete_graph(QPoint pos);
    virtual void set_edit() override;
    virtual void paint(QPainter* qp) override;
    virtual bool in_shape(QPoint pos) override;
    virtual void begin_move(QPoint pos) override;
    virtual Multiple_Graph* clone() override;
    virtual void move(QPoint start_pos, QPoint end_pos) override;
    virtual void stop_move() override;
    virtual void normalize() override;
    virtual void translate_all_points(QPoint qp) override;
private:
    Multiple_Graph(QList<Graph *> graph_list);
    QList<Graph *> graph_list;
    static Multiple_Graph* instance;
};


class Text: public Rectangle{
public:
    Text(QPoint start, QPoint end);
    virtual void paint(QPainter* qp) override;
    virtual Text* clone() override;
    QString get_str();
    void set_str(QString string);

private:
    QString content;

};

#endif // GRAPH_H
