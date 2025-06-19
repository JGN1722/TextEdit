struc GUID def
 {
   match d1-d2-d3-d4-d5, def
    \{
      .Data1 dd 0x\#d1
      .Data2 dw 0x\#d2
      .Data3 dw 0x\#d3
      .Data4 db 0x\#d4 shr 8,0x\#d4 and 0FFh
      .Data5 db 0x\#d5 shr 40,0x\#d5 shr 32 and 0FFh,0x\#d5 shr 24 and 0FFh,0x\#d5 shr 16 and 0FFh,0x\#d5 shr 8 and 0FFh,0x\#d5 and 0FFh
    \}
 }

CLSCTX_INPROC_SERVER	       = 0x1
CLSCTX_INPROC_HANDLER	       = 0x2
CLSCTX_LOCAL_SERVER	       = 0x4
CLSCTX_INPROC_SERVER16	       = 0x8
CLSCTX_REMOTE_SERVER	       = 0x10
CLSCTX_INPROC_HANDLER16     = 0x20
CLSCTX_INPROC_SERVERX86     = 0x40
CLSCTX_INPROC_HANDLERX86    = 0x80
CLSCTX_ESERVER_HANDLER	    = 0x100
CLSCTX_NO_CODE_DOWNLOAD     = 0x400
CLSCTX_NO_CUSTOM_MARSHAL    = 0x1000
CLSCTX_ENABLE_CODE_DOWNLOAD = 0x2000
CLSCTX_NO_FAILURE_LOG	      = 0x4000
CLSCTX_DISABLE_AAA	    = 0x8000
CLSCTX_ENABLE_AAA	    = 0x10000
CLSCTX_FROM_DEFAULT_CONTEXT = 0x20000

S_OK = 0
S_FALSE = 1