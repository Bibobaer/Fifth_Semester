.686
.model flat, stdcall
option casemap:none

include sound.inc

include c:\masm32\include\winmm.inc
include c:\masm32\include\windows.inc
include c:\masm32\include\user32.inc
includelib winmm.lib

include C:\Users\Vulcanix\Desktop\5 семак\Asm\labs\Tennis\Tennis\Strings.mac

.code
;----------------------------------------
PlayAssetSound proc filename:dword
    local buffer[100]:byte

    invoke wsprintf, addr buffer, CStr("%s%s"), CStr("Assets\"), filename
    invoke PlaySound, addr buffer, 0, SND_ASYNC
    ret
PlayAssetSound endp
;----------------------------------------
end