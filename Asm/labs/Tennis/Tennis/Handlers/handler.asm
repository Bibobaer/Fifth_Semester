.686
.model flat, stdcall
option casemap:none

;----------------------------------------
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\gdi32.inc
include c:\masm32\include\windows.inc
include c:\masm32\include\msvcrt.inc

include Strings.mac

include .\Game\game.inc
include .\Game\Draw\draw.inc
include .\Game\Logic\logic.inc
include .\Game\game_info.inc
;----------------------------------------
IDM_START_BTN equ 201
IDM_EXIT_BTN  equ 202
AppWindowName equ <"Tennis">
;----------------------------------------
.data?
    my_game dd ?
.code

RegisterClassMainWindow proc hIns:HINSTANCE
    local WndClass:WNDCLASSEX	; структура класса

    ; заполняем поля структуры
    mov WndClass.cbSize, sizeof (WNDCLASSEX)	; размер структуры класса
    mov WndClass.style, 0
    mov WndClass.lpfnWndProc, WndProcMain		; адрес оконной процедуры класса
    mov WndClass.cbClsExtra, 0
    mov WndClass.cbWndExtra, 0
    mov eax, [hIns]
    mov WndClass.hInstance, eax					; описатель приложения
    invoke LoadIcon, hIns, $CTA0("MainIcon")	; иконка приложения
    mov WndClass.hIcon, eax
    invoke LoadCursor, NULL, IDC_ARROW
    mov WndClass.hCursor, eax
    invoke GetStockObject, WHITE_BRUSH			; кисть для фона
    mov WndClass.hbrBackground, eax
    mov WndClass.lpszMenuName, $CTA0("MainMenu")        ; имя ресарса главного меню
    mov WndClass.lpszClassName, $CTA0(AppWindowName)	; имя класса
    invoke LoadIcon, hIns, $CTA0("MainIcon")
    mov WndClass.hIconSm, eax

    invoke RegisterClassEx, addr WndClass
    ret
RegisterClassMainWindow endp
;--------------------
CreateMainWindow proc hIns:HINSTANCE
    local hwnd:HWND
    local drawed_width:DWORD
    local drawed_height:DWORD

    mov [drawed_width], SCREEN_WIDTH
    add [drawed_width], 2*WINDOWS_OFFSET_X

    mov [drawed_height], SCREEN_HEIGHT
    add [drawed_height], 4*WINDOWS_OFFSET_Y

    ; регистрация класса основного окна
    invoke RegisterClassMainWindow, [hIns]

    ; создание окна зарегестрированного класса
    invoke CreateWindowEx, 
        WS_EX_CONTROLPARENT or WS_EX_APPWINDOW, ; расширенный стиль окна
        $CTA0(AppWindowName),	; имя зарегестрированного класса окна
        $CTA0("Tennis"),	; заголовок окна
        WS_OVERLAPPEDWINDOW,	; стиль окна
        WINDOWS_OFFSET_X,	    ; X-координата левого верхнего угла
        WINDOWS_OFFSET_Y,	    ; Y-координата левого верхнего угла
        drawed_width,    ; ширина окна
        drawed_height,    ; высота окна
        NULL,   ; описатель родительского окна
        NULL,   ; описатель главного меню (для главного окна)
        [hIns], ; идентификатор приложения
        NULL
    mov [hwnd], eax

    .if [hwnd] == 0
        invoke MessageBox, NULL, $CTA0("Ошибка создания основного окна приложения"), NULL, MB_OK
        xor eax, eax
        ret
    .endif

    invoke GetModuleHandle, 0
    invoke CreateWindowEx, 
        WS_EX_TRANSPARENT,
        $CTA0("Button"),
        $CTA0("Start Game"),
        WS_TABSTOP or WS_VISIBLE or WS_CHILD or BS_PUSHBUTTON or BS_CENTER,
        (SCREEN_WIDTH shr 1) - 50,
        (SCREEN_HEIGHT shr 1) - 50 + 25,
        100,
        50,
        hwnd,
        IDM_START_BTN,
        eax,
        NULL

    invoke GetModuleHandle, 0
    invoke CreateWindowEx, 
        WS_EX_TRANSPARENT,
        $CTA0("Button"),
        $CTA0("Exit"),
        WS_TABSTOP or WS_VISIBLE or WS_CHILD or BS_PUSHBUTTON or BS_CENTER,
        (SCREEN_WIDTH shr 1) - 50,
        (SCREEN_HEIGHT shr 1) + 50 + 25,
        100,
        50,
        hwnd,
        IDM_EXIT_BTN,
        eax,
        NULL
        
    invoke ShowWindow, hwnd, SW_SHOWNORMAL
    invoke UpdateWindow, hwnd
    
    mov eax, [hwnd]
    ret
CreateMainWindow endp
;--------------------
SetMenuVisibility proc hwnd:HWND, visible:dword, is_game_paused:byte
    local temp:dword
    .if visible == 0
        mov ebx, SW_HIDE
    .else
        mov ebx, SW_SHOW
    .endif

    invoke GetDlgItem, hwnd, IDM_START_BTN
    .if eax != 0
        .if is_game_paused == 1
            mov temp, eax
            invoke SetWindowText, eax, $CTA0("Continue")
            mov eax, temp
        .endif
        invoke ShowWindow, eax, ebx
    .endif
    invoke GetDlgItem, hwnd, IDM_EXIT_BTN
    .if eax != 0
        invoke ShowWindow, eax, ebx
    .endif

    invoke InvalidateRect, hwnd, NULL, TRUE
    ret
SetMenuVisibility endp
;--------------------
ResizeMenuButtons proc hwnd:HWND
    local screen_rect:RECT
    local new_x:dword
    local new_y:dword

    invoke GetClientRect, hwnd, addr screen_rect

    mov eax, screen_rect.right
    sub eax, screen_rect.top
    shr eax, 1
    sub eax, 50
    mov [new_x], eax

    mov eax, screen_rect.bottom
    sub eax, screen_rect.left
    shr eax, 1
    sub eax, 75
    mov [new_y], eax

    invoke GetDlgItem, hwnd, IDM_START_BTN
    .if eax != 0
        invoke MoveWindow, eax, [new_x], [new_y], 100, 50, TRUE
    .endif

    add [new_y], 100

    invoke GetDlgItem, hwnd, IDM_EXIT_BTN
    .if eax != 0
        invoke MoveWindow, eax, [new_x], [new_y], 100, 50, TRUE
    .endif

    ret
ResizeMenuButtons endp
;--------------------
WndProcMain proc hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM
    local hdc:HDC
    local ps:PAINTSTRUCT

    .if [iMsg] == WM_CREATE
        invoke CreateGame, 1, 0
        mov [my_game], eax

        invoke GetModuleHandle, 0
        invoke InitImages, eax, [my_game]
        invoke InitDoubleBuffering, hwnd, [my_game]

        invoke SetTimer, hwnd, 1, 30, NULL
        xor eax, eax
        ret
    .elseif [iMsg] == WM_DESTROY
        invoke DeleteGame, [my_game]
        invoke PostQuitMessage, 0
        xor eax, eax
        ret

    .elseif [iMsg] == WM_SIZE
        invoke ResizeMenuButtons, hwnd

        invoke ClearUpDoubleBuffer, [my_game]
        invoke InitDoubleBuffering, hwnd, [my_game]
        invoke DrawToBackBuffer, [my_game]

        invoke InvalidateRect, hwnd, NULL, FALSE
        ret
    .elseif [iMsg] == WM_KEYDOWN
        .if wParam == VK_ESCAPE
            invoke IsGameStarted, [my_game]
            invoke SetMenuVisibility, hwnd, 1, al
            invoke StopGame, [my_game]
        .endif
        invoke IsGameEnd, [my_game]

        .if wParam == VK_R && eax != 0
            invoke ReStartGame, [my_game]
            mov [my_game], eax

            invoke GetModuleHandle, 0
            invoke InitImages, eax, [my_game]
            invoke ClearUpDoubleBuffer, [my_game]
            invoke InitDoubleBuffering, hwnd, [my_game]
        .endif
        ret
    .elseif [iMsg] == WM_COMMAND
        .if word ptr[wParam] == IDM_START_BTN
            invoke SetMenuVisibility, hwnd, 0, 0
            invoke StartGame, [my_game]
            ret
        .elseif word ptr[wParam] == IDM_EXIT_BTN
            invoke DeleteGame, [my_game]
            invoke PostQuitMessage, 0
            xor eax, eax
            ret
        .endif
    .elseif [iMsg] == WM_TIMER
        invoke IsGameStarted, [my_game]
        .if eax != 0
            invoke OpponentMovement, [my_game]
            invoke IsGameEnd, [my_game]
            .if eax == 0
                invoke DrawToBackBuffer, [my_game]

                invoke PlayerMovement, [my_game]
                invoke BallMovement, [my_game]
                invoke CheckCollision, [my_game]
                invoke InvalidateRect, hwnd, NULL, FALSE
            .endif
        .endif
        ret
    .elseif [iMsg] == WM_PAINT
        invoke BeginPaint, [hwnd], addr ps
        mov [hdc], eax

        invoke DrawGame, [my_game], [hdc], hwnd
        invoke DrawScore, [my_game], [hdc], hwnd

        invoke IsGameEnd, [my_game]
        .if eax != 0
            invoke DrawEnd, eax, [hdc], hwnd
        .endif

        invoke EndPaint, [hwnd], addr ps
        ret
    .endif
    invoke DefWindowProc, hwnd, iMsg, wParam, lParam
    ret
WndProcMain endp

end