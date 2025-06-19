	; Gets the argument count
	; Arguments: None
      GetArgumentCount:
	push	ebp
	mov	ebp, esp

	invoke	GetCommandLine
	mov	ecx, 0
	
      .count:
	cmp	BYTE [eax], 0
	je	.end_count
	
	cmp	BYTE [eax], 22h ; quote
	je	.skipoverquoted
	
	cmp	BYTE [eax], 20h ; space
	je	.newarg
	cmp	BYTE [eax], 09h ; tab
	je	.newarg
	
	inc	eax
	jmp	.count
	
      .skipoverquoted:
	inc	eax
	cmp	BYTE [eax], 22h
	je	@f
	jmp	.skipoverquoted
      @@:
	inc	eax
	jmp	.count
	
      .newarg:
	inc	eax
	
      @@:
	cmp	BYTE [eax], 20h
	jne	@f
	inc	eax
	jmp	@b
	
      @@:
	
	cmp	BYTE [eax], 0
	je	.end_count
	
	inc	ecx
	jmp	.count
	
      .end_count:
	inc	ecx
	
	mov	eax, ecx
	
	mov	esp, ebp
	pop	ebp
	ret

	; Gets the argument at the specified position
	; Arguments: index
      GetArgument:
	push	ebp
	mov	ebp, esp
	
	invoke	GetCommandLine
	mov	ecx, DWORD [ebp + 8]
	
      .count:
	cmp	ecx, 0
	je	.argument_found
	
	cmp	BYTE [eax], 22h ; quote
	je	.skipoverquoted
	
	cmp	BYTE [eax], 20h ; space
	je	.newarg
	cmp	BYTE [eax], 09h ; tab
	je	.newarg
	
	inc	eax
	jmp	.count
	
      .skipoverquoted:
	inc	eax
	cmp	BYTE [eax], 22h
	je	@f
	jmp	.skipoverquoted
      @@:
	inc	eax
	jmp	.count
	
      .newarg:
	inc	eax
	
      @@:
	cmp	BYTE [eax], 20h
	jne	@f
	inc	eax
	jmp	@b
      @@:
	dec	ecx
	jmp	.count
	
      .argument_found:
	
	push	eax
	inc	ecx
	
	cmp	BYTE [eax], 22h
	je	.quoted
	
      @@:
	inc	eax
	cmp	BYTE [eax], 20h
	je	.copy_in_buffer
	cmp	BYTE [eax], 0
	je	.copy_in_buffer
	inc	ecx
	jmp	@b
	
      .quoted:
	inc	eax
	inc	DWORD [esp]
      @@:
	inc	eax
	cmp	BYTE [eax], 22h
	je	.copy_in_buffer
	inc	ecx
	jmp	@b
	
      .copy_in_buffer:
	push	ecx
	inc	ecx
	invoke	HeapAlloc,[hHeap],HEAP_ZERO_MEMORY,ecx
	pop	ecx
	mov	edi, eax
	mov	esi, DWORD [esp]
	rep movsb
	
	mov	esp, ebp
	pop	ebp

	pop	ebx
	push	ebx
	ret	4
