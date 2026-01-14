
ifdef _WIN64

option casemap:none
option frame:auto
option win64:1

;----------------------------------------

include windows.inc
include stdio.inc
include stdlib.inc
include string.inc
include wingdi.inc
include winuser.inc

else

.686
.model flat, stdcall
option casemap:none

include windows.inc
include gdi32.inc
include user32.inc
include kernel32.inc
include msvcrt.inc

endif

ifdef _WIN64

cword typedef qword
cax equ <rax>
cbx equ <rbx>
ccx equ <rcx>
cdx equ <rdx>
csi equ <rsi>
cdi equ <rdi>
csp equ <rsp>
cbp equ <rbp>

frame_macro equ <frame>

c_malloc equ <malloc>
c_free equ <free>
c_sprintf equ <sprintf>
c_sscanf equ <sscanf>

addr_size equ 8
shift_cnt equ 3

else

cword typedef dword
cax equ <eax>
cbx equ <ebx>
ccx equ <ecx>
cdx equ <edx>
csi equ <esi>
cdi equ <edi>
csp equ <esp>
cbp equ <ebp>
frame_macro equ <>

c_malloc equ <crt_malloc>
c_free equ <crt_free>
c_sprintf equ <crt_sprintf>
c_sscanf equ <crt_sscanf>

addr_size equ 4
shift_cnt equ 2

endif