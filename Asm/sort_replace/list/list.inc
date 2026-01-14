; Struct List
    string equ 0
    next   equ 4
    prev   equ 8

    sizeof_node equ 12
; -----------

Create_List:
    push ebp
    mov ebp, esp
    
    push sizeof_node
    call crt_malloc
    add esp, 4

    mov esp, ebp
    pop ebp
    ret