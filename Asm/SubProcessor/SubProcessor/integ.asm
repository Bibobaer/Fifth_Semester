.686
.model flat, stdcall
option casemap:none

include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\msvcrt.inc
include integ.inc

include Strings.mac

.data
    step_size dq 0.0001

.code 
    
x2 proc 
    fmul st(0), st(0); st(0) *= st(0)
    ret
x2 endp

IntSum proc a:dword, b:dword, func:ptr type_func
    local sum:qword
    local iter:dword
    
    local end_cycle:dword

    finit

    fldz            ; st(0) = 0
    fstp sum        ; sum = 0

    fild b          ; st(0) = b
    fisub a         ; st(0) = b - a
    fdiv step_size  ; st(0) = (b - a) * 1000
    fistp end_cycle

    mov dword ptr[iter], 0
    mov ecx, dword ptr[iter]

    .while ecx < end_cycle
        fild dword ptr[iter]
        fmul step_size
        fiadd a                 ; st(0) = a + i*0.001       

        invoke [func]           ; st(0) = func(st(0))

        fmul step_size          ; st(0) = st(0) * 0.001

        fld sum                 ; st(0) = sum, st(1) = func(temp_x) * 0.001
        fadd                    ; st(0) += st(1)

        fstp sum                ; sum += func(temp_x) * 0.001

        mov dword ptr[iter], ecx

        inc dword ptr [iter]
        mov ecx, dword ptr [iter]
    .endw


    fld sum ; st(0) = sum
    ret
IntSum endp

end