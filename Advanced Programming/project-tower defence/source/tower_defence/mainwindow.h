#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QString>
#include <QtDebug>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Game *game, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);

private:
    Ui::MainWindow *ui;
    Game *game;
    //QPainter *painter;
private slots:
    void window_update();
};
#endif // MAINWINDOW_H
