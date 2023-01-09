#ifndef GRAPH_FACTORY_H
#define GRAPH_FACTORY_H

#include <QPoint>
#include "graph.h"


class graph_factory
{
public:
    graph_factory();
    static Graph* create_graph(graphType graph_type, QPoint start_point);
    static Graph* clone_graph(Graph* graph);
};

#endif // GRAPH_FACTORY_H
