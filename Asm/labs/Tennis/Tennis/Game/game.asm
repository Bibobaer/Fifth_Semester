.686
.model flat, stdcall
option casemap:none

include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\winmm.inc
include c:\masm32\include\gdi32.inc

includelib winmm.lib

include game.inc
include game_info.inc

include Ball\ball.inc
include Player\player.inc
include Logic\logic.inc
include Draw\draw.inc
include Sound\sound.inc

include Strings.mac

Game struct
    player dd ? 
    opponent dd ?  
    ball dd ? 

    difficult dd ?
    is_game_played db ?

    back_buf_DC HDC ?
    back_buf HBITMAP ?

    texture HBITMAP ?
Game ends

.code
;----------------------------------------
CreateGame proc diff:dword, start:byte
    local res:dword

    invoke crt_malloc, sizeof Game
    .if eax == 0
        ret
    .endif

    mov [res], eax
    mov ebx, SCREEN_WIDTH - 10
    sub ebx, PLAYER_WIDTH
    invoke CreatePlayer, ebx, (SCREEN_HEIGHT shr 1) - 50, PLAYER_SPEED, PLAYER_WIDTH, PLAYER_HEIGHT
    .if eax == 0
        invoke crt_free, [res]
        ret
    .endif

    mov esi, [res]
    mov [esi].Game.player, eax

    invoke CreatePlayer, 10, (SCREEN_HEIGHT shr 1) - 50, PLAYER_SPEED,  PLAYER_WIDTH, PLAYER_HEIGHT
    .if eax == 0
        invoke DeletePlayer, [esi].Game.player
        invoke crt_free, [res]
        ret
    .endif

    mov [esi].Game.opponent, eax

    invoke CreateBall, (SCREEN_WIDTH shr 1), (SCREEN_HEIGHT shr 1), BALL_SPEED, BALL_RADIUS
    .if eax == 0
        invoke DeletePlayer, [esi].Game.player
        invoke DeletePlayer, [esi].Game.opponent
        invoke crt_free, [res]
        ret
    .endif

    mov [esi].Game.ball, eax
    invoke ReSpawnBall, [esi].Game.ball

    mov eax, [diff]
    mov [esi].Game.difficult, eax
    mov al, [start]
    mov [esi].Game.is_game_played, al

    mov eax, [res]
    ret
CreateGame endp

ReStartGame proc game:dword
    mov esi, [game]
    .if esi == 0
        ret
    .endif

    invoke DeleteGame, esi
    invoke CreateGame, 1, 1
    ret
ReStartGame endp
;----------------------------------------
PlayerMovement proc game:dword
    mov esi, [game]

    .if esi == 0
        ret
    .endif

    invoke MovePlayer, [esi].Game.player

    ret
PlayerMovement endp

OpponentMovement proc game:dword
    mov esi, [game]

    .if esi == 0
        ret
    .endif

    mov edi, [esi].Game.opponent

    .if [esi].Game.difficult == 1

        invoke GetTickCount
        
        xor edx, edx
        mov ecx, 100
        div ecx

        invoke GetPlayerSpeed, edi
        mov ebx, eax
        .if edx == 0
            neg ebx
            invoke SetPlayerSpeed, edi, ebx
        .endif

        invoke GetPlayerY, edi
        add ebx, eax
        invoke SetPlayerY, edi, ebx
    .elseif [esi].Game.difficult >= 2
        invoke GetPlayerHeight, edi
        mov edx, eax
        shr edx, 1

        invoke GetPlayerY, edi
        add edx, eax

        invoke GetPlayerSpeed, edi
        mov ebx, eax

        and ebx, 80000000h
        .if ebx != 0
            neg eax
        .endif

        mov ebx, eax

        invoke GetPlayerY, edi
        mov ecx, eax

        invoke GetBallY, [esi].Game.ball

        .if edx > eax
            sub ecx, ebx
        .endif
        .if edx <= eax
            add ecx, ebx
        .endif
        invoke SetPlayerY, edi, ecx
    .endif

    invoke CheckBorder, edi

    ret
OpponentMovement endp

BallMovement proc game:dword
    local temp:dword
    mov esi, [game]

    .if esi == 0
        ret
    .endif

    invoke UpdateBall, [esi].Game.ball

    .if eax == 1 || eax == -1
        mov temp, eax
        invoke PlayAssetSound, CStr("Kaboom.wav"); CStr("Claps.wav")
        mov eax, temp
    .endif

    .if eax == 1
        invoke UpdatePlayerScore, [esi].Game.opponent
        invoke UpdateDifficult, [game]
    .elseif eax == -1
        invoke UpdatePlayerScore, [esi].Game.player
        invoke UpdateDifficult, [game]
    .endif

    ret
BallMovement endp
;----------------------------------------
UpdateDifficult proc game:dword
    mov esi, [game]
    .if esi == 0
        ret
    .endif

    mov edi, [esi].Game.player
    invoke CheckScore, edi

    .if eax == 0
        ret
    .endif

    mov [esi].Game.difficult, eax
        
    invoke GetPlayerScore, [esi].Game.opponent
    mov ebx, eax
    invoke GetPlayerScore, [esi].Game.player
    mov edi, eax

    .if edi >= ebx
        ret
    .endif
    invoke CheckScore, [esi].Game.opponent

    .if eax == 1
        ret
    .endif

    dec eax
    mov [esi].Game.difficult, eax

    ret
UpdateDifficult endp

CheckEndGame proc game:dword
    mov esi, [game]

    .if esi == 0
        xor eax, eax
        ret
    .endif

    invoke GetPlayerScore, [esi].Game.player
    .if eax >= 7
        mov eax, 1
        ret
    .endif

    invoke GetPlayerScore, [esi].Game.opponent
    .if eax >= 7
        mov eax, -1
        ret
    .endif

    xor eax, eax
    ret
CheckEndGame endp

IsGameEnd proc game:dword
    mov esi, [game]

    .if esi == 0
        xor eax, eax
        ret
    .endif

    invoke CheckEndGame, [game]
    mov esi, [game]
    .if eax == 1 || eax == -1
        mov [esi].Game.is_game_played, 0
        ret
    .endif
    xor eax, eax
    ret
IsGameEnd endp
;----------------------------------------
DeleteGame proc game:dword
    local pl_t:dword
    local op_t:dword
    local bl_t:dword

    mov esi, [game]

    .if esi == 0
        ret
    .endif

    mov eax, [esi].Game.player
    mov pl_t, eax
    mov eax, [esi].Game.opponent
    mov op_t, eax
    mov eax, [esi].Game.ball
    mov bl_t, eax

    invoke DeletePlayer, pl_t
    invoke DeletePlayer, op_t
    ;invoke DeleteBall, bl_t

    ;invoke crt_free, esi
    ret
DeleteGame endp
;----------------------------------------
IsGameStarted proc uses esi, game:dword
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    xor eax, eax
    mov al, [esi].Game.is_game_played
    ret
IsGameStarted endp
;----------------------------------------
StartGame proc uses esi, game:dword
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    mov [esi].Game.is_game_played, 1
    ret
StartGame endp

StopGame proc uses esi, game:dword
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    mov [esi].Game.is_game_played, 0
    ret
StopGame endp
;----------------------------------------
GetGamePlayer proc uses esi, game:dword, player_type:byte
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    .if player_type == 1
        mov eax, [esi].Game.player
    .else
        mov eax, [esi].Game.opponent
    .endif
    ret
GetGamePlayer endp

GetGameBall proc game:dword
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    mov eax, [esi].Game.ball
    ret
GetGameBall endp

GetGameTexture proc uses esi, game:dword
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    mov eax, [esi].Game.texture
    ret
GetGameTexture endp

GetGameBackBufDC proc uses esi, game:dword
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    mov eax, [esi].Game.back_buf_DC
    ret
GetGameBackBufDC endp

GetGameBackBuf proc uses esi, game:dword
    .if [game] == 0
        xor eax, eax
        ret
    .endif

    mov esi, [game]
    mov eax, [esi].Game.back_buf
    ret
GetGameBackBuf endp
;----------------------------------------
SetGameBackBufferDC proc uses esi, game:dword, new_buf_hdc:HDC
    .if [game] == 0
        ret
    .endif

    mov esi, [game]
    mov eax, [new_buf_hdc]
    mov [esi].Game.back_buf_DC, eax
    ret
SetGameBackBufferDC endp

SetGameBackBuffer proc uses esi, game:dword, new_buf:dword
    .if [game] == 0
        ret
    .endif

    mov esi, [game]
    mov eax, [new_buf]
    mov [esi].Game.back_buf, eax
    ret
SetGameBackBuffer endp

SetGameTexture proc uses esi, game:dword, new_texture:dword
    .if [game] == 0
        ret
    .endif

    mov esi, [game]
    mov eax, [new_texture]
    mov [esi].Game.texture, eax
    ret
SetGameTexture endp
;----------------------------------------
end