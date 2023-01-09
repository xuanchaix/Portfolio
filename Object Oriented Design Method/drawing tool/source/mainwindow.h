#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QPoint>
#include <QDateTime>
#include "graph_factory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void paintEvent(QPaintEvent*);

private slots:
    void on_triangle_button_clicked();

    void on_line_button_clicked();

    void on_rectangle_button_clicked();

    void on_ellipse_button_clicked();

    void on_circle_button_clicked();

    void on_lineEdit_textEdited(const QString &arg1);

    void on_text_button_clicked();

private:
    Ui::MainWindow *ui;
    QList<Graph *> graph_list;
    QList<QList<Graph *>> memory;

    bool is_drawing;
    bool drawing_line;
    bool drawing_triangle;
    bool drawing_rectangle;
    bool drawing_ellipse;
    bool drawing_circle;
    bool drawing_text;

    bool is_editing;
    bool is_moving;
    QPoint move_start_pos;
    Graph *editing_graph;
    Graph *cutting_board_graph;
    Multiple_Graph *multi_editing_graph;
    bool holding_multi_editing_graph;

    //QList<QList<Graph>> memory;

    bool in_board_pos(QPoint pos);
    QPoint refresh_outrange_pos(QPoint start, QPoint cur);
    Graph* choose_graph(QPoint pos);
    void save_graph_list();
};
#endif // MAINWINDOW_H
