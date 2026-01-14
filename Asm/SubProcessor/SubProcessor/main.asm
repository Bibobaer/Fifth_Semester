.686
.model flat, stdcall
option casemap:none		; чувствительность к регистру символов в идентификаторах

include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc

include Strings.mac

.data
	number dq 12.0921
.data?

.const
	float_format db "%f", 13, 10, 0
.code

include integ.inc
include print.inc

main proc c argc:DWORD, argv:DWORD, envp:DWORD	
	
	local a:dword
	local b:dword

	local res:qword

	mov dword ptr [a], 0
	mov dword ptr [b], 50

	push offset x2
	push [b]
	push [a]
	call IntSum
	fstp res

	invoke crt_printf, addr float_format, res

	invoke print_float, number, 3

	mov eax, 0
	ret
main endp
end
