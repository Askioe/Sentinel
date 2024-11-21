.code

extern vmexit_handler : proc

SAVE_GP macro
        push    rax
        push    rcx
        push    rdx
        push    rbx
        push    rbp
        push    rsi
        push    rdi
        push    r8
        push    r9
        push    r10
        push    r11
        push    r12
        push    r13
        push    r14
        push    r15
endm

RESTORE_GP macro
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rdi
        pop     rsi
        pop     rbp
        pop     rbx
        pop     rdx
        pop     rcx
        pop     rax
endm

PUBLIC __read_ldtr
PUBLIC __read_tr
PUBLIC __read_cs
PUBLIC __read_ss
PUBLIC __read_ds
PUBLIC __read_es
PUBLIC __read_fs
PUBLIC __read_gs
PUBLIC __load_ar
PUBLIC vmm_entrypoint
PUBLIC __read_rip
PUBLIC __read_rsp

__read_rip proc
    mov rax, [rsp]
    ret
__read_rip endp
__read_rsp proc
    mov rax, rsp
    add rax, 8h
    ret
__read_rsp endp

__read_ldtr proc
        sldt    ax
        ret
__read_ldtr endp

__read_tr proc
        str     ax
        ret
__read_tr endp

__read_cs proc
        mov     ax, cs
        ret
__read_cs endp

__read_ss proc
        mov     ax, ss
        ret
__read_ss endp

__read_ds proc
        mov     ax, ds
        ret
__read_ds endp

__read_es proc
        mov     ax, es
        ret
__read_es endp

__read_fs proc
        mov     ax, fs
        ret
__read_fs endp

__read_gs proc
        mov     ax, gs
        ret
__read_gs endp

__load_ar proc
        lar     rax, rcx
        jz      no_error
        xor     rax, rax
no_error:
        ret
__load_ar endp


vmm_entrypoint proc
        SAVE_GP

        sub     rsp, 68h
        movaps  xmmword ptr [rsp +  0h], xmm0
        movaps  xmmword ptr [rsp + 10h], xmm1
        movaps  xmmword ptr [rsp + 20h], xmm2
        movaps  xmmword ptr [rsp + 30h], xmm3
        movaps  xmmword ptr [rsp + 40h], xmm4
        movaps  xmmword ptr [rsp + 50h], xmm5

        mov     rcx, rsp
        sub     rsp, 20h
        call    vmexit_handler
        add     rsp, 20h

        movaps  xmm0, xmmword ptr [rsp +  0h]
        movaps  xmm1, xmmword ptr [rsp + 10h]
        movaps  xmm2, xmmword ptr [rsp + 20h]
        movaps  xmm3, xmmword ptr [rsp + 30h]
        movaps  xmm4, xmmword ptr [rsp + 40h]
        movaps  xmm5, xmmword ptr [rsp + 50h]
        add     rsp, 68h

        test    al, al
        jz      exit
        RESTORE_GP
        vmresume
        jmp     vmerror

exit:
        RESTORE_GP
        vmxoff
        jz      vmerror
        jc      vmerror
        push    r8
        popf
        mov     rsp, rdx
        push    rcx
        ret

vmerror:
        int 3
vmm_entrypoint endp



PUBLIC _sgdt

_sgdt PROC
    sgdt [rcx]
    ret
_sgdt ENDP
END

end