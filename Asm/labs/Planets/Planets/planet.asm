include relation.inc
include Strings.mac
include planet.inc

.code


InitPlanet proc frame_macro index:dword, radius:dword, aCoeff:dword, bCoeff:dword, ellipse_coeff:dword, alpha_coeff:dword
    local tmpPlanet:cword

    invoke c_malloc, sizeof Planet
    .if cax == 0
        ret
    .endif
    
    mov tmpPlanet, cax

    mov cdx, cax
    mov eax, index
    mov [cdx].Planet.index, eax
    mov eax, radius
    mov [cdx].Planet.radius, eax

    .if index != 0
        mov [cdx].Planet.x, 0
        mov [cdx].Planet.y, 0
    .else
        mov eax, Solar_X_Center
        mov [cdx].Planet.x, eax
        mov eax, Solar_Y_Center
        mov [cdx].Planet.y, eax
    .endif
    
    fld aCoeff
    fstp [cdx].Planet.a

    fld bCoeff
    fstp [cdx].Planet.b

    fld ellipse_coeff
    fstp [cdx].Planet.ellipse_coeff
    
    fld alpha_coeff
    fstp [cdx].Planet.alpha_coeff
    
    mov cax, tmpPlanet
    ret
InitPlanet endp

DrawPlanet proc frame_macro uses cbx cdi csi, planet:cword, hdc:HDC
    local hBrush:HBRUSH
    local hPen:HPEN
    local hOldBrush:HBRUSH
    local hOldPen:HPEN
    
    local centerX:dword
    local centerY:dword

    local left:dword
    local top:dword
    local right:dword
    local bottom:dword

    local radius:dword

    local ringLeft:dword
    local ringTop:dword
    local ringRight:dword
    local ringBottom:dword

    local hRingPen:HPEN
    
    mov cbx, planet
    .if cbx == 0
        ret
    .endif

    mov eax, [cbx].Planet.x
    mov centerX, eax

    mov eax, [cbx].Planet.y
    mov centerY, eax
    
    mov eax, [cbx].Planet.radius
    mov radius, eax
    
    mov eax, centerX
    sub eax, radius
    mov left, eax
    
    mov eax, centerY
    sub eax, radius
    mov top, eax
    
    mov eax, centerX
    add eax, radius
    mov right, eax
    
    mov eax, centerY
    add eax, radius
    mov bottom, eax
    
    ; Выбираем цвет в зависимости от id планеты
    mov eax, [cbx].Planet.index
    .if eax == 0
        invoke CreateSolidBrush, 000FFFFh
        mov hBrush, cax
        invoke CreatePen, PS_SOLID, 2, 00000099h
    .elseif eax == 1
        invoke CreateSolidBrush, 00A9A9A9h
        mov hBrush, cax
        invoke CreatePen, PS_SOLID, 1, 00696969h
    .elseif eax == 2
        invoke CreateSolidBrush, 00B3DEF5h
        mov hBrush, cax
        invoke CreatePen, PS_SOLID, 1, 008CB4D2h
    .elseif eax == 3
        invoke CreateSolidBrush, 0000FF00h
        mov hBrush, cax
        invoke CreatePen, PS_SOLID, 1, 00C80000h
    .elseif eax == 4
        invoke CreateSolidBrush, 000000FFh
        mov hBrush, cax
        invoke CreatePen, PS_SOLID, 1, 000000C8h
    .elseif eax == 5
        invoke CreateSolidBrush, 00347FE2h
        mov hBrush, cax
        invoke CreatePen, PS_SOLID, 1, 0000A5C8h
    .elseif eax == 6
        invoke CreateSolidBrush, 008FBFEFh
        mov hBrush, cax
        invoke CreatePen, PS_SOLID, 1, 008CB4D2h
    .else 
    .endif
    
    mov hPen, cax
    
    invoke SelectObject, hdc, hBrush
    mov hOldBrush, cax
    invoke SelectObject, hdc, hPen
    mov hOldPen, cax

    invoke Ellipse, hdc, left, top, right, bottom
    
    mov eax, [cbx].Planet.index
    .if eax != 6
        invoke SelectObject, hdc, hOldPen
        invoke SelectObject, hdc, hOldBrush
    .endif

    mov eax, [cbx].Planet.index
    .if eax == 6
        invoke CreatePen, PS_SOLID, 1, 00D2B48Ch
        mov hRingPen, cax
        
        invoke SelectObject, hdc, hRingPen
        
        invoke GetStockObject, NULL_BRUSH
        invoke SelectObject, hdc, cax
        
        mov eax, radius
        mov ebx, eax
        shl ebx, 1
        
        mov eax, radius
        mov ecx, eax
        shr ecx, 1          ; ecx = radius / 2
        add eax, ecx        ; eax = radius * 1.5
        add eax, ecx        ; eax = radius * 2.0
        add eax, ecx        ; eax = radius * 2.5
        
        mov ebx, centerX
        sub ebx, eax
        mov ringLeft, ebx
        
        mov ebx, centerX
        add ebx, eax
        mov ringRight, ebx
        
        mov eax, radius
        mov ecx, eax
        shr ecx, 2          ; ecx = radius / 4
        sub eax, ecx        ; eax = radius * 0.75
        
        mov ebx, centerY
        sub ebx, eax
        mov ringTop, ebx
        
        mov ebx, centerY
        add ebx, eax
        mov ringBottom, ebx
        
        invoke Ellipse, hdc, ringLeft, ringTop, ringRight, ringBottom
        
        invoke SelectObject, hdc, hOldPen
        invoke SelectObject, hdc, hOldBrush
        
        invoke DeleteObject, hRingPen
    .endif
    
    invoke DeleteObject, hPen
    invoke DeleteObject, hBrush
    
    ret
DrawPlanet endp

UpdatePlanet proc frame_macro uses cax cbx csi cdi, planet:cword, sun:cword, days:dword
    local result:dword

    mov cdi, sun

    mov cbx, planet
    .if cbx == 0
        ret
    .endif

    mov eax, [cbx].Planet.index
    .if eax == 0
        ret
    .endif

    fld [cbx].Planet.alpha_coeff
    fild days
    fmul
    fcos
    fld [cbx].Planet.a
    fmul
    fild [cdi].Planet.x
    fadd
    fistp result

    mov eax, result
    mov [cbx].Planet.x, eax

    fld [cbx].Planet.alpha_coeff
    fild days
    fmul
    fsin
    fld [cbx].Planet.b
    fmul
    fild [cdi].Planet.y
    fadd
    fistp result

    mov eax, result
    mov [cbx].Planet.y, eax

    ret
UpdatePlanet endp

DrawOrbit proc frame_macro uses cbx cdi csi, planet:cword, hdc:HDC
    local hPen:HPEN
    local hOldPen:HPEN

    local centerX:dword
    local centerY:dword

    local aCoeff:qword
    local bCoeff:qword
    local x:dword
    local y:dword
    local steps:dword
    local i:dword
    local angleStep:qword
    local angle:qword
    local fpuState[108]:BYTE
    
    fnsave fpuState
    
    mov cbx, planet
    .if cbx == 0
        frstor fpuState
        ret
    .endif

    mov eax, [cbx].Planet.index
    .if eax == 0
        frstor fpuState
        ret
    .endif

    fld [cbx].Planet.a
    fstp aCoeff
    fld [cbx].Planet.b
    fstp bCoeff

    mov eax, Solar_X_Center
    mov centerX, eax
    mov eax, Solar_Y_Center
    mov centerY, eax

    invoke CreatePen, PS_SOLID, 1, 007F7F7Fh
    mov hPen, cax
    invoke SelectObject, hdc, hPen
    mov hOldPen, cax

    mov steps, 72
    
    fldpi
    fadd st(0), st(0)    ; 2pi
    fidiv steps
    fstp angleStep
    
    fldz                ; angle = 0
    fstp angle
    
    fld angle
    fcos
    fld aCoeff
    fmul
    fiadd centerX
    fistp x

    fld angle
    fsin
    fld bCoeff
    fmul
    fiadd centerY
    fistp y

    invoke MoveToEx, hdc, x, y, NULL

    mov i, 1
    mov ecx, i
    .while ecx <= steps
        fld angle
        fld angleStep
        fadd
        fstp angle
        
        fld angle
        fcos
        fld aCoeff
        fmul
        fiadd centerX
        fistp x

        fld angle
        fsin
        fld bCoeff
        fmul
        fiadd centerY
        fistp y

        invoke LineTo, hdc, x, y

        inc i
        mov ecx, i
    .endw

    fldz                ; angle = 0
    fcos
    fld aCoeff
    fmul
    fiadd centerX
    fistp x

    fldz                ; angle = 0  
    fsin
    fld bCoeff
    fmul
    fiadd centerY
    fistp y

    invoke LineTo, hdc, x, y

    invoke SelectObject, hdc, hOldPen
    invoke DeleteObject, hPen

    frstor fpuState

    ret

DrawOrbit endp

end