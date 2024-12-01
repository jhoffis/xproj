20070315 Demo 04 - "Initial Fantasy" released! (Only a preview of the 3D engine)
20070217 Demo 03 - "AntiAliased Q*Bert" released! Use X/B to swap AA & BitBlt.
20070203 Demo 02 - "AntiAliased Pong" updated! Use X/B to swap AntiAliasing & BitBlt.
20070129 Demo 02 - "AntiAliased Pong" released! pbKit fully supports vertex shaders!
20070118 Unstability issue has been understood and slaughtered! Big fun starts now!
20070115 Many fixes. Correct size for frame buffers. Unstability issue still there.
20070109 Demo 01 - (hardware accelerated) "Pong" released! (with an issue though).
20070105 Rectangle fill works! main.c will test it with a triple buffering technic!
20070102 Push buffer DMA engine works! Have a look at it. Not drawing yet.

Demo 04 - Initial Fantasy (3D massive rendering with clipping, light, texture mapping)
Demo 03 - AntiAliased Q*Bert (2D sprites with transparentcy -3 different methods-)
Demo 02 - AntiAliased Pong (PS, VS, Full screen AntiAliasing, Texture mapping, Bitblt)
Demo 01 - Hardware accelerated Pong (Push buffer DMA engine + rectangle fill)

Important note about Demo 04 : Transfert "meshes" subdirectory to your console too.
Demo 04 .xbe reads /meshes/polyship.3ds and /meshes/polyship.bmp in its own directory.

Merge files with files coming from pbKit_core.zip and type "make" to compile source.
(Don't forget to apply changes coming from openxdk_patches.zip to openxdk libraries)

Absolute mode means instant positioning, but may be eratic because of electronic noise.
Relavive mode means you go up or down according to your move (position adjustment).

For wiimote(s) you can use program inp2eth (accelerometers) or mse2eth (IR camera).
These Windows programs capture wiimote(s) input(s) (converted into Windows key or mouse events by GlovePIE thanks to the supplied script) and send them to the console over ethernet with a raw Ping packet request (which won't be answered) at 10hz or 100hz.
mse2eth requires two infrared sources (DO NOT USE CANDLES!!! IT'S TOO DANGEROUS!!!)


Some notes about shaders:

Vertex shader registers description:
 (n) means no more than n of these registers (if they are different)
 can be read in 1 instruction
outputs:
 oD0 is the diffuse color register (sent to v0 of pixel shader) (xbox: oT4 for back faces)
 oD1 is the specular color register (sent to v1 of pixel shader) (xbox: oT5 for back faces)
 oFog is the fog factor (kept in [0,1.0f] and used on fog table) (scalar)
 oPos is the output position register (in viewport area)
 oPts is output point size register (scalar)
 oT0-oT7 are the output texture coordinates registers (oT6-oT7 do not exist on xbox)
inputs:
 v0-v15 are input registers (1)
 r0-r11 are temporary registers (3)
 c0-c95 are float constant registers (1)
 i0-15 are integer constant registers (1) (not supported in vs.1.1)
 b0-15 are boolean constant registers (1) (scalar) (not supported in vs.1.1)
 a0 is address register (1) (not supported in vs.1.1) (ex: c[a0.x+n])
 aL is loop counter register (1) (scalar) (not supported in vs.1.1)
 p0 is predicate (1) (vs.2.x only)
(on xbox, oT4-5 are like oD0-1 but for back faces. oT6-7 do not exist. a0 exists.)

Pixel shader registers description:
 c0-c7 Constant register (2) [-1.0f,1.0f]
 r0-r1 Temporary register (2) [-?.0f,?.0f]
 t0-t3 Texture register (1) [-?.0f,?.0f]
 v0-v1 Color register (1) [0,1.0f]

references:
http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/dx81_c/directx_cpp/Graphics/Reference/Reference.asp
http://www.cs.brown.edu/courses/cs224/2002/docs/Missing_Vertex_Shader_Instruction.pdf

How to create vertex shader & pixel shader pseudo code:
- Use cgc.exe (cg compiler) to convert v/pshader.cg into v/pshader.v/psh
- use v/psa.exe (assembler) to convert v/pshader.v/psh into v/pshader.v/pso or .h

How to obtain Cg compiler 1.3 (& vsa.exe/psa.exe without installing DirectX SDK):
- On site www.nvidia.com, type "sdk" in the search field.
- Click the "NVidia SDK" search result link.
- Download "NVidia SDK 9.5" (or more recent version if available).
- Launch the setup program (820Mb, but don't worry we will keep only 20Mb).
- Accept the desktop icon creation if you want, then click finish.
- Then you are offered the opportunity to install Cg compiler 1.3. Accept!
- Accept also the update of system path, so cgc.exe can be called from anywhere.
- Then you are offered the opportunity to install MS .NET framework 1.1.
- Cancel it, because you probably already have it or a more recent version.
- Copy vsa.exe & psa.exe from SDK 9.5\TOOLS\bin\release\D3D9SDKAsm to Cg\bin
- To save space, you can now uninstall "Nvidia SDK 9.5" (saves 800Mb!)

How to obtain Cg compiler 1.5 (not absolutely necessary) :
- Uninstall Cg compiler 1.3 (but first, create a backup copy of Cg\bin)
- On site www.nvidia.com, type "cg toolkit" in the search field.
- Click the "Cg compiler 1.5" search result link.
- Download the Windows package (30Mb) and install it.
- Copy vsa.exe & psa.exe from old Cg\bin to new Cg\bin
