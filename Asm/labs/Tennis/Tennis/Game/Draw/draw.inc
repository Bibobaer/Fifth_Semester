include c:\masm32\include\windows.inc

; ----------------------------
; Objects dwawing
; ----------------------------

DrawPlayer proto :dword, :HDC, :dword
DrawBall proto :dword, :HDC, :dword
; ----------------------------
; Game drawing
; ----------------------------

DrawGame proto :dword, :HDC, :HWND
DrawScore proto :dword, :HDC, :HWND
DrawEnd proto :dword, :HDC, :HWND
; ----------------------------
; Bitmap Moment
; ----------------------------
InitImages proto :HINSTANCE, :dword

InitDoubleBuffering proto :HWND, :dword
ClearUpDoubleBuffer proto :dword

DrawToBackBuffer proto :dword