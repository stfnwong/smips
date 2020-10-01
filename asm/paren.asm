# PAREN
# Test parenthesis parsing

lw $t1, ($gp)
lw ($t2), (4($gp))
lw $t1, 4($gp)
;le ((($t1))) 4((((($t2)))))
