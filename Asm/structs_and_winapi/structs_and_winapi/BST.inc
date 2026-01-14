
Node struct
	key dd ?
	value dd ?

	left dd ?
	right dd ?

Node ends

Tree struct
	root dd ?
Tree ends

Create_tree proto
Create_node proto :dword, :dword
InsertNode2Tree proto :ptr Tree, :ptr Node

InsertValueByKey proto :ptr Tree, :dword, :dword
SearchValueByKey proto :ptr Tree, :dword

Free_Node proto :ptr Node
Free_Tree proto :ptr Tree

Copy_value proc uses ebx, arg_src:dword
	.if [arg_src] == 0
		xor eax, eax
		ret
	.endif

	invoke crt_strlen, dword ptr[arg_src]
	mov ebx, eax
	inc eax

	invoke crt_malloc, eax
	.if eax != 0
		invoke crt_strcpy, eax, dword ptr[arg_src]
		mov byte ptr[eax + ebx], 0
	.endif
	ret
Copy_value endp

Create_tree proc
	invoke crt_malloc, sizeof Tree
	.if eax == 0
		ret
	.endif
	mov Tree ptr[eax].Tree.root, 0
	ret

Create_tree endp

Create_node proc uses ebx esi edi, KeyType:dword, ValueType:dword
	local temp_key:dword
	local temp_val:dword
	.if dword ptr[KeyType] == 0 || dword ptr[ValueType] == 0
		mov eax, 0
		ret 
	.endif

	invoke crt_malloc, sizeof Node
	.if eax == 0
		ret 
	.endif

	mov ebx, eax

	invoke Copy_value, dword ptr[KeyType]
	mov dword ptr[temp_key], eax
	invoke Copy_value, dword ptr[ValueType]
	mov dword ptr[temp_val], eax

	mov eax, dword ptr[temp_key]
	mov (Node ptr[ebx]).key, eax
	mov eax, dword ptr[temp_val]
	mov (Node ptr[ebx]).value, eax
	mov (Node ptr[ebx]).left, 0
	mov (Node ptr[ebx]).right, 0

	mov eax, ebx

	ret
Create_node endp 

InsertNode2Tree proc uses ebx esi edi, arg_tree:ptr Tree, arg_node:ptr Node
	mov esi, [arg_tree]
	mov edi, [arg_node]

	.if esi == 0 || edi == 0
		ret
	.endif

	.if [esi].Tree.root == 0
		mov [esi].Tree.root, edi
		ret 
	.endif

	mov ebx, [esi].Tree.root

	.while 1
		invoke crt_strcmp, (Node ptr[edi]).key, (Node ptr[ebx]).key

		.if eax == 0
			invoke crt_free, (Node ptr[ebx]).value
			invoke Copy_value, [edi].Node.value
			mov (Node ptr[ebx]).value, eax

			invoke Free_Node, edi
			ret
		.elseif eax < 0FFFFFFFFh
			.if [ebx].Node.right != 0
				mov ebx, (Node ptr[ebx]).right
			.else
				mov (Node ptr[ebx]).right, edi
				ret
			.endif
		.else 
			.if [ebx].Node.left != 0
				mov ebx, (Node ptr[ebx]).left
			.else 
				mov (Node ptr[ebx]).left, edi
				ret
			.endif
		.endif
	.endw
	ret
InsertNode2Tree endp


InsertValueByKey proc arg_tree:ptr Tree, KeyType:dword, ValueType:dword
	invoke Create_node, dword ptr[KeyType], dword ptr[ValueType]
	.if eax != 0
		invoke InsertNode2Tree, [arg_tree], eax
	.endif
	ret
InsertValueByKey endp

SearchValueByKey proc uses ebx esi edi, arg_tree:ptr Tree, KeyType:dword
	.if [arg_tree] == 0
		xor eax, eax
		ret 
	.endif

	mov esi, [arg_tree]

	.if [esi].Tree.root == 0
		xor eax, eax
		ret 
	.endif

	mov esi, [esi].Tree.root
	mov ebx, dword ptr[KeyType]

	.while esi != 0
		invoke crt_strcmp, ebx, [esi].Node.key

		.if eax == 0
			mov eax, esi
			ret
		.elseif eax < 0FFFFFFFFh
			mov esi, [esi].Node.right
		.else
			mov esi, [esi].Node.left
		.endif
	.endw
	xor eax, eax
	ret
SearchValueByKey endp


Free_Node proc uses ebx esi, arg_node:ptr Node
	mov esi, [arg_node]
    .if esi == 0
        ret
    .endif

    invoke Free_Node, (Node ptr [esi]).left
    invoke Free_Node, (Node ptr [esi]).right
    
    invoke crt_free, (Node ptr [esi]).key
    invoke crt_free, (Node ptr [esi]).value
    
    invoke crt_free, esi
    
    ret
Free_Node endp

Free_Tree proc uses esi, arg_tree:ptr Tree
	mov esi, [arg_tree]

	.if esi == 0
		ret
	.endif

	invoke Free_Node, [esi].Tree.root
	invoke crt_free, esi
	ret
Free_Tree endp