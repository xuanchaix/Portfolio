#include "graph_factory.h"


graph_factory::graph_factory()
{

}

Graph* graph_factory::create_graph(graphType graph_type, QPoint start_point){
    switch (graph_type) {
        case graphType::line:
            return new Line(start_point, start_point);
        case graphType::triangle:
            return new Triangle(start_point, start_point);
        case graphType::rectangle:
            return new Rectangle(start_point, start_point);
        case graphType::ellipse:
            return new Ellipse(start_point, start_point);
        case graphType::circle:
            return new Circle(start_point, start_point);
        case graphType::text:
            return new Text(start_point, start_point);
        default:
            return nullptr;
    }
}

Graph* graph_factory::clone_graph(Graph* graph){
    return graph->clone();
}
