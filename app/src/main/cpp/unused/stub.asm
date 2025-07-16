.text
.arch arm64v8a


.globl proto2str
.globl get_pproto2str

.global stub_proto2str
.type stub_proto2str,%function
stub_proto2str:
stp x29,x30,[sp,-0x10]!
stp x0,x1,[sp,-0x10]!
stp x2,x3,[sp,-0x10]!
stp x4,x5,[sp,-0x10]!
stp x6,x7,[sp,-0x10]!
stp x8,x9,[sp,-0x10]!
stp x10,x11,[sp,-0x10]!
stp x12,x13,[sp,-0x10]!
stp x14,x15,[sp,-0x10]!

mov x16,x0
mov x0,x3

stp x16,x0,[sp,-0x10]!
bl proto2str


bl get_pproto2str
mov x17,x0


ldp x16,x0,[sp],0x10
ldp x14,x15,[sp],0x10
ldp x12,x13,[sp],0x10
ldp x10,x11,[sp],0x10
ldp x8,x9,[sp],0x10
ldp x6,x7,[sp],0x10
ldp x4,x5,[sp],0x10
ldp x2,x3,[sp],0x10
ldp x0,x1,[sp],0x10

blr x17

ldp x29,x30,[sp],0x10
ret

.end