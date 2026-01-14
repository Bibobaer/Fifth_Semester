.686
.model flat, c
option casemap:none		; чувствительность к регистру символов в идентификаторах

include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc

include Strings.mac

.data
	_my_array dd 1, 2, 3, 4, 5
	_array_size dd 10

	arr3 dd 1, 5, 6, 2, 7, 0, 1, -1
.data?
	arr1 dd ?
	arr2 dd ?

	res_arr dd 10 dup(?)

	buffer dd 10 dup(?)
.const
	string db "Hello World TEst1 Test2 Test3", 0
	_sub db " ", 0
	_sub2 db " __ ", 0

	format db "%s",13, 10, 0
	int_format db "%d ", 0
	endline db 13, 10, 0
.code

include func.inc

Print_array proc c uses eax ebx, _arg_array:DWORD, _arg_size:DWORD
	xor ebx, ebx
	.while ebx != dword ptr[_arg_size]
		push ebx
		shl ebx, 2
		add ebx, dword ptr[_arg_array]
		invoke crt_printf, offset int_format, dword ptr[ebx]
		pop ebx
		inc ebx
	.endw
	invoke crt_printf, offset endline
	ret
Print_array endp

main proc c argc:DWORD, argv:DWORD, envp:DWORD
	
	invoke Split, offset string, offset _sub
	.if eax == 0
		jmp main_ret
	.endif
	mov esi, eax
	mov edi, eax 
	.while dword ptr[esi] != 0
		mov ebx, dword ptr[esi]
		invoke crt_printf, offset format, ebx
		add esi, 4
	.endw

	invoke Join, edi, offset _sub2
	mov ebx, eax
	.if eax == 0
		jmp main_ret
	.endif
	invoke crt_printf, offset format, eax

	invoke crt_free, ebx
	mov ebx, edi
	.while dword ptr[ebx] != 0
		invoke crt_free, dword ptr[ebx]
		add ebx, 4
	.endw

	invoke crt_free, edi
	; ---------------------- Очистка строк --------------------
	invoke Generate_random_array, dword ptr[_array_size]
	.if eax == 0
		jmp main_ret
	.endif
	invoke Print_array, eax, dword ptr[_array_size]
	invoke Reverse_Array, eax, dword ptr[_array_size]
	.if eax == 0
		jmp main_ret
	.endif
	invoke Print_array, eax, dword ptr[_array_size]

	invoke crt_free, eax
	; --------------- Очистка массива ------------------------
	; Вот тут не понятные вещи творятся
	invoke Generate_random_array, dword ptr[_array_size]
	.if eax == 0
		jmp main_ret
	.endif
	mov dword ptr[arr1], eax
	invoke crt__sleep, 1000

	invoke Generate_random_array, dword ptr[_array_size]
	.if eax == 0
		jmp main_ret
	.endif
	mov dword ptr[arr2], eax


	invoke Add_Arrays, offset res_arr, dword ptr[arr1], dword ptr[arr2], dword ptr[_array_size]
	invoke Print_array, eax, dword ptr[_array_size]

	invoke crt_free, dword ptr[arr1]
	invoke crt_free, dword ptr[arr2]

	invoke Processing_Array, offset buffer, offset arr3, 8, offset My_CMP
	.if eax == 0
		jmp main_ret
	.endif
	invoke Print_array, offset buffer, eax

	main_ret:
	mov eax, 0
	ret

main endp
end
