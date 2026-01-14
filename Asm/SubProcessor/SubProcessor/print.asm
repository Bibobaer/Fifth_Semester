.686
.model flat, stdcall
option casemap:none

include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc
include c:\masm32\include\msvcrt.inc
include print.inc

include Strings.mac

.data

.code 

print_float proc value:qword, precision:dword
    local whole_part:qword
    local frac_part:qword
    local temp_value:qword
    local multi:dword

    fld qword ptr[value]
    fst qword ptr[temp_value]

    fisttp qword ptr[whole_part]
    mov eax, dword ptr[whole_part]
    invoke crt_printf, $CTA0("%d"), eax

    fld qword ptr[value]
    fild qword ptr[whole_part]
    fsubp st(1), st(0)

    mov ebx, [precision]
    mov eax, 1

    .while ebx > 0
        imul eax, 10
        dec ebx
    .endw

    mov [multi], eax
    fild [multi]
    fmulp st(1), st(0)

    fisttp qword ptr[frac_part]
    mov eax, dword ptr[frac_part]

    invoke crt_printf, $CTA0(".%d"), eax

    ret
print_float endp

end