.686
.model flat, stdcall
option casemap:none

include c:\masm32\include\windows.inc
include c:\masm32\include\msvcrt.inc

include Strings.mac
include Game\game_info.inc
include Game\Sound\sound.inc
include ball.inc

Ball struct
    x dd ?
    y dd ?
    speed_x dd ?
    speed_y dd ?
    radius dd ?
Ball ends

.code

;----------------------------------------
CreateBall proc uses ebx, x:dword, y:dword, speed:dword, radius:dword
    invoke crt_malloc, sizeof Ball

    .if eax == 0
        ret
    .endif

    mov ebx, [x]
    mov [eax].Ball.x, ebx
    mov ebx, [y]
    mov [eax].Ball.y, ebx
    mov ebx, [speed]
    mov [eax].Ball.speed_x, ebx
    mov [eax].Ball.speed_y, ebx
    mov ebx, [radius]
    mov [eax].Ball.radius, ebx

    ret
CreateBall endp

ReSpawnBall proc uses esi edi, ball:dword
    invoke crt_time, 0
    invoke crt_srand, eax

    mov esi, ball

    mov eax, (SCREEN_WIDTH shr 1)
    sub eax, [esi].Ball.radius

    mov [esi].Ball.x, eax

    mov eax, (SCREEN_HEIGHT shr 1)
    sub eax, [esi].Ball.radius
    mov [esi].Ball.y, eax

    mov [esi].Ball.speed_x, BALL_SPEED

    invoke crt_rand
    and eax, 1

    .if eax == 0
        neg [esi].Ball.speed_x
    .endif

    invoke crt_rand
    and eax, 1

    inc [esi].Ball.speed_y

    .if eax == 0
        neg [esi].Ball.speed_y
        sub [esi].Ball.speed_y, 2
    .endif

    ret
ReSpawnBall endp
;----------------------------------------
UpdateBall proc uses esi, ball:dword
    local posX:dword
    local posY:dword

    mov esi, [ball]

    mov eax, [esi].Ball.speed_x
    mov edx, [esi].Ball.speed_y
    add [esi].Ball.x, eax
    add [esi].Ball.y, edx

    mov eax, [esi].Ball.x
    mov [posX], eax

    mov eax, [esi].Ball.radius
    shl eax, 1
    add [posX], eax

    mov eax, [esi].Ball.y
    mov [posY], eax

    mov eax, [esi].Ball.radius
    shl eax, 1
    add [posY], eax

    cmp [esi].Ball.x, 0
    jl Player_scored

    .if [posX] >= SCREEN_WIDTH 
        invoke ReSpawnBall, ball

        mov eax, 1
        ret
    .elseif [posY] >= SCREEN_HEIGHT - WINDOWS_OFFSET_Y
        UpdateBall_zero_y:
        invoke PlayAssetSound, CStr("Beat.wav")
        neg [esi].Ball.speed_y
        xor eax, eax
        ret
    .endif

    jmp CMP_Y

    Player_scored:
    invoke ReSpawnBall, ball
    mov eax, -1
    ret

    CMP_Y:
    cmp [esi].Ball.y, 0
    jl UpdateBall_zero_y
    xor eax, eax
    ret
UpdateBall endp
;----------------------------------------
DeleteBall proc ball:dword
    mov esi, [ball]

    .if esi == 0
        ret
    .endif

    invoke crt_free, [ball]
DeleteBall endp
;----------------------------------------
GetBallX proc uses esi, ball:dword
    .if [ball] == 0
        ret
    .endif

    mov esi, [ball]
    mov eax, [esi].Ball.x
    ret
GetBallX endp

GetBallY proc ball:dword
    .if [ball] == 0
        ret
    .endif

    mov esi, [ball]
    mov eax, [esi].Ball.y
    ret
GetBallY endp

GetBallRadius proc uses esi, ball:dword
    .if [ball] == 0
        ret
    .endif

    mov esi, [ball]
    mov eax, [esi].Ball.radius
    ret
GetBallRadius endp

GetBallSpeedX proc uses esi, ball:dword
    .if [ball] == 0
        ret
    .endif

    mov esi, [ball]
    mov eax, [esi].Ball.speed_x
    ret
GetBallSpeedX endp

GetBallSpeedY proc uses esi, ball:dword
    .if [ball] == 0
        ret
    .endif

    mov esi, [ball]
    mov eax, [esi].Ball.speed_y
    ret
GetBallSpeedY endp
;----------------------------------------
SetBallSpeedX proc uses esi, ball:dword, new_speed:dword
    .if [ball] == 0
        ret
    .endif

    mov esi, [ball]
    mov eax, [new_speed]
    mov [esi].Ball.speed_x, eax
    ret
SetBallSpeedX endp

SetBallSpeedY proc uses esi, ball:dword, new_speed:dword
    .if [ball] == 0
        ret
    .endif

    mov esi, [ball]
    mov eax, [new_speed]
    mov [esi].Ball.speed_y, eax
    ret
SetBallSpeedY endp

end