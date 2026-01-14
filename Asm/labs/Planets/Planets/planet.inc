include relation.inc

.data

Solar_X_Center dd 500
Solar_Y_Center dd 370

daysCount dd 0
const_180 dd 180

Planet struct

	index dd ?

	x dd 0
	y dd 0

	a dd ?
	b dd ?

	ellipse_coeff dd ?
	alpha_coeff dd ?

	radius dd ?

Planet ends

InitPlanet proto :dword, :dword, :dword, :dword, :dword, :dword
DrawPlanet proto :cword, :HDC
UpdatePlanet proto :cword, :cword, :dword
DrawOrbit proto :cword, :HDC