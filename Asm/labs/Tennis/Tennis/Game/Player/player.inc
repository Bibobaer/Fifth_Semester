; ----------------------------
; Construction
; ----------------------------
CreatePlayer proto :dword, :dword, :dword, :dword, :dword
; ----------------------------
; Player movement
; ----------------------------
MovePlayer proto :dword
; ----------------------------
; Destruction
; ----------------------------
DeletePlayer proto :dword
; ----------------------------
; Getters and Setters
; ----------------------------
GetPlayerX proto :dword
GetPlayerY proto :dword
GetPlayerWidth proto :dword
GetPlayerHeight proto :dword
GetPlayerSpeed proto :dword
GetPlayerScore proto :dword

SetPlayerY proto :dword, :dword
SetPlayerSpeed proto :dword, :dword
SetPlayerScore proto :dword, :dword