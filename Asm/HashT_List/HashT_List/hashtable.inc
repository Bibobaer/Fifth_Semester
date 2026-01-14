HashNode struct
	key dd ?
	value dd ?
	next dd ?
HashNode ends

HashTable struct 
	bucket dd ?
	buck_capacity dd ?
HashTable ends

; djb5 hash
get_hash proc uses ebx esi edi, arg_key:dword, table_size:dword
	local hash:dword
	.if dword ptr[arg_key] == 0
		mov eax, -1
		ret 
	.endif

	mov dword ptr[hash], 5831
	mov esi, dword ptr[arg_key]

	xor ebx, ebx
	mov bl, byte ptr[esi]

	.while bl != 0
		mov eax, dword ptr[hash]

		shl dword ptr[hash], 5
		add dword ptr[hash], eax 
		add dword ptr[hash], ebx ; hash = (((hash << 5) + hash) + sym)

		inc esi
		mov bl, byte ptr[esi]
	.endw

	xor edx, edx
	mov eax, dword ptr[hash]

	div dword ptr[table_size] ; eax = hash % table_size

	mov eax, edx
	ret
get_hash endp

Create_HashTable proc uses ebx esi edi, arg_size:dword
	.if dword ptr[arg_size] == 0
		xor eax, eax
		ret
	.endif

	invoke crt_malloc, sizeof HashTable
	.if eax == 0
		ret
	.endif

	mov ebx, dword ptr[arg_size]
	mov (HashTable ptr[eax]).buck_capacity, ebx

	mov ebx, eax

	invoke crt_calloc, dword ptr[arg_size], 4 ; calloc(size, sizeof(HashNode*))
	.if eax == 0
		invoke crt_free, ebx
		xor eax, eax
		ret
	.endif

	mov (HashTable ptr[ebx]).bucket, eax

	mov eax, ebx
	ret
Create_HashTable endp

Create_HashNode proc uses ebx esi edi, arg_key:dword, arg_value:dword
	.if dword ptr[arg_key] == 0 || dword ptr[arg_value] == 0
		xor eax, eax
		ret
	.endif

	invoke crt_malloc, sizeof HashNode
	.if eax == 0
		ret 
	.endif

	mov ebx, eax

	invoke crt__strdup, dword ptr[arg_key]
	.if eax == 0
		invoke crt_free, ebx
		xor eax, eax
		ret
	.endif
	mov (HashNode ptr[ebx]).key, eax

	invoke crt__strdup, dword ptr[arg_value]
	.if eax == 0
		invoke crt_free, (HashNode ptr[ebx]).key
		invoke crt_free, ebx
		xor eax, eax
		ret
	.endif
	mov (HashNode ptr[ebx]).value, eax

	mov (HashNode ptr[ebx]).next, 0

	mov eax, ebx
	ret
Create_HashNode endp

Insert_to_HashTable proc uses ebx esi edi, table:ptr HashTable, arg_key:dword, arg_value:dword
	local index:dword
	local current:ptr HashNode


	.if dword ptr[arg_key] == 0 || dword ptr[arg_value] == 0
		xor eax, eax
		ret 
	.endif

	mov esi, [table]
	.if esi == 0
		xor eax, eax
		ret
	.endif
	.if [esi].HashTable.bucket == 0
		xor eax, eax
		ret
	.endif

	invoke get_hash, dword ptr[arg_key], [esi].HashTable.buck_capacity
	mov [index], eax

	mov esi, [esi].HashTable.bucket
	mov eax, dword ptr[index]
	shl eax, 2
	add esi, eax

	mov esi, [esi]
	mov [current], esi

	.while [current] != 0
		mov esi, [current]
		mov ebx, [esi].HashNode.key

		invoke crt_strcmp, ebx, dword ptr[arg_key]
		.if eax == 0
			mov ebx, dword ptr[arg_value]
			invoke crt_free, [esi].HashNode.value
			mov [esi].HashNode.value, ebx
			mov eax, esi
			ret
		.endif

		mov esi, [esi].HashNode.next
		mov [current], esi
	.endw

	invoke Create_HashNode, dword ptr[arg_key], dword ptr[arg_value]
	.if eax == 0
		ret
	.endif

	mov esi, [table]
	mov esi, [esi].HashTable.bucket
	mov ebx, dword ptr[index]
	shl ebx, 2
	add esi, ebx

	mov edi, [esi]
	mov (HashNode ptr[eax]).next, edi
	mov [esi], eax

	ret 
Insert_to_HashTable endp

Search_Value_HashTable proc uses ebx esi edi, table:ptr HashTable, arg_key:dword
	local index:dword
	local current:ptr HashNode

	.if dword ptr[arg_key] == 0
		xor eax, eax
		ret 
	.endif

	mov esi, [table]

	.if esi == 0
		xor eax, eax
		ret
	.endif
	.if [esi].HashTable.bucket == 0
		xor eax, eax
		ret
	.endif

	invoke get_hash, dword ptr[arg_key], [esi].HashTable.buck_capacity
	mov [index], eax

	mov esi, [esi].HashTable.bucket
	mov eax, dword ptr[index]
	shl eax, 2
	add esi, eax

	mov esi, [esi]
	mov [current], esi

	.while [current] != 0
		mov esi, [current]
		mov ebx, [esi].HashNode.key

		invoke crt_strcmp, ebx, dword ptr[arg_key]
		.if eax == 0
			mov eax, esi
			ret
		.endif

		mov esi, [esi].HashNode.next
		mov [current], esi
	.endw

	xor eax, eax
	ret
Search_Value_HashTable endp