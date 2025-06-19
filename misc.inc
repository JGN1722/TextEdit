	; Counts the digits of a number
	; Arguments: number
      DigitCount:
	push	ebp
	mov	ebp, esp
	push	ebx

	mov	eax, DWORD [ebp + 8]

	cmp	eax, 0
	jne	.begin
	mov	ecx, 1
	jmp	.end

       .begin:
	mov	ecx, 0
	mov	ebx, 10

       .count_loop:
	xor	edx, edx
	div	ebx
	inc	ecx

	cmp	eax, 0
	jne	.count_loop

       .end:
	mov	eax, ecx
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	4


	; Converts two unsigned ints to strings in the
	; format num1:num2
	; Arguments: number1, number2
      LineCoords:
	push	ebp
	mov	ebp, esp

	sub	esp, 4

	mov	DWORD [esp], 2
	stdcall DigitCount, DWORD [ebp + 8]
	mov	ebx, eax
	add	DWORD [esp], eax
	stdcall DigitCount, DWORD [ebp + 12]
	add	DWORD [esp], eax

	invoke	HeapAlloc, [hHeap], HEAP_ZERO_MEMORY, DWORD [esp]
	mov	DWORD [esp], eax

	stdcall CStr, DWORD [ebp + 8], eax

	mov	eax, DWORD [esp]
	add	eax, ebx
	mov	BYTE [eax], ':'
	inc	eax

	stdcall CStr, DWORD [ebp + 12], eax

	pop	eax

	mov	esp, ebp
	pop	ebp
	ret	8



	; Writes the string representation of an unsigned
	; number at the specified address
	; Arguments: number, addr
      CStr:
	push	ebp
	mov	ebp, esp

	push	ebx

	stdcall DigitCount, DWORD [ebp + 8]
	push	eax

	xor	ecx, ecx
	mov	edi, DWORD [ebp + 12]
	add	edi, DWORD [esp]
	dec	edi
	add	esp, 4
	
	mov	eax, DWORD [ebp + 8]
	mov	ebx, 10
	
	cmp	eax, 10
	jb	@f
	
       .convert:
	
	xor	edx, edx
	div	ebx
	
	add	dl, '0'
	mov	BYTE [edi], dl
	inc	ecx
	dec	edi
	
	cmp	eax, 10
	ja	.convert
	
       @@:
	inc	ecx
	add	al, '0'
	mov	BYTE [edi], al

	pop	ebx

	mov	esp, ebp
	pop	ebp
	ret	8