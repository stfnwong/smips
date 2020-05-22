#!/bin/bash
# Run unit tests


for t in bin/test/*; do
    bash $t || rc = $?
    if [[ rc -ne 0 ]] ; then
        exit $rc
    fi
done
