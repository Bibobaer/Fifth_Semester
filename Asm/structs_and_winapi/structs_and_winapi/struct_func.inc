; ----------------------------------------------------
struct_strstr proc uses ebx esi edi, arg_string:ptr my_string, arg_substring:ptr my_string
	local counter:dword

	local i:dword
	local j:dword

	mov esi, [arg_string]
    mov edi, [arg_substring]

    .if esi == 0 || edi == 0
        mov eax, 0
        ret
    .endif
    
    .if [esi].my_string.str_ptr == 0 || [edi].my_string.str_ptr == 0
        mov eax, 0
        ret
    .endif
    
    mov ecx, [esi].my_string.str_len
    mov edx, [edi].my_string.str_len
    
    .if edx > ecx
        mov eax, 0
        ret
	.elseif edx == 0
		mov eax, [esi].my_string.str_ptr
		ret
    .endif

	mov dword ptr[counter], 0
	mov esi, [esi].my_string.str_len
	mov edi, [edi].my_string.str_len

	mov dword ptr[i], 0
	.while dword ptr[i] < esi
		mov dword ptr[j], 0
		.while dword ptr[j] < edi
			mov ebx, [arg_string]
			mov ebx, [ebx].my_string.str_ptr
			add ebx, dword ptr[j]
			add ebx, dword ptr[i]

			mov al, byte ptr[ebx]

			mov ebx, [arg_substring]
			mov ebx, [ebx].my_string.str_ptr
			add ebx, dword ptr[j]

			.if al != byte ptr[ebx]
				.break
			.endif

			inc dword ptr[counter]
			inc dword ptr[j]
		.endw

		mov eax, [arg_substring]
		mov eax, [eax].my_string.str_len
		
		.if dword ptr[counter] == eax
			mov eax, [arg_string]
			mov eax, [eax].my_string.str_ptr
			add eax, dword ptr[i]
			ret
		.endif
		
		mov dword ptr[counter], 0

		inc dword ptr[i]
	.endw

	mov eax, [arg_string]
	mov eax, [eax].my_string.str_ptr
	ret
struct_strstr endp
; ----------------------------------------------------
struct_strcpy proc uses ebx esi edi, arg_dst_str:ptr my_string, arg_src_str:ptr my_string
	mov esi, [arg_src_str]
    mov edi, [arg_dst_str]
    .if esi == 0 || edi == 0
        mov eax, 0
        ret
    .endif

    .if [esi].my_string.str_ptr == 0 || [edi].my_string.str_ptr == 0
        mov eax, 0
        ret 
    .endif

	.if [esi].my_string.str_len == 0
		mov [edi].my_string.str_len, 0
		mov eax, [edi].my_string.str_ptr
		ret
	.endif

	mov ebx, [esi].my_string.str_len
	mov esi, [esi].my_string.str_ptr
	mov edi, [edi].my_string.str_ptr

	.while ebx > 0
		mov al, byte ptr[esi]
		mov byte ptr[edi], al 

		inc esi
		inc edi

		dec ebx
	.endw

	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]

	mov ecx, [esi].my_string.str_len
	mov [edi].my_string.str_len, ecx

	mov esi, [edi].my_string.str_ptr
	add esi, [edi].my_string.str_len

	mov byte ptr[esi], 0

	mov eax, [edi].my_string.str_ptr
	ret
struct_strcpy endp
; ----------------------------------------------------
struct_strlen proc, arg_string:ptr my_string
	mov eax, [arg_string]
	.if eax == 0
		mov eax, 0
		ret
	.endif

	.if [eax].my_string.str_ptr == 0
		mov eax, 0
		ret
	.endif

	mov eax, [eax].my_string.str_len
	ret
struct_strlen endp
; ----------------------------------------------------
struct_strcat proc uses ebx esi edi, arg_dst_str:ptr my_string, arg_src_str:ptr my_string
	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]

	.if esi == 0 || edi == 0
		mov eax, 0
		ret
	.endif

	.if [esi].my_string.str_ptr == 0 || [edi].my_string.str_ptr == 0
		mov eax, 0
		ret
	.endif

	invoke struct_strlen, [arg_dst_str]
	add [edi].my_string.str_ptr, eax
	mov ebx, eax

	invoke struct_strcpy, edi, esi

	sub [edi].my_string.str_ptr, ebx
	sub eax, [edi].my_string.str_ptr
	ret
struct_strcat endp
; ----------------------------------------------------
struct_strcmp proc uses ebx esi edi, arg_str1:ptr my_string,  arg_str2:ptr my_string
	mov esi, [arg_str1]
	mov edi, [arg_str2]

	.if esi == 0 || edi == 0
		mov eax, 0
		ret
	.endif

	.if [esi].my_string.str_ptr == 0 || [edi].my_string.str_ptr == 0
		mov eax, 0
		ret
	.endif

	mov ecx, [esi].my_string.str_len
    mov edx, [edi].my_string.str_len

    .if ecx <= edx
        mov ebx, ecx
    .else
        mov ebx, edx
    .endif

	mov esi, [esi].my_string.str_ptr
	mov edi, [edi].my_string.str_ptr

	.while ebx > 0
		mov al, byte ptr[esi]
		.if al < byte ptr[edi]
			mov eax, -1
			ret
		.elseif al > byte ptr[edi]
			mov eax, 1
			ret
		.endif

		inc esi
		inc edi
		dec ebx
	.endw

	mov eax, [arg_str1]
	mov eax, [eax].my_string.str_len
	mov ebx, [arg_str2]
	mov ebx, [ebx].my_string.str_len

	.if eax < ebx
		mov eax, -1
		ret
	.elseif eax > ebx
		mov eax, 1
		ret
	.endif

	mov eax, 0
	ret
struct_strcmp endp
; ----------------------------------------------------
struct_strchr proc uses ebx esi edi, arg_string:ptr my_string, arg_char:dword
	local temp:my_string
	local buf:dword

	mov esi, [arg_string]
	.if esi == 0
		mov eax, 0
		ret
	.endif

	.if [esi].my_string.str_ptr == 0
		mov eax, 0
		ret
	.endif

	mov [temp].str_len, 1

	mov eax, [arg_char]
	mov [buf], eax
	lea eax, [buf]

	mov [temp].str_ptr, eax

	invoke struct_strstr, esi, addr temp

	ret
struct_strchr endp

; ----------------------------------------------------
struct_strncpy proc uses ebx esi edi, arg_dst_str:ptr my_string, arg_src_str:ptr my_string, arg_size:dword
	mov esi, [arg_src_str]
    mov edi, [arg_dst_str]
    .if esi == 0 || edi == 0
        mov eax, 0
        ret
    .endif

    .if [esi].my_string.str_ptr == 0 || [edi].my_string.str_ptr == 0
        mov eax, 0
        ret 
    .endif

	.if [esi].my_string.str_len == 0
		mov [edi].my_string.str_len, 0
		mov eax, [edi].my_string.str_ptr
		ret
	.endif

	mov ebx, dword ptr[arg_size]
	mov esi, [esi].my_string.str_ptr
	mov edi, [edi].my_string.str_ptr

	.while ebx > 0
		mov al, byte ptr[esi]
		mov byte ptr[edi], al 

		inc esi
		inc edi

		dec ebx
	.endw

	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]

	mov ecx, [esi].my_string.str_len
	mov [edi].my_string.str_len, ecx

	mov esi, [edi].my_string.str_ptr
	add esi, dword ptr[arg_size]
	dec esi

	mov byte ptr[esi], 0

	mov eax, [edi].my_string.str_ptr
	ret
struct_strncpy endp
; ----------------------------------------------------
struct_strnlen proc, arg_string:ptr my_string, arg_size:dword
	mov eax, [arg_string]
	.if eax == 0
		mov eax, 0
		ret
	.endif

	.if [eax].my_string.str_ptr == 0
		mov eax, 0
		ret 
	.endif

	mov eax, [eax].my_string.str_len

	.if eax < dword ptr[arg_size]
		ret
	.endif
	mov eax, dword ptr[arg_size]
	ret
struct_strnlen endp
; ----------------------------------------------------
struct_strncat proc uses ebx esi edi, arg_dst_str:ptr my_string, arg_src_str:ptr my_string, arg_size:dword
	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]

	.if esi == 0 || edi == 0
		mov eax, 0
		ret
	.endif

	.if [esi].my_string.str_ptr == 0 || [edi].my_string.str_ptr == 0
		mov eax, 0
		ret
	.endif

	invoke struct_strlen, [arg_dst_str]
	add [edi].my_string.str_ptr, eax
	mov ebx, eax

	invoke struct_strncpy, edi, esi, dword ptr[arg_size]

	sub [edi].my_string.str_ptr, ebx
	sub eax, [edi].my_string.str_ptr
	ret
struct_strncat endp
; ----------------------------------------------------
struct_strncmp proc uses ebx esi edi, arg_str1:ptr my_string, arg_str2:ptr my_string, arg_size:dword
	mov esi, [arg_str1]
	mov edi, [arg_str2]

	.if esi == 0 || edi == 0
		mov eax, 0
		ret
	.endif

	.if [esi].my_string.str_ptr == 0 || [edi].my_string.str_ptr == 0
		mov eax, 0
		ret
	.endif

	mov ecx, [esi].my_string.str_len
    mov edx, [edi].my_string.str_len

    .if ecx <= edx
		.if ecx <= dword ptr[arg_size]
			mov ebx, ecx
		.else
			mov ebx, dword ptr[arg_size]
		.endif
    .else
		.if edx <= dword ptr[arg_size]
			mov ebx, edx
		.else
			mov ebx, dword ptr[arg_size]
		.endif
    .endif

	mov esi, [esi].my_string.str_ptr
	mov edi, [edi].my_string.str_ptr

	.while ebx > 0
		mov al, byte ptr[esi]
		.if al < byte ptr[edi]
			mov eax, -1
			ret
		.elseif al > byte ptr[edi]
			mov eax, 1
			ret
		.endif

		inc esi
		inc edi
		dec ebx
	.endw

	mov eax, 0
	ret
struct_strncmp endp
; ----------------------------------------------------