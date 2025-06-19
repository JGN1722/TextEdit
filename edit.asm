; ROVER IDE
; Written by: JGN1722 (Github)
format PE GUI 6.0
include 'win32a.inc'
include 'ccom.inc'
include 'filedlg.inc'
entry main

TRUE  = 1
FALSE = 0

WINDOW_X      = 128
WINDOW_Y      = 128
WINDOW_WIDTH  = 512
WINDOW_HEIGHT = 256

IDR_ICON  = 17
IDR_MENU  = 37
ID_STATUS = 42

IDM_NEW    = 101
IDM_OPEN   = 102
IDM_RELOAD = 103
IDM_SAVE   = 104
IDM_SAVEAS = 105
IDM_EXIT   = 106
IDM_ABOUT  = 901
IDM_LICENSE= 902

MSG_SETSAVED = WM_USER + 100
MSG_SETMODIFIED = WM_USER + 101

section '.text' code readable executable
      main:
	; get the useful values and store them
	invoke	GetModuleHandle, NULL
	mov	[hInstance], eax

	invoke	GetProcessHeap
	mov	[hHeap], eax

	; Initialize COM objects
	invoke	CoInitialize, NULL
	cmp	eax,S_OK
	jne	errors.startup_error

	; Open the window
	mov	eax, [hInstance]
	mov	[wc.hInstance], eax
	invoke	LoadIcon, eax, IDR_ICON
	mov	[wc.hIcon], eax
	invoke	LoadCursor, 0, IDC_ARROW
	mov	[wc.hCursor], eax
	invoke	RegisterClass, wc
	test	eax, eax
	jz	errors.startup_error

	invoke	LoadMenu, [wc.hInstance], IDR_MENU
	invoke	CreateWindowEx, 0,\
				ide_classname,\
				title,\
				WS_VISIBLE+WS_OVERLAPPEDWINDOW+WS_CLIPCHILDREN,\
				WINDOW_X,\
				WINDOW_Y,\
				WINDOW_WIDTH,\
				WINDOW_HEIGHT,\
				NULL,\
				eax,\
				[wc.hInstance],\
				NULL
	test	eax, eax
	jz	errors.startup_error
	mov	[hWnd], eax

	; Manage the file opening
	call	 GetArgumentCount
	cmp	 eax, 1
	je .no_file_to_open

	; Open the file
	stdcall GetArgument, 1
	push	eax
	;mov	 eax, _filename

	stdcall GetFullPath, eax
	stdcall OpenExistingFile, eax
	cmp	eax, 0
	je	errors.file_open_error

	pop	eax
	invoke	HeapFree, [hHeap], 0, eax

	jmp	msg_loop

      .no_file_to_open:
	call	OpenDefaultFile

	; main application loop
      msg_loop:
	invoke	GetMessage, msg, NULL, 0, 0
	cmp	eax, 1
	jb	app_end
	jne	msg_loop
	invoke	TranslateMessage, msg
	invoke	DispatchMessage, msg
	jmp	msg_loop

      app_end:
	invoke	ExitProcess, [msg.wParam]


	; error actions
      errors:
      .startup_error:
	invoke	MessageBoxA, 0, _error_startup, _error, MB_ICONERROR+MB_OK
	jmp	app_end
      .file_open_error:
	invoke	MessageBoxA, 0, _error_read_file, _error, MB_ICONERROR+MB_OK
	jmp	app_end
      .file_write_error:
	invoke	MessageBoxA, 0, _error_write_file, _error, MB_ICONERROR+MB_OK
	ret	; When we call this we want to return
      .file_create_error:
	invoke	MessageBoxA, 0, _error_create_file, _error, MB_ICONERROR+MB_OK
	ret	; When we call this we want to return
      .file_dialog_error:
	invoke	MessageBoxA, 0, _error_file_dialog, _error, MB_ICONERROR+MB_OK
	ret	; When we call this we want to return
      .file_dialog_result_error:
	invoke	MessageBoxA, 0, _error_file_dialog_result, _error, MB_ICONERROR+MB_OK
	ret	; When we call this we want to return


      DisplayTextCoords:
	push	ebp
	mov	ebp, esp

	invoke	HeapFree, [hHeap], 0, [hCoordTxt]

	sub	esp, 4
	mov	ebx, esp
	invoke	SendMessage,[hEdit],EM_GETSEL,ebx,NULL
	invoke	SendMessage,[hEdit],EM_LINEFROMCHAR,DWORD [ebx],NULL
	push	eax

	invoke	SendMessage,[hEdit],EM_LINEINDEX,eax,NULL
	mov	ecx, eax
	sub	DWORD [ebx], ecx
	mov	ecx, DWORD [ebx]
	inc	ecx

	pop	eax
	inc	eax
	add	esp, 4
	stdcall LineCoords, eax, ecx
	mov	[hCoordTxt], eax

	invoke	SendMessage, [hStatus], SB_SETTEXT, 0, [hCoordTxt]

	mov	esp, ebp
	pop	ebp
	ret

	; The callback for the main window
	; Arguments: lParam, wParam, uMsg, hWnd
      MainWindowCallback:
	push	ebp
	mov	ebp, esp

	; save needed registers
	push	ebx
	push	esi
	push	edi

	mov	eax, DWORD [ebp + 12]
	cmp	eax, WM_CREATE
	je	.wm_create
	cmp	eax, WM_DESTROY
	je	.wm_destroy
	cmp	eax, WM_CLOSE
	je	.wm_close
	cmp	eax, WM_SIZE
	je	.wm_size
	cmp	eax, WM_GETMINMAXINFO
	je	.wm_getminmaxinfo
	cmp	eax, WM_SETFOCUS
	je	.wm_setfocus
	cmp	eax, WM_COMMAND
	je	.wm_command
	cmp	eax, MSG_SETSAVED
	je	.msg_setsaved
	cmp	eax, MSG_SETMODIFIED
	je	.msg_setmodified
      .default:
	invoke	DefWindowProc, DWORD [ebp + 8], DWORD [ebp + 12], DWORD [ebp + 16], DWORD [ebp + 20]
	jmp	.finish
      .wm_create:
	; Create a status bar
	invoke	CreateStatusWindow, WS_CHILD+WS_VISIBLE, _null_string, DWORD [ebp + 8], ID_STATUS
	test	eax, eax
	jz	errors.startup_error
	mov	[hStatus], eax

	; Get the height of the status bar for later use
	invoke	GetClientRect, eax, rect
	mov	eax, [rect.bottom]
	mov	[StatusBarHeight], eax

	; Set the number of parts of the status bar
	invoke	SendMessage, [hStatus], SB_SETPARTS, 3, status_parts_width

	; Create the RichEdit
	invoke	GetClientRect, DWORD [ebp + 8], rect
	invoke	CreateWindowEx, WS_EX_CLIENTEDGE, edit_classname, 0, WS_CHILD+WS_VISIBLE+WS_VSCROLL+WS_HSCROLL+ES_MULTILINE+ES_AUTOVSCROLL+ES_AUTOHSCROLL, [rect.left], [rect.top], [rect.right], [rect.bottom], DWORD [ebp + 8], 0, [hInstance], 0
	test	eax, eax
	jz	errors.startup_error
	mov	[hEdit], eax

	; set the font
	invoke	CreateFont, 18, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH+FF_MODERN, _font
	test	eax, eax
	jz	errors.startup_error
	mov	[hFont], eax
	invoke	SendMessage, [hEdit], WM_SETFONT, eax, 1
	invoke	SendMessage, [hStatus], WM_SETFONT, [hFont], 1

	; set the tab size to 8 instead of 6
	invoke	SendMessage, [hEdit], EM_SETTABSTOPS, 1, tab_size
	invoke	InvalidateRect, [hEdit], NULL, TRUE

	; subclass the edit to add functionality
	invoke	SetWindowLongA, [hEdit], -4, EditCustomCallback ; GWLP_WNDPROC = -4
	test	eax, eax
	jz	errors.startup_error
	mov	[EditCtrlWndProc], eax

	jmp	.finish_with_0
      .wm_close:
	stdcall AskToSave
	cmp	eax, 0
	je	.finish_with_0
	invoke	PostQuitMessage, 0
      .wm_destroy:
	invoke	DeleteObject, [hFont]
	jmp	.finish_with_0
      .wm_size:
	invoke	GetClientRect, [hWnd], rect
	invoke	MoveWindow, [hStatus], [rect.left], [rect.top], [rect.right], [rect.bottom], TRUE
	mov	eax, [StatusBarHeight]
	sub	[rect.bottom], eax
	invoke	MoveWindow, [hEdit], [rect.left], [rect.top], [rect.right], [rect.bottom], TRUE
	jmp	.finish_with_0
      .wm_getminmaxinfo:
	mov	eax, DWORD [ebp + 20]
	add	eax, 6 * 4 ; 3 * sizeof(POINT) = 6 * sizeof(LONG)
	mov	DWORD [eax], 200
	mov	DWORD [eax + 4], 200
	jmp	.finish_with_0
      .wm_setfocus:
	invoke	SetFocus, [hEdit]
	jmp	.finish_with_0
      .wm_command:
	mov	eax, DWORD [ebp + 16]
	cmp	eax, EN_UPDATE * 65536
	je	.en_update
	cmp	ax, IDM_NEW
	je	.new
	cmp	ax, IDM_RELOAD
	je	.reload
	cmp	ax, IDM_OPEN
	je	.open
	cmp	ax, IDM_SAVE
	je	.save
	cmp	ax, IDM_SAVEAS
	je	.saveas
	cmp	ax, IDM_ABOUT
	je	.about
	cmp	ax, IDM_LICENSE
	je	.license
	cmp	ax, IDM_EXIT
	je	.wm_close
	jmp	.default
       .new:
	call	OpenDefaultFile
	jmp	.finish_with_0
       .reload:
	cmp	BYTE [IsFileDefault], TRUE
	je	.finish_with_0
	stdcall OpenExistingFile, DWORD [FileName]
	jmp	.finish_with_0
       .open:
	call	OpenFileDialog
	jmp	.finish_with_0
       .save:
	call	SaveFile
	jmp	.finish_with_0
       .saveas:
	call	SaveFileAs
	jmp	.finish_with_0
       .about:
	invoke	MessageBox, [hWnd], _about_text, _about_title, MB_ICONINFORMATION+MB_OK
	jmp	.finish_with_0
       .license:
	invoke	MessageBox, [hWnd], _license_text, _license_title, MB_ICONINFORMATION+MB_OK
	jmp	.finish_with_0
       .en_update:
	cmp	BYTE [IsFileSaved], TRUE
	jne	.finish_with_0
	invoke	SendMessage, [hWnd], MSG_SETMODIFIED, 0, 0
	jmp	.finish_with_0
      .msg_setsaved:
	invoke	SendMessage, [hStatus], SB_SETTEXT, 1, _null_string
	mov	BYTE [IsFileSaved], TRUE
	jmp	.finish_with_0
      .msg_setmodified:
	invoke	SendMessage, [hStatus], SB_SETTEXT, 1, _modified
	mov	BYTE [IsFileSaved], FALSE
	jmp	.finish_with_0

      .finish_with_true:
	mov	eax, TRUE
	jmp	.finish
      .finish_with_0:
	xor	eax, eax

      .finish:
	pop	edi
	pop	esi
	pop	ebx

	mov esp, ebp
	pop ebp
	ret

	; The subclassed callback for the edit
	; The keyboard shortcuts are handled here, because the main window doesn't receive key presses
	; Arguments: lParam, wParam, uMsg, hWnd
      EditCustomCallback:
	push	ebp
	mov	ebp, esp

	; save needed registers
	push	ebx
	push	esi
	push	edi

	mov	eax, DWORD [ebp + 12]
	cmp	eax, EM_GETSEL
	je	@f
	cmp	eax, EM_LINEFROMCHAR
	je	@f
	cmp	eax, EM_LINEINDEX
	je	@f
	stdcall DisplayTextCoords

      @@:
	mov	eax, DWORD [ebp + 12]
	cmp	eax, WM_MOUSEWHEEL
	je	.wm_mousewheel
	cmp	eax, WM_KEYDOWN
	je	.wm_keydown
      .default:
	invoke	CallWindowProc, DWORD [EditCtrlWndProc], DWORD [ebp + 8], DWORD [ebp + 12], DWORD [ebp + 16], DWORD [ebp + 20]
	jmp	.finish
      .wm_mousewheel:
	mov	ax, WORD [ebp + 18]
	test	ax, ax
	js	.less
	mov	eax, SB_LINEUP
	jmp	.end_scroll
       .less:
	mov	eax, SB_LINEDOWN
       .end_scroll:
	invoke	SendMessage, [hEdit], WM_VSCROLL, eax, NULL
	mov	eax, TRUE
	jmp	.finish
      .wm_keydown:
	invoke	GetKeyState, VK_CONTROL
	test	eax, 80h
	jz	.default
	mov	eax, DWORD [ebp + 16]
	cmp	eax, 'S'
	je	.save
	cmp	eax, 'W'
	je	.close
	cmp	eax, 'O'
	je	.open
	cmp	eax, 'R'
	je	.reload
	cmp	eax, 'N'
	je	.new
	cmp	eax, 'A'
	je	.selectall
       .no_shortcut:
	jmp	.default
       .save:
	call	SaveFile
	jmp	.finish_with_0
       .open:
	call	OpenFileDialog
	jmp	.finish_with_0
       .close:
	invoke	SendMessage, [hWnd], WM_CLOSE, 0, 0
	jmp	.finish_with_0
       .new:
	call	OpenDefaultFile
	jmp	.finish_with_0
       .reload:
	cmp	BYTE [IsFileDefault], TRUE
	je	.finish_with_0
	stdcall OpenExistingFile, DWORD [FileName]
	jmp	.finish_with_0
       .selectall:
	invoke	SendMessage,[hEdit], EM_SETSEL, 0, -1
	jmp	.finish_with_0


      .finish_with_0:
	xor	eax, eax

      .finish:
	pop	edi
	pop	esi
	pop	ebx

	mov esp, ebp
	pop ebp
	ret

	include 'file.inc'
	include 'commandline.inc'
	include 'misc.inc'

section '.data' data readable writeable
	; error messages
	_error db 'Error',0
	_error_startup db 'Error on application startup',0
	_error_read_file db 'The specified file could not be opened',0
	_error_write_file db 'The specified file could not be written to',0
	_error_create_file db 'The specified file could not be created',0
	_error_file_dialog db 'The file dialog could not be created',0
	_error_file_dialog_result db 'The file dialog result was invalid',0

	; warning messages
	_warning db 'Warning',0
	_warning_non_existent_file db 'This file does not exist. Do you want to create it now ?',0
	_warning_not_saved db 'Do you want to save this file ? If you do not, the modifications will be lost.',0
	_warning_not_saved_default db 'You',39,'re about to close a file that doesn',39,'t exist. All the text you',39
				   db 've written will be lost. Do you want to save it ?',0

	; diverse string
	_filename db 'test.txt',0
	_font db 'Consolas',0
	_about_title db 'About Edit',0
	_about_text db 'This is an IDE / Text Editor created with flat assembler',0
	_license_title db 'WTFPL License',0
	_license_text db 'This program is licensed under the DO WHAT THE FUCK YOU WANT TO DO public license',0
	_not_implemented db 'This feature is not implemented',0
	_modified db 'Modified',0
	_no_file db 'No file',0
	_debug db 'Debug'
	_null_string db 0

	; global variables
	hInstance dd 0
	hHeap dd 0
	hWnd dd 0
	hStatus dd 0
	hEdit dd 0
	hFont dd 0
	hCoordTxt dd 0

	FileName dd 0
	IsFileDefault db 1
	IsFileSaved db 1
	StatusBarHeight dd 0
	DlgFlags dd 0
	fname dd 0

	EditCtrlWndProc dd NULL

	; Misc data
	tab_size dd 8 * 4
	status_parts_width dd 70, 140, -1

	c_rgSaveTypes dd _st1,_st2
	c_size	      = 1
	 _st1	      du 'Text Document (*.txt)',0
	 _st2	      du '*.txt',0

	; structures
	wc WNDCLASS 0, MainWindowCallback, 0, 0, NULL, NULL, NULL, COLOR_BTNFACE+1, NULL, ide_classname
	msg MSG
	rect RECT

	; technical window stuff
	ide_classname db 'IDE Window',0
	edit_classname db 'EDIT', 0
	title db 'Edit',0

	; COM stuff
	CLSID_FileOpenDialog GUID DC1C5A9C-E88A-4dde-A5A1-60F82A20AEF7
	CLSID_FileSaveDialog GUID C0B4E2F3-BA21-4773-8DBA-335EC946EB8B

	IID_IFileSaveDialog GUID 84bccd23-5fde-4cdb-aea4-af64b83d78ab
	IID_IFileOpenDialog GUID d57c7288-d4ad-4768-be02-9d969532d960
	IID_IFileDialogEvents GUID 973510db-7d7f-452b-8975-74a85828d354
	IID_IShellItem GUID 43826d1e-e718-42ee-bc55-a1e261c37bfe

	FileSaveDialog IFileSaveDialog
	FileOpenDialog IFileOpenDialog
	FileDialogEvents IFileDialogEvents
	ShellItem IShellItem

section '.rsrc' resource data readable
	directory RT_MENU, menus,\
		  RT_ICON, icons,\
		  RT_GROUP_ICON, group_icons,\
		  RT_VERSION, versions

	resource menus,\
		 IDR_MENU, LANG_ENGLISH+SUBLANG_DEFAULT, main_menu

	resource icons,\
		 1, LANG_NEUTRAL, icon_data

	resource group_icons,\
		 IDR_ICON, LANG_NEUTRAL, main_icon

	resource versions,\
		 1, LANG_NEUTRAL, version

	_ equ ,09h,

	menu main_menu
		menuitem '&File', 0, MFR_POPUP
			menuitem '&New'    _ 'Ctrl+N', IDM_NEW
			menuitem '&Reload' _ 'Ctrl+R', IDM_RELOAD
			menuitem '&Open'   _ 'Ctrl+O', IDM_OPEN
			menuitem '&Save'   _ 'Ctrl+S', IDM_SAVE
			menuitem 'Save As'	     , IDM_SAVEAS
			menuseparator
			menuitem '&Quit'   _ 'Ctrl+W', IDM_EXIT, MFR_END
		menuitem '&About', 0, MFR_POPUP+MFR_END
			menuitem 'License'	     , IDM_LICENSE
			menuitem 'About...'	     , IDM_ABOUT, MFR_END

	icon main_icon, icon_data, 'edit.ico'

	versioninfo version,VOS__WINDOWS32,VFT_APP,VFT2_UNKNOWN,LANG_ENGLISH+SUBLANG_DEFAULT,0,\
		'FileDescription','Text editor',\
		'LegalCopyright','No rights reserved, f*ck capitalism.',\
		'FileVersion','1.0',\
		'ProductVersion','1.0',\
		'OriginalFilename','edit.exe'

section '.idata' import data readable writeable
	; standard DLL imports
	library kernel32, 'KERNEL32.DLL',\
		user32, 'USER32.DLL',\
		gdi32, 'GDI32.DLL',\
		comctl32, 'COMCTL32.DLL',\
		ole32, 'OLE32.DLL'
	include 'api\kernel32.inc'
	include 'api\user32.inc'
	include 'api\gdi32.inc'
	include 'api\comctl32.inc'
	import ole32,\
		CoInitialize, 'CoInitialize',\
		CoCreateInstance, 'CoCreateInstance'