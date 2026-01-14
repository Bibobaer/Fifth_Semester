; 1. Реализовать на языке ассемблера стандартные функции работы со строками (без использования строковых инструкций):
; char * strstr (const char *str, const char *strSearch);
; char * strcpy (char *dst, const char *src);
; unsigned int strlen (const char *str);
; char * strcat (char *dst, const char *src);
; int strcmp (const char *string1, const char *string2);
; char * strchr (const char *str, int c);

; Передачу аргументов, доступ к аргументам, доступ к локальным переменным необходимо организовать самостоятельно через обращения к стековой памяти через регистры ebp/esp. Т.е. нельзя пользоваться директивами ассемблера proc, invoke, local и др.
; Нельзя использовать регистры в качестве переменных (только для вспомогательных задач).

; Нельзя использовать строковые инструкции процессора.

; Функции должны придерживаться соглашения cdecl.
; Для тестирования написать программу, вызывающую функции с аргументами из командной строки.

my_strstr: ; arg1 - str, arg2 - strSearched
    push ebp
    mov ebp, esp
    sub esp, 4 ; counter
    mov dword ptr[ebp + var1], 0 ; init counter
    push ebx
    push esi

    ; Проверка длины строки
    push [ebp + arg1]
    call my_strlen ; size_str
    add esp, 4
    mov ecx, eax

    push [ebp + arg2]
    call my_strlen
    add esp, 4

    cmp ecx, eax
    jl no_sub_str
    ; ----------------------
    ; Бегу по подстроке, пока символы совпадают
    ; Иначе прыгаю обратно на исходную строку, вычитаю сколько счетчик насчитал и прибавляю к адресу 1

    for_source_string:
        mov ebx, [ebp + arg1] ; Если исходная строка завершилась, то подстроки нет
        cmp byte ptr[ebx], 0
        je no_sub_str

        for_searched_string:
            mov esi, [ebp + arg2] ; Если исходная строка завершилась, то подстрока найдена
            cmp byte ptr[esi], 0  ; Даже если подстрока NULL тогда он вернет исходную строку
            je check_counter

            mov ebx, [ebp + arg1]
            mov al, byte ptr[ebx]

            cmp al, byte ptr[esi]
            jne back_to_src_string

            inc dword ptr[ebp + arg1]
            inc dword ptr[ebp + arg2] 
            inc dword ptr[ebp + var1]

            jmp for_searched_string

    back_to_src_string:
        mov ebx, [ebp + arg1]
        sub ebx, [ebp + var1]
        mov [ebp + arg1], ebx

        mov ebx, [ebp + arg2]
        sub ebx, [ebp + var1]
        mov [ebp + arg2], ebx

        mov dword ptr[ebp + var1], 0
        inc dword ptr[ebp + arg1]

        jmp for_source_string

    check_counter:
        mov eax, [ebp + arg1]
        sub eax, [ebp + var1]
        jmp strstr_ret

    no_sub_str:
        mov eax, 0

    strstr_ret:
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret ; eax -> char*

;----------------------------------------------
my_strcpy: ; arg1 - dst, arg2 - src
    push ebp
    mov ebp, esp
    sub esp, 4

    mov eax, [ebp + arg1]
    mov [ebp + var1], eax ; temp_dst = dst

    strcpy_cyc:
        mov eax, [ebp + arg2]
        cmp byte ptr[eax], 0
        je strcpy_ret

        mov al, byte ptr[eax]
        mov edx, [ebp + var1]
        
        mov byte ptr[edx], al

        inc dword ptr[ebp + var1]
        inc dword ptr[ebp + arg2]

        jmp strcpy_cyc

    strcpy_ret:
    mov eax, [ebp + arg1]

    mov esp, ebp
    pop ebp
    ret ; eax -> char*
;----------------------------------------------
my_strlen: ; arg1 - str
    push ebp
    mov ebp, esp
    sub esp, 4 ; end

    mov eax, [ebp + arg1] 
    mov [ebp + var1], eax ; end = start(str)

    strlen_cyc:
        mov eax, [ebp + var1]
        cmp byte ptr[eax], 0
        je strlen_ret

        inc dword ptr[ebp + var1]

        jmp strlen_cyc

    strlen_ret:
    mov eax, [ebp + var1] ; eax = end
    sub eax, [ebp + arg1] ; eax = end - start

    mov esp, ebp
    pop ebp
    ret ; eax -> unsigned int
;----------------------------------------------
my_strcat: ; arg1 - dst, arg2 - src
    push ebp
    mov ebp, esp

    push [ebp + arg1]
    call my_strlen
    add esp, 4

    push eax
    add [ebp + arg1], eax ; Передвигаем указатель на конец строки

    push [ebp + arg2]
    push [ebp + arg1]
    call my_strcpy
    add esp, 8

    pop eax
    sub [ebp + arg1], eax
    mov eax, [ebp + arg1]

    mov esp, ebp
    pop ebp
    ret ; eax -> char*
;----------------------------------------------
my_strcmp: ; arg1 - str1, arg2 - str2
    push ebp
    mov ebp, esp
    push ebx
    push esi
    ; Проверка длин строк
    push [ebp + arg1]
    call my_strlen
    add esp, 4
    mov ecx, eax

    push [ebp + arg2]
    call my_strlen
    add esp, 4

    cmp ecx, eax
    jg str1_gr_str2
    jl str1_le_str2
    ; -------------------

    strcmp_cyc:
        mov ebx, [ebp + arg1]
        cmp byte ptr[ebx], 0
        je str1_eq_str2

        mov esi, [ebp + arg2]
        mov al, byte ptr[ebx]
        cmp al, byte ptr[esi]
        jg str1_gr_str2
        jl str1_le_str2

        inc dword ptr[ebp + arg1]
        inc dword ptr[ebp + arg2]

        jmp strcmp_cyc

    str1_gr_str2:
        mov eax, 1
        jmp strcmp_ret
    str1_le_str2:
        mov eax, -1
        jmp strcmp_ret
    str1_eq_str2:
        mov eax, 0

    strcmp_ret:
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret ; eax -> int
;----------------------------------------------
my_strchr: ; arg1 - str, arg2 - char
    push ebp
    mov ebp, esp
    sub esp, 4

    mov eax, [ebp + arg2]
    mov [ebp + var1], eax

    lea eax, [ebp + var1]

    push eax
    push [ebp + arg1]
    call my_strstr
    add esp, 8

    mov esp, ebp
    pop ebp
    ret ; eax -> char*
;----------------------------------------------