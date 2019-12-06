    #!/bin/bash

    MIN=123257
    MAX=647015
    # Part 1
    seq $MIN $MAX | grep -E "^0*1*2*3*4*5*6*7*8*9*$" | grep -E ".*(.)\1.*" | wc -l
    # Part 2
    seq $MIN $MAX | grep -E "^0*1*2*3*4*5*6*7*8*9*$" | perl -pe 's/(.)\1{2,}/#/g' | grep -E "^.*([0-9])\1.*$" | wc -l
