; This file is assembly for MSVC x64.
; Call cpuid.

.code

; C++: extern "C" void cpuid_msvc_asmx64(int* p, int i)
; p -> rcx
; i -> rdx
public cpuid_msvc_asmx64

cpuid_msvc_asmx64 PROC
    
    push rbx
    push rsi

    mov rsi, rcx    ; p to rsi
    mov eax, edx    ; i to eax

    xor ecx, ecx    ; clear rcx

    cpuid

    mov dword ptr [rsi],        eax ; p[0] = eax
    mov dword ptr [rsi + 4],    ebx ; p[1] = ebx
    mov dword ptr [rsi + 8],    ecx ; p[2] = ecx
    mov dword ptr [rsi + 12],   edx ; p[3] = edx

    pop rsi
    pop rbx
    
    ret

cpuid_msvc_asmx64 ENDP

END
