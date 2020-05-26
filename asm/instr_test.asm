# INSTR TEST
# All instructions supported by assembler in various combinations

.data 
ten: .word 10

; LOAD UPPER IMMEDIATE
lui $t0 ten     # load 'ten' into $t0
lui $at 4096    # use temp register
#lui $5 5       # at the time of writing this comment we don't support direct mapping
