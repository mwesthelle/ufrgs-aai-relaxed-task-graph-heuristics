#! /bin/bash

for file in castle/castle*.pddl; do
    instance=${file#castle/}
    instance=${instance%.pddl}
    echo "Running eager greedy search with planopt_ff for instance $instance."
    (
        ulimit -t 60
        ulimit -v 2000000
        python fast-downward/fast-downward.py castle/domain.pddl "$file" --search "eager_greedy([planopt_ff()])" > ex-f-results/"$instance"-hff.txt
    )
done
