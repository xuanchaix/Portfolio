#include "mainwindow.h"

MainWindow::MainWindow(Game *game, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , game(game)
{
    ui->setupUi(this);
    QTimer *qt = new QTimer(this);
    connect(qt, SIGNAL(timeout()), this, SLOT(window_update()));
    qt->start(15);
    game->game_start();
    //QTimer::singleShot(1000, this, SLOT(game->game_start()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete game;
    //delete painter;
    game = NULL;
    ui = NULL;
    //painter = NULL;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter *painter = new QPainter(this);
    //painter->begin(this);
    game->refresh(painter);
    //delete painter;
}

void MainWindow::mousePressEvent(QMouseEvent * event){
    QPoint pos = event->pos();
    if(event->button() == Qt::LeftButton && game->state == game_running){
        QList<Tower *>twlst = game->tower_list;
        for(auto tw: twlst){
            if(tw->in_position(pos) && !(game->show_tower_button)){
                int offset = 45;
                game->show_tower_button = true;
                game->button_list.clear();
                if(tw->get_type() == ranged_cons){
                    QPoint pos = tw->get_position();
                    game->button_list.push_back(new TowerButton(ranged_tower, pos + QPoint(-offset, -offset), tw));
                    game->button_list.push_back(new TowerButton(magic_tower, pos + QPoint(0, -offset), tw));
                    game->button_list.push_back(new TowerButton(mortar, pos + QPoint(offset, -offset), tw));
                    /*
                    QPoint twpos = tw->get_leftupperposition();
                    delete tw;
                    game->tower_list.removeAt(i);
                    RangedTower *t = new RangedTower(twpos, ranged_tower);
                    game->tower_list.push_back(t);
                    game->money -= RangedTower::get_cost();
                    break;*/
                }
                else if(tw->get_type() == melee_cons){
                    QPoint pos = tw->get_position();
                    game->button_list.push_back(new TowerButton(trap_tower, pos + QPoint(0, -offset), tw));
                }
                else if(tw->get_type() == trap_tower){
                    QPoint pos = tw->get_position();
                    game->button_list.push_back(new TowerButton(wall_tower, pos + QPoint(-offset, -offset), tw));
                    game->button_list.push_back(new TowerButton(fortress, pos + QPoint(offset, -offset), tw));

                }
                else if(tw->get_type() == wall_tower){
                    QPoint pos = tw->get_position();
                    game->button_list.push_back(new TowerButton(great_wall_tower, pos + QPoint(0, -offset), tw));
                }
                else if(tw->get_type() == ranged_tower){
                    QPoint pos = tw->get_position();
                    game->button_list.push_back(new TowerButton(spawn_tower, pos + QPoint(0, -offset), tw));
                }
                for(auto n_tw: twlst){
                    n_tw->show_range = false;
                }
                tw->show_range = !tw->show_range;
                break;
            }
            else if(game->show_tower_button){
                tw->show_range = false;
                for(auto i: game->button_list){
                    if(i->in_pos(pos)){
                        int tw_type = i->get_type();
                        if(game->money >= tower_cost[tw_type]){
                            Tower *this_tw = i->get_from_tower();
                            QPoint twpos = this_tw->get_position();
                            Tower *t = NULL;
                            switch (tw_type) {
                                case ranged_tower: t = new RangedTower(twpos, ranged_tower);break;
                                case wall_tower: t = new WallTower(twpos, wall_tower);break;
                                case magic_tower: t = new MagicTower(twpos, magic_tower);break;
                                case mortar: t = new Mortar(twpos, mortar);break;
                                case trap_tower: t = new TrapTower(twpos, trap_tower);break;
                                case fortress: t = new Fortress(twpos, fortress);break;
                                case great_wall_tower: t = new GreatWallTower(twpos, this_tw->get_hp(), great_wall_tower);break;
                                case spawn_tower: t = new SpawnTower(twpos, this_tw->get_hp(), spawn_tower);break;
                                default: t = NULL;break;
                            }
                            if(t){
                                game->tower_list.push_back(t);
                                game->money -= tower_cost[tw_type];
                                this_tw->is_upgrade();
                                //game->tower_list.removeOne(this_tw);
                                //delete this_tw;
                            }
                        }
                        else{
                            //
                        }
                        break;
                    }
                }
                game->show_tower_button = false;
                for(auto tb: game->button_list){
                    delete tb;
                }
                game->button_list.clear();
            }
            else{
                game->show_tower_button = false;
                for(auto tb: game->button_list){
                    delete tb;
                }
                game->button_list.clear();
                tw->show_range = false;
            }
        }
        //update();
    }
    else if(event->button() == Qt::LeftButton && (game->state == lose_game || game->state == win_game)){
        if(pos.x() > 250 && pos.x() < 400 && pos.y() > 300 && pos.y() < 400){
            Mission *mission = new Mission(game->get_mission()->mission_path);
            game = new Game(mission);
            game->state = game_running;
        }
        else if(pos.x() > 450 && pos.x() < 550 && pos.y() > 300 && pos.y() < 400){
            qApp->quit();
        }
    }
}

void MainWindow::window_update(){
    if(game->state == game_running){
        game->game_update();
    }
    update();
}
