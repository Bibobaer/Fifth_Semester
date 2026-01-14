include c:\masm32\include\windows.inc
; ----------------------------
; Construction and Restart
; ----------------------------

CreateGame proto :dword, :byte
ReStartGame proto :dword
; ----------------------------
; Objects movement
; ----------------------------

PlayerMovement proto :dword
OpponentMovement proto :dword
BallMovement proto :dword
; ----------------------------
; Updaters and Checkers
; ----------------------------

UpdateDifficult proto :dword
CheckEndGame proto :dword
IsGameEnd proto :dword
; ----------------------------
; Destruction
; ----------------------------

DeleteGame proto :dword
; ----------------------------
; Game logic
; ----------------------------

IsGameStarted proto :dword
StartGame proto :dword
StopGame proto :dword
; ----------------------------
; Getters and Setters
; ----------------------------

GetGamePlayer proto :dword, :byte
GetGameBall proto :dword

GetGameTexture proto :dword
GetGameBackBufDC proto :dword
GetGameBackBuf proto :dword

SetGameBackBufferDC proto :dword, :HDC
SetGameBackBuffer proto :dword, :dword
SetGameTexture proto :dword, :dword