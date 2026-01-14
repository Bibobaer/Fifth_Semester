
HashTable struct
	keys dd ?           ; Array of keys
	values dd ?         ; Array of values
	array_count dd ?    ; Count elements
	array_capacity dd ? ; Capacity 
HashTable ends


HashFunction proto :dword ; string -> unsigned int

CreateTable proc uses ebx esi edi, capacity:dword
	local out:dword
	invoke crt_malloc, sizeof HashTable
	.if eax == 0
		ret
	.endif

	mov dword ptr[out], eax

	mov ebx, dword ptr[capacity]
	mov (HashTable ptr[eax]).array_capacity, ebx 




CreateTable endp