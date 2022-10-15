global encode
extern malloc
SECTION .text

;rcx *pixels
;rdx *outsize
;r8 *desc
encode:
    push RDX
    XOR R10, R10
    XOR R9, R9
    XOR R11, R11
    
    ;max size of output
    MOV EAX, [R8]      ;width
    MOV R9D, [R8+4]    ;height
    MOV R10b, [R8+8]   ;channels
    MOV R11b, [R8+9]   ;color

    ;max_size = width * height * (channels + 1)
    MUL R9D
    INC R10w
    MUL R10
    ADD EAX, 22 ;header and padding

    ;move the max size to the out_size ptr
    pop RDX
    MOV [RDX],EAX

    ;allocate memory for the encoded image
    PUSH RCX
    MOV RCX,RAX
    CALL malloc
    ;MOV RAX,RCX
    POP RCX


    RET