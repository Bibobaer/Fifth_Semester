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
	f1 db "file1.txt", 0
	f2 db "file2.txt", 0

	read_file  db "r", 0
	write_file db "w", 0

.code

My_CopyFile proc c filename1:DWORD, filename2:DWORD
	local pointer_buf:DWORD

	local desc_file1:DWORD
	local desc_file2:DWORD

	invoke crt_malloc, 512
	mov [pointer_buf], eax

	invoke crt_fopen, [filename1], addr read_file
	test eax, eax
	je My_CopyFile_ret
	mov [desc_file1], eax



	invoke crt_fopen, [filename2], addr write_file
	mov [desc_file2], eax


	jmp while_check

	while_loop:
		invoke crt_fwrite, [pointer_buf], 1, eax, [desc_file2]

	while_check:
		invoke crt_fread, [pointer_buf], 1, 512, [desc_file1]
		cmp eax, 0
		jg while_loop

	invoke crt_fclose, [desc_file1]
	invoke crt_fclose, [desc_file2]

	My_CopyFile_ret:
	ret
My_CopyFile endp

main proc c argc:DWORD, argv:DWORD, envp:DWORD

	invoke My_CopyFile, addr f1, addr f2
	
	mov eax, 0
	ret

main endp
end
