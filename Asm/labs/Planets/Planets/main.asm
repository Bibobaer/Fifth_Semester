include relation.inc

include planet.inc
include planet_math.inc
include planet_date.inc

include Strings.mac

;----------------------------------------

AppWindowName equ <"Application">

;----------------------------------------

.data
    ; planet coeffs
    Mercury_A_Coeff dd 0.387
    Mercury_E_Coeff dd 0.205
    Mercury_Alpha_Coeff dd 87.9689

    Venus_A_Coeff dd 0.723
    Venus_E_Coeff dd 0.007
    Venus_Alpha_Coeff dd 224.701

    Earth_A_Coeff dd 1.0
    Earth_E_Coeff dd 0.017
    Earth_Alpha_Coeff dd 365.256
    
    Mars_A_Coeff dd 1.524
    Mars_E_Coeff dd 0.094
    Mars_Alpha_Coeff dd 686.98

    Jupiter_A_Coeff dd 2.204
    Jupiter_E_Coeff dd 0.049
    Jupiter_Alpha_Coeff dd 4332.589

    Saturn_A_Coeff dd 3.58
    Saturn_E_Coeff dd 0.057
    Saturn_Alpha_Coeff dd 10759.22
    ; date
    currentDays dd 2
    currentMonths dd 1
    currentYears dd 2026
    ; double buf
    hBackBuffer HBITMAP ?
    hBackDC HDC ?
    bufferWidth dd 1000
    bufferHeight dd 800
    ; User interface
    hPlanetDialog HWND ?
    hParamDialog HWND ?
    ; Choose
    SelectedPlanet dd 0
    StopModel db 0
    ; Buttons
    IDM_EDIT_PLANETS equ 1001
    IDM_RESET_VALUES equ 1002
    IDM_EXIT equ 1003
.data?
    hIns HINSTANCE ?

    HwndMainWindow HWND ?

    SolarSystem cword 7 dup(?)

    float_buffer db 256 dup(?)

.const
    fmt db "%f", 0
.code

;----------------------------------------

RegisterClassMainWindow proto

CreateMainWindow proto

WndProcMain proto hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM

ReCreateSolarSystem proto

PlanetDialogProc proto hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM
ParamDialogProc proto hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM
ShowPlanetDialog proto
ShowParamDialog proto :dword

GetPlanetNameBySelected proto

FloatToStr proto :dword
StrToFloat proto :ptr byte

;----------------------------------------

WinMain proc frame_macro hInstance:HINSTANCE, hPrevInstance:HINSTANCE, szCmdLine:PSTR, iCmdShow:DWORD
    local msg: MSG
    local hAccel:HACCEL

    mov cax, [hInstance]
    mov [hIns], cax

    invoke CreateMainWindow
    mov [HwndMainWindow], cax
    .if [HwndMainWindow] == 0
        ret
    .endif
    
    invoke LoadAccelerators, [hIns], $CTA0("Accel")
    mov [hAccel], cax
	
    .while TRUE
        invoke GetMessage, addr msg, NULL, 0, 0
            .break .if cax == 0

        invoke TranslateAccelerator, [HwndMainWindow], [hAccel], addr msg
        .if !cax
            invoke TranslateMessage, addr msg
            invoke DispatchMessage, addr msg
        .endif

    .endw

    mov cax, [msg].wParam
    ret
WinMain endp

;--------------------

RegisterClassMainWindow proc frame_macro
    local WndClass:WNDCLASSEX	; структура класса

    ; заполняем поля структуры
    mov WndClass.cbSize, sizeof (WNDCLASSEX)	; размер структуры класса
    mov WndClass.style, 0
    mov cax, WndProcMain
    mov WndClass.lpfnWndProc, cax		; адрес оконной процедуры класса
    mov WndClass.cbClsExtra, 0
    mov WndClass.cbWndExtra, 0
    mov cax, [hIns]
    mov WndClass.hInstance, cax					; описатель приложения
    invoke LoadIcon, hIns, $CTA0("MainIcon")	; иконка приложения
    mov WndClass.hIcon, cax
    invoke LoadCursor, NULL, IDC_ARROW
    mov WndClass.hCursor, cax
    invoke GetStockObject, BLACK_BRUSH			; кисть для фона
    mov WndClass.hbrBackground, cax
    mov cax, $CTA0("MainMenu") 
    mov WndClass.lpszMenuName, cax       ; имя ресарса главного меню
    mov cax, $CTA0(AppWindowName)
    mov WndClass.lpszClassName, cax	; имя класса
    invoke LoadIcon, hIns, $CTA0("MainIcon")
    mov WndClass.hIconSm, cax

    invoke RegisterClassEx, addr WndClass
    ret
RegisterClassMainWindow endp

;--------------------

CreateMainWindow proc frame_macro
    local hwnd:HWND

    ; регистрация класса основного окна
    invoke RegisterClassMainWindow

    ; создание окна зарегестрированного класса
    invoke CreateWindowEx, 
        WS_EX_CONTROLPARENT or WS_EX_APPWINDOW, ; расширенный стиль окна
        $CTA0(AppWindowName),	; имя зарегестрированного класса окна
        $CTA0("Application"),	; заголовок окна
        WS_OVERLAPPEDWINDOW,	; стиль окна
        10,	    ; X-координата левого верхнего угла
        10,	    ; Y-координата левого верхнего угла
        1000,    ; ширина окна
        800,    ; высота окна
        NULL,
        NULL,
        [hIns], ; идентификатор приложения
        NULL
    mov [hwnd], cax
    
    .if [hwnd] == 0
        invoke MessageBox, NULL, $CTA0("Ошибка создания основного окна приложения"), NULL, MB_OK
        xor cax, cax
        ret
    .endif
        
    invoke ShowWindow, hwnd, SW_SHOWNORMAL
    invoke UpdateWindow, hwnd
    
    mov cax, [hwnd]
    ret
CreateMainWindow endp

;--------------------

WndProcMain proc frame_macro hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM
    local hdc:HDC
    local ps:PAINTSTRUCT
    local iter:cword

    .if [iMsg] == WM_CREATE

        INVOKE SetTimer, hwnd, 0, 1, NULL

        invoke GetDC, hwnd
        mov hdc, cax
    
        invoke CreateCompatibleDC, hdc
        mov hBackDC, cax
    
        invoke CreateCompatibleBitmap, hdc, bufferWidth, bufferHeight
        mov hBackBuffer, cax
    
        invoke SelectObject, hBackDC, hBackBuffer
    
        invoke ReleaseDC, hwnd, hdc
    
        invoke GetStockObject, BLACK_BRUSH
        invoke SelectObject, hBackDC, cax
        invoke PatBlt, hBackDC, 0, 0, bufferWidth, bufferHeight, PATCOPY

        invoke DateToDays, currentYears, currentMonths, currentDays
        mov daysCount, eax

        invoke ReCreateSolarSystem

        xor cax, cax
        ret
     
    .elseif [iMsg] == WM_DESTROY
        invoke KillTimer, hwnd, 0
    
        .if hBackBuffer != 0
            invoke DeleteObject, hBackBuffer
        .endif
        .if hBackDC != 0
            invoke DeleteDC, hBackDC
        .endif
    
        invoke PostQuitMessage, 0
        xor cax, cax
        ret
    .elseif [iMsg] == WM_KEYDOWN
        mov cax, [wParam]

        .if cax == VK_SPACE
            not StopModel
        .endif

        .if cax == VK_LEFT
            .if StopModel != 0
                dec daysCount
                invoke DecrementDate, addr currentYears, addr currentMonths, addr currentDays
                invoke InvalidateRect, hwnd, NULL, FALSE
            .endif
        .elseif cax == VK_RIGHT
            .if StopModel != 0
                inc daysCount
                invoke UpdateDate, addr currentYears, addr currentMonths, addr currentDays
                invoke InvalidateRect, hwnd, NULL, FALSE
            .endif
        .endif 
        
        xor cax, cax
        ret
    .elseif [iMsg] == WM_TIMER
        mov cax, [wParam]

        .if cax == 0 && StopModel == 0
            invoke UpdateDate, addr currentYears, addr currentMonths, addr currentDays
            invoke DateToDays, currentYears, currentMonths, currentDays
            mov daysCount, eax

            invoke InvalidateRect, hwnd, NULL, FALSE
        .endif

    .elseif [iMsg] == WM_PAINT
        invoke BeginPaint, hwnd, addr ps
        mov [hdc], cax

        invoke GetStockObject, BLACK_BRUSH
        push cax
        invoke SelectObject, hBackDC, cax
        pop cax
        invoke PatBlt, hBackDC, 0, 0, bufferWidth, bufferHeight, PATCOPY
    
        lea cbx, SolarSystem
        mov cword ptr[iter], 0
        .while cword ptr[iter] < 7
            mov cax, cword ptr[iter]
            shl cax, shift_cnt
            mov cax, [cbx + cax]
            invoke DrawOrbit, cax, hBackDC
            inc cword ptr[iter]
        .endw

        lea cbx, SolarSystem
        mov cword ptr[iter], 0
        .while cword ptr[iter] < 7
            mov cax, cword ptr[iter]
            shl cax, shift_cnt
            mov cax, [cbx + cax]
            invoke UpdatePlanet, cax, SolarSystem, daysCount
            invoke DrawPlanet, cax, hBackDC
            inc cword ptr[iter]
        .endw

        invoke DrawPlanetDate, hBackDC, hwnd, currentYears, currentMonths, currentDays
        invoke BitBlt, hdc, 0, 0, bufferWidth, bufferHeight, hBackDC, 0, 0, SRCCOPY
        invoke EndPaint, [hwnd], addr ps
        
        xor eax, eax
        ret
    
    .elseif [iMsg] == WM_COMMAND
        mov cax, [wParam]
        .if cax == IDM_EDIT_PLANETS
            invoke ShowPlanetDialog
        .elseif cax == IDM_RESET_VALUES
            mov Mercury_A_Coeff, 0.387
            mov Mercury_E_Coeff, 0.205
            mov Mercury_Alpha_Coeff, 87.9689
            
            mov Venus_A_Coeff, 0.723
            mov Venus_E_Coeff, 0.007
            mov Venus_Alpha_Coeff, 224.701
            
            mov Earth_A_Coeff, 1.0
            mov Earth_E_Coeff, 0.017
            mov Earth_Alpha_Coeff, 365.256
            
            mov Mars_A_Coeff, 1.524
            mov Mars_E_Coeff, 0.094
            mov Mars_Alpha_Coeff, 686.98
            
            mov Jupiter_A_Coeff, 2.204
            mov Jupiter_E_Coeff, 0.049
            mov Jupiter_Alpha_Coeff, 4332.589
            
            mov Saturn_A_Coeff, 3.58
            mov Saturn_E_Coeff, 0.057
            mov Saturn_Alpha_Coeff, 10759.22
            
            ; Пересоздаем солнечную систему
            invoke ReCreateSolarSystem
            invoke InvalidateRect, hwnd, NULL, TRUE
            
            invoke MessageBox, hwnd, $CTA0("Параметры планет сброшены к исходным значениям"), 
                           $CTA0("Сброс"), MB_OK or MB_ICONINFORMATION
        .elseif cax == IDM_EXIT
            invoke DestroyWindow, hwnd
        .endif
        xor cax, cax
        ret
    .endif

    invoke DefWindowProc, hwnd, iMsg, wParam, lParam
    ret
WndProcMain endp

ReCreateSolarSystem proc frame_macro
    local floatA:dword
    local floatB:dword
    local floatAlpha:dword

    local iter:cword

    lea cbx, [SolarSystem]

    mov cword ptr[iter], 0

    .while cword ptr[iter] < 7
        mov cax, cword ptr [iter]
        shl cax, shift_cnt
        mov cax, [cbx + cax]
        .if cax != 0
            invoke c_free, cax
        .endif
        inc cword ptr[iter]
    .endw

    ; -----------------------------
    ; ----------- Sun -------------
    ; -----------------------------
    invoke InitPlanet, 0, 20, 0, 0, 0, 0
    lea cbx, SolarSystem
    mov [cbx + 0*addr_size], cax
    ; -----------------------------
    ; --------- Mercury ------------
    ; -----------------------------
    invoke CalcAlfaCoeff, Mercury_Alpha_Coeff
    fstp floatAlpha

    invoke CalcBNum, Mercury_A_Coeff, Mercury_E_Coeff
    fstp floatB

    invoke CalcANum, Mercury_A_Coeff
    fstp floatA

    invoke InitPlanet, 1, 4, floatA, floatB, Mercury_E_Coeff, floatAlpha
    lea cbx, SolarSystem
    mov [cbx + 1*addr_size], cax
    ; -----------------------------
    ; ---------- Venus ------------
    ; -----------------------------
    invoke CalcAlfaCoeff, Venus_Alpha_Coeff
    fstp floatAlpha

    invoke CalcBNum, Venus_A_Coeff, Venus_E_Coeff
    fstp floatB

    invoke CalcANum, Venus_A_Coeff
    fstp floatA

    invoke InitPlanet, 2, 6, floatA, floatB, Venus_E_Coeff, floatAlpha
    lea cbx, SolarSystem
    mov [cbx + 2*addr_size], cax
    ; -----------------------------
    ; ---------- Earth ------------
    ; -----------------------------
    invoke CalcAlfaCoeff, Earth_Alpha_Coeff
    fstp floatAlpha

    invoke CalcBNum, Earth_A_Coeff, Earth_E_Coeff
    fstp floatB

    invoke CalcANum, Earth_A_Coeff
    fstp floatA

    invoke InitPlanet, 3, 7, floatA, floatB, Earth_E_Coeff, floatAlpha
    lea cbx, SolarSystem
    mov [cbx + 3*addr_size], cax
    ; -----------------------------
    ; ----------- Mars ------------
    ; -----------------------------
    invoke CalcAlfaCoeff, Mars_Alpha_Coeff
    fstp floatAlpha

    invoke CalcBNum, Mars_A_Coeff, Mars_E_Coeff
    fstp floatB

    invoke CalcANum, Mars_A_Coeff
    fstp floatA

    invoke InitPlanet, 4, 5, floatA, floatB, Mars_E_Coeff, floatAlpha
    lea cbx, SolarSystem
    mov [cbx + 4*addr_size], cax
    ; -----------------------------
    ; --------- Jupiter -----------
    ; -----------------------------
    invoke CalcAlfaCoeff, Jupiter_Alpha_Coeff
    fstp floatAlpha

    invoke CalcBNum, Jupiter_A_Coeff, Jupiter_E_Coeff
    fstp floatB

    invoke CalcANum, Jupiter_A_Coeff
    fstp floatA

    invoke InitPlanet, 5, 15, floatA, floatB, Jupiter_E_Coeff, floatAlpha
    lea cbx, SolarSystem
    mov [cbx + 5*addr_size], cax
    ; -----------------------------
    ; --------- Saturn ------------
    ; -----------------------------
    invoke CalcAlfaCoeff, Saturn_Alpha_Coeff
    fstp floatAlpha

    invoke CalcBNum, Saturn_A_Coeff, Saturn_E_Coeff
    fstp floatB

    invoke CalcANum, Saturn_A_Coeff
    fstp floatA

    invoke InitPlanet, 6, 12, floatA, floatB, Saturn_E_Coeff, floatAlpha
    lea cbx, SolarSystem
    mov [cbx + 6*addr_size], cax

    ret
ReCreateSolarSystem endp

PlanetDialogProc proc frame_macro hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM
    .if [iMsg] == WM_INITDIALOG
        invoke SetWindowText, hwnd, $CTA0("Выберите планету для редактирования")
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Меркурий"), 
                            WS_VISIBLE or WS_CHILD or BS_AUTORADIOBUTTON,
                            20, 20, 200, 25, hwnd, 101, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Венера"), 
                            WS_VISIBLE or WS_CHILD or BS_AUTORADIOBUTTON,
                            20, 50, 200, 25, hwnd, 102, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Земля"), 
                            WS_VISIBLE or WS_CHILD or BS_AUTORADIOBUTTON,
                            20, 80, 200, 25, hwnd, 103, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Марс"), 
                            WS_VISIBLE or WS_CHILD or BS_AUTORADIOBUTTON,
                            20, 110, 200, 25, hwnd, 104, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Юпитер"), 
                            WS_VISIBLE or WS_CHILD or BS_AUTORADIOBUTTON,
                            20, 140, 200, 25, hwnd, 105, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Сатурн"), 
                            WS_VISIBLE or WS_CHILD or BS_AUTORADIOBUTTON,
                            20, 170, 200, 25, hwnd, 106, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("OK"), 
                            WS_VISIBLE or WS_CHILD or BS_DEFPUSHBUTTON,
                            50, 230, 80, 25, hwnd, IDOK, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Отмена"), 
                            WS_VISIBLE or WS_CHILD or BS_PUSHBUTTON,
                            150, 230, 80, 25, hwnd, IDCANCEL, [hIns], NULL
        
        invoke SendDlgItemMessage, hwnd, 101, BM_SETCHECK, BST_CHECKED, 0
        mov SelectedPlanet, 1
        
        xor cax, cax
        ret
    .elseif [iMsg] == WM_COMMAND
        mov cax, [wParam]
        .if cax == IDOK
            invoke SendDlgItemMessage, hwnd, 101, BM_GETCHECK, 0, 0
            .if cax == BST_CHECKED
                mov SelectedPlanet, 1
            .endif
            invoke SendDlgItemMessage, hwnd, 102, BM_GETCHECK, 0, 0
            .if cax == BST_CHECKED
                mov SelectedPlanet, 2
            .endif
            invoke SendDlgItemMessage, hwnd, 103, BM_GETCHECK, 0, 0
            .if cax == BST_CHECKED
                mov SelectedPlanet, 3
            .endif
            invoke SendDlgItemMessage, hwnd, 104, BM_GETCHECK, 0, 0
            .if cax == BST_CHECKED
                mov SelectedPlanet, 4
            .endif
            invoke SendDlgItemMessage, hwnd, 105, BM_GETCHECK, 0, 0
            .if cax == BST_CHECKED
                mov SelectedPlanet, 5
            .endif
            invoke SendDlgItemMessage, hwnd, 106, BM_GETCHECK, 0, 0
            .if cax == BST_CHECKED
                mov SelectedPlanet, 6
            .endif
            
            invoke EndDialog, hwnd, IDOK
            invoke ShowParamDialog, SelectedPlanet
        .elseif cax == IDCANCEL
            invoke EndDialog, hwnd, IDCANCEL
        .endif
        xor cax, cax
        ret
    .elseif [iMsg] == WM_CLOSE
        invoke EndDialog, hwnd, IDCANCEL
    .endif

    xor cax, cax
    ret
PlanetDialogProc endp

ParamDialogProc proc frame_macro hwnd:HWND, iMsg:UINT, wParam:WPARAM, lParam:LPARAM
    local hEditA:HWND
    local hEditE:HWND
    local hEditAlpha:HWND
    local szTitle[64]:BYTE
    local planetName[32]:BYTE
    
    .if [iMsg] == WM_INITDIALOG
        ; Получаем ID планеты из lParam
        mov cax, [lParam]
        mov SelectedPlanet, eax

        invoke GetPlanetNameBySelected
        invoke SetWindowText, hwnd, cax
        
        ; Создаем статические тексты
        invoke CreateWindowEx, 0, $CTA0("STATIC"), $CTA0("Большая полуось (a):"), 
                            WS_VISIBLE or WS_CHILD,
                            20, 20, 150, 20, hwnd, 0, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("STATIC"), $CTA0("Эксцентриситет (e):"), 
                            WS_VISIBLE or WS_CHILD,
                            20, 60, 150, 20, hwnd, 0, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("STATIC"), $CTA0("Период обращения (дней):"), 
                            WS_VISIBLE or WS_CHILD,
                            20, 100, 150, 20, hwnd, 0, [hIns], NULL
        
        ; Создаем поля ввода
        invoke CreateWindowEx, WS_EX_CLIENTEDGE, $CTA0("EDIT"), NULL, 
                            WS_VISIBLE or WS_CHILD or WS_BORDER or ES_AUTOHSCROLL,
                            180, 20, 100, 20, hwnd, 201, [hIns], NULL
        mov hEditA, cax
        
        invoke CreateWindowEx, WS_EX_CLIENTEDGE, $CTA0("EDIT"), NULL, 
                            WS_VISIBLE or WS_CHILD or WS_BORDER or ES_AUTOHSCROLL,
                            180, 60, 100, 20, hwnd, 202, [hIns], NULL
        mov hEditE, cax
        
        invoke CreateWindowEx, WS_EX_CLIENTEDGE, $CTA0("EDIT"), NULL, 
                            WS_VISIBLE or WS_CHILD or WS_BORDER or ES_AUTOHSCROLL,
                            180, 100, 100, 20, hwnd, 203, [hIns], NULL
        mov hEditAlpha, cax
        
        ; Заполняем текущие значения
        mov ebx, SelectedPlanet
        .if ebx == 1
            invoke FloatToStr, Mercury_A_Coeff
            invoke SetWindowText, hEditA, addr float_buffer
            
            invoke FloatToStr, Mercury_E_Coeff
            invoke SetWindowText, hEditE, addr float_buffer
            
            invoke FloatToStr, Mercury_Alpha_Coeff
            invoke SetWindowText, hEditAlpha, addr float_buffer
            
        .elseif ebx == 2
            invoke FloatToStr, Venus_A_Coeff
            invoke SetWindowText, hEditA, addr float_buffer
            
            invoke FloatToStr, Venus_E_Coeff
            invoke SetWindowText, hEditE, addr float_buffer
            
            invoke FloatToStr, Venus_Alpha_Coeff
            invoke SetWindowText, hEditAlpha, addr float_buffer
            
        .elseif ebx == 3
            invoke FloatToStr, Earth_A_Coeff
            invoke SetWindowText, hEditA, addr float_buffer
            
            invoke FloatToStr, Earth_E_Coeff
            invoke SetWindowText, hEditE, addr float_buffer
            
            invoke FloatToStr, Earth_Alpha_Coeff
            invoke SetWindowText, hEditAlpha, addr float_buffer
            
        .elseif ebx == 4
            invoke FloatToStr, Mars_A_Coeff
            invoke SetWindowText, hEditA, addr float_buffer
            
            invoke FloatToStr, Mars_E_Coeff
            invoke SetWindowText, hEditE, addr float_buffer
            
            invoke FloatToStr, Mars_Alpha_Coeff
            invoke SetWindowText, hEditAlpha, addr float_buffer
            
        .elseif ebx == 5
            invoke FloatToStr, Jupiter_A_Coeff
            invoke SetWindowText, hEditA, addr float_buffer
            
            invoke FloatToStr, Jupiter_E_Coeff
            invoke SetWindowText, hEditE, addr float_buffer
            
            invoke FloatToStr, Jupiter_Alpha_Coeff
            invoke SetWindowText, hEditAlpha, addr float_buffer
            
        .elseif ebx == 6
            invoke FloatToStr, Saturn_A_Coeff
            invoke SetWindowText, hEditA, addr float_buffer
            
            invoke FloatToStr, Saturn_E_Coeff
            invoke SetWindowText, hEditE, addr float_buffer
            
            invoke FloatToStr, Saturn_Alpha_Coeff
            invoke SetWindowText, hEditAlpha, addr float_buffer
        .endif
        
        ; Информационное сообщение
        invoke CreateWindowEx, 0, $CTA0("STATIC"), $CTA0("Введите новые значения (положительные числа)"), 
                            WS_VISIBLE or WS_CHILD,
                            20, 130, 260, 20, hwnd, 0, [hIns], NULL
        
        ; Кнопки OK и Отмена
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Применить"), 
                            WS_VISIBLE or WS_CHILD or BS_DEFPUSHBUTTON,
                            50, 160, 80, 25, hwnd, IDOK, [hIns], NULL
        
        invoke CreateWindowEx, 0, $CTA0("BUTTON"), $CTA0("Отмена"), 
                            WS_VISIBLE or WS_CHILD or BS_PUSHBUTTON,
                            150, 160, 80, 25, hwnd, IDCANCEL, [hIns], NULL
        
        xor eax, eax
        ret
        
    .elseif [iMsg] == WM_COMMAND
        mov cax, [wParam]
        .if cax == IDOK
            invoke GetDlgItemText, hwnd, 201, addr float_buffer, 256
            invoke StrToFloat, addr float_buffer
            
            fldz
            fcomip st(0), st(1)
            jbe @F
            fstp st(0)
            invoke MessageBox, hwnd, $CTA0("Большая полуось должна быть положительным числом!"), 
                           $CTA0("Ошибка"), MB_OK or MB_ICONERROR
            ret
        @@:
            mov ebx, SelectedPlanet
            .if ebx == 1
                fstp Mercury_A_Coeff
            .elseif ebx == 2
                fstp Venus_A_Coeff
            .elseif ebx == 3
                fstp Earth_A_Coeff
            .elseif ebx == 4
                fstp Mars_A_Coeff
            .elseif ebx == 5
                fstp Jupiter_A_Coeff
            .elseif ebx == 6
                fstp Saturn_A_Coeff
            .endif
            
            invoke GetDlgItemText, hwnd, 202, addr float_buffer, 256
            invoke StrToFloat, addr float_buffer
            
            fldz
            fcomip st(0), st(1)
            jbe @F
            fstp st(0)
            invoke MessageBox, hwnd, $CTA0("Эксцентриситет не может быть отрицательным!"), 
                           $CTA0("Ошибка"), MB_OK or MB_ICONERROR
            ret
        @@:
            fld1
            fcomip st(0), st(1)
            ja @F
            fstp st(0)
            invoke MessageBox, hwnd, $CTA0("Эксцентриситет должен быть меньше 1!"), 
                           $CTA0("Ошибка"), MB_OK or MB_ICONERROR
            ret
        @@:
            mov ebx, SelectedPlanet
            .if ebx == 1
                fstp Mercury_E_Coeff
            .elseif ebx == 2
                fstp Venus_E_Coeff
            .elseif ebx == 3
                fstp Earth_E_Coeff
            .elseif ebx == 4
                fstp Mars_E_Coeff
            .elseif ebx == 5
                fstp Jupiter_E_Coeff
            .elseif ebx == 6
                fstp Saturn_E_Coeff
            .endif
            
            invoke GetDlgItemText, hwnd, 203, addr float_buffer, 256
            invoke StrToFloat, addr float_buffer
            
            fldz
            fcomip st(0), st(1)
            jbe @F
            fstp st(0)
            invoke MessageBox, hwnd, $CTA0("Период должен быть положительным числом!"), 
                           $CTA0("Ошибка"), MB_OK or MB_ICONERROR
            ret
        @@:
            
            mov ebx, SelectedPlanet
            .if ebx == 1
                fstp Mercury_Alpha_Coeff
            .elseif ebx == 2
                fstp Venus_Alpha_Coeff
            .elseif ebx == 3
                fstp Earth_Alpha_Coeff
            .elseif ebx == 4
                fstp Mars_Alpha_Coeff
            .elseif ebx == 5
                fstp Jupiter_Alpha_Coeff
            .elseif ebx == 6
                fstp Saturn_Alpha_Coeff
            .endif

            invoke ReCreateSolarSystem
            invoke InvalidateRect, HwndMainWindow, NULL, TRUE
            
            invoke MessageBox, hwnd, $CTA0("Параметры успешно обновлены!"), 
                           $CTA0("Успех"), MB_OK or MB_ICONINFORMATION
            
            invoke EndDialog, hwnd, IDOK
            
        .elseif cax == IDCANCEL
            invoke EndDialog, hwnd, IDCANCEL
        .endif
        
    .elseif [iMsg] == WM_CLOSE
        invoke EndDialog, hwnd, IDCANCEL
        
    .endif
    
    xor eax, eax
    ret
ParamDialogProc endp

ShowPlanetDialog proc frame_macro
    lea cbx, PlanetDialogProc
    invoke DialogBoxParam, [hIns], 2001, [HwndMainWindow], 
                           cbx, 0
    ret
ShowPlanetDialog endp

ShowParamDialog proc frame_macro planetID:dword
    xor cax, cax
    mov eax, planetID
    lea cbx, ParamDialogProc
    invoke DialogBoxParam, [hIns], 2002, [HwndMainWindow], 
                           cbx, cax
    ret
ShowParamDialog endp

GetPlanetNameBySelected proc frame_macro
    .if SelectedPlanet == 1
        mov cax, $CTA0("Меркурий")
    .elseif SelectedPlanet == 2
        mov cax, $CTA0("Венера")        
    .elseif SelectedPlanet == 3
        mov cax, $CTA0("Земля")        
    .elseif SelectedPlanet == 4
        mov cax, $CTA0("Марс")        
    .elseif SelectedPlanet == 5
        mov cax, $CTA0("Юпитер")        
    .else
        mov cax, $CTA0("Сатурн")        
    .endif
    ret
GetPlanetNameBySelected endp

FloatToStr proc frame_macro floatAlpha:dword
    local temp:qword
    fld floatAlpha
    fstp qword ptr[temp]

    lea cbx, [float_buffer]
    lea csi, [fmt]
    invoke c_sprintf, cbx, csi, [temp]
    
    mov cax, offset float_buffer
    ret
FloatToStr endp

StrToFloat proc buffer:ptr byte
    local temp:dword
    invoke c_sscanf, buffer, $CTA0("%f"), addr temp
    fld temp
    ret
StrToFloat endp

end