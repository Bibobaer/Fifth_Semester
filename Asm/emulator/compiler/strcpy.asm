mov byte [0xC00], 72
mov byte [0xC01], 101
mov byte [0xC02], 108
mov byte [0xC03], 108
mov byte [0xC04], 111

push 0xC00
push 0xC06
call 0x106E
add r30, 8
hlt


push r20
mov r20, r30

add r20, 8
mov r1, [r20]
add r20, 4
mov r2, [r20]
sub r20, 12

cmp byte [r2], 0
je 0x10FD

mov byte [r1], byte [r2]
add r1, 1
add r2, 1

jne 0x10BB

add r20, 8
mov r0, [r20]
sub r20, 8

mov r30, r20
pop r20
ret
