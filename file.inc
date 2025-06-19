	; Returns the full path name of a file relative to the working directory
	; Arguments: file_name
      GetFullPath:
	push	ebp
	mov	ebp, esp

	invoke	HeapAlloc, [hHeap], HEAP_ZERO_MEMORY, MAX_PATH + 1
	push	eax
	invoke	GetFullPathName, DWORD [ebp + 8], MAX_PATH, eax, NULL
	pop	eax

	mov	esp, ebp
	pop	ebp
	ret	4

	; Prompts the user to choose a file to open
	; Arguments: None
      OpenFileDialog:
	push	ebp
	mov	ebp, esp

	call	BasicFileOpen
	invoke	MessageBox, 0, _not_implemented, _error, MB_ICONERROR+MB_OK

	mov	esp, ebp
	pop	ebp
	ret

	; Returns 0 if the file does not exist
	; Arguments: file_name
      FileExists:
	push	ebp
	mov	ebp, esp

	invoke	GetFileAttributes, DWORD [ebp + 8]

	mov	ebx, 0
	cmp	eax, -1 ; INVALID_FILE_ATTRIBUTES
	setne	bl
	and	eax, FILE_ATTRIBUTE_DIRECTORY
	not	eax
	and	eax, ebx

	mov	esp, ebp
	pop	ebp

	pop	ebx
	push	ebx
	ret	4

	; Opens a file, returns 0 on error. The argument should not be freed.
	; Arguments: file_name
      OpenExistingFile:
	push	ebp
	mov	ebp, esp

	stdcall AskToSave
	cmp	eax, FALSE
	je     .end

       .already_saved:
	push	DWORD [ebp + 8]
	call	FileExists
	cmp	eax, FALSE
	je	.non_existent_file

	mov	eax, DWORD [ebp + 8]
	mov	DWORD [FileName], eax
	mov	BYTE [IsFileDefault], FALSE

	invoke	CreateFile, DWORD [FileName], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	test	eax, eax
	jz	errors.file_open_error
	push	eax

	invoke	GetFileSize, eax, NULL
	mov	ebx, eax
	inc	eax ; For the null terminator
	invoke	HeapAlloc, [hHeap], HEAP_ZERO_MEMORY, eax
	push	eax

	invoke	ReadFile, DWORD [ebp - 4], eax, ebx, NULL, NULL
	test	eax, eax
	jz	errors.file_open_error

	invoke	CloseHandle, DWORD [ebp - 4]

	invoke	SetWindowText, [hEdit], DWORD [ebp - 8]
	test	eax, eax
	jz	errors.file_open_error

	pop	eax
	invoke	HeapFree, [hHeap], 0, eax

	invoke	SendMessage, [hStatus], SB_SETTEXT, 2, DWORD [ebp + 8]

	; Reset the "modified" flag to saved because the WM_SETTEXT sends EN_CHANGE
	invoke	PostMessage, [hWnd], MSG_SETSAVED, 0, 0
	stdcall DisplayTextCoords


	mov	eax, 1
	jmp	.end

      .non_existent_file:
	invoke	MessageBox, 0, _warning_non_existent_file, _warning, MB_ICONWARNING+MB_YESNOCANCEL
	cmp	eax, IDCANCEL
	je	.abort
	cmp	eax, IDYES
	jne	.end
	stdcall CreateNonExistentFile, DWORD [ebp + 8]
	stdcall OpenExistingFile, DWORD [ebp + 8]
	jmp	.end

      .abort:
	invoke	SendMessage,[hWnd],WM_CLOSE,0,0

      .end:
	mov	esp, ebp
	pop	ebp
	ret	4

	; Creates a non-existent file
	; Arguments: filename
      CreateNonExistentFile:
	push	ebp
	mov	ebp, esp

	invoke	CreateFileA, DWORD [ebp + 8], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
	test	eax, eax
	jz	errors.file_create_error
	invoke	CloseHandle, eax

	mov	esp, ebp
	pop	ebp
	ret	4

	; Opens a non-existent file
	; Arguments: None
      OpenDefaultFile:
	push	ebp
	mov	ebp, esp

	stdcall AskToSave
	cmp	eax, 0
	je	.end

	invoke	SendMessage, [hEdit], WM_SETTEXT, 0, 0
	cmp	BYTE [IsFileDefault], TRUE
	je	@f
	invoke	HeapFree, [hHeap], 0, [FileName]
	mov	BYTE [IsFileDefault], TRUE
      @@:
	mov	[FileName], NULL

	; Set the text of the status bar
	invoke	SendMessage, [hStatus], SB_SETTEXT, 2, _no_file

	; Reset the "modified" flag to saved because the WM_SETTEXT sends EN_CHANGE
	invoke	PostMessage, [hWnd], MSG_SETSAVED, 0, 0
	stdcall DisplayTextCoords

      .end:
	mov	esp, ebp
	pop	ebp
	ret


	; Asks the user if he wants to save the file in
	; case it's not already saved
	; Arguments: None
	; Returned value: 0 if the operation is canceled, 1 otherwise
      AskToSave:
	push	ebp
	mov	ebp, esp

	cmp	BYTE [IsFileSaved], TRUE
	je	.end_with_1

	cmp	BYTE [IsFileDefault], TRUE
	jne	.not_default
	invoke	MessageBox,[hWnd],_warning_not_saved_default,_warning,MB_ICONWARNING+MB_YESNOCANCEL+MB_SETFOREGROUND+MB_APPLMODAL
	cmp	eax, IDCANCEL
	je	.end_with_0
	cmp	eax, IDNO
	je	.end_with_1

	; TODO: actually save the file
	invoke	MessageBox, 0, _not_implemented, _not_implemented, 0
	jmp	.end_with_1

       .not_default:
	invoke	MessageBox, 0, _warning_not_saved, _warning, MB_ICONWARNING+MB_YESNOCANCEL
	cmp	eax, IDCANCEL
	je	.end_with_0
	cmp	eax, IDNO
	je	.end_with_1

	stdcall SaveFile

       .end_with_1:
	mov	eax, 1
	jmp	.end

       .end_with_0:
	xor	eax, eax

       .end:
	mov	esp, ebp
	pop	ebp
	ret

	; Saves the current file
	; Arguments: None
      SaveFile:
	push	ebp
	mov	ebp, esp

	cmp	BYTE [IsFileDefault], TRUE
	je	.non_existent_file

	push	DWORD [FileName]
	call	FileExists
	cmp	eax, FALSE
	je	.non_existent_file

	invoke	GetWindowTextLength, [hEdit]
	inc	eax
	mov	ebx, eax
	invoke	HeapAlloc, [hHeap], HEAP_ZERO_MEMORY, ebx
	push	eax

	invoke	GetWindowText, [hEdit], eax, ebx

	invoke	CreateFile, DWORD [FileName], GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	test	eax, eax
	je	.non_existent_file
	push	eax

	invoke	WriteFile, eax, DWORD [ebp - 4], ebx, NULL, NULL
	test	eax, eax
	je	errors.file_write_error

	pop	eax
	invoke	CloseHandle, eax
	pop	eax
	invoke	HeapFree, [hHeap], 0, eax

	invoke	PostMessage, [hWnd], MSG_SETSAVED, 0, 0

	jmp	.end

      .non_existent_file:
	call	SaveFileAs

      .end:
	mov	esp, ebp
	pop	ebp
	ret

	; Saves the current file as
	; Arguments: None
      SaveFileAs:
	push	ebp
	mov	ebp, esp

	invoke MessageBox, 0, _not_implemented, _debug, MB_ICONERROR+MB_OK

	mov	esp, ebp
	pop	ebp
	ret

      BasicFileOpen:
	invoke	CoCreateInstance,CLSID_FileOpenDialog,NULL,CLSCTX_INPROC_SERVER,IID_IFileOpenDialog,FileOpenDialog
	cmp	eax,S_OK
	jne	.error_create
	cmp	[FileOpenDialog],0
	jz	.error_create

	cominvk FileOpenDialog,GetOptions,DlgFlags
	cmp	eax,S_OK
	jne	.error_create

	or	[DlgFlags],FOS_FORCEFILESYSTEM

	cominvk FileOpenDialog,SetOptions,[DlgFlags]
	cmp	eax,S_OK
	jne	.error_create

	cominvk FileOpenDialog, SetFileTypes,c_size,c_rgSaveTypes
	cmp	eax,S_OK
	jne	.error_create

	cominvk FileOpenDialog,Show,[hWnd]

	cominvk FileOpenDialog,GetResults,ShellItem
	cmp	eax,S_OK
	jne	.error_result

	invoke	MessageBox,[hWnd],_debug,_debug,MB_OK
	cominvk ShellItem,GetDisplayName,SIGDN_FILESYSPATH,fname
	
	invoke	MessageBox,[hWnd],[fname],[fname],MB_OK


	cominvk FileOpenDialog,Release
	cominvk ShellItem,Release
	jmp	.finish

      .error_create:
	call	errors.file_dialog_error
	jmp	.finish
      .error_result:
	call	errors.file_dialog_result_error

      .finish:
	ret
