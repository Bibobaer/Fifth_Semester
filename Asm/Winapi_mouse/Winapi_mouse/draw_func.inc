ResizeDrawingBuffer proto :dword, :dword

InitDrawing proc hwnd:HWND
    local rect:RECT
    
    invoke GetClientRect, [hwnd], addr rect
    mov eax, rect.right
    mov [canvasWidth], eax
    mov eax, rect.bottom
    mov [canvasHeight], eax
    
    invoke ResizeDrawingBuffer, [canvasWidth], [canvasHeight]
    ret
InitDrawing endp

CleanupDrawing proc
    .if [hCurrentPen] != 0
        invoke DeleteObject, [hCurrentPen]
        mov [hCurrentPen], 0
    .endif
    .if [hbmMem] != 0
        invoke DeleteObject, [hbmMem]
        mov [hbmMem], 0
    .endif
    .if [hdcMem] != 0
        invoke DeleteDC, [hdcMem]
        mov [hdcMem], 0
    .endif
    ret
CleanupDrawing endp

ResizeDrawingBuffer proc newWidth:dword, newHeight:dword
	local hdc:HDC
    local hdcNew:HDC
    local hbmNew:HBITMAP
    local rect:RECT
    local oldBitmap:dword
    
    mov eax, [newWidth]
    mov [canvasWidth], eax
    mov eax, [newHeight]
    mov [canvasHeight], eax
    
    invoke GetDC, HwndMainWindow
    mov [hdc], eax
    
    invoke CreateCompatibleDC, [hdc]
    mov [hdcNew], eax
    
    invoke CreateCompatibleBitmap, [hdc], [newWidth], [newHeight]
    mov [hbmNew], eax
    
    invoke SelectObject, [hdcNew], [hbmNew]
    mov [oldBitmap], eax
    
    invoke GetStockObject, WHITE_BRUSH
    invoke SetRect, addr rect, 0, 0, [newWidth], [newHeight]
    invoke FillRect, [hdcNew], addr rect, eax
    
    .if [hdcMem] != 0 && [hbmMem] != 0
        invoke BitBlt, [hdcNew], 0, 0, [canvasWidth], [canvasHeight], [hdcMem], 0, 0, SRCCOPY
    .endif
    
    ; Очистка старых ресурсов
    .if [hbmMem] != 0
        invoke DeleteObject, [hbmMem]
    .endif
    .if [hdcMem] != 0
        invoke DeleteDC, [hdcMem]
    .endif
    
    ; Установка новых ресурсов
    mov eax, [hdcNew]
    mov [hdcMem], eax
    mov eax, [hbmNew]
    mov [hbmMem], eax
    
    ; Освобождение контекста устройства
    invoke ReleaseDC, HwndMainWindow, [hdc]
    
    ret
ResizeDrawingBuffer endp

CreateCurrentPen proc 
	.if [hCurrentPen] != 0
		invoke DeleteObject, [hCurrentPen]
	.endif
	invoke CreatePen, [penStyle], [penWidth], [penColor]
	mov [hCurrentPen], eax
	ret
CreateCurrentPen endp

DrawLineTo proc x:dword, y:dword
    local hOldPen:dword
    
    .if [bDrawing] == TRUE && [hdcMem] != 0
        ; Выбор черного пера
        invoke SelectObject, [hdcMem], [hCurrentPen]
        mov [hOldPen], eax
        
        ; Перемещение к начальной точке
        .if [prevX] != 0 || [prevY] != 0
            invoke MoveToEx, [hdcMem], [prevX], [prevY], NULL
        .else
            invoke MoveToEx, [hdcMem], [x], [y], NULL
        .endif
        
        ; Рисование линии
        invoke LineTo, [hdcMem], [x], [y]
        
        ; Восстановление старого пера
        invoke SelectObject, [hdcMem], [hOldPen]
        
        ; Обновление предыдущих координат
        mov eax, [x]
        mov [prevX], eax
        mov eax, [y]
        mov [prevY], eax
        
        ; Принудительная перерисовка окна
        invoke InvalidateRect, HwndMainWindow, NULL, FALSE
        invoke UpdateWindow, HwndMainWindow
    .endif
    ret
DrawLineTo endp

ClearCanvas proc
	local rect:RECT
    local hBrush:HBRUSH
    
    .if [hdcMem] != 0
        invoke SetRect, addr rect, 0, 0, [canvasWidth], [canvasHeight]
        invoke GetStockObject, WHITE_BRUSH
        mov [hBrush], eax
        invoke FillRect, [hdcMem], addr rect, [hBrush]
        
        ; Принудительная перерисовка
        invoke InvalidateRect, HwndMainWindow, NULL, TRUE
    .endif
    
    mov [prevX], 0
    mov [prevY], 0
    ret
ClearCanvas endp

ShowColorDialog proc
    local choosecolor:CHOOSECOLOR
    local custcolors[16]:DWORD
    
    ; Инициализация структуры CHOOSECOLOR
    mov choosecolor.lStructSize, sizeof CHOOSECOLOR
    mov eax, HwndMainWindow
    mov choosecolor.hwndOwner, eax
    mov eax, hIns
    mov choosecolor.hInstance, eax
    mov eax, [penColor]
    mov choosecolor.rgbResult, eax
    lea eax, [custcolors]
    mov choosecolor.lpCustColors, eax
    mov choosecolor.Flags, CC_ANYCOLOR or CC_RGBINIT or CC_FULLOPEN
    mov choosecolor.lCustData, 0
    mov choosecolor.lpfnHook, 0
    mov choosecolor.lpTemplateName, 0
    
    invoke ChooseColor, addr choosecolor
    .if eax != 0
        mov eax, choosecolor.rgbResult
        mov [penColor], eax
        invoke CreateCurrentPen
    .endif
    ret
ShowColorDialog endp

UpdatePenSettings proc
    ; Обновление стиля пера
    invoke SendMessage, [hStyleSolid], BM_GETCHECK, 0, 0
    .if eax == BST_CHECKED
        mov [penStyle], PS_SOLID
    .endif
    
    invoke SendMessage, [hStyleDash], BM_GETCHECK, 0, 0
    .if eax == BST_CHECKED
        mov [penStyle], PS_DASH
    .endif
    
    invoke SendMessage, [hStyleDot], BM_GETCHECK, 0, 0
    .if eax == BST_CHECKED
        mov [penStyle], PS_DOT
    .endif
    
    ; Обновление толщины пера
    invoke SendMessage, [hSliderWidth], TBM_GETPOS, 0, 0
    mov [penWidth], eax
    
    ; Создание нового пера
    invoke CreateCurrentPen
    ret
UpdatePenSettings ENDP