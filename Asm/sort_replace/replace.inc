; void Replace (char *str, char c1, char c2);
replace_char: ; ebx - str, al - c1, ah - c2

    xor ecx, ecx

    Replace_cyc:
        cmp byte ptr[ebx], 0
        je Replace_ret

        cmp byte ptr[ebx], al
        je Replace_edit_sym

        Replace_cyc_back:
        inc ecx
        inc ebx

        jmp Replace_cyc

    Replace_edit_sym:
        mov byte ptr[ebx], ah
        jmp Replace_cyc_back

    Replace_ret:
    sub ebx, ecx
    ret
; --------------------------------------------------------------------

; char * ReplaceStr (const char *text, const char *pattern, const char *rep);
replace_str:
    sub esp, 40
    ; ---Local vars------ ecx - count, new_len - eax (временно)
        loc_text_len    equ 0
        loc_pat_len     equ 4
        loc_rep_len     equ 8

        loc_count       equ 12
        loc_pos         equ 16

        loc_result      equ 20
        loc_current     equ 24
        loc_start       equ 28
        loc_found       equ 32

        loc_seg_len     equ 36

        text            equ 44
        pattern         equ 48
        _rep            equ 52
    ; -------------------

    cmp dword ptr[esp + text], 0
    je replace_str_err
    cmp dword ptr[esp + pattern], 0
    je replace_str_err
    cmp dword ptr[esp + _rep], 0
    jne replace_str_init_lens

    replace_str_err:
        mov eax, 0
        jmp replace_str_ret

    replace_str_init_lens:
        push dword ptr[esp + text]
        call my_strlen
        add esp, 4

        mov dword ptr[esp + loc_text_len], eax ; text_len = strlen(text)
        ; --------------------------------------------------------------------
        push dword ptr[esp + pattern]
        call my_strlen
        add esp, 4

        mov dword ptr[esp + loc_pat_len], eax ; pattern_len = strlen(pattern)
        ; --------------------------------------------------------------------
        push dword ptr[esp + _rep]
        call my_strlen
        add esp, 4

        mov dword ptr[esp + loc_rep_len], eax ; rep_len = strlen(rep)
        ; --------------------------------------------------------------------
    mov dword ptr[esp + loc_count], 0
    mov eax, dword ptr[esp + text]
    mov dword ptr[esp + loc_pos], eax
    jmp replace_str_malloc_loop_check

    replace_str_malloc_loop:
        inc dword ptr[esp + loc_count]
        mov eax, dword ptr[esp + loc_pat_len]
        add dword ptr[esp + loc_pos], eax

    replace_str_malloc_loop_check:
        push dword ptr[esp + pattern]
        push dword ptr[esp + loc_pos + 4] ; осознание пришло в 12 ночи...
        call my_strstr ; Вот здесь падает. Почему то strstr считает, что loc_pos - null.
        add esp, 8

        mov dword ptr[esp + loc_pos], eax
        cmp eax, 0
        jne replace_str_malloc_loop

    mov edx, dword ptr[esp + loc_count]
    mov eax, dword ptr[esp + loc_rep_len]
    sub eax, dword ptr[esp + loc_pat_len]

    imul edx, eax

    mov eax, dword ptr[esp + loc_text_len]
    add eax, edx
    inc eax

    mov ebx, eax

    push eax
    call crt_malloc
    add esp, 4

    mov dword ptr[esp + loc_result], eax

    push ebx
    push 0
    push dword ptr[esp + loc_result + 8]
    call crt_memset
    add esp, 12

    mov eax, dword ptr[esp + loc_result]
    mov dword ptr[esp + loc_current], eax ; current. буду изменять ее, чтобы не изменять result

    mov eax, dword ptr[esp + text]
    mov dword ptr[esp + loc_start], eax ; start = text
    jmp replace_str_check_loop

    replace_str_loop:

        mov eax, dword ptr[esp + loc_found]
        sub eax, dword ptr[esp + loc_start]

        mov dword ptr[esp + loc_seg_len], eax

        push dword ptr[esp + loc_seg_len]
        push dword ptr[esp + loc_start + 4]
        push dword ptr[esp + loc_current + 8]
        call my_strncpy
        add esp, 12

        mov eax, dword ptr[esp + loc_seg_len]
        add dword ptr[esp + loc_current], eax

        push dword ptr[esp + loc_rep_len]
        push dword ptr[esp + _rep + 4]
        push dword ptr[esp + loc_current + 8]
        call my_strncpy
        add esp, 12

        mov eax, dword ptr[esp + loc_rep_len]
        add dword ptr[esp + loc_current], eax

        mov edx, dword ptr[esp + loc_pat_len]
        mov eax, dword ptr[esp + loc_found]
        add eax, edx
        mov dword ptr[esp + loc_start], eax

    replace_str_check_loop:
        push dword ptr[esp + pattern]
        push dword ptr[esp + loc_start + 4]
        call my_strstr
        add esp, 8

        mov dword ptr[esp + loc_found], eax
        cmp dword ptr[esp + loc_found], 0
        jne replace_str_loop

        push dword ptr[esp + loc_start]
        push dword ptr[esp + loc_current + 4]
        call my_strcpy
        add esp, 8

        mov eax, dword ptr[esp + loc_result]
    replace_str_ret:
    add esp, 40
    ret


; Задание.

; На языке ассемблера реализовать функции и программы для их проверки. Аргументы для функций брать из командной строки.
; Нельзя пользоваться директивами ассемблера для организации функций (proc, invoke, local и др.).
; Нельзя использовать строковые инструкции процессора.


; 1. Замена символов в строке
; void Replace (char *str, char c1, char c2);
; c1  что заменять
; c2  на что заменять
; Изменять саму строку str.
; Не формировать стековый фрейм для функции. Для вычислений не использовать стековую память, а только регистры (для аргументов и локальных переменных).
; 1 балл.


; 2. Замена подстрок.
; char * ReplaceStr (const char *text, const char *pattern, const char *rep);
; text    где заменять
; pattern    что заменять
; rep      на что заменять
; Возвращать новую строку (нужной длины) с заменёнными вхождениями подстроки pattern.
; Функция в нотации cdecl.
; Не использовать регистр ebp для организации стекового фрейма. Доступ к аргументам и локальным переменным через регистр esp.
; 1 балл.


; 3. Реализовать сортировку списка (двусвязного) строк.
; Функции в нотации stdcall.
; В функцию сортировки должен передаваться указатель на функцию сравнения строк.
; Для тестирования генерировать список случайных строк (количество и длину строк принимать в аргументах командной строки).
; 2 балла.
