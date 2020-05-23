# PSUEDO
# Test for expansion of psuedo instructions

.data
arr: .word 3    # actually, addr should be 0x10000000 (default start addr)

.text
bgt $s0, $t1, 8 
li $t2, 5       # load a 16-bit immediate
li $t0, 75000   # load a 32-bit immediate
la $t1 arr      # load from address 'arr'

; branching with symbols 
branch_label:
bgt $s1 $t1 2
blt $s0 $t0 32
blt $s0 $t2 64      
