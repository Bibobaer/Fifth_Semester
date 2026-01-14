.686
.model flat, stdcall
option casemap:none
;----------------------------------------

include c:\masm32\include\user32.inc
include c:\masm32\include\windows.inc

include Strings.mac
include .\Handlers\handler.inc

.code
;----------------------------------------
WinMain proc stdcall hInstance:HINSTANCE, hPrevInstance:HINSTANCE, szCmdLine:PSTR, iCmdShow:DWORD
    local msg: MSG
    local hAccel:HACCEL
    local hWndMain:HWND

    invoke CreateMainWindow, [hInstance]

    mov [hWndMain], eax
    .if [hWndMain] == 0
        ret
    .endif

    .while TRUE
        invoke GetMessage, addr msg, NULL, 0, 0
            .break .if eax == 0

        invoke TranslateAccelerator, [hWndMain], [hAccel], addr msg
        .if !eax
            invoke TranslateMessage, addr msg
            invoke DispatchMessage, addr msg
        .endif

    .endw

    mov eax, [msg].wParam
    ret
WinMain endp
;----------------------------------------
end
