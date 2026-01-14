; ----------------------------------------------------
string_strlen proc uses edi esi, arg_string:dword
	.if dword ptr[arg_string] == 0
		mov eax, 0
		ret
	.endif

	cld
	mov edi, [arg_string]
	mov ecx, -1
	mov al,0
	repne scasb
	not ecx
	lea eax, [ecx - 1]	; not ecx = strlen + 1
	
	ret
string_strlen endp
; ----------------------------------------------------
string_strstr proc uses ebx esi edi, arg_string:dword, arg_substring:dword
	local substring_len:dword
	.if dword ptr[arg_string] == 0 || dword ptr[arg_substring] == 0
		mov eax, 0
		ret
	.endif

	invoke string_strlen, dword ptr[arg_substring]
	mov dword ptr[substring_len], eax
	
	.while 1
		cld
		mov ecx, dword ptr[substring_len]
		mov edi, dword ptr[arg_substring]
		mov esi, dword ptr[arg_string]

		repe cmpsb	

		.if ecx == 0
			dec esi
			dec edi
			cmpsb
			.if ZERO?
				.break
			.else
				inc esi
				inc edi
				inc dword ptr[arg_string]
				.continue
			.endif
		.endif
		
		inc dword ptr[arg_string]

	 .endw

	mov eax, dword ptr[arg_string]
	ret	
string_strstr endp
; ----------------------------------------------------
string_strcpy proc uses ebx esi edi, arg_dst_str:dword, arg_src_str:dword
	.if dword ptr[arg_dst_str] == 0 || dword ptr[arg_src_str] == 0
		mov eax, 0
		ret
	.endif

	invoke string_strlen, [arg_src_str]
	cld
	lea ecx, [eax + 1]
	mov ebx, ecx
	shr ecx, 2				; ecx /= 4

	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]
	rep movsd

	and ebx, 3  ; при сдвиге тер€ютс€ последние 2 бита поэтому их докопируем
	mov ecx, ebx
	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]
	rep movsb

	mov eax, [arg_dst_str]
	ret
string_strcpy endp
; ----------------------------------------------------
string_strcat proc uses ebx esi edi, arg_dst_str:dword, arg_src_str:dword
	.if dword ptr[arg_dst_str] == 0 || dword ptr[arg_dst_str] == 0
		mov eax, 0
		ret
	.endif

	invoke string_strlen, [arg_dst_str]
	add dword ptr[arg_dst_str], eax
	mov ebx, eax

	invoke string_strcpy, dword ptr[arg_dst_str], dword ptr[arg_src_str]

	sub eax, ebx
	ret
string_strcat endp
; ----------------------------------------------------
string_strcmp proc uses ebx esi edi, arg_str1:dword, arg_str2:dword
	.if dword ptr[arg_str1] == 0 || dword ptr[arg_str2] == 0
		mov eax, 0
		ret
	.endif

	invoke string_strlen, dword ptr[arg_str1]
	mov ebx, eax
	invoke string_strlen, dword ptr[arg_str2]
	mov esi, eax

	.if ebx <= esi
        mov ecx, ebx
    .else
        mov ecx, esi
    .endif

	cld
	mov edi, dword ptr[arg_str1]
	mov esi, dword ptr[arg_str2]
	repe cmpsb

	xor eax, eax
	dec edi
	dec esi
	mov al, byte ptr[edi]
	sub al, byte ptr[esi]

	ret
string_strcmp endp
; ----------------------------------------------------
string_strchr proc uses ebx esi edi, arg_string:dword, arg_symbol:dword
	.if dword ptr[arg_string] == 0
		ret
	.endif

	invoke string_strlen, dword ptr[arg_string]
	mov ecx, eax
	cld
	mov edi, [arg_string]
	mov al, byte ptr[arg_symbol]
	repne scasb

	.if ecx == 0
		mov eax, edi
		dec eax

		mov bl, byte ptr[eax]
		.if bl != byte ptr[arg_symbol]
			xor eax, eax
			ret
		.endif
	.endif
	
	mov eax, edi
	dec eax
	ret
string_strchr endp
; ----------------------------------------------------
string_strncpy proc uses ebx esi edi, arg_dst_str:dword, arg_src_str:dword, arg_size:dword
	.if dword ptr[arg_dst_str] == 0 || dword ptr[arg_src_str] == 0
		mov eax, 0
		ret
	.endif

	cld
	mov ecx, dword ptr[arg_size]
	mov ebx, ecx
	shr ecx, 2				; ecx /= 4

	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]
	rep movsd

	and ebx, 3  ; при сдвиге тер€ютс€ последние 2 бита поэтому их докопируем
	mov ecx, ebx
	mov esi, [arg_src_str]
	mov edi, [arg_dst_str]
	rep movsb

	mov eax, [arg_dst_str]
	ret
string_strncpy endp
; ----------------------------------------------------
string_strnlen proc uses ebx esi edi, arg_string:dword, arg_size:dword
	.if dword ptr[arg_string] == 0
		ret 
	.endif

	cld
	
	mov edi, dword ptr[arg_string]
	mov ecx, dword ptr[arg_size]
	mov al,0
	repne scasb
	.if ecx == 0
		mov eax, dword ptr[arg_size]
		ret
	.endif 
	mov eax, dword ptr[arg_size]
	sub eax, ecx
	ret
string_strnlen endp
; ----------------------------------------------------
string_strncat proc uses ebx esi edi, arg_dst_str:dword, arg_src_str:dword, arg_size:dword
	.if dword ptr[arg_dst_str] == 0 || dword ptr[arg_src_str] == 0
		xor eax, eax 
		ret 
	.endif

	mov edi, [arg_dst_str]	
	invoke string_strlen, [arg_dst_str]
	add edi, eax
	invoke string_strlen, [arg_src_str]
	mov ecx, dword ptr[arg_size]

	mov esi, [arg_src_str]
	
	cld
	rep movsd

	mov eax, [arg_dst_str]
	ret
string_strncat endp
; ----------------------------------------------------
string_strncmp proc uses ebx esi edi, arg_str1:dword, arg_str2:dword, arg_size:dword
	.if dword ptr[arg_str1] == 0 || dword ptr[arg_str2] == 0
		xor eax, eax
		ret 
	.endif

	cld
	mov ecx, dword ptr[arg_size]
	mov esi, [arg_str2]
	mov edi, [arg_str1]
	repe cmpsb

	xor eax, eax
	dec edi
	dec esi
	mov al, byte ptr[edi]
	mov bl, byte ptr[esi]
	sub al, bl
	ret
string_strncmp endp
; ----------------------------------------------------