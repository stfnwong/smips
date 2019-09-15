#  DATA_REGION_EX
# Example of handling assembly directives for data regions

.data # Section where data is stored in memory (allocated in RAM), similar to
      # variables in higher level languages

  # Declarations follow a ( label: .type value(s) ) form of declaration
  hello_world: .asciiz "Hello World\n"      # Declare a null terminated string
  num1: .word 42                            # Integers are referred to as words
