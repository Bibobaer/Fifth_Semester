.686
.model flat, stdcall
option casemap:none

include c:\masm32\include\winmm.inc
includelib winmm.lib

include logic.inc

include Game\game_info.inc
include Game\game.inc
include Game\Ball\ball.inc
include Game\Player\player.inc 
include Game\Sound\sound.inc

include Strings.mac

.code
;----------------------------------------
CheckCollisionBallPlayer proc uses ebx esi edi, player:dword, ball:dword

    local w_pl:dword
    local w_bl:dword

    mov esi, [player]
    mov edi, [ball]

    invoke GetBallX, [ball]
    mov ecx, eax
    invoke GetBallRadius, [ball]
    mov edx, eax
    shl edx, 1

    mov [w_bl], ecx
    add [w_bl], edx

    invoke GetPlayerWidth, [player]
    mov ebx, eax
    invoke GetPlayerX, [player]

    mov [w_pl], eax
    add [w_pl], ebx

    .if ([w_bl] > eax) && (ecx < [w_pl])
        invoke GetBallY, [ball]
        mov ecx, eax
        invoke GetBallRadius, [ball]
        mov edx, eax
        shl edx, 1

        mov [w_bl], ecx
        add [w_bl], edx

        invoke GetPlayerHeight, [player]
        mov ebx, eax
        invoke GetPlayerY, [player]

        mov [w_pl], eax
        add [w_pl], ebx

        .if ([w_bl] > eax) && (ecx < [w_pl])
            mov eax, 1
            ret
        .endif
    .endif

    xor eax, eax
    ret
CheckCollisionBallPlayer endp
;----------------------------------------
CheckBorder proc uses esi, player:dword
    mov esi, [player]

    .if esi == 0
        ret
    .endif

    invoke GetPlayerY, esi

    cmp eax, 0
    jl CheckBorder_zero_y

    mov ecx, eax
    invoke GetPlayerHeight, esi
    add ecx, eax
    .if ecx > SCREEN_HEIGHT
        mov ecx, SCREEN_HEIGHT
        sub ecx, eax
        invoke SetPlayerY, esi, ecx
    .endif

    jmp CheckBorder_ret

    CheckBorder_zero_y:
    invoke SetPlayerY, esi, 0

    CheckBorder_ret:

    ret
CheckBorder endp
;----------------------------------------
CheckCollision proc game:dword
    
    local pl_res:dword
    local op_res:dword

    mov esi, [game]
    invoke GetGamePlayer, game, 0
    mov ebx, eax
    invoke GetGameBall, game
    mov edi, eax
    invoke GetGamePlayer, game, 1

    invoke CheckCollisionBallPlayer, eax, edi
    mov [pl_res], eax

    invoke CheckCollisionBallPlayer, ebx, edi
    mov [op_res], eax

    .if [pl_res] == 1
        invoke GetBallSpeedX, edi

        and eax, 80000000h
        .if eax != 0
        .else
            invoke GetBallSpeedX, edi
            neg eax
            mov ebx, eax

            and ebx, 80000000h
            .if ebx != 0
                dec eax
            .else
                inc eax
            .endif

            invoke SetBallSpeedX, edi, eax
            invoke PlayAssetSound, CStr("Racket.wav")
        .endif
    .endif

    .if [op_res] == 1
        invoke GetBallSpeedX, edi

        and eax, 80000000h
        .if eax != 0
            invoke GetBallSpeedX, edi
            neg eax
            mov ebx, eax

            and ebx, 80000000h
            .if ebx != 0
                dec eax
            .else
                inc eax
            .endif

            invoke SetBallSpeedX, edi, eax
            invoke PlayAssetSound, CStr("Racket.wav")
        .endif
    .endif
    ret
CheckCollision endp
;----------------------------------------
UpdatePlayerScore proc uses esi, player:dword
    .if [player] == 0
        ret
    .endif

    mov esi, [player]
    invoke GetPlayerScore, esi
    inc eax
    invoke SetPlayerScore, esi, eax
    ret
UpdatePlayerScore endp
;----------------------------------------
CheckScore proc uses esi, player:dword
    mov esi, [player]
    .if esi == 0
        mov eax, 1
        ret
    .endif

    invoke GetPlayerScore, player

    .if eax >= 2
        mov eax, 2
        ret
    .endif

    mov eax, 1
    ret
CheckScore endp
;----------------------------------------
end