;
; RAX, R10, R11 (EAX, ECX, EDX for x86) are the only registers we can edit during a dispatching.
; For fastcall in x86, EAX is only one we can edit.
; Luckily, most APIs in Windows are using stdcall.
;

; struct KEX_VFT_REPLACEMENT_WRAPPER (x64) {
; 	struct Bundles {
; 	 	pOriginalVtbl (Pointer)
; 	 	pThis (Pointer)
;	 	ExtraData (Unknown Size)
; 	}
; 	pBundles (Pointer)				 (p - 40)
; 	ModuleAddrStart (Pointer)		 (p - 32)
; 	ModuleAddrEnd (Pointer)			 (p - 24)
; 	pOriginalVtbl (Pointer)			 (p - 16)
; 	pModifiedVtbl (Pointer)			 (p - 8)
; 	VWrapVtblTable (Pointer)*numFunc (p - 0)
; 	ModifiedVtbl (Pointer)*numFunc
; }


WRAPCALLTABLE_SIZE = 192

IFDEF RAX
_TEXT SEGMENT

GENERATE_VTBL_WRAPCALL MACRO ID4
ALIGN 4
@CatStr(<__vwrapcall>, %ID4):
	mov r10, [rcx]				;This->lpVtbl
	mov rcx, [r10-40]			;This = Bundles
	mov r10, [r10-8]			;ModifiedVtbl
	jmp qword ptr [r10+ID4*8]
ENDM
	
	ALIGN 4
ExternalOnlyVWrapcall:
	mov   rax, [rsp]				;RetAddr
	mov   r10, [rcx]				;This->lpVtbl
	movzx r11, r11b					;r11 = (uint64)r11b

	; Determine if the caller function is within the target module.
	; If it is, go to the original implementation.
	;
	;	if(ModuleAddrStart <= RetAddr && ModuleAddrEnd > RetAddr)
	;		CallOriginalFunction();
	cmp [r10-32], rax			;ModuleAddrStart
	jg  CallModifiedFunc
	cmp [r10-24], rax			;ModuleAddrEnd
	jle CallModifiedFunc
	mov r10, [r10-16]			;OriginalVtbl
	jmp qword ptr [r10+r11*8]
	
CallModifiedFunc:
	mov rcx, [r10-40]			;This = Bundles
	mov r10, [r10-8]			;ModifiedVtbl
	jmp qword ptr [r10+r11*8]
	
GENERATE_EXTERNALONLY_VTBL_WRAPCALL MACRO ID2
ALIGN 4
@CatStr(<__extr_vwrapcall>, %ID2):
	; We want to save some bytes for each functions
	; so we can have lesser space taken

	mov r11b, ID2
	jmp ExternalOnlyVWrapcall
ENDM

ELSE
_TEXT SEGMENT USE32

GENERATE_VTBL_WRAPCALL MACRO ID5
ALIGN 4
@CatStr(<__vwrapcall>, %ID5):
	mov eax, [esp+4]			;This
	mov ecx, [eax]				;This->lpVtbl
	mov eax, [ecx-4]			;ModifiedVtbl
	mov ecx, [ecx-20]			;pBundles
	mov [esp+4], ecx			;[This] = pBundles
	jmp dword ptr [eax+ID5*4]
ENDM

	ALIGN 4
ExternalOnlyWrapcallX86:
	mov   eax, [esp]				;RetAddr
	mov   ecx, [esp+4]				;This
	mov   ecx, [ecx]				;This->lpVtbl
	movzx edx, dl					;edx = (uint32)dl

	cmp [ecx-16], eax			;ModuleAddrStart
	jg  CallModifiedFuncX86
	cmp [ecx-12], eax			;ModuleAddrEnd
	jle CallModifiedFuncX86
	mov ecx, [ecx-8]			;OriginalVtbl
	jmp dword ptr [ecx+edx*4]

CallModifiedFuncX86:
	mov eax, [ecx-4]			;ModifiedVtbl
	mov ecx, [ecx-20]			;pBundles
	mov [esp+4], ecx			;[This] = pBundles
	jmp dword ptr [eax+edx*4]

GENERATE_EXTERNALONLY_VTBL_WRAPCALL MACRO ID3
ALIGN 4
@CatStr(<__extr_vwrapcall>, %ID3):
	mov dl, ID3
	jmp ExternalOnlyWrapcallX86
ENDM

ENDIF

ALIGN 8

COUNTER = 0
REPEAT WRAPCALLTABLE_SIZE
	GENERATE_EXTERNALONLY_VTBL_WRAPCALL %COUNTER
	COUNTER = COUNTER + 1
ENDM

COUNTER = 0
REPEAT WRAPCALLTABLE_SIZE
	GENERATE_VTBL_WRAPCALL %COUNTER
	COUNTER = COUNTER + 1
ENDM

_TEXT ENDS



IFDEF RAX
CONST SEGMENT READONLY

PUBLIC VWrapCallFuncTable
VWrapCallFuncTable DQ OFFSET __vwrapcall0
COUNTER = 1
REPEAT (WRAPCALLTABLE_SIZE - 1)
	DQ OFFSET @CatStr(<__vwrapcall>, %COUNTER)
	COUNTER = COUNTER + 1
ENDM

PUBLIC ExternalOnlyVWrapCallFuncTable
ExternalOnlyVWrapCallFuncTable DQ OFFSET __extr_vwrapcall0
COUNTER = 1
REPEAT (WRAPCALLTABLE_SIZE - 1)
	DQ OFFSET @CatStr(<__extr_vwrapcall>, %COUNTER)
	COUNTER = COUNTER + 1
ENDM

ELSE
CONST SEGMENT USE32 READONLY

PUBLIC _VWrapCallFuncTable
_VWrapCallFuncTable DD OFFSET __vwrapcall0
COUNTER = 1
REPEAT (WRAPCALLTABLE_SIZE - 1)
	DD OFFSET @CatStr(<__vwrapcall>, %COUNTER)
	COUNTER = COUNTER + 1
ENDM

PUBLIC _ExternalOnlyVWrapCallFuncTable
_ExternalOnlyVWrapCallFuncTable DD OFFSET __extr_vwrapcall0
COUNTER = 1
REPEAT (WRAPCALLTABLE_SIZE - 1)
	DD OFFSET @CatStr(<__extr_vwrapcall>, %COUNTER)
	COUNTER = COUNTER + 1
ENDM

ENDIF

CONST ENDS
END