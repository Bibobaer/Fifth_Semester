include planet_date.inc
include Strings.mac

.code
;------------------------------------------------------------------------------
; Функция для преобразования даты в дни от 01.01.1970
; Вход: year, month, day
; Выход: EAX = количество дней
;------------------------------------------------------------------------------
DateToDays proc frame_macro year:dword, month:dword, day:dword
    local totalDays:dword
    local y:dword
    local m:dword
    
    ; Инициализация
    mov totalDays, 0
    
    ; Добавляем дни из полных лет с 1970 до года-1
    mov eax, year
    sub eax, 1970
    .if eax > 0
        mov y, 1970
        mov edi, year
        .while y < edi
            ; Проверяем високосный год
            xor cax, cax
            mov eax, y
            xor edx, edx
            mov ecx, 4
            div ecx
            .if edx == 0
                ; Год делится на 4 - проверяем дальше
                mov eax, y
                xor edx, edx
                mov ecx, 100
                div ecx
                .if edx == 0
                    ; Год делится на 100 - проверяем на 400
                    mov eax, y
                    xor edx, edx
                    mov ecx, 400
                    div ecx
                    .if edx == 0
                        ; Високосный (делится на 400)
                        add totalDays, 366
                    .else
                        ; Не високосный (делится на 100, но не на 400)
                        add totalDays, 365
                    .endif
                .else
                    ; Високосный (делится на 4, но не на 100)
                    add totalDays, 366
                .endif
            .else
                ; Не високосный
                add totalDays, 365
            .endif
            inc y
        .endw
    .endif
    
    ; Добавляем дни из полных месяцев в текущем году
    mov m, 1
    mov edi, month
    .while m < edi
        .if m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12
            add totalDays, 31
        .elseif m == 4 || m == 6 || m == 9 || m == 11
            add totalDays, 30
        .elseif m == 2
            ; Проверяем високосный год для февраля
            mov eax, year
            xor edx, edx
            mov ecx, 4
            div ecx
            .if edx == 0
                mov eax, year
                xor edx, edx
                mov ecx, 100
                div ecx
                .if edx == 0
                    mov eax, year
                    xor edx, edx
                    mov ecx, 400
                    div ecx
                    .if edx == 0
                        ; Високосный
                        add totalDays, 29
                    .else
                        ; Не високосный
                        add totalDays, 28
                    .endif
                .else
                    ; Високосный
                    add totalDays, 29
                .endif
            .else
                ; Не високосный
                add totalDays, 28
            .endif
        .endif
        inc m
    .endw
    
    mov eax, day
    dec eax
    add totalDays, eax
    
    mov eax, totalDays
    ret
DateToDays endp
;------------------------------------------------------------------------------
; Функция для обновления даты
; Вход: yearPtr, monthPtr, dayPtr
; Выход: 
;------------------------------------------------------------------------------
UpdateDate proc frame_macro uses cbx csi cdi, yearPtr:cword, monthPtr:cword, dayPtr:cword
    mov csi, [dayPtr]
    add dword ptr [csi], 1
        
    .while dword ptr [csi] > 28
        mov cax, [monthPtr]
        .if dword ptr[cax] == 1 || dword ptr[cax] == 3 || dword ptr[cax] == 5 || dword ptr[cax] == 7 || dword ptr[cax] == 8 || dword ptr[cax] == 10 || dword ptr[cax] == 12
            mov ccx, 31
        .elseif dword ptr[cax] == 4 || dword ptr[cax] == 6 || dword ptr[cax] == 9 || dword ptr[cax] == 11
            mov ccx, 30
        .elseif dword ptr[cax] == 2
            mov cax, [yearPtr]
            mov eax, dword ptr[cax]
            xor cdx, cdx

            push ccx
            mov ccx, 4
            div ccx
            pop ccx
            .if edx == 0
                mov cax, [yearPtr]
                mov eax, dword ptr[cax]
                xor edx, edx

                push ccx
                mov ccx, 100
                div ccx
                pop ccx

                .if cdx == 0
                    mov cax, [yearPtr]
                    mov eax, dword ptr[cax]
                    xor cdx, cdx

                    push ccx
                    mov ccx, 400
                    div ccx
                    pop ccx

                    .if cdx == 0
                        mov ccx, 29  ; Високосный
                    .else
                        mov ccx, 28  ; Не високосный
                    .endif
                .else
                    mov ccx, 29  ; Високосный
                .endif
            .else
                mov ccx, 28  ; Не високосный
            .endif
        .endif
            
        mov cax, [dayPtr]
        mov eax, dword ptr[cax]
        .if eax > ecx
            sub eax, ecx
            mov csi, [dayPtr]
            mov dword ptr[csi], eax
            mov csi, [monthPtr]
            inc dword ptr[csi]
                
            .if dword ptr[csi] > 12
                mov dword ptr[csi], 1
                mov csi, [yearPtr]
                inc dword ptr [csi]
            .endif
        .else
            .break
        .endif
    .endw
    
    ret
UpdateDate endp

;------------------------------------------------------------------------------
; Функция для уменьшения даты (на 1 день назад)
; Вход: yearPtr, monthPtr, dayPtr
;------------------------------------------------------------------------------
DecrementDate proc frame_macro uses cbx csi cdi, yearPtr:cword, monthPtr:cword, dayPtr:cword
    mov csi, [dayPtr]
    sub dword ptr [csi], 1
    
    .if dword ptr [csi] < 1
        ; Переходим к предыдущему месяцу
        mov csi, [monthPtr]
        dec dword ptr [csi]
        
        .if dword ptr [csi] < 1
            ; Переходим к предыдущему году
            mov dword ptr [csi], 12
            mov csi, [yearPtr]
            dec dword ptr [csi]
        .endif
        
        ; Определяем количество дней в новом месяце
        mov csi, [monthPtr]
        mov eax, dword ptr [csi]
        
        .if eax == 1 || eax == 3 || eax == 5 || eax == 7 || eax == 8 || eax == 10 || eax == 12
            mov ecx, 31
        .elseif eax == 4 || eax == 6 || eax == 9 || eax == 11
            mov ecx, 30
        .elseif eax == 2
            ; Проверяем високосный год для февраля
            mov csi, [yearPtr]
            mov eax, dword ptr [csi]
            xor edx, edx
            
            push ccx
            mov ccx, 4
            div ccx
            pop ccx
            
            .if edx == 0
                mov csi, [yearPtr]
                mov eax, dword ptr [csi]
                xor edx, edx
                
                push ccx
                mov ccx, 100
                div ccx
                pop ccx
                
                .if cdx == 0
                    mov csi, [yearPtr]
                    mov eax, dword ptr [csi]
                    xor cdx, cdx
                    
                    push ccx
                    mov ccx, 400
                    div ccx
                    pop ccx
                    
                    .if cdx == 0
                        mov ccx, 29  ; Високосный
                    .else
                        mov ccx, 28  ; Не високосный
                    .endif
                .else
                    mov ccx, 29  ; Високосный
                .endif
            .else
                mov ccx, 28  ; Не високосный
            .endif
        .endif
        
        ; Устанавливаем день как последний день месяца
        mov csi, [dayPtr]
        mov dword ptr [csi], ecx
    .endif
    
    ret
DecrementDate endp

DrawPlanetDate proc frame_macro hdc:HDC, hwnd:HWND, year:dword, month:dword, day:dword
    local bufferText[128]:BYTE
    local rect:RECT
    local clientRect:RECT
    local center:dword
    
    invoke SetTextColor, hdc, 00FFFFFFh
    invoke SetBkMode, hdc, TRANSPARENT

    mov rect.left, 850
    mov rect.top, 10
    mov rect.right, 1000
    mov rect.bottom, 800
    mov cbx, $CTA0("%02d.%02d.%04d")
    invoke wsprintf, addr bufferText, cbx, day, month, year
    invoke DrawTextA, hdc, addr bufferText, -1, addr rect, DT_CENTER or DT_TOP

    ret

DrawPlanetDate endp

end