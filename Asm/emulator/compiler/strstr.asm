mov byte [0xC00], 72
mov byte [0xC01], 101
mov byte [0xC02], 108
mov byte [0xC03], 108
mov byte [0xC04], 111

mov byte [0xC06], 108
push 0xC06
push 0xC00
call 0x1079
add r30, 8
hlt


push r20
mov r20, r30

add r20, 8
mov r1, [r20]
add r20, 4
mov r2, [r20]
sub r20, 12
xor r3, r3

cmp byte [r1], 0
je 0x11B8

cmp byte [r2], 0
je 0x118C

cmp byte [r1], byte [r2]
jne 0x114A

add r1, 1
add r2, 1 
add r3, 1

cmp 0, 0
je 0x10E7

sub r1, r3
sub r2, r3
xor r3, r3

add r1, 1

cmp 23, 23
je 0x10D1

sub r1, r3
mov r0, r1
cmp 0, 0
je 0x11C3

mov r0, 0

mov r30, r20
pop r20
ret