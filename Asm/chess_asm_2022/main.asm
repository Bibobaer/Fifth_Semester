;
; ћодуль main.asm.
;
; ƒемонстрирует пример перемещени€ фигур на шахматной доске.
;
; ћаткин »ль€ јлександрович 01.12.2013
;

;----------------------------------------

.686
.model flat, stdcall
option casemap:none

;----------------------------------------

include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\gdi32.inc
include c:\masm32\include\windows.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\msvcrt.inc

include Strings.mac
include chess.inc

;----------------------------------------

FILE struct
    _ptr    DWORD       ?
    _cnt    DWORD       ?
    _base   DWORD       ?
    _flag   DWORD       ?
    _file   DWORD       ?
    _charbuf DWORD      ?
    _bufsiz DWORD       ?
    _tmpfname DWORD     ?
FILE ends

__iob_func proto c

_O_TEXT equ 4000h
_IONBF  equ 0004h

;----------------------------------------

; строкова€ константа с именем окна
AppWindowName equ <"Application">

;----------------------------------------

.data

glChar dd 0

; —труктура дл€ хранени€ информации о фигурах
FigureStruct struct
    type_   FIGTYPE     ?
    color   COLORREF    ?
FigureStruct ends

figures FigureStruct 64 dup(<>)

.data?

hIns HINSTANCE ?
HwndMainWindow HWND ?

.const

.code

;----------------------------------------

RegisterClassMainWindow proto
CreateMainWindow proto
WndProcMain proto hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM
ShowFigures proto hwnd:HWND
InitFigures proto hwnd:HWND

;----------------------------------------

__acrt_iob_func proto c :dword

;
; ‘ункци€ создани€ консоли
;
CreateExtraConsole proc stdcall

    local stdin:DWORD
    local stdout:DWORD
    local stderr:DWORD

    invoke AllocConsole
    .if eax == 0
        ret
    .endif

    invoke __acrt_iob_func, 0
    mov [stdin], eax
    invoke __acrt_iob_func, 1
    mov [stdout], eax
    invoke __acrt_iob_func, 2
    mov [stderr], eax
    
    invoke SetConsoleTitle, $CTA0("Debug console")
    
    invoke GetStdHandle, STD_INPUT_HANDLE
    invoke crt__open_osfhandle, eax, _O_TEXT
    invoke crt__fdopen, eax, $CTA0("r")
    invoke crt_memcpy, [stdin], eax, sizeof (FILE)
    
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke crt__open_osfhandle, eax, _O_TEXT
    invoke crt__fdopen, eax, $CTA0("w")
    invoke crt_memcpy, [stdout], eax, sizeof (FILE)
    
    invoke GetStdHandle, STD_ERROR_HANDLE
    invoke crt__open_osfhandle, eax, _O_TEXT
    invoke crt__fdopen, eax, $CTA0("w")
    invoke crt_memcpy, [stderr], eax, sizeof (FILE)
    
    invoke crt_setvbuf, [stdout], NULL, _IONBF, 0
    .if eax
        xor eax, eax
        ret
    .endif
    
    invoke crt_setvbuf, [stderr], NULL, _IONBF, 0
    .if eax
        xor eax, eax
        ret
    .endif
    
    mov eax, 1
    ret

CreateExtraConsole endp

;--------------------

WinMain proc stdcall hInstance:HINSTANCE, hPrevInstance:HINSTANCE, szCmdLine:PSTR, iCmdShow:DWORD

    local msg: MSG

    mov eax, [hInstance]
    mov [hIns], eax

    invoke CreateExtraConsole
    invoke crt_printf, $CTA0("Hello, World\n")

    invoke CreateMainWindow
    mov [HwndMainWindow], eax
    .if [HwndMainWindow] == 0
        xor eax, eax
        ret
    .endif
	
    .while TRUE
        invoke GetMessage, addr msg, NULL, 0, 0
            .break .if eax == 0

        invoke TranslateMessage, addr msg
        invoke DispatchMessage, addr msg

    .endw

    mov eax, [msg].wParam
    ret

WinMain endp

;--------------------

;
; –егистраци€ класса основного окна приложени€
;
RegisterClassMainWindow proc

    local WndClass:WNDCLASSEX	; структура класса

    ; заполн€ем пол€ структуры
    mov WndClass.cbSize, sizeof (WNDCLASSEX)	; размер структуры класса
    mov WndClass.style, CS_DBLCLKS
    mov WndClass.lpfnWndProc, WndProcMain		; адрес оконной процедуры класса
    mov WndClass.cbClsExtra, 0
    mov WndClass.cbWndExtra, 0
    mov eax, [hIns]
    mov WndClass.hInstance, eax					; описатель приложени€
    invoke LoadIcon, hIns, $CTA0("MainIcon")	; иконка приложени€
    mov WndClass.hIcon, eax
    invoke LoadCursor, NULL, IDC_ARROW
    mov WndClass.hCursor, eax
    invoke GetStockObject, WHITE_BRUSH			; кисть дл€ фона (бела€ как в C версии)
    mov WndClass.hbrBackground, eax
    mov WndClass.lpszMenuName, NULL
    mov WndClass.lpszClassName, $CTA0(AppWindowName)	; им€ класса
    invoke LoadIcon, hIns, $CTA0("MainIcon")
    mov WndClass.hIconSm, eax

    invoke RegisterClassEx, addr WndClass
    ret

RegisterClassMainWindow endp

;--------------------

;
; —оздание основного окна приложени€
;
CreateMainWindow proc

    local hwnd:HWND

    ; регистраци€ класса основного окна
    invoke RegisterClassMainWindow

    ; создание окна зарегестрированного класса
    invoke CreateWindowEx, 
        WS_EX_CONTROLPARENT or WS_EX_APPWINDOW, ; расширенный стиль окна
        $CTA0(AppWindowName),	; им€ зарегестрированного класса окна
        $CTA0("Simple Chess"),	; заголовок окна (как в C версии)
        WS_OVERLAPPEDWINDOW,	; стиль окна
        10,	    ; X-координата левого верхнего угла
        10,	    ; Y-координата левого верхнего угла
        600,    ; ширина окна (как в C версии)
        600,    ; высота окна (как в C версии)
        NULL,   ; описатель родительского окна
        NULL,   ; описатель главного меню (дл€ главного окна)
        [hIns], ; идентификатор приложени€
        NULL
    mov [hwnd], eax
    
    .if [hwnd] == 0
        invoke MessageBox, NULL, $CTA0("ќшибка создани€ основного окна приложени€"), NULL, MB_OK
        xor eax, eax
        ret
    .endif
        
    invoke ShowWindow, hwnd, SW_SHOWNORMAL
    invoke UpdateWindow, hwnd
    
    mov eax, [hwnd]
    ret

CreateMainWindow endp

;--------------------

;
; ‘ункци€ обработки сообщений главного окна приложени€.
;
WndProcMain proc hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM

    local hdc:HDC
    local white_brush:HBRUSH
    local black_brush:HBRUSH
    local green_brush:HBRUSH
    local border_pen:HPEN
    local color_brush:COLORREF
    local ps:PAINTSTRUCT
    local rect:RECT
    local i:dword
    local j:dword

    .if [iMsg] == WM_CREATE
        ; создание окна
        
        invoke RegisterClassChessWindow, [hIns]
        invoke InitFigures, [hwnd]
        
        xor eax, eax
        ret
        
    .elseif [iMsg] == WM_DESTROY
        ; закрытие окна
        
        invoke PostQuitMessage, 0
        xor eax, eax
        ret
        
    .elseif [iMsg] == WM_CHESS_CENTER
        ; wParam - handle фигуры
        ; lParam - указатель на массив из 2 dword [x, y]
    
        mov eax, lParam
        mov ebx, [eax]      ; текущий X
        mov ecx, [eax+4]    ; текущий Y
    
        .if ebx < 100 || ebx >= 500 || ecx < 100 || ecx >= 500
            xor eax, eax
            ret
        .endif
    
        sub ebx, 100
        mov eax, ebx
        mov esi, 50
        xor edx, edx
        div esi
    
        ; ѕровер€ем границы по X
        .if eax >= 8
            mov eax, 7
        .endif
    
        imul eax, 50
        add eax, 125
        mov ebx, eax
     
        sub ecx, 100
        mov eax, ecx
        xor edx, edx
        div esi
    
        ; ѕровер€ем границы по Y
        .if eax >= 8
            mov eax, 7
        .endif
    
        imul eax, 50
        add eax, 125
        mov ecx, eax
    
        mov eax, lParam
        mov [eax], ebx      ; выровненный X
        mov [eax+4], ecx    ; выровненный Y
    
        xor eax, eax
        ret
        
    .elseif [iMsg] == WM_PAINT
        invoke BeginPaint, [hwnd], addr ps
        mov [hdc], eax

        invoke GetStockObject, BLACK_BRUSH
        mov [black_brush], eax

        invoke GetStockObject, WHITE_BRUSH
        mov [white_brush], eax

        invoke CreateSolidBrush, 0000FF00h
        mov [green_brush], eax

        invoke CreatePen, PS_SOLID, 2, 00ff00ffh
        mov [border_pen], eax

        invoke SelectObject, [hdc], [border_pen]
        invoke SelectObject, [hdc], [green_brush]
        invoke Rectangle, [hdc], 80, 80, 520, 520

        invoke DeleteObject, [border_pen]
        invoke DeleteObject, [green_brush]

        invoke CreatePen, PS_SOLID, 2, 0
        mov [border_pen], eax

        invoke SelectObject, [hdc], [border_pen]
        invoke SelectObject, [hdc], [white_brush]
        invoke Rectangle, [hdc], 100, 100, 500, 500

        mov [i], 0
        .while [i] < 8
            mov [j], 0
            .while [j] < 8
                ; left = 100 + j*50
                mov eax, [j]
                imul eax, 50
                add eax, 100
                mov [rect].left, eax

                ; top = 100 + i*50
                mov eax, [i]
                imul eax, 50
                add eax, 100
                mov [rect].top, eax

                ; right = left + 50
                mov eax, [rect].left
                add eax, 50
                mov [rect].right, eax

                ; bottom = top + 50
                mov eax, [rect].top
                add eax, 50
                mov [rect].bottom, eax

                mov eax, [i]
                add eax, [j]
                and eax, 1
                .if eax == 0
                    invoke SelectObject, [hdc], [white_brush]
                .else
                    invoke SelectObject, [hdc], [black_brush]
                .endif

                invoke Rectangle, [hdc], [rect].left, [rect].top, [rect].right, [rect].bottom

                inc [j]
            .endw
            inc [i]
        .endw

        invoke DeleteObject, [border_pen]

        invoke EndPaint, [hwnd], addr ps

        xor eax, eax
        ret

    .endif

    invoke DefWindowProc, hwnd, iMsg, wParam, lParam
    ret

WndProcMain endp

;--------------------

ShowFigures proc hwnd:HWND

    local i:dword
    local j:dword
    local index:dword

    mov [i], 0
    .while [i] < 8
        mov [j], 0
        .while [j] < 8
            mov eax, [i]
            imul eax, 8
            add eax, [j]
            mov [index], eax
            
            mov ecx, [index]
            imul ecx, sizeof(FigureStruct)
            add ecx, offset figures
            .if [ecx].FigureStruct.type_ != FIG_NULL
                mov eax, [j]
                imul eax, 50
                add eax, 100 + 25
                mov edx, eax
                
                mov eax, [i]
                imul eax, 50
                add eax, 100 + 25
                
                invoke CreateChessWindow, [hIns], [hwnd], [ecx].FigureStruct.color, edx, eax, [ecx].FigureStruct.type_
            .endif
            
            inc [j]
        .endw
        inc [i]
    .endw
    
    ret
    
ShowFigures endp

;--------------------

InitFigures proc hwnd:HWND

    local i:dword
    local index:dword
    
    ; »нициализаци€ чЄрных фигур на первой линии
    mov [i], 0
    .while [i] < 8
        mov eax, [i]
        mov [index], eax
        
        mov ecx, [index]
        imul ecx, sizeof(FigureStruct)
        add ecx, offset figures
        mov [ecx].FigureStruct.color, COLOR_BLACK
        
        .if [i] == 0 || [i] == 7
            mov [ecx].FigureStruct.type_, FIG_CASTLE
        .elseif [i] == 1 || [i] == 6
            mov [ecx].FigureStruct.type_, FIG_KNIGHT
        .elseif [i] == 2 || [i] == 5
            mov [ecx].FigureStruct.type_, FIG_BISHOP
        .elseif [i] == 3
            mov [ecx].FigureStruct.type_, FIG_QUEEN
        .elseif [i] == 4
            mov [ecx].FigureStruct.type_, FIG_KING
        .endif
        
        inc [i]
    .endw
    
    ; „Єрные пешки на второй линии
    mov [i], 0
    .while [i] < 8
        mov eax, 8
        add eax, [i]
        mov [index], eax
        
        mov ecx, [index]
        imul ecx, sizeof(FigureStruct)
        add ecx, offset figures
        mov [ecx].FigureStruct.color, COLOR_BLACK
        mov [ecx].FigureStruct.type_, FIG_PAWN
        
        inc [i]
    .endw
    
    ; Ѕелые фигуры на седьмой линии
    mov [i], 0
    .while [i] < 8
        mov eax, 6
        imul eax, 8
        add eax, [i]
        mov [index], eax
        
        mov ecx, [index]
        imul ecx, sizeof(FigureStruct)
        add ecx, offset figures
        mov [ecx].FigureStruct.color, COLOR_WHITE
        mov [ecx].FigureStruct.type_, FIG_PAWN
        
        inc [i]
    .endw
    
    ; Ѕелые фигуры на восьмой линии
    mov [i], 0
    .while [i] < 8
        mov eax, 7
        imul eax, 8
        add eax, [i]
        mov [index], eax
        
        mov ecx, [index]
        imul ecx, sizeof(FigureStruct)
        add ecx, offset figures
        mov [ecx].FigureStruct.color, COLOR_WHITE
        
        .if [i] == 0 || [i] == 7
            mov [ecx].FigureStruct.type_, FIG_CASTLE
        .elseif [i] == 1 || [i] == 6
            mov [ecx].FigureStruct.type_, FIG_KNIGHT
        .elseif [i] == 2 || [i] == 5
            mov [ecx].FigureStruct.type_, FIG_BISHOP
        .elseif [i] == 3
            mov [ecx].FigureStruct.type_, FIG_QUEEN
        .elseif [i] == 4
            mov [ecx].FigureStruct.type_, FIG_KING
        .endif
        
        inc [i]
    .endw
    
    invoke ShowFigures, [hwnd]
    ret
    
InitFigures endp

;--------------------
;--------------------
;--------------------

end