FOS_OVERWRITEPROMPT = 2
FOS_STRICTFILETYPES = 4
FOS_NOCHANGEDIR = 8
FOS_PICKFOLDERS = $20
FOS_FORCEFILESYSTEM = $40
FOS_ALLNONSTORAGEITEMS = $80
FOS_NOVALIDATE = $100
FOS_ALLOWMULTISELECT = $200
FOS_PATHMUSTEXIST = $800
FOS_FILEMUSTEXIST = $1000
FOS_CREATEPROMPT = $2000
FOS_SHAREAWARE = $4000
FOS_NOREADONLYRETURN = $8000
FOS_NOTESTFILECREATE = $10000
FOS_HIDEMRUPLACES = $20000
FOS_HIDEPINNEDPLACES = $40000
FOS_NODEREFERENCELINKS = $100000
FOS_DONTADDTORECENT = $2000000
FOS_FORCESHOWHIDDEN = $10000000
FOS_DEFAULTNOMINIMODE = $20000000
FOS_FORCEPREVIEWPANEON = $40000000

SIGDN_NORMALDISPLAY = 0
SIGDN_PARENTRELATIVEPARSING = $80018001
SIGDN_DESKTOPABSOLUTEPARSING = $80028000
SIGDN_PARENTRELATIVEEDITING = $80031001
SIGDN_DESKTOPABSOLUTEEDITING = $8004c000
SIGDN_FILESYSPATH = $80058000
SIGDN_URL = $80068000
SIGDN_PARENTRELATIVEFORADDRESSBAR = $8007c001
SIGDN_PARENTRELATIVE = $80080001



;IID_IFileSaveDialog GUID $84bccd23,$5fde,$4cdb,<$ae,$a4,$af,$64,$b8,$3d,$78,$ab>
interface IFileSaveDialog,\
	  QueryInterface,\
	  AddRef,\
	  Release,\
	  Show,\
	  SetFileTypes,\
	  SetFileTypeIndex,\
	  GetFileTypeIndex,\
	  Advise,\
	  Unadvise,\
	  SetOptions,\
	  GetOptions,\
	  SetDefaultFolder,\
	  SetFolder,\
	  GetFolder,\
	  GetCurrentSelection,\
	  SetFileName,\
	  GetFileName,\
	  SetTitle,\
	  SetOkButtonLabel,\
	  SetFileNameLabel,\
	  GetResult,\
	  AddPlace,\
	  SetDefaultExtension,\
	  Close,\
	  SetClientGuid,\
	  ClearClientData,\
	  SetFilter,\
	  SetSaveAsItem,\
	  SetProperties,\
	  SetCollectedProperties,\
	  GetProperties,\
	  ApplyProperties


;IID_IFileOpenDialog GUID $d57c7288,$d4ad,$4768,<$be,2,$9d,$96,$95,$32,$d9,$60>
interface IFileOpenDialog,\
	  QueryInterface,\
	  AddRef,\
	  Release,\
	  Show,\
	  SetFileTypes,\
	  SetFileTypeIndex,\
	  GetFileTypeIndex,\
	  Advise,\
	  Unadvise,\
	  SetOptions,\
	  GetOptions,\
	  SetDefaultFolder,\
	  SetFolder,\
	  GetFolder,\
	  GetCurrentSelection,\
	  SetFileName,\
	  GetFileName,\
	  SetTitle,\
	  SetOkButtonLabel,\
	  SetFileNameLabel,\
	  GetResult,\
	  AddPlace,\
	  SetDefaultExtension,\
	  Close,\
	  SetClientGuid,\
	  ClearClientData,\
	  SetFilter,\
	  GetResults,\
	  GetSelectedItems

;IID_IFileDialogEvents GUID $973510db,$7d7f,$452b,<$89,$75,$74,$a8,$58,$28,$d3,$54>
interface IFileDialogEvents,\
	  QueryInterface,\
	  AddRef,\
	  Release,\
	  OnFileOk,\
	  OnFolderChanging,\
	  OnFolderChange,\
	  OnSelectionChange,\
	  OnShareViolation,\
	  OnTypeChange,\
	  OnOverwrite

;IID_IShellItem GUID $43826d1e,$e718,$42ee,<$bc,$55,$a1,$e2,$61,$c3,$7b,$fe>
interface IShellItem,\
	  QueryInterface,\
	  AddRef,\
	  Release,\
	  BindToHandler,\
	  GetParent,\
	  GetDisplayName,\
	  GetAttributes,\
	  Compare
