# A really simple program. 
# Taken from https://courses.cs.washington.edu/courses/cse378/03wi/lectures/mips-asm-examples.html)
    lw     $t0, 4($gp)       # fetch N
    mul    $t0, $t0, $t0     # N*N
    lw     $t1, 4($gp)       # fetch N
    ori    $t2, $zero, 3     # 3
    mul    $t1, $t1, $t2     # 3*N
    add    $t2, $t0, $t1     # N*N + 3*N
    sw     $t2, 0($gp)       # i = ...
