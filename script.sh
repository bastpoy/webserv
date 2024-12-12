#!/bin/bash

# Define the range of FDs to check.
MAX_FD=1024  # You can adjust this depending on your system limits.

# Iterate over all possible FDs.
for fd in $(seq 3 $MAX_FD); do
    # Check if the FD is open.
    if [ -e /proc/self/fd/$fd ]; then
        # Close the FD.
        exec {fd}>&-
    fi
done

echo "All non-standard FDs closed."
