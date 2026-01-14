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

    RET_STR equ 0
    RET_CPY equ 1
    RET_LEN equ 2
    RET_CAT equ 3
    RET_CMP equ 4
    RET_CHR equ 5
    RET_ERR equ 6
; -------------

.data?
    src dd ?
    dst dd ?
    func_call dd ? 
.data
.const  
    __str db "strstr", 0
    __cpy db "strcpy", 0
    __len db "strlen", 0
    __cat db "strcat", 0
    __cmp db "strcmp", 0
    __chr db "strchr", 0
    __error db "Unknown func", 0

    format1 db "%s", 13, 10, 0
    format2 db "%d", 13, 10, 0


.code
include strings.inc

check_func: ;  arg1 - argv[1]
    push ebp
    mov ebp, esp

    push offset __str
    push dword ptr [ebp + arg1]
    call my_strcmp
    add esp, 8

    cmp eax, 0
    je check_func_str
    ; ----------------------------
    push offset __cpy
    push dword ptr [ebp + arg1]
    call my_strcmp
    add esp, 8

    cmp eax, 0
    je check_func_cpy
    ; ----------------------------
    push offset __len
    push dword ptr [ebp + arg1]
    call my_strcmp
    add esp, 8

    cmp eax, 0
    je check_func_len
    ; ----------------------------
    push offset __cmp
    push dword ptr [ebp + arg1]
    call my_strcmp
    add esp, 8

    cmp eax, 0
    je check_func_cmp
    ; ----------------------------
    push offset __cat
    push dword ptr [ebp + arg1]
    call my_strcmp
    add esp, 8

    cmp eax, 0
    je check_func_cat
    ; ----------------------------
    push offset __chr
    push dword ptr [ebp + arg1]
    call my_strcmp
    add esp, 8

    cmp eax, 0
    je check_func_chr

    mov eax, RET_ERR
    jmp check_func_ret

    check_func_str:
        mov eax, RET_STR
        jmp check_func_ret
    check_func_cpy:
        mov eax, RET_CPY
        jmp check_func_ret
    check_func_len:
        mov eax, RET_LEN
        jmp check_func_ret
    check_func_cmp:
        mov eax, RET_CMP
        jmp check_func_ret
    check_func_cat:
        mov eax, RET_CAT
        jmp check_func_ret
    check_func_chr:
        mov eax, RET_CHR
    
    check_func_ret:
    mov esp, ebp
    pop ebp
    ret

main proc
    push ebp
    mov ebp, esp
    sub esp, 10h

    mov eax, dword ptr[ebp + arg2]
    mov dword ptr[ebp + var1], eax
    add dword ptr[ebp + var1], 4 ; var1 - argv[1], ...

    mov eax, dword ptr[ebp + var1]
    cmp dword ptr[eax], 0
    je main_ret

    push dword ptr[eax]
    call check_func
    add esp, 4

    mov dword ptr[func_call], eax
    add dword ptr[ebp + var1], 4

    cmp dword ptr[func_call], RET_STR
    jne check_strcpy_num

    mov eax, dword ptr[ebp + var1]
    push dword ptr[eax] ; src - в обратную строну будет
    add dword ptr[ebp + var1], 4
    mov eax, dword ptr[ebp + var1]
    push dword ptr[eax] ; dst
    call my_strstr
    add esp, 8

    invoke crt_printf, offset format1, eax
    jmp main_ret

    check_strcpy_num:
        cmp dword ptr[func_call], RET_CPY
        jne check_strlen_num

        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; src - в обратную строну будет
        add dword ptr[ebp + var1], 4
        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; dst
        call my_strcpy
        add esp, 8

        invoke crt_printf, offset format1, eax
        jmp main_ret

    check_strlen_num:
        cmp dword ptr[func_call], RET_LEN
        jne check_strcat_num

        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; src - в обратную строну будет
        call my_strlen
        add esp, 4

        invoke crt_printf, offset format2, eax
        jmp main_ret

    check_strcat_num:
        cmp dword ptr[func_call], RET_CAT
        jne check_strcmp_num

        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; src - в обратную строну будет
        add dword ptr[ebp + var1], 4
        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; dst
        call my_strcat
        add esp, 8

        invoke crt_printf, offset format1, eax
        jmp main_ret

    check_strcmp_num:
        cmp dword ptr[func_call], RET_CMP
        jne check_strchr_num

        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; src - в обратную строну будет
        add dword ptr[ebp + var1], 4
        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; dst
        call my_strcmp
        add esp, 8

        invoke crt_printf, offset format2, eax
        jmp main_ret

    check_strchr_num:
        cmp dword ptr[func_call], RET_CHR
        jne write_error

        mov eax, dword ptr[ebp + var1]
        mov ebx, dword ptr[eax]
        xor ch, ch
        mov cl, byte ptr[ebx]
        push cx ; src - в обратную строну будет
        add dword ptr[ebp + var1], 4
        mov eax, dword ptr[ebp + var1]
        push dword ptr[eax] ; dst
        call my_strchr
        add esp, 8

        invoke crt_printf, offset format1, eax
        jmp main_ret

    write_error:
        invoke crt_printf, offset format1, offset __error

    main_ret:
    mov esp, ebp
    pop ebp
    ret
main endp
end