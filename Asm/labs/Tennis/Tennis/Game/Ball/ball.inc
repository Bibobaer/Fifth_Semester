; ----------------------------
; Construction and respawn ball
; ----------------------------
CreateBall proto :dword, :dword, :dword, :dword
ReSpawnBall proto :dword
; ----------------------------
; Ball movement
; ----------------------------
UpdateBall proto :dword
; ----------------------------
; Destruction
; ----------------------------
DeleteBall proto :dword
; ----------------------------
; Getters and Setters
; ----------------------------
GetBallX proto :dword
GetBallY proto :dword
GetBallRadius proto :dword
GetBallSpeedX proto :dword
GetBallSpeedY proto :dword

SetBallSpeedX proto :dword, :dword
SetBallSpeedY proto :dword, :dword