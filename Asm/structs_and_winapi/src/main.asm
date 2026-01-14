.686
.model flat, stdcall
option casemap:none		; чувствительность к регистру символов в идентификаторах

include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc

include Strings.mac

my_string struct
	str_len dword ?
	str_ptr dword ?
my_string ends

.data
.data?

.const
	; strings test
	test_str db "1234567", 0
	sub_str db "23", 0

	string_format db "%s", 13, 10, 0
	int_format db "%d", 13, 10, 0


	; Tree test
	szKey1 db "apple", 0
	szValue1 db "red fruit", 0
	szKey2 db "banana", 0
	szValue2 db "yellow fruit", 0
	szKey3 db "cherry", 0
	szValue3 db "small red fruit", 0
	szKey4 db "apple", 0
	szValue4 db "Updated value apple", 0
.code

include struct_func.inc
include string_func.inc
include BST.inc

main proc c argc:DWORD, argv:DWORD, envp:DWORD
	local s1:my_string
	local s2:my_string

	local my_tree:ptr Tree

	mov [s1].str_len, 7
	mov eax, offset test_str
	mov [s1].str_ptr, eax

	mov [s2].str_len, 2
	mov eax, offset sub_str
	mov [s2].str_ptr, eax

	invoke struct_strstr, addr s1, addr s2
	invoke crt_printf, addr string_format, eax

	invoke string_strchr, offset test_str, '4'
	invoke crt_printf, addr string_format, eax

	invoke Create_tree
	mov [my_tree], eax
	.if [my_tree] == 0
		mov eax, 1
		ret
	.endif

	invoke InsertValueByKey, [my_tree], addr szKey1, addr szValue1
	invoke InsertValueByKey, [my_tree], addr szKey2, addr szValue2
	invoke InsertValueByKey, [my_tree], addr szKey3, addr szValue3


	invoke SearchValueByKey, [my_tree], offset szKey1
	.if eax != 0
		invoke crt_printf, addr string_format, (Node ptr[eax]).value
	.endif

	invoke InsertValueByKey, [my_tree], addr szKey4, addr szValue4
	invoke SearchValueByKey, [my_tree], offset szKey1
	.if eax != 0
		invoke crt_printf, addr string_format, (Node ptr[eax]).value
	.endif

	invoke Free_Tree, [my_tree]

	mov eax, 0
	ret

main endp
end
