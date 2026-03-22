start:
lda 10
sta $240

loop:
out $240
inc
sta $240
cmp 15
bne loop

out
hlt
