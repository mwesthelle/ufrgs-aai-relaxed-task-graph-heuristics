#include "relaxed_task_graph.h"

#include <iostream>
#include <deque>
#include <unordered_set>
#include <vector>

using namespace std;

namespace planopt_heuristics {
RelaxedTaskGraph::RelaxedTaskGraph(const TaskProxy &task_proxy)
    : relaxed_task(task_proxy),
      variable_node_ids(relaxed_task.propositions.size()) {
    /*
      TODO: add your code for exercise 2 (b) here. Afterwards
        - variable_node_ids[i] should contain the node id of the variable node for variable i
        - initial_node_id should contain the node id of the initial node
        - goal_node_id should contain the node id of the goal node
        - the graph should contain precondition and effect nodes for all operators
        - the graph should contain all necessary edges.
    */
    for (size_t i = 0; i < relaxed_task.propositions.size(); i++) {
        variable_node_ids[i] = graph.add_node(NodeType::OR);
    }

    initial_node_id = graph.add_node(NodeType::AND);
    goal_node_id = graph.add_node(NodeType::AND);

    for (PropositionID id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[id], initial_node_id);
    }

    for (const RelaxedOperator &op : relaxed_task.operators) {
        NodeID operator_id = graph.add_node(NodeType::AND, op.cost);

        NodeID precondition_node_id = graph.add_node(NodeType::AND);
        graph.add_edge(operator_id, precondition_node_id);
        for (PropositionID pre_id : op.preconditions) {
            graph.add_edge(precondition_node_id, variable_node_ids[pre_id]);
        }

        NodeID effect_node_id = graph.add_node(NodeType::AND);
        graph.add_edge(effect_node_id, operator_id);
        for (PropositionID eff_id : op.effects) {
            graph.add_edge(variable_node_ids[eff_id], effect_node_id);
        }

    }

    for (NodeID id : relaxed_task.goal) {
        graph.add_edge(goal_node_id, variable_node_ids[id]);
    }
}

void RelaxedTaskGraph::change_initial_state(const GlobalState &global_state) {
    // Remove all initial edges that were introduced for relaxed_task.initial_state.
    for (PropositionID id : relaxed_task.initial_state) {
        graph.remove_edge(variable_node_ids[id], initial_node_id);
    }

    // Switch initial state of relaxed_task
    relaxed_task.initial_state = relaxed_task.translate_state(global_state);

    // Add all initial edges for relaxed_task.initial_state.
    for (PropositionID id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[id], initial_node_id);
    }
}

bool RelaxedTaskGraph::is_goal_relaxed_reachable() {
    // Compute the most conservative valuation of the graph and use it to
    // return true iff the goal is reachable in the relaxed task.

    graph.most_conservative_valuation();
    return graph.get_node(goal_node_id).forced_true;
}

int RelaxedTaskGraph::additive_cost_of_goal() {
    // Compute the weighted most conservative valuation of the graph and use it
    // to return the h^add value of the goal node.

    // TODO: add your code for exercise 2 (c) here.
    graph.weighted_most_conservative_valuation();
    return graph.get_node(goal_node_id).additive_cost;
}

int RelaxedTaskGraph::ff_cost_of_goal() {
    // TODO: add your code for exercise 2 (e) here.
    graph.weighted_most_conservative_valuation();
    deque <NodeID> nodes;
    unordered_set <NodeID> visited;
    nodes.push_back(goal_node_id);
    int ff_cost = 0;
    while (!nodes.empty()) {
        NodeID current_node_id = nodes.front();
        nodes.pop_front();
        AndOrGraphNode current_node = graph.get_node(current_node_id);
        if (current_node.type == NodeType::AND) {
            for (NodeID successor_id : current_node.successor_ids) {
                if (visited.find(successor_id) == visited.end()) {
                    visited.insert(successor_id);
                    AndOrGraphNode successor = graph.get_node(successor_id);
                    ff_cost += successor.direct_cost;
                    nodes.push_back(successor_id);
                }
            }
        }
        else if (current_node.type == NodeType::OR) {
            if (visited.find(current_node.achiever) == visited.end()) {
                visited.insert(current_node.achiever);
                AndOrGraphNode achiever = graph.get_node(current_node.achiever);
                ff_cost += achiever.direct_cost;
                nodes.push_back(current_node.achiever);
            }
        }
    }

    return ff_cost;
}

}
