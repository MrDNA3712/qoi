global encode
SECTION .text

;rdi *pixels
;rsi *outsize
;rdx *desc
encode:
    ;max size of output
    MOV EAX, [RDX]      ;width
    MOV R9D, [RDX+4]    ;height
    MOV R10b, [RDX+5]   ;channels
    MOV R11b, [RDX+6]   ;color
    MUL R9D
    INC R10b
    MOV R10b,5
    MUL R10b    ;TODO Overflow!
    ADD EAX, 22 ;header and padding
    MOV [RSI],EAX
    MOV RAX,RDI
    RET