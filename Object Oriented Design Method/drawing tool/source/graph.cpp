#include "graph.h"

Graph::Graph(QPoint start, QPoint end):
    start(start),
    end(end)
{
    is_editing = false;
}

void Graph::set_end_pos(QPoint end){
    this->end = end;
}

void Graph::set_start_pos(QPoint start){
    this->start = start;
}

QPoint Graph::get_start_pos(){
    return start;
}

QPoint Graph::get_end_pos(){
    return end;
}

void Graph::set_edit(){
    is_editing = !is_editing;
}

bool Graph::is_edit(){
    return is_editing;
}

void Graph::set_move_all(){
    move_all = !move_all;
}

bool Graph::is_move_all(){
    return move_all;
}

graphType Graph::get_type(){
    return type;
}

Line::Line(QPoint start, QPoint end):
    Graph(start, end)
{
    move_all = false;
    move_end = false;
    move_start = false;
    type = graphType::line;
}

void Line::paint(QPainter* qp){
    if(is_editing){
        qp->drawLine(start, end);
        qp->drawEllipse(start, 4, 4);
        qp->drawEllipse(end, 4, 4);
    }
    else{
        qp->drawLine(start, end);
    }
}

bool Line::in_shape(QPoint pos){
    double d;
    double d1 = (double)(start.x() - pos.x()) * (start.x() - end.x()) + (double)(start.y() - pos.y()) * (start.y() - end.y());
    if(d1 < 0){
        d = sqrt((double)pow((start.x() - pos.x()), 2) + (double)pow((start.y() - pos.y()), 2));
    }
    else{
        double d2 = (double)(end.x() - pos.x()) * (end.x() - start.x()) + (double)(end.y() - pos.y()) * (end.y() - start.y());
        if(d2 < 0){
            d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
        }
        else{
            double k = (double)(start.y() - end.y()) / (start.x() - end.x());
            double b = (double)start.y() - k * start.x();
            d = abs(k * pos.x() - pos.y() + b) / sqrt(k * k + 1);
        }
    }
    //qDebug() << d;
    return d <= 8;
}

void Line::begin_move(QPoint pos){
    double d;
    d = sqrt((double)pow((start.x() - pos.x()), 2) + (double)pow((start.y() - pos.y()), 2));
    if(d <= 8){
        move_start = true;
    }
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        move_end = true;
    }
    if(!move_start && !move_end){
        move_all = true;
    }
}

void Line::move(QPoint start_pos, QPoint end_pos){
    if(move_all){
        start = start - start_pos + end_pos;
        end = end - start_pos + end_pos;
    }
    else if(move_start){
        start = start - start_pos + end_pos;
    }
    else if(move_end){
        end = end - start_pos + end_pos;
    }
}

void Line::stop_move(){
    move_all = false;
    move_end = false;
    move_start = false;
}

void Line::translate_all_points(QPoint qp){
    start += qp;
    end += qp;
}

void Line::normalize(){
    return;
}

Line* Line::clone(){
    return new Line(*this);
}


Triangle::Triangle(QPoint start, QPoint end):
    Graph(start, end),
    the_other(start)
{
    move_all = false;
    move_end = false;
    move_the_other = false;
    move_start = false;
    type = graphType::triangle;
}

void Triangle::paint(QPainter* qp){
    if(is_editing){
        qp->drawLine(start, end);
        qp->drawLine(end, the_other);
        qp->drawLine(the_other, start);
        qp->drawEllipse(start, 4, 4);
        qp->drawEllipse(end, 4, 4);
        qp->drawEllipse(the_other, 4, 4);
    }
    else{
        qp->drawLine(start, end);
        qp->drawLine(end, the_other);
        qp->drawLine(the_other, start);
    }
}

bool Triangle::in_shape(QPoint pos){
    double u, v;
    QPoint a_b = end - start;
    QPoint a_c = the_other - start;
    QPoint a_p = pos - start;
    u = ((double)QPoint::dotProduct(a_b, a_b) * QPoint::dotProduct(a_p, a_c) - (double)QPoint::dotProduct(a_c, a_b) * QPoint::dotProduct(a_p, a_b))
        / ((double)QPoint::dotProduct(a_c, a_c) * QPoint::dotProduct(a_b, a_b) - (double)QPoint::dotProduct(a_c, a_b) * QPoint::dotProduct(a_b, a_c));
    v = ((double)QPoint::dotProduct(a_c, a_c) * QPoint::dotProduct(a_p, a_b) - (double)QPoint::dotProduct(a_b, a_c) * QPoint::dotProduct(a_p, a_c))
            / ((double)QPoint::dotProduct(a_c, a_c) * QPoint::dotProduct(a_b, a_b) - (double)QPoint::dotProduct(a_c, a_b) * QPoint::dotProduct(a_b, a_c));
    bool on_point = false;
    double d;
    d = sqrt((double)pow((start.x() - pos.x()), 2) + (double)pow((start.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    d = sqrt((double)pow((the_other.x() - pos.x()), 2) + (double)pow((the_other.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    return (u >= 0 && v >= 0 && u + v <= 1) || on_point;
}

void Triangle::begin_move(QPoint pos){
    double d;
    d = sqrt((double)pow((start.x() - pos.x()), 2) + (double)pow((start.y() - pos.y()), 2));
    if(d <= 8){
        move_start = true;
    }
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        move_end = true;
    }
    d = sqrt((double)pow((the_other.x() - pos.x()), 2) + (double)pow((the_other.y() - pos.y()), 2));
    if(d <= 8){
        move_the_other = true;
    }

    if(!move_start && !move_end && !move_the_other){
        move_all = true;
    }
}

void Triangle::move(QPoint start_pos, QPoint end_pos){
    if(move_all){
        start = start - start_pos + end_pos;
        end = end - start_pos + end_pos;
        the_other = the_other - start_pos + end_pos;
    }
    else if(move_start){
        start = start - start_pos + end_pos;
    }
    else if(move_end){
        end = end - start_pos + end_pos;
    }
    else if(move_the_other){
        the_other = the_other - start_pos + end_pos;
    }
}

void Triangle::stop_move(){
    move_all = false;
    move_end = false;
    move_start = false;
    move_the_other = false;
}

void Triangle::normalize(){
    the_other.setX((int)((double)(start.x() - end.x()) * 0.5 - (start.y() - end.y()) * 0.866025 + end.x()));
    the_other.setY((int)((double)(start.x() - end.x()) * 0.866025 + (start.y() - end.y()) * 0.5 + end.y()));

}

void Triangle::translate_all_points(QPoint qp){
    start += qp;
    end += qp;
    the_other += qp;
}

Triangle* Triangle::clone(){
    return new Triangle(*this);
}


Rectangle::Rectangle(QPoint start, QPoint end):
    Graph(start, end)
{
    move_all = false;
    move_end = false;
    move_start = false;
    type = graphType::rectangle;
}

void Rectangle::paint(QPainter* qp){
    if(is_editing){
        qp->drawRect(start.x(), start.y(), end.x() - start.x(), end.y() - start.y());
        qp->drawEllipse(start, 4, 4);
        qp->drawEllipse(end, 4, 4);
    }
    else{
        qp->drawRect(start.x(), start.y(), end.x() - start.x(), end.y() - start.y());
    }
}

bool Rectangle::in_shape(QPoint pos){
    bool on_point = false;
    double d;
    d = sqrt((double)pow((start.x() - pos.x()), 2) + (double)pow((start.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    return (pos.x() >= qMin(start.x(), end.x()) && pos.x() <= qMax(start.x(), end.x()) && pos.y() >= qMin(start.y(), end.y()) && pos.y() <= qMax(start.y(), end.y())) || on_point;
}

void Rectangle::begin_move(QPoint pos){
    double d;
    d = sqrt((double)pow((start.x() - pos.x()), 2) + (double)pow((start.y() - pos.y()), 2));
    if(d <= 8){
        move_start = true;
    }
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        move_end = true;
    }

    if(!move_start && !move_end){
        move_all = true;
    }
}

void Rectangle::move(QPoint start_pos, QPoint end_pos){
    if(move_all){
        start = start - start_pos + end_pos;
        end = end - start_pos + end_pos;
    }
    else if(move_start){
        start = start - start_pos + end_pos;
    }
    else if(move_end){
        end = end - start_pos + end_pos;
    }
}

void Rectangle::stop_move(){
    move_all = false;
    move_end = false;
    move_start = false;
}

void Rectangle::normalize(){

}

void Rectangle::translate_all_points(QPoint qp){
    start += qp;
    end += qp;
}

Rectangle* Rectangle::clone(){
    return new Rectangle(*this);
}


Ellipse::Ellipse(QPoint start, QPoint end):
    Graph(start, end),
    the_other(start)
{
    move_the_other = false;
    move_all = false;
    move_end = false;
    move_start = false;
    type = graphType::ellipse;
}

void Ellipse::paint(QPainter* qp){
    if(is_editing){
        qp->drawEllipse(start, abs(end.x() - start.x()), abs(the_other.y() - start.y()));
        qp->drawEllipse(the_other, 4, 4);
        qp->drawEllipse(end, 4, 4);
    }
    else{
        qp->drawEllipse(start, abs(end.x() - start.x()), abs(the_other.y() - start.y()));
    }
}

bool Ellipse::in_shape(QPoint pos){
    double res = (double)pow((pos.x() - start.x()), 2) / (double)pow(start.x() - end.x(), 2) + (double)pow((pos.y() - start.y()), 2) / (double)pow(start.y() - the_other.y(), 2);
    bool on_point = false;
    double d;
    d = sqrt((double)pow((the_other.x() - pos.x()), 2) + (double)pow((the_other.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    return res <= 1 || on_point;
}

void Ellipse::begin_move(QPoint pos){
    double d;
    d = sqrt((double)pow((the_other.x() - pos.x()), 2) + (double)pow((the_other.y() - pos.y()), 2));
    if(d <= 8){
        move_the_other = true;
    }
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        move_end = true;
    }

    if(!move_the_other && !move_end){
        move_all = true;
    }
}

void Ellipse::move(QPoint start_pos, QPoint end_pos){
    if(move_all){
        start = start - start_pos + end_pos;
        end = end - start_pos + end_pos;
        the_other = the_other - start_pos + end_pos;
    }
    else if(move_the_other){
        the_other = the_other - start_pos + end_pos;
    }
    else if(move_end){
        end = end - start_pos + end_pos;
    }
}

void Ellipse::stop_move(){
    move_all = false;
    move_end = false;
    move_start = false;
    move_the_other = false;
}

void Ellipse::normalize(){
    the_other = QPoint(start.x(), start.y() - end.x() + start.x());
}

void Ellipse::translate_all_points(QPoint qp){
    start += qp;
    end += qp;
    the_other += qp;
}

Ellipse* Ellipse::clone(){
    return new Ellipse(*this);
}


Circle::Circle(QPoint start, QPoint end):
    Ellipse(start, end)
{
    move_all = false;
    move_end = false;
    move_start = false;
    type = graphType::circle;
}

void Circle::paint(QPainter* qp){
    double r = sqrt((double)pow(start.x() - end.x(), 2) + (double)pow(start.y() - end.y(), 2));
    if(is_editing){
        qp->drawEllipse(QPointF(start), (qreal)r, (qreal)r);
        qp->drawEllipse(end, 4, 4);
    }
    else{
        qp->drawEllipse(QPointF(start), (qreal)r, (qreal)r);
    }
}

bool Circle::in_shape(QPoint pos){
    double r = (double)pow(start.x() - end.x(), 2) + (double)pow(start.y() - end.y(), 2);
    double res = (double)pow(start.x() - pos.x(), 2) + (double)pow(start.y() - pos.y(), 2);
    bool on_point = false;
    double d;
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        on_point = true;
    }
    return res <= r || on_point;
}

void Circle::begin_move(QPoint pos){
    double d;
    d = sqrt((double)pow((end.x() - pos.x()), 2) + (double)pow((end.y() - pos.y()), 2));
    if(d <= 8){
        move_end = true;
    }

    if(!move_end){
        move_all = true;
    }
}

void Circle::move(QPoint start_pos, QPoint end_pos){
    if(move_all){
        start = start - start_pos + end_pos;
        end = end - start_pos + end_pos;
    }
    else if(move_end){
        end = end - start_pos + end_pos;
    }
}

void Circle::stop_move(){
    move_all = false;
    move_end = false;
    move_start = false;
}

void Circle::normalize(){
}

void Circle::translate_all_points(QPoint qp){
    start += qp;
    end += qp;
}

Circle* Circle::clone(){
    return new Circle(*this);
}


Multiple_Graph* Multiple_Graph::instance = nullptr;

Multiple_Graph::Multiple_Graph(QList<Graph *> graph_list):
    Graph(QPoint(0, 0), QPoint(0, 0))
{
    for(auto i: graph_list){
        if(!i->is_edit()){
            i->set_edit();
        }
        this->graph_list.push_back(i);
    }
    move_all = false;
    move_end = false;
    move_start = false;
    type = graphType::multiple_graph;
}

void Multiple_Graph::paint(QPainter* qp){
    for(auto i: graph_list){
        i->paint(qp);
    }
}

bool Multiple_Graph::in_shape(QPoint pos){
    for(auto i: graph_list){
        if(i->in_shape(pos)){
            return true;
        }
    }
    return false;
}

void Multiple_Graph::begin_move(QPoint pos){
    move_all = true;
    for(auto i: graph_list){
        if(!i->is_move_all()){
            i->set_move_all();
        }
    }
}

void Multiple_Graph::move(QPoint start_pos, QPoint end_pos){
    if(move_all){
        for(auto i: graph_list){
            i->move(start_pos, end_pos);
        }
    }
}

void Multiple_Graph::stop_move(){
    move_all = false;
    for(auto i: graph_list){
        if(i->is_move_all()){
            i->set_move_all();
        }
    }
}

void Multiple_Graph::normalize(){
}

void Multiple_Graph::translate_all_points(QPoint qp){
    for(auto i: graph_list){
        i->translate_all_points(qp);
    }
}

void Multiple_Graph::add_graph(Graph * graph){
    graph_list.push_back(graph);
    if(!graph->is_edit()){
        graph->set_edit();
    }
}

QList<Graph *> Multiple_Graph::get_graph_list(){
    return graph_list;
}

Graph* Multiple_Graph::delete_graph(QPoint pos){
    //qDebug() << "in multi: " << graph_list.size();
    for(auto i: graph_list){
        if(i->in_shape(pos)){
            graph_list.removeOne(i);
            return i;
        }
    }
    return nullptr;
}

Multiple_Graph* Multiple_Graph::clone(){
    QList<Graph *> new_list;
    for(auto i: graph_list){
        new_list.push_back(i->clone());
    }
    return new Multiple_Graph(new_list);
}

void Multiple_Graph::set_edit(){
    is_editing = !is_editing;
    if(is_editing){
        for(auto i: graph_list){
            if(!i->is_edit()){
                i->set_edit();
            }
        }
    }
    else{
        for(auto i: graph_list){
            if(i->is_edit()){
                i->set_edit();
            }
        }
    }
}

Graph* Multiple_Graph::get_instance(){
    return instance;
}

Graph* Multiple_Graph::build_instance(QList<Graph *> graph_list){
    instance = new Multiple_Graph(graph_list);
    return instance;
}

void Multiple_Graph::clear_instance(){
    instance = nullptr;
}


Text::Text(QPoint start, QPoint end):
    Rectangle(start, end)
{
    move_all = false;
    move_end = false;
    move_start = false;
    content = "请输入文字";
    type = graphType::text;
}

void Text::paint(QPainter* qp){
    int max_font_size = 300;
    int min_font_size = 1;
    if(is_editing){
        QRectF qrect(start.x(), start.y(), end.x() - start.x(), end.y() - start.y());
        qp->drawRect(qrect);
        QTextOption qto;
        qto.setAlignment(Qt::AlignCenter);
        qto.setWrapMode(QTextOption::WrapAnywhere);
        QFont font = qp->font();
        for(int i = min_font_size; i < max_font_size; i++){
            font.setPixelSize(i);
            qp->setFont(font);
            QRectF rect = qp->boundingRect(qrect, content, qto);
            if(!qrect.contains(rect)){
                if(i == min_font_size){
                    font.setPixelSize(min_font_size);
                }
                else{
                    font.setPixelSize(i - 1);
                }
                qp->setFont(font);
                break;
            }
        }
        qp->drawText(qrect, content, qto);
        qp->drawEllipse(start, 4, 4);
        qp->drawEllipse(end, 4, 4);
    }
    else{
        QRectF qrect(start.x(), start.y(), end.x() - start.x(), end.y() - start.y());
        QTextOption qto;
        qto.setAlignment(Qt::AlignCenter);
        qto.setWrapMode(QTextOption::WrapAnywhere);
        QFont font = qp->font();
        for(int i = min_font_size; i < max_font_size; i++){
            font.setPixelSize(i);
            qp->setFont(font);
            QRectF rect = qp->boundingRect(qrect, content, qto);
            if(!qrect.contains(rect)){
                if(i == min_font_size){
                    font.setPixelSize(min_font_size);
                }
                else{
                    font.setPixelSize(i - 1);
                }
                qp->setFont(font);
                break;
            }
        }
        qp->drawText(qrect, content, qto);
    }
}

Text* Text::clone(){
    return new Text(*this);
}


QString Text::get_str(){
    return content;
}

void Text::set_str(QString string){
    content = string;
}

