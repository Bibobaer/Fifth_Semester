include relation.inc
include planet_math.inc

; ---------------
; -- Constans ---
; ---------------
.data
const_2 dword 2.0
distance_coeff dword 100.0

.code

ComputeSemiMajorOrbit proc frame_macro ; st(0) = e, st(1) = a
    fld st(0)
    fmul st(0), st(0)
    
    fld1
    fsubr
    
    fsqrt
    
    fxch st(2)
    fmulp st(2), st(0)
    fstp st(0)          ; st(0) = a*sqrt(1-e^2)
    
    ret
ComputeSemiMajorOrbit endp

CalcAlfaCoeff proc frame_macro planet_alfa:dword
    fld const_2
    fldpi
    fmul                ; st(0) = 2*pi
    fld planet_alfa
    fdiv                ; st(0) = 2*pi / planet_alfa

    ret
CalcAlfaCoeff endp

CalcANum proc frame_macro planet_a_coeff:dword ; большая полуось эллипса
    fld planet_a_coeff
    fld distance_coeff
    fmul                ; st(0) = a * distance
    ret
CalcANum endp 

CalcBNum proc frame_macro planet_a_coeff:dword, planet_e_coeff:dword ; малая полуось эллипса
    fld planet_a_coeff
    fld planet_e_coeff
    invoke ComputeSemiMajorOrbit
    fld distance_coeff
    fmul                ; st(0) = Comp(e, a) * distance
    ret
CalcBNum endp

end