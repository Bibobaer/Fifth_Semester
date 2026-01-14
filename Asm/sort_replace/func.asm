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

RET_CHR     equ 0
RET_STR     equ 1
RET_SORT    equ 2
RET_ERR     equ 3
; -------------

.data?
    _str dd ? 
    c1 db ?
    c2 db ?

    func_call dd ? 
.data
    format db "%s", 13, 10, 0
    format_for_int db "%d", 13, 10, 0
    formar_for_poi db "%p", 13, 10, 0

.const
    __repchar db "char", 0
    __repstr  db "str",  0
    __sort    db "sort", 0
    temp_string db "Is Working...", 0

.code 

include strings.inc
include replace.inc

check_arg: ; arg1 - argv[1]
    push ebp
    mov ebp, esp

    cmp dword ptr[ebp + arg1], 0
    jne check_arg_char
    mov eax, RET_ERR
    jmp check_arg_ret

    check_arg_char:
        push dword ptr[ebp + arg1]
        push offset __repchar
        call my_strcmp
        add esp, 8

        cmp eax, 0
        jne check_arg_str

        mov eax, RET_CHR
        jmp check_arg_ret

    check_arg_str:
        push dword ptr[ebp + arg1]
        push offset __repstr
        call my_strcmp
        add esp, 8

        cmp eax, 0
        jne check_arg_sort

        mov eax, RET_STR
        jmp check_arg_ret

    check_arg_sort:
        push dword ptr[ebp + arg1]
        push offset __sort
        call my_strcmp
        add esp, 8

        cmp eax, 0
        jne check_arg_error

        mov eax, RET_SORT
        jmp check_arg_ret

    check_arg_error:
        mov eax, RET_ERR

    check_arg_ret:
    mov esp, ebp
    pop ebp
    ret

main proc
    push ebp
    mov ebp, esp
    sub esp, 10h

    mov eax, dword ptr[ebp + arg2]
    mov dword ptr[ebp + var1], eax
    add dword ptr[ebp + var1], 4

    mov eax, dword ptr[ebp + var1]

    push dword ptr[eax]
    call check_arg
    add esp, 4

    mov dword ptr[func_call], eax
    add dword ptr[ebp + var1], 4

    ; call_replace_char
        cmp dword ptr[func_call], RET_CHR
        jne call_replace_str

        mov ebx, dword ptr[ebp + var1]
        mov ecx, dword ptr[ebx]
        mov ah, byte ptr[ecx]
        add dword ptr[ebp + var1], 4

        mov ebx, dword ptr[ebp + var1]
        mov ecx, dword ptr[ebx]
        mov al, byte ptr[ecx]
        add dword ptr[ebp + var1], 4

        mov ebx, dword ptr[ebp + var1]
        mov ebx, dword ptr[ebx]

        call replace_char

        invoke crt_printf, offset format, ebx
        jmp main_ret

    call_replace_str:
        cmp dword ptr[func_call], RET_STR
        jne call_sort_list

        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; rep
        add dword ptr[ebp + var1], 4
        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; pat
        add dword ptr[ebp + var1], 4
        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; text
        call replace_str
        add esp, 12

        invoke crt_printf, offset format, eax

    call_sort_list:

    main_ret:
    mov esp, ebp
    pop ebp
    ret
main endp
end