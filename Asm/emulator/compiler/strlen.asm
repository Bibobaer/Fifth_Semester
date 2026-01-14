mov byte [0xC00], 72
mov byte [0xC01], 101
mov byte [0xC02], 108
mov byte [0xC03], 108
mov byte [0xC04], 111

push 0xC00
call 0x1063
add r30, 4
hlt


push r20
mov r20, r30

add r20, 8
mov r1, [r20]
sub r20, 8
xor r0, r0

cmp byte [r1], 0
je 0x10DC
add r0, 1
add r1, 1
jne 0x10A5

mov r30, r20
pop r20
ret