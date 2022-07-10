//
// Created by Lukas on 24.06.2022.
//

#include "EdgeSequence.h"

#include "algorithm"

EdgeSequence::EdgeSequence(const std::vector<Graph::EdgeId> edge_ids, Graph::NodeId endnode_a, Graph::NodeId endnode_b, Graph::PathLength length) :
        _edge_ids(edge_ids),
        _endnode_a(endnode_a),
        _endnode_b(endnode_b),
        _length(length)
        {}


EdgeSequence::EdgeSequence(){}

Graph::NodeId EdgeSequence::endnode_a() const {
    return _endnode_a;
}

Graph::NodeId EdgeSequence::endnode_b() const {
    return _endnode_b;
}

Graph::PathLength EdgeSequence::length() const {
    return _length;
}

const std::vector<Graph::EdgeId>& EdgeSequence::edge_ids() const {
    return _edge_ids;
}

