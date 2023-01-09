#include "mainwindow.h"
#include "ui_mainwindow.h"
/*
存在的问题：
*/


#define BOARD_START_X 50
#define BOARD_START_Y 100
#define BOARD_END_X 1230
#define BOARD_END_Y 720

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setEnabled(false);
    is_drawing = false;
    drawing_line = false;
    drawing_triangle = false;
    drawing_rectangle = false;
    drawing_ellipse = false;
    drawing_circle = false;
    drawing_text = false;
    is_editing = false;
    is_moving = false;
    editing_graph = 0;
    cutting_board_graph = 0;
    holding_multi_editing_graph = false;
    multi_editing_graph = 0;
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_line_button_clicked()
{
    drawing_line = true;
    is_drawing = true;
    is_editing = false;
    if(editing_graph){
        editing_graph->set_edit();
    }
    if(multi_editing_graph){
        for(auto i: multi_editing_graph->get_graph_list()){
            if(i->is_edit()){
                i->set_edit();
            }
            graph_list.push_back(i);
        }
        graph_list.removeOne(multi_editing_graph);
        delete multi_editing_graph;
        Multiple_Graph::clear_instance();
        multi_editing_graph = 0;
    }
    editing_graph = 0;
    update();
}


void MainWindow::on_triangle_button_clicked()
{
    drawing_triangle = true;
    is_drawing = true;
    is_editing = false;
    if(editing_graph){
        editing_graph->set_edit();
    }
    if(multi_editing_graph){
        for(auto i: multi_editing_graph->get_graph_list()){
            if(i->is_edit()){
                i->set_edit();
            }
            graph_list.push_back(i);
        }
        graph_list.removeOne(multi_editing_graph);
        delete multi_editing_graph;
        Multiple_Graph::clear_instance();
        multi_editing_graph = 0;
    }
    editing_graph = 0;
    update();
}


void MainWindow::on_rectangle_button_clicked()
{
    drawing_rectangle = true;
    is_drawing = true;
    is_editing = false;
    if(editing_graph){
        editing_graph->set_edit();
    }
    if(multi_editing_graph){
        for(auto i: multi_editing_graph->get_graph_list()){
            if(i->is_edit()){
                i->set_edit();
            }
            graph_list.push_back(i);
        }
        graph_list.removeOne(multi_editing_graph);
        delete multi_editing_graph;
        Multiple_Graph::clear_instance();
        multi_editing_graph = 0;
    }
    editing_graph = 0;
    update();
}


void MainWindow::on_ellipse_button_clicked()
{
    drawing_ellipse = true;
    is_drawing = true;
    is_editing = false;
    if(editing_graph){
        editing_graph->set_edit();
    }
    if(multi_editing_graph){
        for(auto i: multi_editing_graph->get_graph_list()){
            if(i->is_edit()){
                i->set_edit();
            }
            graph_list.push_back(i);
        }
        graph_list.removeOne(multi_editing_graph);
        delete multi_editing_graph;
        Multiple_Graph::clear_instance();
        multi_editing_graph = 0;
    }
    editing_graph = 0;
    update();
}


void MainWindow::on_circle_button_clicked()
{
    drawing_circle = true;
    is_drawing = true;
    is_editing = false;
    if(editing_graph){
        editing_graph->set_edit();
    }
    if(multi_editing_graph){
        for(auto i: multi_editing_graph->get_graph_list()){
            if(i->is_edit()){
                i->set_edit();
            }
            graph_list.push_back(i);
        }
        graph_list.removeOne(multi_editing_graph);
        delete multi_editing_graph;
        Multiple_Graph::clear_instance();
        multi_editing_graph = 0;
    }
    editing_graph = 0;
    update();
}


void MainWindow::on_text_button_clicked()
{
    drawing_text = true;
    is_drawing = true;
    is_editing = false;
    if(editing_graph){
        editing_graph->set_edit();
    }
    if(multi_editing_graph){
        for(auto i: multi_editing_graph->get_graph_list()){
            if(i->is_edit()){
                i->set_edit();
            }
            graph_list.push_back(i);
        }
        graph_list.removeOne(multi_editing_graph);
        delete multi_editing_graph;
        Multiple_Graph::clear_instance();
        multi_editing_graph = 0;
    }
    editing_graph = 0;
    update();
}


void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    if(is_editing && editing_graph->get_type() == graphType::text){
        ((Text *)editing_graph)->set_str(arg1);
    }
    update();
}


// 每次刷新时画图
void MainWindow::paintEvent(QPaintEvent *){
    QPainter *qp = new QPainter(this);
    qp->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    qp->fillRect(BOARD_START_X, BOARD_START_Y, BOARD_END_X - BOARD_START_X, BOARD_END_Y - BOARD_START_Y, Qt::white);
    for(auto i: graph_list){
        i->paint(qp);
    }
    qp->fillRect(0, 0, BOARD_START_X, 768, Qt::gray);
    qp->fillRect(BOARD_END_X, 0, 1280 - BOARD_END_X, 768, Qt::gray);
    qp->fillRect(0, 0, 1280, BOARD_START_Y, Qt::gray);
    qp->fillRect(0, BOARD_END_Y, 1280, 768 - BOARD_END_Y, Qt::gray);
    //qDebug() << "total: " << graph_list.size();
    delete qp;
    qp = 0;
}


void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton && in_board_pos(event->pos())){
        // 如果在画一条线，初始化这条线
        if(drawing_line){
            save_graph_list();
            graph_list.push_back(graph_factory::create_graph(graphType::line, event->pos()));
        }
        else if(drawing_triangle){
            save_graph_list();
            graph_list.push_back(graph_factory::create_graph(graphType::triangle, event->pos()));
        }
        else if(drawing_rectangle){
            save_graph_list();
            graph_list.push_back(graph_factory::create_graph(graphType::rectangle, event->pos()));
        }
        else if(drawing_ellipse){
            save_graph_list();
            graph_list.push_back(graph_factory::create_graph(graphType::ellipse, event->pos()));
        }
        else if(drawing_circle){
            save_graph_list();
            graph_list.push_back(graph_factory::create_graph(graphType::circle, event->pos()));
        }
        else if(drawing_text){
            save_graph_list();
            graph_list.push_back(graph_factory::create_graph(graphType::text, event->pos()));
        }
        // 如果选择了一个画完的图
        else if(choose_graph(event->pos())){
            // 如果和正在编辑的图不是同一个
            if(editing_graph != choose_graph(event->pos())){
                // 如果按下了ctrl多选
                if(editing_graph && event->modifiers() == Qt::ControlModifier){
                    if(multi_editing_graph){
                        //qDebug() << editing_graph << " " << multi_editing_graph << " " << choose_graph(event->pos());
                        Graph *temp = choose_graph(event->pos());
                        graph_list.removeOne(temp);
                        multi_editing_graph->add_graph(temp);
                    }
                    else{
                        editing_graph->set_edit();
                        QList<Graph *> editing_graphs;
                        editing_graphs.push_back(editing_graph);
                        graph_list.removeOne(editing_graph);
                        editing_graphs.push_back(choose_graph(event->pos()));
                        graph_list.removeOne(choose_graph(event->pos()));
                        multi_editing_graph = (Multiple_Graph *)Multiple_Graph::build_instance(editing_graphs);
                        graph_list.push_back(multi_editing_graph);
                        editing_graph = multi_editing_graph;
                        //qDebug() << multi_editing_graph->get_graph_list().size();
                        is_editing = true;
                    }
                }
                else{
                    if(editing_graph){
                        editing_graph->set_edit();
                    }
                    if(multi_editing_graph){
                        for(auto i: multi_editing_graph->get_graph_list()){
                            if(i->is_edit()){
                                i->set_edit();
                            }
                            graph_list.push_back(i);
                        }
                        graph_list.removeOne(multi_editing_graph);
                        delete multi_editing_graph;
                        Multiple_Graph::clear_instance();
                        multi_editing_graph = 0;
                    }
                    Graph *g = choose_graph(event->pos());
                    g->set_edit();
                    if(g->get_type() == graphType::text){
                        ui->lineEdit->setText(((Text *)g)->get_str());
                        ui->lineEdit->setEnabled(true);
                    }
                    is_editing = true;
                    editing_graph = g;
                }
            }
            // 如果和正在编辑的图是同一个
            else{
                save_graph_list();
                editing_graph->begin_move(event->pos());
                is_moving = true;
                move_start_pos = event->pos();
            }
        }
        // 如果点击了空白区域，去掉编辑图
        else if(is_editing && !choose_graph(event->pos())){
            if(editing_graph){
                editing_graph->set_edit();
            }
            if(editing_graph->get_type() == graphType::text){
                ui->lineEdit->setText("");
                ui->lineEdit->setEnabled(false);
            }
            if(multi_editing_graph){
                for(auto i: multi_editing_graph->get_graph_list()){
                    if(i->is_edit()){
                        i->set_edit();
                    }
                    graph_list.push_back(i);
                }
                graph_list.removeOne(multi_editing_graph);
                delete multi_editing_graph;
                Multiple_Graph::clear_instance();
                multi_editing_graph = 0;
            }
            is_editing = false;
            editing_graph = 0;
        }
        update();
    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event){
    QPoint pos;
    // 在画
    if(is_drawing){
        if(in_board_pos(event->pos()))
            pos = event->pos();
        else
            pos = refresh_outrange_pos(graph_list[graph_list.size() - 1]->get_start_pos(), event->pos());
        graph_list[graph_list.size() - 1]->set_end_pos(pos);
        graph_list[graph_list.size() - 1]->normalize();
        //qDebug() << event->pos().x() << " " << event->pos().y();
    }
    // 在移动
    else if(is_moving){
        //if(in_board_pos(event->pos()))
        pos = event->pos();
        //else
        //    pos = refresh_outrange_pos(graph_list[graph_list.size() - 1]->get_start_pos(), event->pos());
        editing_graph->move(move_start_pos, pos);
        move_start_pos = pos;
    }
    update();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        // 线完成
        if(drawing_line){
            graph_list[graph_list.size() - 1]->set_end_pos(event->pos());
            drawing_line = false;
            is_drawing = false;
        }
        // 三角形完成
        else if(drawing_triangle){
            graph_list[graph_list.size() - 1]->set_end_pos(event->pos());
            graph_list[graph_list.size() - 1]->normalize();
            drawing_triangle = false;
            is_drawing = false;
        }
        // 长方形完成
        else if(drawing_rectangle){
            graph_list[graph_list.size() - 1]->set_end_pos(event->pos());
            graph_list[graph_list.size() - 1]->normalize();
            drawing_rectangle = false;
            is_drawing = false;
        }
        // 椭圆完成
        else if(drawing_ellipse){
            graph_list[graph_list.size() - 1]->set_end_pos(event->pos());
            graph_list[graph_list.size() - 1]->normalize();
            drawing_ellipse = false;
            is_drawing = false;
        }
        // 椭圆完成
        else if(drawing_circle){
            graph_list[graph_list.size() - 1]->set_end_pos(event->pos());
            graph_list[graph_list.size() - 1]->normalize();
            drawing_circle = false;
            is_drawing = false;
        }
        // 文本框完成
        else if(drawing_text){
            graph_list[graph_list.size() - 1]->set_end_pos(event->pos());
            graph_list[graph_list.size() - 1]->normalize();
            drawing_text = false;
            is_drawing = false;
        }
        // 移动完成
        else if(is_moving){
            is_moving = false;
            editing_graph->stop_move();
            /*
            if(multi_editing_graph && choose_graph(event->pos()) == multi_editing_graph && pow((move_start_pos - event->pos()).x(), 2) + pow((move_start_pos - event->pos()).y(), 2) <= 1){
                Graph *temp = multi_editing_graph->delete_graph(event->pos());
                if(temp){
                    if(temp->is_edit()){
                        temp->set_edit();
                    }
                    graph_list.push_back(temp);
                }
            }*/
        }

        update();
    }
}


void MainWindow::keyReleaseEvent(QKeyEvent *event){
    // 删除操作
    if(is_editing && event->key() == Qt::Key_Delete && !is_drawing && !is_moving){
        is_editing = false;
        if(editing_graph){
            save_graph_list();
            graph_list.removeOne(editing_graph);
            delete editing_graph;
            Multiple_Graph::clear_instance();
            multi_editing_graph = 0;
            editing_graph = 0;
        }
    }
    // ctrl C复制
    else if(is_editing && event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier && !is_drawing && !is_moving){
        if(editing_graph){
            if(cutting_board_graph){
                delete cutting_board_graph;
            }
            if(multi_editing_graph){
                holding_multi_editing_graph = true;
            }
            else{
                holding_multi_editing_graph = false;
            }
            cutting_board_graph = graph_factory::clone_graph(editing_graph);
        }
    }
    // ctrl X剪切
    else if(is_editing && event->key() == Qt::Key_X && event->modifiers() == Qt::ControlModifier && !is_drawing && !is_moving){
        is_editing = false;
        if(editing_graph){
            if(cutting_board_graph){
                delete cutting_board_graph;
            }
            cutting_board_graph = graph_factory::clone_graph(editing_graph);
            if(multi_editing_graph){
                holding_multi_editing_graph = true;
            }
            else{
                holding_multi_editing_graph = false;
            }
            save_graph_list();
            graph_list.removeOne(editing_graph);
            delete editing_graph;
            editing_graph = 0;
            Multiple_Graph::clear_instance();
            multi_editing_graph = 0;
        }
    }
    // ctrl V粘贴
    else if(event->key() == Qt::Key_V && event->modifiers() == Qt::ControlModifier && !is_drawing && !is_moving){
        if(cutting_board_graph){
            Graph *g = graph_factory::clone_graph(cutting_board_graph);
            g->translate_all_points(QPoint(30, 30));
            save_graph_list();
            if(!g->is_edit()){
                g->set_edit();
            }
            if(editing_graph){
                editing_graph->set_edit();
            }
            if(holding_multi_editing_graph){
                if(multi_editing_graph){
                    for(auto i: multi_editing_graph->get_graph_list()){
                        if(i->is_edit()){
                            i->set_edit();
                        }
                        graph_list.push_back(i);
                    }
                    graph_list.removeOne(multi_editing_graph);
                    delete multi_editing_graph;
                }
                multi_editing_graph = (Multiple_Graph *)g;
            }
            graph_list.push_back(g);
            is_editing = true;
            editing_graph = g;
        }
    }
    // ctrl Z撤销
    else if(event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier && !is_drawing && !is_moving){
        if(!memory.empty()){
            for(auto i: graph_list){
                delete i;
            }
            graph_list.clear();
            is_editing = false;
            editing_graph = 0;
            multi_editing_graph = 0;
            Multiple_Graph::clear_instance();
            graph_list = memory.front();
            // 去掉之前图形的编辑信息，也可以保留
            for(auto i: graph_list){
                if(i->is_edit()){
                    i->set_edit();
                }
                if(i->get_type() == graphType::multiple_graph){
                    for(auto j: ((Multiple_Graph *)i)->get_graph_list()){
                        if(j->is_edit()){
                            j->set_edit();
                        }
                        graph_list.push_back(j);
                    }
                    graph_list.removeOne(i);
                    delete i;
                }
            }
            memory.pop_front();
        }
    }
    // ctrl U 提高到最上层
    else if(event->key() == Qt::Key_U && event->modifiers() == Qt::ControlModifier && !is_drawing && !is_moving && is_editing){
        graph_list.removeOne(editing_graph);
        graph_list.push_back(editing_graph);
    }
    // ctrl L 降低到最底层
    else if(event->key() == Qt::Key_L && event->modifiers() == Qt::ControlModifier && !is_drawing && !is_moving && is_editing){
        graph_list.removeOne(editing_graph);
        graph_list.push_front(editing_graph);
    }
    // ctrl S 保存截图
    else if(event->key() == Qt::Key_S && event->modifiers() == Qt::ControlModifier && !is_drawing && !is_moving && !is_editing){
        QPixmap p = this->grab(QRect(BOARD_START_X, BOARD_START_Y, BOARD_END_X - BOARD_START_X, BOARD_END_Y - BOARD_START_Y));
        QString file_name = "./saves/" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz") + ".png";
        p.save(file_name, "png");
    }
    update();
}


bool MainWindow::in_board_pos(QPoint pos){
    return pos.x() >= BOARD_START_X && pos.x() <= BOARD_END_X && pos.y() >= BOARD_START_Y && pos.y() <= BOARD_END_Y;
}


QPoint MainWindow::refresh_outrange_pos(QPoint start, QPoint cur){
    if(cur.x() < BOARD_START_X){
        double y = (double)(cur.y() - start.y()) / (cur.x() - start.x()) * (BOARD_START_X - cur.x()) + cur.y();
        y = (int)y;
        if(y >= BOARD_START_Y && y <= BOARD_END_Y){
            return QPoint(BOARD_START_X, y);
        }
    }
    if(cur.x() > BOARD_END_X){
        double y = (double)(cur.y() - start.y()) / (cur.x() - start.x()) * (BOARD_END_X - cur.x()) + cur.y();
        y = (int)y;
        if(y >= BOARD_START_Y && y <= BOARD_END_Y){
            return QPoint(BOARD_END_X, y);
        }
    }
    if(cur.y() < BOARD_START_Y){
        double x = (double)(cur.x() - start.x()) / (cur.y() - start.y()) * (BOARD_START_Y - cur.y()) + cur.x();
        return QPoint((int)x, BOARD_START_Y);
    }
    if(cur.y() > BOARD_END_Y){
        double x = (double)(cur.x() - start.x()) / (cur.y() - start.y()) * (BOARD_END_Y - cur.y()) + cur.x();
        return QPoint((int)x, BOARD_END_Y);
    }
    return QPoint(BOARD_END_X, BOARD_END_Y);
}


Graph* MainWindow::choose_graph(QPoint pos){
    for(int i = graph_list.size() - 1; i >= 0; i--){
        if(graph_list[i]->in_shape(pos)){
            return graph_list[i];
        }
    }
    return nullptr;
}


void MainWindow::save_graph_list(){
    QList<Graph *> new_mem = QList<Graph *>();
    for(auto i: graph_list){
        new_mem.push_back(graph_factory::clone_graph(i));
    }
    memory.push_front(new_mem);
}






























