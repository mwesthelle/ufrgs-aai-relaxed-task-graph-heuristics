#! /bin/bash

for file in castle/castle*.pddl; do
    instance=${file#castle/}
    instance=${instance%.pddl}
    echo "Running blind search for instance $instance"
    (
        ulimit -t 60
        ulimit -v 2000000
        python fast-downward/fast-downward.py castle/domain.pddl "$file" --search "astar(blind())" > ex-b-results/"$instance"-blind.txt
    )
    echo "Running search for instance $instance with relaxed task graph heuristic"
    (
        ulimit -t 60
        ulimit -v 2000000
        python fast-downward/fast-downward.py castle/domain.pddl "$file" --search "astar(planopt_relaxed_task_graph())" > ex-b-results/"$instance"-relaxed-task-graph.txt
    )
done
