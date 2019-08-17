## ARRAYS ##
.data
  list: .word 3, 0, 1, 2, 6                 # This is an array of words
  char_arr: .asciiz "hello"                 # This is a char array
  buffer: .space 128                        # Allocates a block in memory, does
                                            # not automatically clear
                                            # These blocks of memory are aligned
                                            # next each other
.text
  la $s0, list                              # Load address of list
  li $t0, 0                                 # Counter
  li $t1, 5                                 # Length of the list

  loop:
    bgt $t0, $t1, end_loop

    lw $a0, ($s0) 
    li $v0, 1
    syscall                                 # Print the number

    addi $s0, $s0, 4                        # Size of a word is 4 bytes
    addi $t0, $t0, 1                        # Increment
    j loop
  end_loop:
