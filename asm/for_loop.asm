# For loop example
# Taken from https://courses.cs.washington.edu/courses/cse378/03wi/lectures/mips-asm-examples.html

    add    $t0, $gp, $zero      # &A[0] - 28
    lw     $t1, 4($gp)          # fetch N
    sll    $t1, $t1, 2          # N as byte offset
    add    $t1, $t1, $gp        # &A[N] - 28
    ori    $t2, $zero, 256      # MAX_SIZE
top:
    sltu   $t3, $t0, $t1        # have we reached the final address?
    beq    $t3, $zero, done     # yes, we're done
    sw     $t2, 28($t0)         # A[i] = 0
    addi   $t0, $t0, 4          # update $t0 to point to next element
    j      top                  # go to top of loop
done:
