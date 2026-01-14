.686
.model flat, stdcall
option casemap:none

include c:\masm32\include\kernel32.inc
include c:\masm32\include\msvcrt.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\gdi32.inc

include draw.inc
include Game\Player\player.inc
include Game\Ball\ball.inc
include Game\game.inc
include Game\game_info.inc

include Strings.mac

.code

; ---------------------------------------------------
DrawBitmap proc hdc:HDC, hBitmap:HBITMAP, xStart:DWORD, yStart:DWORD
    local bm:BITMAP
    local hdcMem:HDC
    local hOldBitmap:DWORD
    local bi:BITMAPINFOHEADER
    local pBits:DWORD
    
    invoke CreateCompatibleDC, hdc
    mov hdcMem, eax
    
    invoke SelectObject, hdcMem, hBitmap
    mov hOldBitmap, eax
    
    invoke GetObject, hBitmap, sizeof BITMAP, addr bm
    
    mov bi.biSize, sizeof BITMAPINFOHEADER
    mov eax, bm.bmWidth
    mov bi.biWidth, eax
    mov eax, bm.bmHeight
    neg eax  ; Для top-down DIB
    mov bi.biHeight, eax
    mov bi.biPlanes, 1
    mov bi.biBitCount, 24
    mov bi.biCompression, BI_RGB
    mov bi.biSizeImage, 0
    mov bi.biXPelsPerMeter, 0
    mov bi.biYPelsPerMeter, 0
    mov bi.biClrUsed, 0
    mov bi.biClrImportant, 0

    mov eax, bm.bmWidthBytes
    cdq
    imul eax, bm.bmHeight
    invoke crt_malloc, eax
    mov pBits, eax
    
    .if eax != 0
        ; Получаем биты
        invoke GetDIBits, hdcMem, hBitmap, 0, bm.bmHeight, pBits, addr bi, DIB_RGB_COLORS
        
        ; Рисуем с растяжением
        invoke SetStretchBltMode, hdc, HALFTONE
        invoke StretchDIBits, hdc, xStart, yStart, SCREEN_WIDTH, SCREEN_HEIGHT,
                              0, 0,
                              bm.bmWidth, bm.bmHeight,
                              pBits,
                              addr bi,
                              DIB_RGB_COLORS,
                              SRCCOPY
        invoke crt_free, pBits
    .endif

    invoke SelectObject, hdcMem, hOldBitmap
    invoke DeleteDC, hdcMem
    
    ret
DrawBitmap endp
; ---------------------------------------------------
DrawPlayer proc uses esi ebx, player:dword, hdc:HDC, color:dword
    local brush:HBRUSH
    local pen:HPEN

    local pX:dword
    local pY:dword
    local w:dword
    local h:dword

    mov esi, [player]
    .if esi == 0
        ret
    .endif

    invoke CreateSolidBrush, [color]
    mov [brush], eax

    invoke CreatePen, PS_SOLID, 1, 0
    mov [pen], eax

    invoke GetPlayerX, esi
    mov [pX], eax
    mov ebx, eax
    invoke GetPlayerWidth, esi
    add eax, ebx
    mov [w], eax

    invoke GetPlayerY, esi
    mov [pY], eax
    mov ebx, eax
    invoke GetPlayerHeight, esi
    add eax, ebx
    mov [h], eax

    invoke SelectObject, [hdc], [pen]
    invoke SelectObject, [hdc], [brush]

    invoke Rectangle, [hdc], [pX], [pY], [w], [h]

    invoke DeleteObject, [brush]
    invoke DeleteObject, [pen]
    
    ret
DrawPlayer endp

DrawBall proc uses esi, ball:dword, hdc:HDC, color:dword
    local brush:HBRUSH
    local pen:HPEN

    local _bX:dword
    local bY:dword
    local w:dword
    local h:dword

    mov esi, [ball]
    .if esi == 0
        ret
    .endif

    invoke CreateSolidBrush, [color]
    mov [brush], eax

    invoke CreatePen, PS_SOLID, 2, 0
    mov [pen], eax

    invoke GetBallRadius, esi
    shl eax, 1
    mov ebx, eax
    invoke GetBallX, esi
    mov [_bX], eax
    add eax, ebx
    mov [w], eax            ; w = x + 2*radius = x + diametr

    invoke GetBallRadius, esi
    shl eax, 1
    mov ebx, eax
    invoke GetBallY, esi
    mov [bY], eax
    add eax, ebx
    mov [h], eax            ; h = y + 2*radius = y + diametr

    invoke SelectObject, [hdc], [pen]
    invoke SelectObject, [hdc], [brush]

    invoke Ellipse, [hdc], [_bX], [bY], [w], [h]

    invoke DeleteObject, [brush]
    invoke DeleteObject, [pen]
    ret
DrawBall endp
;----------------------------------------
DrawGame proc game:dword, hdc:HDC, hwnd:HWND
    local rect:RECT
    local offsetX:dword
    local offsetY:dword
    
    invoke GetClientRect, hwnd, addr rect
    
    mov eax, rect.right
    sub eax, SCREEN_WIDTH
    cdq
    mov ecx, 2
    idiv ecx
    mov offsetX, eax
    
    mov eax, rect.bottom
    sub eax, SCREEN_HEIGHT
    cdq
    idiv ecx
    mov offsetY, eax
    
    invoke GetGameBackBufDC, game
    mov esi, eax
    
    invoke StretchBlt, hdc,
                      0, 0,
                      rect.right, rect.bottom,
                      esi,
                      0, 0,
                      SCREEN_WIDTH, SCREEN_HEIGHT,
                      SRCCOPY
    ret
DrawGame endp

DrawScore proc game:dword, hdc:HDC, hwnd:HWND
    local buf[16]:byte
    local rect:RECT
    local clientRect:RECT
    local center:dword
    
    mov esi, game
    .if esi == 0
        ret
    .endif
    
    invoke GetClientRect, hwnd, addr clientRect
    
    invoke SetTextColor, hdc, 0
    invoke SetBkMode, hdc, TRANSPARENT
    
    mov eax, clientRect.right
    shr eax, 1
    mov [center], eax
    
    ; --- Opponent score ---
    invoke GetGamePlayer, esi, 0
    invoke GetPlayerScore, eax
    invoke wsprintf, addr buf, $CTA0("%d"), eax
    
    mov ecx, eax
    sub ecx, 150
    mov rect.left, ecx
    mov rect.top, 20
    mov eax, [center]
    sub eax, 50
    mov rect.right, eax
    mov rect.bottom, 50
    
    invoke DrawTextA, hdc, addr buf, -1, addr rect, DT_RIGHT or DT_TOP or DT_SINGLELINE
    
    ; --- Player score ---
    invoke GetGamePlayer, esi, 1
    invoke GetPlayerScore, eax
    invoke wsprintf, addr buf, $CTA0("%d"), eax

    mov eax, [center]
    add eax, 50
    mov rect.left, eax
    mov rect.top, 20
    mov eax, [center]
    add eax, 150
    mov rect.right, eax
    mov rect.bottom, 50
    
    invoke DrawTextA, hdc, addr buf, -1, addr rect, DT_LEFT or DT_TOP or DT_SINGLELINE
    
    ; -- Spliter ---
    mov eax, [center]
    mov rect.left, eax
    sub rect.left, 2
    mov rect.top, 20
    mov rect.right, eax
    add rect.right, 2
    mov rect.bottom, 50
    invoke DrawTextA, hdc, $CTA0("|"), -1, addr rect, DT_CENTER or DT_TOP or DT_SINGLELINE
    
    ret
DrawScore endp
;----------------------------------------
DrawEnd proc winner:dword, hdc:HDC, hwnd:HWND
    local buf[100]:byte
    local rect:RECT
    
    .if [winner] == 0
        ret
    .endif

    invoke SetTextColor, [hdc], 0
    invoke SetBkMode, [hdc], TRANSPARENT

    invoke GetClientRect, hwnd, addr rect
    
    .if [winner] == 1
        invoke wsprintf, addr buf, CStr("Player win!")
    .else
        invoke wsprintf, addr buf, CStr("Opponent win!")
    .endif
    
    invoke DrawText, [hdc], addr buf, -1, addr rect, 
                     DT_CENTER or DT_VCENTER or DT_SINGLELINE
    ret
DrawEnd endp
;----------------------------------------
InitImages proc uses esi, hIns:HINSTANCE, game:dword
    .if [game] == 0
        ret
    .endif

    invoke LoadImage, [hIns], $CTA0("backgroungSprite"), IMAGE_BITMAP, NULL, NULL, NULL
    invoke SetGameTexture, game, eax
    ret
InitImages endp
;----------------------------------------
InitDoubleBuffering proc hwnd:HWND, game:dword
    local hdc:HDC

    invoke GetDC, hwnd
    mov hdc, eax
    invoke CreateCompatibleDC, hdc
    invoke SetGameBackBufferDC, game, eax

    invoke CreateCompatibleBitmap, hdc, (SCREEN_WIDTH shl 1), (SCREEN_HEIGHT shl 1)
    invoke SetGameBackBuffer, game, eax

    invoke GetGameBackBufDC, game
    mov ebx, eax
    invoke GetGameBackBuf, game

    invoke SelectObject, ebx, eax

    invoke ReleaseDC, hwnd, hdc
    
    invoke SetStretchBltMode, ebx, COLORONCOLOR

    ret
InitDoubleBuffering endp

ClearUpDoubleBuffer proc game:dword
    invoke GetGameBackBufDC, game
    mov ebx, eax
    invoke GetGameBackBuf, game
    .if eax != 0
        invoke DeleteObject, eax
    .endif
    .if ebx != 0
        invoke DeleteDC, ebx
    .endif
    ret
ClearUpDoubleBuffer endp

DrawToBackBuffer proc uses esi, game:dword
    local brush:HBRUSH
    local dc:HDC

    .if [game] == 0
        ret
    .endif

    invoke CreateSolidBrush, 00ffffffh
    mov [brush], eax

    invoke GetGameBackBufDC, game
    mov [dc], eax
    invoke SelectObject, [dc], [brush]

    invoke DeleteObject, [brush]

    invoke GetGameTexture, game
    invoke DrawBitmap, [dc], eax, 0, 0

    invoke GetGamePlayer, game, 1
    invoke DrawPlayer, eax, [dc], 0000ff00h

    invoke GetGamePlayer, game, 0
    invoke DrawPlayer, eax, [dc], 00ff0000h

    invoke GetGameBall, game
    invoke DrawBall, eax, [dc], 000000ffh
    ret
DrawToBackBuffer endp
;----------------------------------------
end