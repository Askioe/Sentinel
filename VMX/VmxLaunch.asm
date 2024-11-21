.code

; bool __vm_launch();
?vm_launch@@YA_NXZ proc
  ; set VMCS_GUEST_RIP to the address of <successful_launch>
  mov rax, 681Eh
  mov rdx, successful_launch
  vmwrite rax, rdx

  vmlaunch

  ; if we reached here, then we failed to launch
  xor al, al
  ret

successful_launch:
  mov al, 1
  ret
?vm_launch@@YA_NXZ endp

end

