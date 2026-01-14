
; ------------------------ Split ----------------------------------
Split proc c uses ebx, _arg_string:DWORD, _arg_substr:DWORD
	local substr_len:DWORD
	local result:DWORD
	local start:DWORD
	local _end:DWORD
	local _count:DWORD

	.if dword ptr[_arg_string] == 0
		mov eax, 0
		jmp Split_ret
	.elseif dword ptr[_arg_substr] == 0
		mov eax, 0
		jmp Split_ret
	.endif

	invoke crt_strlen, dword ptr[_arg_substr]
	mov [substr_len], eax

	mov dword ptr[_count], 1			; count - с единицы для NULL в конце массива
	mov ebx, dword ptr[_arg_string]	; ptr

	; while (ptr )
	invoke crt_strstr, ebx, dword ptr[_arg_substr]
	.while eax != 0
		inc dword ptr[_count]
		mov ebx, eax
		add ebx, dword ptr[substr_len]

		invoke crt_strstr, ebx, dword ptr[_arg_substr]
	.endw

	mov eax, dword ptr[_count]
	inc eax
	imul eax, 4 ; eax = (count + 1) * sizeof(char*)

	invoke crt_malloc, eax
	.if eax == 0
		jmp Split_ret
	.endif
	mov [result], eax

	mov dword ptr[_count], 0 ; count

	mov eax, dword ptr[_arg_string]
	mov [start], eax ; start = string

	invoke crt_strstr, [start], [_arg_substr]
	mov dword ptr [_end], eax

	.while [_end] != 0
		mov eax, [_end]
		sub eax, [start]
		inc eax
		
		mov ebx, eax

		invoke crt_malloc, eax
		.if eax != 0
			mov edx, dword ptr[result]
			add edx, dword ptr[_count]
			mov dword ptr[edx], eax

			dec ebx
			mov edx, dword ptr[result]
			add edx, dword ptr[_count]
			invoke crt_strncpy, dword ptr[edx], [start], ebx

			mov eax, [result]
			add eax, dword ptr[_count]
			mov eax, [eax]

			add eax, ebx
			mov byte ptr[eax], 0 ; result[count][len] = \0
		.endif

		add dword ptr[_count], 4 ; count++
		mov eax, [_end]

		add eax, dword ptr[substr_len]

		mov dword ptr[start], eax

		invoke crt_strstr, dword ptr[start], dword ptr[_arg_substr]
		mov dword ptr [_end], eax
	.endw

	invoke crt_strlen, [_arg_string]
	add eax, dword ptr[_arg_string]
	sub eax, dword ptr[start]
	inc eax
	mov ebx, eax

	invoke crt_malloc, eax 
	.if eax != 0
		mov edx, dword ptr[result]
		add edx, dword ptr[_count]
		mov dword ptr[edx], eax

		dec ebx
		invoke crt_strncpy, dword ptr[edx], [start], ebx

		mov eax, [result]
		add eax, dword ptr[_count]
		mov eax, [eax]

		add eax, ebx
		mov byte ptr[eax], 0 ; result[count][len] = \0
	.endif
	mov edx, dword ptr[result]
	add edx, dword ptr[_count]
	add edx, 4
	mov dword ptr[edx], 0

	mov eax, dword ptr[result]
	Split_ret:
	ret
Split endp
; -------------------------- Join ---------------------------------
Join proc c uses ebx, _arg_array_str:DWORD, _arg_string:DWORD
	local string_len:DWORD
	local total_len:DWORD
	local _count:DWORD
	local result:DWORD

	.if dword ptr[_arg_array_str] == 0
		mov eax, 0
		jmp Join_ret
	.elseif dword ptr[_arg_string] == 0
		mov eax, 0
		jmp Join_ret
	.endif

	mov dword ptr[_count], 0
	mov dword ptr[total_len], 0

	invoke crt_strlen, dword ptr[_arg_string]
	mov dword ptr[string_len], eax

	mov ebx, dword ptr[_arg_array_str]
	.while dword ptr[ebx] != 0
		invoke crt_strlen, dword ptr[ebx]
		add dword ptr[total_len], eax

		inc dword ptr[_count]
		add ebx, 4
	.endw

	.if dword ptr[_count] > 0
		dec dword ptr[_count]
		mov eax, dword ptr[_count]
		imul eax, dword ptr[string_len]

		add dword ptr[total_len], eax
	.endif

	add dword ptr[total_len], 1
	invoke crt_malloc, dword ptr[total_len]
	.if eax == 0
		jmp Join_ret
	.endif

	mov dword ptr[result], eax
	mov byte ptr[eax], 0

	mov ebx, dword ptr[_arg_array_str]
	.while dword ptr[ebx] != 0 
		invoke crt_strcat, dword ptr[result], dword ptr[ebx]
		add ebx, 4
		.if dword ptr[ebx] != 0 
			invoke crt_strcat, dword ptr[result], dword ptr[_arg_string]
		.endif 
	.endw

	sub dword ptr[total_len], 1
	mov eax, dword ptr[result]
	add eax, dword ptr[total_len]

	mov byte ptr[eax], 0
	sub eax, dword ptr[total_len]

	Join_ret:
	ret
Join endp
; -------------------- Swap and add Arrays ---------------------------
Swap proc c uses ebx, p1:DWORD, p2:DWORD
	.if dword ptr[p1] == 0
		mov eax, 0
		jmp Swap_ret
	.elseif dword ptr[p2] == 0
		mov eax, 0
		jmp Swap_ret
	.endif

	mov eax, [p1]
    mov edx, [p2]
    
    mov ecx, [eax]   ; ecx = *p1
    mov ebx, [edx]   ; ebx = *p2
    
    mov [eax], ebx   ; *p1 = *p2
    mov [edx], ecx   ; *p2 = temp (через ecx)

	mov eax, 1
	Swap_ret:
	ret
Swap endp

Reverse_Array proc c uses ebx esi, _arg_array:DWORD, _arg_size:DWORD
	local iter:DWORD
	local half_size:DWORD

	.if dword ptr[_arg_array] == 0
		mov eax, 0
		jmp Reverse_Array_ret
	.elseif dword ptr[_arg_size] == 0
		mov eax, 0
		jmp Reverse_Array_ret
	.endif

	mov dword ptr[iter], 0
	mov eax, dword ptr[_arg_size]
	shr eax, 1

	mov dword ptr[half_size], eax
	mov ecx, dword ptr[iter]

	shl dword ptr[_arg_size], 2 ; size*4

	.while ecx != dword ptr[half_size]
		mov ebx, dword ptr[_arg_array]
		shl ecx, 2 ; ecx *= 4
		add ebx, ecx ; array[iter]

		mov esi, dword ptr[_arg_array]
		sub esi, ecx
		sub esi, 4 ; array[-iter-1]

		add esi, dword ptr[_arg_size]

		invoke Swap, ebx, esi

		inc dword ptr[iter]
		mov ecx, dword ptr[iter]
	.endw

	mov eax, dword ptr[_arg_array]

	Reverse_Array_ret:
	ret
Reverse_Array endp

Generate_random_array proc c uses ebx, _arg_size:DWORD
	local res:DWORD
	local one_hdr:DWORD

	.if dword ptr[_arg_size] == 0
		mov eax, 0
		jmp Generate_random_array_ret
	.endif

	mov dword ptr[one_hdr], 100

	shl dword ptr[_arg_size], 2 ; size *= 4
	invoke crt_malloc, dword ptr[_arg_size]

	.if eax == 0
		jmp Generate_random_array_ret
	.endif

	mov dword ptr[res], eax

	invoke crt_time, 0
	invoke crt_srand, eax

	xor ebx, ebx
	.while ebx < dword ptr[_arg_size]
		invoke crt_rand
		xor edx, edx
		idiv dword ptr[one_hdr]

		mov eax, edx

		mov edx, dword ptr[res]
		add edx, ebx

		mov dword ptr[edx], eax
		add ebx, 4
	.endw

	mov eax, dword ptr[res]

	Generate_random_array_ret:
	ret
Generate_random_array endp
; -------------------- Add arrays ------------------------------------
Add_Arrays proc c uses ebx, Res_Arr:DWORD, Arr1:DWORD, Arr2:DWORD, arr_size:DWORD
	.if dword ptr[Arr1] == 0
		xor eax, eax
		jmp Add_Arrays_ret
	.elseif dword ptr[Arr2] == 0
		xor eax, eax
		jmp Add_Arrays_ret
	.elseif dword ptr[Res_Arr] == 0
		xor eax, eax
		jmp Add_Arrays_ret
	.elseif dword ptr[arr_size] == 0
		xor eax, eax
		jmp Add_Arrays_ret
	.endif

	xor ebx, ebx
	.while ebx != dword ptr[arr_size]
		shl ebx, 2
		mov edx, dword ptr[Res_Arr]
		add edx, ebx

		mov ecx, dword ptr[Arr1]
		add ecx, ebx
		mov ecx, dword ptr[ecx] ; *(arr1 + i)

		mov eax, dword ptr[Arr2]
		add eax, ebx

		mov eax, dword ptr[eax] ; *(arr2 + i)
		add eax, ecx

		mov dword ptr[edx], eax

		shr ebx, 2
		inc ebx
	.endw

	mov eax ,dword ptr[Res_Arr]

	Add_Arrays_ret:
	ret
Add_Arrays endp
; -------------------- Compare array ------------------------------------
My_CMP proc c variable:DWORD
	.if dword ptr[variable] < 3 && SIGN?
		mov eax, 1
	.else 
		mov eax, 0
	.endif
	ret
My_CMP endp

Processing_Array proc c uses ebx, dst_arr:DWORD, src_arr:DWORD, src_size:DWORD, filter:DWORD
	local temp_dst:DWORD
	local loc_end:DWORD

	.if dword ptr[dst_arr] == 0
		mov eax, 0
		jmp Processing_Array_ret
	.elseif dword ptr[src_arr] == 0
		mov eax, 0
		jmp Processing_Array_ret
	.elseif dword ptr[src_size] == 0
		mov eax, 0
		jmp Processing_Array_ret
	.endif

	mov eax, dword ptr[dst_arr]
	mov dword ptr[temp_dst], eax ; temp = dst

	mov eax, dword ptr[src_arr]
	mov dword ptr[loc_end], eax ; loc_end = src

	mov ebx, dword ptr[loc_end]
	sub ebx, dword ptr[src_arr] ; ebx = loc_end - src
	shl dword ptr[src_size], 2 ; size *= 4
	
	.while ebx != dword ptr[src_size] ; loc_end - src != 4*size - т.к int
		mov eax, dword ptr[loc_end]
		
		push dword ptr[eax] ; eax = *(src[j])
		call dword ptr[filter]
		add esp, 4

		.if eax == 1
			mov eax, dword ptr[dst_arr]
			mov ebx, dword ptr[loc_end]
			mov ebx, dword ptr[ebx] ; ebx = *(src[i])

			mov dword ptr[eax], ebx

			add dword ptr[dst_arr], 4
		.endif	

		add dword ptr[loc_end], 4

		mov ebx, dword ptr[loc_end]
		sub ebx, dword ptr[src_arr]
	.endw

	mov eax, dword ptr[dst_arr]
	sub eax, dword ptr[temp_dst]
	shr eax, 2 ; eax /= 4

	Processing_Array_ret:
	ret
Processing_Array endp