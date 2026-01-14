;
; Модуль main.asm.
;
; Шаблон оконного приложения
;
; Маткин Илья Александрович 16.10.2013
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
include c:\masm32\include\comdlg32.inc
include c:\masm32\include\msvcrt.inc

include Strings.mac

RGB macro r:REQ, g:REQ, b:REQ
	tmp = (r) or ((g) shl 8) or ((b) shl 16)
	%echo @CatStr(%tmp)
    exitm <tmp>
endm

;----------------------------------------

; строковая константа с именем окна
AppWindowName equ <"Application">
ED_1          equ 201     ; идентификатор верхнего текстового поля
ST_1          equ 202     ; идентификатор статического окна
BTN_CLEAR	  equ 203     ; кнопка очистки холста
BTN_COLOR	  equ 204	  ; кнопка смена цвета
STYLE_SOLID	  equ 205	  ; стиль - сплошная
STYLE_DASH	  equ 206	  ; стиль - пунктир
STYLE_DOT	  equ 207	  ; стиль - точки
SLIDER_WIDTH  equ 208	  ; слайдер толщины
;----------------------------------------


.data

	glChar dd 0
	hIns               HINSTANCE 0        ; описатель приложения
	glWindowMainWidth  DWORD     0        ; ширина главного окна
	glWindowMainHeight DWORD     0        ; высота главного окна
	HwndMainWindow     HWND      0        ; описатель главного окна
	HwndEdit1          HWND      0        ; описатель верхнего текстового поля
	HwndStatic1        HWND      0        ; описатель статического окна
	
	; Элементы управления
	hBtnClear          HWND      0        ; кнопка очистки
	hBtnColor          HWND      0        ; кнопка выбора цвета
	hStyleSolid        HWND      0        ; радиокнопка сплошная линия
	hStyleDash         HWND      0        ; радиокнопка пунктир
	hStyleDot          HWND      0        ; радиокнопка точки
	hSliderWidth       HWND      0        ; слайдер толщины
	hStaticWidth	   HWND		 0        ; статическое поле для слайдера
	
	; Переменные для рисования
	bDrawing           BOOL      FALSE    ; флаг рисования
	hdcMem             HDC       0        ; контекст устройства для двойной буферизации
	hbmMem             HBITMAP   0        ; битмап для буфера
	hCurrentPen        HPEN      0        ; текущее перо
	prevX              DWORD     0        ; предыдущая X координата
	prevY              DWORD     0        ; предыдущая Y координата
	
	; Настройки пера
	penColor           DWORD     000000h  ; цвет пера (черный по умолчанию)
	penStyle           DWORD     PS_SOLID ; стиль пера
	penWidth           DWORD     1        ; толщина пера

	; Размеры буфера рисования
	canvasWidth        DWORD     0        ; ширина холста
	canvasHeight       DWORD     0        ; высота холста

	; Строки
	szTitle            db "Paint-like Drawing Application",0
	szError            db "Ошибка создания основного окна приложения",0
	szStatic1          db "Координаты:",0
	szEditClass        db "edit",0
	szStaticClass      db "static",0
	szButtonClass      db "button",0
	szTrackbarClass    db "msctls_trackbar32",0
	szBtnClear         db "Очистить (C)",0
	szBtnColor         db "Цвет",0
	szStyleSolid       db "Сплошная",0
	szStyleDash        db "Пунктир",0
	szStyleDot         db "Точки",0
	szSliderText       db "Толщина:",0
	szFormatXY         db "x = %d",13,10,"y = %d",0
	szFormatClick      db "Нажата левая кнопка мыши в точке (%d,%d)",13,10,0
	szLeftUp           db "Отпущена левая кнопка мыши",13,10,0
	szLeftDblClk       db "Дважды нажата левая кнопка мыши",13,10,0
	szRightDown        db "Нажата правая кнопка мыши",13,10,0
	szRightUp          db "Отпущена правая кнопка мыши",13,10,0
	szRightDblClk      db "Дважды нажата правая кнопка мыши",13,10,0

; Буферы
	buf                db 100 dup(0)

.data?

.const

.code

;----------------------------------------
include draw_func.inc
;----------------------------------------

RegisterClassMainWindow proto;

CreateMainWindow proto;

WndProcMain proto hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM

;----------------------------------------

WinMain proc stdcall hInstance:HINSTANCE, hPrevInstance:HINSTANCE, szCmdLine:PSTR, iCmdShow:DWORD

    local msg: MSG

    mov eax, [hInstance]
    mov [hIns], eax

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
; Регистрация класса основного окна приложения
;
RegisterClassMainWindow proc

    local WndClass:WNDCLASSEX	; структура класса

    ; заполняем поля структуры
    mov WndClass.cbSize, sizeof (WNDCLASSEX)			; размер структуры класса
    mov WndClass.style, CS_DBLCLKS or CS_HREDRAW or CS_VREDRAW
    mov WndClass.lpfnWndProc, offset WndProcMain		; адрес оконной процедуры класса
    mov WndClass.cbClsExtra, 0
    mov WndClass.cbWndExtra, 0
    mov eax, [hIns]
    mov WndClass.hInstance, eax							; описатель приложения
    invoke LoadIcon, hIns, $CTA0("MainIcon")			; иконка приложения
    mov WndClass.hIcon, eax
    invoke LoadCursor, NULL, IDC_ARROW
    mov WndClass.hCursor, eax
    invoke GetStockObject, WHITE_BRUSH					; кисть для фона
    mov WndClass.hbrBackground, eax
    mov WndClass.lpszMenuName, NULL
    mov WndClass.lpszClassName, $CTA0(AppWindowName)	; имя класса
    invoke LoadIcon, hIns, $CTA0("MainIcon")
    mov WndClass.hIconSm, eax

    invoke RegisterClassEx, addr WndClass
    ret

RegisterClassMainWindow endp

;--------------------

;
; Создание основного окна приложения
;
CreateMainWindow proc

    local hwnd:HWND

    ; регистрация класса основного окна
    invoke RegisterClassMainWindow

    ; создание окна зарегестрированного класса
    invoke CreateWindowEx, 
        WS_EX_CONTROLPARENT or WS_EX_APPWINDOW, ; расширенный стиль окна
        $CTA0(AppWindowName),					; имя зарегестрированного класса окна
        $CTA0("Mouse Example"),					; заголовок окна
        WS_OVERLAPPEDWINDOW,					; стиль окна
        10,										; X-координата левого верхнего угла
        10,										; Y-координата левого верхнего угла
        650,									; ширина окна
        650,									; высота окна
        NULL,									; описатель родительского окна
        NULL,									; описатель главного меню (для главного окна)
        [hIns],								; идентификатор приложения
        NULL
    mov [hwnd], eax
    
    .if [hwnd] == 0
        invoke MessageBox, NULL, $CTA0("Ошибка создания основного окна приложения"), NULL, MB_OK
        xor eax, eax
        ret
    .endif
        
    invoke ShowWindow, hwnd, SW_SHOWNORMAL
    invoke UpdateWindow, hwnd
    
    mov eax, [hwnd]
    ret

CreateMainWindow endp

; Создание управляющих элементов главного окна
CreateControlWindowsMain proc hwnd:HWND
    ; Создание текстового поля
    invoke CreateWindowEx, 0, offset szEditClass, NULL, 
						  WS_CHILD or WS_VISIBLE or WS_VSCROLL or WS_HSCROLL or WS_BORDER or ES_LEFT or ES_MULTILINE or ES_AUTOVSCROLL or ES_AUTOHSCROLL or ES_READONLY,
                          10, 400, 
                          780, 290,
                          [hwnd], ED_1, hIns, NULL
    mov HwndEdit1, eax
    
    ; Создание статического окна
    invoke CreateWindowEx, 0, offset szStaticClass, offset szStatic1,
                         WS_CHILD or WS_VISIBLE or WS_BORDER,
                         10, 10, 80, 40,
		                 [hwnd], ST_1, hIns, NULL
    mov HwndStatic1, eax
    
    ; Кнопки очистки
    invoke CreateWindowEx, 0, offset szButtonClass, offset szBtnClear, 
						  WS_CHILD or WS_VISIBLE or BS_PUSHBUTTON, 
						  100, 10, 100, 25, 
						  [hwnd], BTN_CLEAR, hIns, NULL
	mov hBtnClear, eax

	; Кнопка изменения цвета
    invoke CreateWindowEx, 0, offset szButtonClass, offset szBtnColor,
                          WS_CHILD or WS_VISIBLE or BS_PUSHBUTTON,
                          210, 10, 80, 25,
                          hwnd, BTN_COLOR, hIns, NULL
    mov hBtnColor, eax
    
    ; Радиокнопки стилей линий
    invoke CreateWindowEx, 0, offset szButtonClass, offset szStyleSolid,
                          WS_CHILD or WS_VISIBLE or BS_AUTORADIOBUTTON or WS_GROUP,
                          300, 10, 80, 20,
                          hwnd, STYLE_SOLID, hIns, NULL
    mov hStyleSolid, eax
    invoke SendMessage, hStyleSolid, BM_SETCHECK, BST_CHECKED, 0
    
    invoke CreateWindowEx, 0, offset szButtonClass, offset szStyleDash,
                          WS_CHILD or WS_VISIBLE or BS_AUTORADIOBUTTON,
                          300, 35, 80, 20,
                          hwnd, STYLE_DASH, hIns, NULL
    mov hStyleDash, eax
    
    invoke CreateWindowEx, 0, offset szButtonClass, offset szStyleDot,
                          WS_CHILD or WS_VISIBLE or BS_AUTORADIOBUTTON,
                          300, 60, 80, 20,
                          hwnd, STYLE_DOT, hIns, NULL
    mov hStyleDot, eax
    
    ; Статический текст для слайдера
    invoke CreateWindowEx, 0, offset szStaticClass, offset szSliderText,
                          WS_CHILD or WS_VISIBLE,
                          390, 10, 70, 20,
                          hwnd, 0, hIns, NULL
    mov hStaticWidth, eax
    
    ; Слайдер толщины линии
    invoke CreateWindowEx, 0, offset szTrackbarClass, NULL,
                          WS_CHILD or WS_VISIBLE or TBS_AUTOTICKS or TBS_HORZ,
                          465, 10, 120, 30,
                          hwnd, SLIDER_WIDTH, hIns, NULL
    mov hSliderWidth, eax
    
    invoke SendMessage, hSliderWidth, TBM_SETRANGE, TRUE, 1 + (20 shl 16) ; от 1 до 20
    invoke SendMessage, hSliderWidth, TBM_SETPOS, TRUE, 1 ; начальная позиция
    
    ret
CreateControlWindowsMain ENDP

ProcessingSizeEvent proc uses esi edi, lParam:LPARAM
	local newWidth:dword
    local newHeight:dword
    
    mov eax, [lParam]
    and eax, 0ffffh          ; LOWORD(lParam) - ширина
    mov [newWidth], eax
    mov [glWindowMainWidth], eax
    
    mov eax, [lParam]
    shr eax, 16              ; HIWORD(lParam) - высота
    mov [newHeight], eax
    mov [glWindowMainHeight], eax
    
    mov eax, [glWindowMainHeight]
    sub eax, 120
    shr eax, 1                ; (glWindowMainHeight-120)/2
    add eax, 110              ; 110 + (glWindowMainHeight-120)/2
    
    invoke MoveWindow, HwndEdit1,
                       10,
                       eax,
                       [glWindowMainWidth],
                       [glWindowMainHeight],
                       TRUE
    
    ; Пересоздание буфера рисования если размер изменился
    mov esi, [canvasWidth]
    mov edi, [canvasHeight]
    .if [newWidth] > esi || [newHeight] > edi
        invoke ResizeDrawingBuffer, [newWidth], [newHeight]
    .endif
    ret
ProcessingSizeEvent endp

InsertStringTailEdit proc strPtr:DWORD
	local textLength:dword
	
	invoke GetWindowTextLength, HwndEdit1
    mov textLength, eax
    
    ; Установка курсора в конец текста
    invoke SendMessage, HwndEdit1, EM_SETSEL, textLength, textLength
    
    ; Вставка строки
    invoke SendMessage, HwndEdit1, EM_REPLACESEL, 0, strPtr
    ret
InsertStringTailEdit endp
;--------------------

;
; Функция обработки сообщений главного окна приложения.
; Вызывается системой при поступлении сообщения для главного окна
; с соответствующими параметрами.
;
; Агрументы:
;
; hwnd      описатель окна, получившего сообщение
; iMsg      идентификатор (номер) сообщения
; wParam    параметр сообщения
; lParam    параметр сообщения
;
WndProcMain proc hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM

    local hdc:HDC
    local ps:PAINTSTRUCT
    
    local xPos:dword
    local yPos:dword

    .if [iMsg] == WM_CREATE
        ; создание окна
        invoke CreateControlWindowsMain, hwnd
        invoke InitDrawing, hwnd
        invoke ClearCanvas
        xor eax, eax
        ret
    .elseif [iMsg] == WM_SIZE
		invoke ProcessingSizeEvent, [lParam] 
		ret
    .elseif [iMsg] == WM_DESTROY
        ; закрытие окна
        invoke CleanupDrawing
        invoke PostQuitMessage, 0
        xor eax, eax
        ret
    .elseif [iMsg] == WM_PAINT
        ; перерисовка окна
        
        ; получаем контекст устройства
        invoke BeginPaint, HwndMainWindow, addr ps
        mov [hdc], eax
        
        .if [hdcMem] != 0
            invoke BitBlt, [hdc], 
                          0, 0,
                          [canvasWidth], [canvasHeight],
                          [hdcMem], 
                          0, 0,
                          SRCCOPY
        .endif
        
        ; завершение перерисовки
        invoke EndPaint, [hwnd], addr ps
        
        xor eax, eax
        ret
    .elseif [iMsg] == WM_COMMAND
		mov eax, [wParam]
        and eax, 0ffffh
        
        .if eax == BTN_CLEAR
            invoke ClearCanvas
        .elseif eax == BTN_COLOR
            invoke ShowColorDialog
        .elseif eax == STYLE_SOLID || eax == STYLE_DASH || eax == STYLE_DOT
            invoke UpdatePenSettings
        .endif
        ret
    .elseif [iMsg] == WM_HSCROLL
        mov eax, [lParam]
        .if eax == [hSliderWidth]
            invoke UpdatePenSettings
        .endif
        ret
        
    .elseif iMsg == WM_KEYDOWN
        .if [wParam] == VK_C
            invoke ClearCanvas
        .endif
        ret
    .elseif [iMsg] == WM_MOUSEMOVE
		mov eax, [lParam]
        and eax, 0ffffh
        mov [xPos], eax
        
        mov eax, [lParam]
        shr eax, 16
        mov [yPos], eax
        
        ; Форматирование строки
        invoke crt_sprintf, addr buf, addr szFormatXY, [xPos], [yPos]
        
        ; Установка текста в статическое окно
        invoke SetWindowText, [HwndStatic1], addr buf
        
        .if wParam & MK_LBUTTON
            invoke DrawLineTo, [xPos], [yPos]
        .endif
        
		ret
		
	.elseif [iMsg] == WM_LBUTTONDOWN
		mov eax, [lParam]
        and eax, 0ffffh
        mov [xPos], eax
        mov [prevX], eax
        
        mov eax, [lParam]
        shr eax, 16
        mov [yPos], eax
        mov [prevY], eax
        
        mov [bDrawing], TRUE
        
        invoke crt_sprintf, addr buf, addr szFormatClick, [xPos], [yPos]
        invoke InsertStringTailEdit, addr buf
        
        .if wParam & MK_LBUTTON
            invoke DrawLineTo, [xPos], [yPos]
        .endif
        
        ret
        
     .elseif [iMsg] == WM_LBUTTONUP
		mov [bDrawing], FALSE
		mov [prevX], 0
		mov [prevY], 0
        invoke InsertStringTailEdit, addr szLeftUp
        ret
    .elseif [iMsg] == WM_LBUTTONDBLCLK
        invoke InsertStringTailEdit, addr szLeftDblClk
        ret
    .elseif [iMsg] == WM_RBUTTONDOWN
        invoke InsertStringTailEdit, addr szRightDown
        ret
    .elseif [iMsg] == WM_RBUTTONUP
        invoke InsertStringTailEdit, addr szRightUp
        ret
    .elseif [iMsg] == WM_RBUTTONDBLCLK
        invoke InsertStringTailEdit, addr szRightDblClk  
		ret
    .endif
    
    ; Необработанные сообщения направляются в функцию
    ; обработки по умолчанию.
    invoke DefWindowProc, hwnd, iMsg, wParam, lParam
    ret

WndProcMain endp

;--------------------

;--------------------


end
