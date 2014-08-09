; Listing generated by Microsoft (R) Optimizing Compiler Version 18.00.30501.0 

	TITLE	D:\Dropbox\Faks\6. semestar\SP\Projekat\VS\SP-Projekat-Repo\SP Projekat\Emulator\Core.c
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB MSVCRTD
INCLUDELIB OLDNAMES

PUBLIC	_brPrekid
PUBLIC	_work
PUBLIC	_prekid
_DATA	SEGMENT
COMM	_cpu:BYTE:018H
COMM	_memory:BYTE:08000H
COMM	_ir0:BYTE
COMM	_ir1:BYTE
_DATA	ENDS
_BSS	SEGMENT
_prekid	DB	01H DUP (?)
_BSS	ENDS
_DATA	SEGMENT
_brPrekid DB	0ffH
_work	DB	01H
_DATA	ENDS
PUBLIC	_Emulate
PUBLIC	_GetPA
PUBLIC	_LoadProgram
PUBLIC	??_C@_0BD@HGNMMEKI@Emulation?5stared?$CB?6?$AA@	; `string'
PUBLIC	??_C@_0M@DKLIDFJF@program?4bin?$AA@		; `string'
PUBLIC	??_C@_0CB@KJMJACDB@Flags?5N?5Z?5O?5C?6?5?5?5?5?5?5?$CFd?5?$CFd?5?$CFd?5?$CFd?6@ ; `string'
PUBLIC	??_C@_02JDPG@rb?$AA@				; `string'
PUBLIC	??_C@_0CE@CKPCJLBJ@Error?5in?5opening?5file?$CB?5Error?5cod@ ; `string'
EXTRN	__imp__fopen_s:PROC
EXTRN	__imp__fread:PROC
EXTRN	__imp__printf:PROC
EXTRN	_GetInfoFromByte:PROC
EXTRN	_GetFlag:PROC
EXTRN	_add:PROC
EXTRN	_sub:PROC
EXTRN	_mul:PROC
EXTRN	_div:PROC
EXTRN	_cmp:PROC
EXTRN	_and:PROC
EXTRN	_or:PROC
EXTRN	_not:PROC
EXTRN	_test:PROC
EXTRN	_ldr:PROC
EXTRN	_str:PROC
EXTRN	@_RTC_CheckStackVars@8:PROC
EXTRN	@__security_check_cookie@4:PROC
EXTRN	__RTC_CheckEsp:PROC
EXTRN	__RTC_InitBase:PROC
EXTRN	__RTC_Shutdown:PROC
EXTRN	___security_cookie:DWORD
;	COMDAT rtc$TMZ
rtc$TMZ	SEGMENT
__RTC_Shutdown.rtc$TMZ DD FLAT:__RTC_Shutdown
rtc$TMZ	ENDS
;	COMDAT rtc$IMZ
rtc$IMZ	SEGMENT
__RTC_InitBase.rtc$IMZ DD FLAT:__RTC_InitBase
rtc$IMZ	ENDS
;	COMDAT ??_C@_0CE@CKPCJLBJ@Error?5in?5opening?5file?$CB?5Error?5cod@
CONST	SEGMENT
??_C@_0CE@CKPCJLBJ@Error?5in?5opening?5file?$CB?5Error?5cod@ DB 'Error in'
	DB	' opening file! Error code:', 0aH, 00H	; `string'
CONST	ENDS
;	COMDAT ??_C@_02JDPG@rb?$AA@
CONST	SEGMENT
??_C@_02JDPG@rb?$AA@ DB 'rb', 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_0CB@KJMJACDB@Flags?5N?5Z?5O?5C?6?5?5?5?5?5?5?$CFd?5?$CFd?5?$CFd?5?$CFd?6@
CONST	SEGMENT
??_C@_0CB@KJMJACDB@Flags?5N?5Z?5O?5C?6?5?5?5?5?5?5?$CFd?5?$CFd?5?$CFd?5?$CFd?6@ DB 'F'
	DB	'lags N Z O C', 0aH, '      %d %d %d %d', 0aH, 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_0M@DKLIDFJF@program?4bin?$AA@
CONST	SEGMENT
??_C@_0M@DKLIDFJF@program?4bin?$AA@ DB 'program.bin', 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_0BD@HGNMMEKI@Emulation?5stared?$CB?6?$AA@
CONST	SEGMENT
??_C@_0BD@HGNMMEKI@Emulation?5stared?$CB?6?$AA@ DB 'Emulation stared!', 0aH
	DB	00H						; `string'
CONST	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File d:\dropbox\faks\6. semestar\sp\projekat\vs\sp-projekat-repo\sp projekat\emulator\core.c
;	COMDAT _LoadProgram
_TEXT	SEGMENT
_entryAddr$ = -24					; size = 2
_file$ = -12						; size = 4
__$ArrayPad$ = -4					; size = 4
_fileName$ = 8						; size = 4
_LoadProgram PROC					; COMDAT

; 121  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 220				; 000000dcH
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-220]
	mov	ecx, 55					; 00000037H
	mov	eax, -858993460				; ccccccccH
	rep stosd
	mov	eax, DWORD PTR ___security_cookie
	xor	eax, ebp
	mov	DWORD PTR __$ArrayPad$[ebp], eax

; 122  : 	FILE *file = NULL;

	mov	DWORD PTR _file$[ebp], 0

; 123  : 	//int error = fopen_s(&file, fileName, "rb");
; 124  : #ifdef __gnu_linux__
; 125  : 	file = fopen(fileName, "rb");
; 126  : #else
; 127  : #ifdef _WIN32
; 128  : 	fopen_s(&file, fileName, "rb");

	mov	esi, esp
	push	OFFSET ??_C@_02JDPG@rb?$AA@
	mov	eax, DWORD PTR _fileName$[ebp]
	push	eax
	lea	ecx, DWORD PTR _file$[ebp]
	push	ecx
	call	DWORD PTR __imp__fopen_s
	add	esp, 12					; 0000000cH
	cmp	esi, esp
	call	__RTC_CheckEsp

; 129  : #endif
; 130  : #endif
; 131  : 
; 132  : 
; 133  : 
; 134  : 	ADDR entryAddr = 0;

	xor	eax, eax
	mov	WORD PTR _entryAddr$[ebp], ax

; 135  : 
; 136  : 	if (file == NULL)

	cmp	DWORD PTR _file$[ebp], 0
	jne	SHORT $LN1@LoadProgra

; 137  : 	{
; 138  : 		printf("Error in opening file! Error code:\n");

	mov	esi, esp
	push	OFFSET ??_C@_0CE@CKPCJLBJ@Error?5in?5opening?5file?$CB?5Error?5cod@
	call	DWORD PTR __imp__printf
	add	esp, 4
	cmp	esi, esp
	call	__RTC_CheckEsp

; 139  : 		return -1;

	mov	eax, 65535				; 0000ffffH
	jmp	SHORT $LN2@LoadProgra
$LN1@LoadProgra:

; 140  : 	}
; 141  : 
; 142  : 	fread(memory + entryAddr, sizeof(BYTE), 18, file);

	mov	esi, esp
	mov	eax, DWORD PTR _file$[ebp]
	push	eax
	push	18					; 00000012H
	push	1
	movzx	ecx, WORD PTR _entryAddr$[ebp]
	add	ecx, OFFSET _memory
	push	ecx
	call	DWORD PTR __imp__fread
	add	esp, 16					; 00000010H
	cmp	esi, esp
	call	__RTC_CheckEsp

; 143  : 
; 144  : 	return entryAddr;

	mov	ax, WORD PTR _entryAddr$[ebp]
$LN2@LoadProgra:

; 145  : }

	push	edx
	mov	ecx, ebp
	push	eax
	lea	edx, DWORD PTR $LN6@LoadProgra
	call	@_RTC_CheckStackVars@8
	pop	eax
	pop	edx
	pop	edi
	pop	esi
	pop	ebx
	mov	ecx, DWORD PTR __$ArrayPad$[ebp]
	xor	ecx, ebp
	call	@__security_check_cookie@4
	add	esp, 220				; 000000dcH
	cmp	ebp, esp
	call	__RTC_CheckEsp
	mov	esp, ebp
	pop	ebp
	ret	0
$LN6@LoadProgra:
	DD	1
	DD	$LN5@LoadProgra
$LN5@LoadProgra:
	DD	-12					; fffffff4H
	DD	4
	DD	$LN4@LoadProgra
$LN4@LoadProgra:
	DB	102					; 00000066H
	DB	105					; 00000069H
	DB	108					; 0000006cH
	DB	101					; 00000065H
	DB	0
_LoadProgram ENDP
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File d:\dropbox\faks\6. semestar\sp\projekat\vs\sp-projekat-repo\sp projekat\emulator\core.c
;	COMDAT _GetPA
_TEXT	SEGMENT
_vaddr$ = 8						; size = 2
_GetPA	PROC						; COMDAT

; 15   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 192				; 000000c0H
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-192]
	mov	ecx, 48					; 00000030H
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 16   : 	if (!GetFlag(VM))

	push	15					; 0000000fH
	call	_GetFlag
	add	esp, 4
	test	eax, eax
	jne	SHORT $LN2@GetPA

; 17   : 		return vaddr;

	mov	ax, WORD PTR _vaddr$[ebp]
	jmp	SHORT $LN3@GetPA

; 18   : 	else

	jmp	SHORT $LN3@GetPA
$LN2@GetPA:

; 19   : 	{
; 20   : 		prekid = 1;

	mov	BYTE PTR _prekid, 1

; 21   : 		return -1;

	mov	eax, 65535				; 0000ffffH
$LN3@GetPA:

; 22   : 	}
; 23   : }

	pop	edi
	pop	esi
	pop	ebx
	add	esp, 192				; 000000c0H
	cmp	ebp, esp
	call	__RTC_CheckEsp
	mov	esp, ebp
	pop	ebp
	ret	0
_GetPA	ENDP
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File d:\dropbox\faks\6. semestar\sp\projekat\vs\sp-projekat-repo\sp projekat\emulator\core.c
;	COMDAT _Emulate
_TEXT	SEGMENT
tv88 = -268						; size = 4
_noPageFault$1 = -65					; size = 1
_opCode$ = -53						; size = 1
_z$ = -41						; size = 1
_n$ = -29						; size = 1
_c$ = -17						; size = 1
_o$ = -5						; size = 1
_Emulate PROC						; COMDAT

; 26   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 268				; 0000010cH
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-268]
	mov	ecx, 67					; 00000043H
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 27   : 	printf("Emulation stared!\n");

	mov	esi, esp
	push	OFFSET ??_C@_0BD@HGNMMEKI@Emulation?5stared?$CB?6?$AA@
	call	DWORD PTR __imp__printf
	add	esp, 4
	cmp	esi, esp
	call	__RTC_CheckEsp

; 28   : 
; 29   : 	cpu.pc = LoadProgram("program.bin");

	push	OFFSET ??_C@_0M@DKLIDFJF@program?4bin?$AA@
	call	_LoadProgram
	add	esp, 4
	mov	WORD PTR _cpu+16, ax

; 30   : 
; 31   : 	if (cpu.pc == -1)

	movsx	eax, WORD PTR _cpu+16
	cmp	eax, -1
	jne	SHORT $LN20@Emulate

; 32   : 		return;

	jmp	$LN21@Emulate
$LN20@Emulate:

; 33   : 
; 34   : 	char o, c, n, z;
; 35   : 	BYTE opCode;
; 36   : 
; 37   : 	//cpu.r[0] = -32768;
; 38   : 	cpu.r[1] = -1;

	mov	eax, 2
	shl	eax, 0
	or	ecx, -1
	mov	WORD PTR _cpu[eax], cx
$LN19@Emulate:

; 39   : 
; 40   : 	while (work)

	movsx	eax, BYTE PTR _work
	test	eax, eax
	je	$LN21@Emulate

; 41   : 	{
; 42   : 		char noPageFault = 1;

	mov	BYTE PTR _noPageFault$1[ebp], 1

; 43   : 
; 44   : 		ir1 = memory[cpu.pc];

	movsx	eax, WORD PTR _cpu+16
	mov	cl, BYTE PTR _memory[eax]
	mov	BYTE PTR _ir1, cl

; 45   : 		ir0 = memory[cpu.pc + 1];

	movsx	eax, WORD PTR _cpu+16
	mov	cl, BYTE PTR _memory[eax+1]
	mov	BYTE PTR _ir0, cl

; 46   : 		cpu.pc += 2;

	movsx	eax, WORD PTR _cpu+16
	add	eax, 2
	mov	WORD PTR _cpu+16, ax

; 47   : 
; 48   : 		opCode = GetInfoFromByte(7, 3, ir1);

	movzx	eax, BYTE PTR _ir1
	push	eax
	push	3
	push	7
	call	_GetInfoFromByte
	add	esp, 12					; 0000000cH
	mov	BYTE PTR _opCode$[ebp], al

; 49   : 
; 50   : 		switch (opCode)

	movsx	eax, BYTE PTR _opCode$[ebp]
	mov	DWORD PTR tv88[ebp], eax
	cmp	DWORD PTR tv88[ebp], 10			; 0000000aH
	ja	$LN16@Emulate
	mov	ecx, DWORD PTR tv88[ebp]
	jmp	DWORD PTR $LN23@Emulate[ecx*4]
$LN15@Emulate:

; 51   : 		{
; 52   : 		case 0:
; 53   : 			add();

	call	_add

; 54   : 			break;

	jmp	SHORT $LN16@Emulate
$LN14@Emulate:

; 55   : 		case 1:
; 56   : 			sub();

	call	_sub

; 57   : 			break;

	jmp	SHORT $LN16@Emulate
$LN13@Emulate:

; 58   : 		case 2:
; 59   : 			mul();

	call	_mul

; 60   : 			break;

	jmp	SHORT $LN16@Emulate
$LN12@Emulate:

; 61   : 		case 3:
; 62   : 			div();

	call	_div

; 63   : 			break;

	jmp	SHORT $LN16@Emulate
$LN11@Emulate:

; 64   : 		case 4:
; 65   : 			cmp();

	call	_cmp

; 66   : 			break;

	jmp	SHORT $LN16@Emulate
$LN10@Emulate:

; 67   : 		case 5:
; 68   : 			and();

	call	_and

; 69   : 			break;

	jmp	SHORT $LN16@Emulate
$LN9@Emulate:

; 70   : 		case 6:
; 71   : 			or();

	call	_or

; 72   : 			break;

	jmp	SHORT $LN16@Emulate
$LN8@Emulate:

; 73   : 		case 7:
; 74   : 			not();

	call	_not

; 75   : 			break;

	jmp	SHORT $LN16@Emulate
$LN7@Emulate:

; 76   : 		case 8:
; 77   : 			test();

	call	_test

; 78   : 			break;

	jmp	SHORT $LN16@Emulate
$LN6@Emulate:

; 79   : 		case 9:
; 80   : 			noPageFault = ldr();

	call	_ldr
	mov	BYTE PTR _noPageFault$1[ebp], al

; 81   : 			if (!noPageFault)

	movsx	eax, BYTE PTR _noPageFault$1[ebp]
	test	eax, eax
	jne	SHORT $LN5@Emulate

; 82   : 			{
; 83   : 				prekid = 1;

	mov	BYTE PTR _prekid, 1

; 84   : 				brPrekid = 1;

	mov	BYTE PTR _brPrekid, 1

; 85   : 				goto PREKID;

	jmp	$PREKID$24
$LN5@Emulate:

; 86   : 			}
; 87   : 			break;

	jmp	SHORT $LN16@Emulate
$LN4@Emulate:

; 88   : 		case 10:
; 89   : 			noPageFault = str();

	call	_str
	mov	BYTE PTR _noPageFault$1[ebp], al

; 90   : 			if (!noPageFault)

	movsx	eax, BYTE PTR _noPageFault$1[ebp]
	test	eax, eax
	jne	SHORT $LN16@Emulate

; 91   : 			{
; 92   : 				prekid = 1;

	mov	BYTE PTR _prekid, 1

; 93   : 				brPrekid = 1;

	mov	BYTE PTR _brPrekid, 1

; 94   : 				goto PREKID;

	jmp	SHORT $PREKID$24
$LN16@Emulate:

; 95   : 			}
; 96   : 			break;
; 97   : 		}
; 98   : 
; 99   : 		z = GetFlag(Z);

	push	0
	call	_GetFlag
	add	esp, 4
	mov	BYTE PTR _z$[ebp], al

; 100  : 		n = GetFlag(N);

	push	3
	call	_GetFlag
	add	esp, 4
	mov	BYTE PTR _n$[ebp], al

; 101  : 		c = GetFlag(C);

	push	2
	call	_GetFlag
	add	esp, 4
	mov	BYTE PTR _c$[ebp], al

; 102  : 		o = GetFlag(O);

	push	1
	call	_GetFlag
	add	esp, 4
	mov	BYTE PTR _o$[ebp], al

; 103  : 
; 104  : 		printf("Flags N Z O C\n      %d %d %d %d\n", n, z, o, c);

	movsx	eax, BYTE PTR _c$[ebp]
	mov	esi, esp
	push	eax
	movsx	ecx, BYTE PTR _o$[ebp]
	push	ecx
	movsx	edx, BYTE PTR _z$[ebp]
	push	edx
	movsx	eax, BYTE PTR _n$[ebp]
	push	eax
	push	OFFSET ??_C@_0CB@KJMJACDB@Flags?5N?5Z?5O?5C?6?5?5?5?5?5?5?$CFd?5?$CFd?5?$CFd?5?$CFd?6@
	call	DWORD PTR __imp__printf
	add	esp, 20					; 00000014H
	cmp	esi, esp
	call	__RTC_CheckEsp
$PREKID$24:

; 105  : 
; 106  : 	PREKID:
; 107  : 		if (prekid)

	movsx	eax, BYTE PTR _prekid
	test	eax, eax
	je	SHORT $LN2@Emulate

; 108  : 		{
; 109  : 			if (brPrekid == 1)

	movzx	eax, BYTE PTR _brPrekid
	cmp	eax, 1
	jne	SHORT $LN1@Emulate

; 110  : 				cpu.pc -= 2;

	movsx	eax, WORD PTR _cpu+16
	sub	eax, 2
	mov	WORD PTR _cpu+16, ax
$LN1@Emulate:

; 111  : 
; 112  : 			// save context
; 113  : 
; 114  : 			cpu.pc = brPrekid << 1;

	movzx	eax, BYTE PTR _brPrekid
	shl	eax, 1
	mov	WORD PTR _cpu+16, ax
$LN2@Emulate:

; 115  : 		}
; 116  : 	}

	jmp	$LN19@Emulate
$LN21@Emulate:

; 117  : }

	pop	edi
	pop	esi
	pop	ebx
	add	esp, 268				; 0000010cH
	cmp	ebp, esp
	call	__RTC_CheckEsp
	mov	esp, ebp
	pop	ebp
	ret	0
	npad	3
$LN23@Emulate:
	DD	$LN15@Emulate
	DD	$LN14@Emulate
	DD	$LN13@Emulate
	DD	$LN12@Emulate
	DD	$LN11@Emulate
	DD	$LN10@Emulate
	DD	$LN9@Emulate
	DD	$LN8@Emulate
	DD	$LN7@Emulate
	DD	$LN6@Emulate
	DD	$LN4@Emulate
_Emulate ENDP
_TEXT	ENDS
END
