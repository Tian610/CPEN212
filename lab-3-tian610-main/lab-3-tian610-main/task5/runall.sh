#!/bin/bash

# Loop through trace files from trace01.lua to trace24.lua
for i in {01..32}; do
    echo "Executing ./cpen212alloc with trace $i"
    ./cpen212alloc traces/trace$i.lua
done
    

echo "All tasks completed!"