.686
.model flat, stdcall
option casemap:none

include c:\masm32\include\windows.inc
include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\gdi32.inc

include player.inc

include Game\game_info.inc
include Game\Logic\logic.inc

Player struct
    x dd ?
    y dd ?

    pl_width dd ?
    pl_height dd ?

    speed dd ?
    score dd ?
Player ends

.code
;----------------------------------------
CreatePlayer proc uses ebx, x:dword, y:dword, speed:dword, pl_width:dword, pl_height:dword
    invoke crt_malloc, sizeof Player

    .if eax == 0
        ret
    .endif

    mov ebx, [x]
    mov [eax].Player.x, ebx
    mov ebx, [y]
    mov [eax].Player.y, ebx
    mov ebx, [speed]
    mov [eax].Player.speed, ebx
    mov ebx, [pl_width]
    mov [eax].Player.pl_width, ebx
    mov ebx, [pl_height]
    mov [eax].Player.pl_height, ebx
    mov [eax].Player.score, 0

    ret
CreatePlayer endp
;----------------------------------------
MovePlayer proc player:dword
    local up:dword
    local down:dword

    mov esi, [player]
    .if esi == 0
        ret
    .endif

    invoke GetAsyncKeyState, VK_W
    mov [up], eax
    invoke GetAsyncKeyState, VK_S
    mov [down], eax

    mov eax, [esi].Player.speed
    .if [up] & 8000h
        sub [esi].Player.y, eax
    .elseif [down] & 8000h
        add [esi].Player.y, eax
    .endif

    invoke CheckBorder, esi
    ret
MovePlayer endp
;----------------------------------------
DeletePlayer proc player:dword
    mov esi, [player]

    .if esi == 0
        ret
    .endif

    invoke crt_free, [player]
    
    ret
DeletePlayer endp
; -------------------------------------------------------------------

GetPlayerX proc uses esi, player:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [esi].Player.x
    ret
GetPlayerX endp

GetPlayerY proc uses esi, player:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [esi].Player.y
    ret
GetPlayerY endp

GetPlayerWidth proc uses esi, player:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [esi].Player.pl_width
    ret
GetPlayerWidth endp

GetPlayerHeight proc uses esi, player:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [esi].Player.pl_height
    ret
GetPlayerHeight endp

GetPlayerSpeed proc uses esi, player:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [esi].Player.speed
    ret
GetPlayerSpeed endp

GetPlayerScore proc uses esi, player:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [esi].Player.score
    ret
GetPlayerScore endp
; -------------------------------------------------------------------
SetPlayerY proc uses esi, player:dword, new_y:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [new_y]
    mov [esi].Player.y, eax
    ret
SetPlayerY endp

SetPlayerSpeed proc uses esi, player:dword, new_speed:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [new_speed]
    mov [esi].Player.speed, eax
    ret
SetPlayerSpeed endp

SetPlayerScore proc uses esi, player:dword, new_score:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    mov eax, [new_score]
    mov [esi].Player.score, eax
    ret
SetPlayerScore endp

end