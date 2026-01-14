.686
.model flat, stdcall
option casemap:none		; чувствительность к регистру символов в идентификаторах

include c:\masm32\include\msvcrt.inc
include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc

include Strings.mac

.data
	
.data?

.const
	htKey1 db "apple", 0
	htValue1 db "red", 0
	htKey2 db "banana", 0
	htValue2 db "yellow", 0
	htKey3 db "eggplant", 0
	htValue3 db "purple", 0

	str_form db "%s", 13, 10, 0
.code

include hashtable.inc

main proc c argc:DWORD, argv:DWORD, envp:DWORD	
	local table:ptr HashTable
	invoke Create_HashTable, 10
	.if eax == 0
		ret
	.endif

	mov [table], eax

	invoke Insert_to_HashTable, [table], offset htKey1, offset htValue1
	invoke Insert_to_HashTable, [table], offset htKey2, offset htValue2
	invoke Insert_to_HashTable, [table], offset htKey3, offset htValue3

	invoke Search_Value_HashTable, [table], offset htKey3
	.if eax != 0
		invoke crt_printf, offset str_form, [eax].HashNode.value
	.endif

	mov eax, 0
	ret
main endp
end
