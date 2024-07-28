#! /bin/bash

for file in castle/castle*.pddl; do
    instance=${file#castle/}
    instance=${instance%.pddl}
    echo "Running eager greedy search with our own h_add implementation for instance $instance."
    (
        ulimit -t 60
        ulimit -v 2000000
        python fast-downward/fast-downward.py castle/domain.pddl "$file" --search "eager_greedy([planopt_add()])" > ex-d-results/"$instance"-hadd.txt
    )
    echo "Running astar with the lmcut heuristic on a delete relaxation for instance $instance."
    (
        ulimit -t 60
        ulimit -v 2000000
        python fast-downward/fast-downward.py castle/domain.pddl "$file" --search "astar(lmcut())" --translate-options --relaxed > ex-d-results/"$instance"-lmcut-relaxed.txt
    )
    echo "Running astar with the lmcut heuristic instance $instance."
    (
        ulimit -t 60
        ulimit -v 2000000
        python fast-downward/fast-downward.py castle/domain.pddl "$file" --search "astar(lmcut())" > ex-d-results/"$instance"-lmcut.txt
    )
done
