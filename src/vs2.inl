// cgc version 3.1.0013, build date Apr 24 2012
// command line args: -profile vp20
// source file: src/vs2.vs.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile vp20
//program main
//semantic main.m_viewport
//semantic main.m_model
//semantic main.m_view
//semantic main.m_proj
//semantic main.camera_pos
//var float4 I.color : $vin.DIFFUSE :  : 0 : 0
//var float4 I.pos : $vin.POSITION : ATTR0 : 0 : 1
//var float4x4 m_viewport :  : , 4 : 1 : 0
//var float4x4 m_model :  : c[0], 4 : 2 : 1
//var float4x4 m_view :  : c[4], 4 : 3 : 1
//var float4x4 m_proj :  : c[8], 4 : 4 : 1
//var float4 camera_pos :  :  : 5 : 0
//var float4 main.pos : $vout.POSITION : HPOS : -1 : 1
//var float4 main.col : $vout.COLOR : COL0 : -1 : 1
//const c[12] = 0 1
// 16 instructions, 0 R-regs
0x00000000, 0x004c2055, 0x0836186c, 0x2f0007f8,
0x00000000, 0x008c0000, 0x0836186c, 0x1f0007f8,
0x00000000, 0x008c40aa, 0x0836186c, 0x1f0007f8,
0x00000000, 0x006c601b, 0x0436106c, 0x3f0007f8,
0x00000000, 0x004ca055, 0x0436186c, 0x2f1007f8,
0x00000000, 0x008c8000, 0x0436186c, 0x5f1007f8,
0x00000000, 0x008cc0aa, 0x0436186c, 0x5f1007f8,
0x00000000, 0x008ce0ff, 0x0436186c, 0x5f0007f8,
0x00000000, 0x004d2055, 0x0436186c, 0x2f1007f8,
0x00000000, 0x008d0000, 0x0436186c, 0x5f1007f8,
0x00000000, 0x008d40aa, 0x0436186c, 0x5f1007f8,
0x00000000, 0x008d60ff, 0x0436186c, 0x5f0007f8,
0x00000000, 0x0400001b, 0x08361300, 0x101807f8,
0x00000000, 0x0040001b, 0x0400286c, 0x2070e800,
0x00000000, 0x0020001b, 0x0436106c, 0x20701800,
0x00000000, 0x002d8005, 0x0c36106c, 0x2070f819,
