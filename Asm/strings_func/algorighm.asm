; 2. Выбрать одно подзадание.
; 1) Пузырьковая сортировка массива (генерировать случайный) чисел. 1 балл.
; 2) Быстрая сортировка массива (генерировать случайный) строк (указателей на строки). 2 балла.
; 3) Рекурсивная реализация алгоритма Евклида нахождения НОД. 1 балл.
; 4) Рекурсивная реализация расширенного алгоритма Евклида нахождения НОД. 2 балла.
.686
.model flat, c

include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc

; -- Defines --
arg1 equ 8
arg2 equ 12
arg3 equ 16
arg4 equ 20

var1 equ -4
var2 equ -8
var3 equ -12
var4 equ -16
; -------------

.data?
    x dd ?
    y dd ?
.const
    a dd 18
    b dd 1014
    format_1 db "%d*%d + %d*%d", 13, 10, 0
    format_2 db "gcd = %d", 13, 10, 0

.code

extended_gcd proc; arg1 - a, arg2 - b, arg3 - *x, arg3 - *y
    push ebp
    mov ebp, esp
    sub esp, 8
    ; Local vars
        x1 equ var1
        y1 equ var2
    ; ----------

    cmp dword ptr[ebp + arg2], 0 ; b = 0 -> gcb = a
    je base_case

    lea esi, dword ptr[ebp + y1] ; esi = x
    lea ebx, dword ptr[ebp + x1] ; ebx = y
    
    xor edx, edx
    push eax
    mov eax, [ebp + arg1]
    idiv dword ptr[ebp + arg2] ; edx = a % b
    pop eax

    push esi
    push ebx
    push edx
    push dword ptr[ebp + arg2]
    call extended_gcd ; extended_gcd(b, a % b, &x1, &y1)
    add esp, 16

    mov ebx, [ebp + arg3]
    mov esi, [ebp + y1]
    mov [ebx], esi ; *x = y1

    push eax

    xor edx, edx
    mov eax, [ebp + arg1] ; eax = a
    idiv dword ptr[ebp + arg2] ; edx:eax / b -> eax = a / b, edx - remain ; (a / b)
    imul dword ptr[ebp + y1] ; edx:eax = eax * y1 = (a / b) * y1; 

    mov ebx, [ebp + arg4]
    mov esi, [ebp + x1]
    mov [ebx], esi
    sub [ebx], eax ; *y = x1 - (a % b) * y1

    pop eax
    jmp extended_gcd_ret

    base_case:
        mov esi, [ebp + arg3]
        mov dword ptr[esi], 1

        mov esi, [ebp + arg4]
        mov dword ptr[esi], 0

        mov eax, [ebp + arg1]
         
    extended_gcd_ret:
    mov esp, ebp
    pop ebp
    ret
extended_gcd endp

main proc
    lea ecx, [x]
    lea edx, [y]

    push edx
    push ecx
    push dword ptr[b]
    push dword ptr[a]
    call extended_gcd
    add esp, 16

    invoke crt_printf, offset format_2, eax
    invoke crt_printf, offset format_1, dword ptr[a], dword ptr[x], dword ptr[b], dword ptr[y]
    invoke ExitProcess,0
main endp
end