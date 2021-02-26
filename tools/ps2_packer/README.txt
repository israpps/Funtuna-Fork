PS2-Packer version 0.4.4
========================

Overview
--------

  Just like UPX http://upx.sourceforge.net/ this tool is designed to help you
create packed ELF to run on the PS2. It has a modular design, so anybody can
write any kind of module to it. It actually has a zlib module, a lzo module,
three ucl modules (n2b, n2d and n2e) and a null module, for demo purpose only.


Changelog
---------

  2004/08/03: release of version 0.1, first version.
              release of version 0.1.1, included zlib, removing bloats :)
  2004/08/04: disabled the buggy "fast" memzero in the stub
              worked out an endian independant version.
	      release of version 0.2 :-P (yeah, okay, okay, a bit too fast...)
  2004/08/05: commenting the source, putting it into ps2dev's CVS.
  2004/08/10: removing error messages into zlib, saving a few bytes.
  2004/08/12: adding module capability to the whole, moved code into modules.
              adding "null" module as an example.
	      adding "lzo" module.
	      tagging as version 0.3b
	      adding "ucl" modules (n2b, n2d and n2e algos)
	      tagging as version 0.3b2 (yeah, okay, still a bit fast :D)
	      changing alignment of data sections to 0x80 instead of the
	        standard 0x1000. Caution: may break things.
	      added a small code to remove the extra zeroes at the end of the
	        section, moving them to bss.
	      cleaning up ucl's uncrunching source code.
	      changed default to use n2e algorithm instead of zlib.
	      changed "memzero" in the stubs to a small asm version.
	      tagging as 0.3b3 (sigh...)
  2004/08/13: adding 1d00 stubs
              adding "alternative" elf packing method
	      changing packer selection method (using prefix)
	      tagging as 0.3 (ho well...)
  2004/08/14: fixed some alignments bugs, added alignment option.
              changing ExecPS2 in stubs to a more ps2link-friendly thingy,
	        as a special compilation option
	      added verbose option :p
	      added n2e.S, 84 instructions ucl-nrv2e uncompression code.
	      added a special "one section" ucl-nrv2e asm only stub, used when
	        the input file has only one section (total of 416 bytes).
	      added a special "multiple sections" ucl-nrv2e asm only stub, used
	        the input file has only one section, untested.
	      tagging as 0.4
  2004/10/26: finally fixed that damn bss section bug...
              tagging as 0.4.1
  2004/11/06: fixed compilation for MacOS X
              added code to handle modules and stub in global path.
	      cleaning Makefiles
	      tagging as 0.4.2
  2004/11/26: added module search path in argv[0] as well.
              fixed a multiple-section critical bug.
	      fixed a bit the asm stub code.
	      tagging as 0.4.3
  2004/12/26: added reload option, and used branches instead of jumps in
                the asm stubs.
	      created lite version - see README-lite.txt for informations.
	      tagging as 0.4.4


Todo
----

  -) Changing current module design to pass on arguments.
  -) Write a proper documentation about "how to write new modules".
  -) Add RC4 modules.


Some facts
----------

180972 - ps2link-embed.elf
105528 - ps2link-embed-lzo.elf
 90455 - ps2link-embed-zlib.elf
 78856 - ps2link-embed-n2b.elf
 78536 - ps2link-embed-n2d.elf
 77792 - ps2link-embed-n2e.elf
 76768 - ps2link-embed-asm-n2e.elf

444240 - ps2menu.elf
239064 - ps2menu-lzo.elf
187927 - ps2menu-zlib.elf
167228 - ps2menu-n2b.elf
166044 - ps2menu-n2d.elf
164124 - ps2menu-n2e.elf
163088 - ps2menu-asm-n2e.elf


History
-------

  Well, I wrote this piece of junk in one day, because Drakonite said me zlib
would be better than lzo, and that it would be quite a challenge to get it
working for PS2. I wanted to see if he was right :)


Source code and legal stuff
---------------------------

  This code is covered by GPL. Actually, I don't give a shit about licenses
and stuff. If you don't like it covered by GPL, just ask me, and we'll change
it. The only problem is it uses modified version of a lot of GPLed code, so...

  This code was inspired by various sources, especially sjcrunch's main.c, and
sjuncrunch. Some idea from mrbrown too :)

  Beeing a ps2dev.org developper got me banned from ps2ownz.com's website. Thus,
as a special exception to the GPL, since I can not go on their forums, and react
and help people about that software, I to NOT give them the autorization to
mirror these packages on their website, only to link to the homepage of this
project, that is, http://www.nobis-crew.org/ps2-packer nor are they authorized
to support their users on their forums on questions about that software.

  If you want to reach me, and find support about ps2-packer, either ask me
directly, by mail, or by reaching me on IRC (channel #ps2dev on EfNet), or ask
your questions on ps2dev.org's forums and ps2-scene's forums.

  I actually know they won't give a shit about these restrictions, but I felt
like proceeding so. If you want real and *legit* ps2 development, go on the
genuine ps2dev website, that is, http://ps2dev.org


How it works
------------

  Usage: ps2-packer [-v] [-b X] [-p X] [-s X] [-a X] [-r X] <in_elf> <out_elf>
  
  Options description:
    -v             verbose mode.
    -b base        sets the loading base of the compressed data. When activated
                      it will activate the alternative packing way. See below.
    -p packer      sets a packer name. n2e by default.
    -s stub        sets another uncruncher stub. stub/n2e-1d00-stub by default,
                      or stub/n2e-0088-stub when using alternative packing.
    -r reload      sets a reload base of the stub. Beware, that will only works
                      with the special asm stubs.
    -a align       sets section alignment. 16 by default. Any value accepted.
                      However, here is a list of alignments to know:
		1 - no alignment, risky, shouldn't work.
		4 - 32-bits alignment, tricky, should on certain loaders.
	       16 - 128-bits alignment, normal, should work everywhere.
	      128 - 128-bytes alignment, dma-safe.
	     4096 - supra-safe, this is the default alignment of binutils.

  Now, you have to understand the mechanisms.
  
  In normal mode, the output elf will contain one program section. Its loading
location depends on the selected stub. For example, with a stub loading at
0x1d00000, compressed data will be located *below* that address. I provide

  However, if you specify a base loading address on the command line, the data
will be forced to reside at a certain location. That's the alternative packing
method. The output elf will contain two program sections. The first one will
be the uncruncher stub. The second section contains the packed data, loaded at
the address you specified.

  The reload option is meant to forcibily relink the stub to another address.
This will only work with the asm stubs though; be careful when using it.

  So, depending on your needs, just move the data around, to get the desired
results.


Examples
--------

~$ ./ps2-packer ./ps2menu.elf ./ps2menu-packed.elf
PS2-Packer v0.3 (C) 2004 Nicolas "Pixel" Noble
This is free software with ABSOLUTELY NO WARRANTY.

Loading stub file.
Stub PC = 01D00008
Loaded stub: 0000057C bytes (with 00000204 zeroes) based at 01D00000
Opening packer.
Preparing output elf file.
Packing.
ELF PC = 00100008
Loaded section: 0006B438 bytes (with 0033CB72 zeroes) based at 00100000
Section packed, from 439350 to 162565 bytes, ratio = 63.00%
Final base address: 01CD84E0
Writing stub.
All data written, writing program header.
Done!


~$ ./ps2-packer -b 0x1b00000 ./ps2menu.elf ./ps2menu-packed-alt.elf
PS2-Packer v0.3 (C) 2004 Nicolas "Pixel" Noble
This is free software with ABSOLUTELY NO WARRANTY.

Using alternative packing method.
Loading stub file.
Stub PC = 00088008
Loaded stub: 0000057C bytes (with 00000204 zeroes) based at 00088000
Opening packer.
Preparing output elf file.
Actual pointer in file = 000005FC
Realigned pointer in file = 00000600
Packing.
ELF PC = 00100008
Loaded section: 0006B438 bytes (with 0033CB72 zeroes) based at 00100000
Section packed, from 439350 to 162565 bytes, ratio = 63.00%
All data written, writing program header.
Done!


$ ./ps2-packer -p lzo ./ps2menu.elf ./ps2menu-packed-lzo.elf
PS2-Packer v0.3 (C) 2004 Nicolas "Pixel" Noble
This is free software with ABSOLUTELY NO WARRANTY.

Loading stub file.
Stub PC = 01D00008
Loaded stub: 000006FC bytes (with 00000204 zeroes) based at 01D00000
Opening packer.
Preparing output elf file.
Packing.
ELF PC = 00100008
Loaded section: 0006B438 bytes (with 0033CB72 zeroes) based at 00100000
Section packed, from 439350 to 237121 bytes, ratio = 46.03%
Final base address: 01CC61A4
Writing stub.
All data written, writing program header.
Done!


$ ls -l ps2menu.elf ps2menu-packed*
-rw-r--r--    1 pixel    pixel      444240 Aug 12 23:33 ps2menu.elf
-rw-r--r--    1 pixel    pixel      164124 Aug 13 15:07 ps2menu-packed.elf
-rw-r--r--    1 pixel    pixel      164125 Aug 13 15:07 ps2menu-packed-alt.elf
-rw-r--r--    1 pixel    pixel      239064 Aug 13 15:08 ps2menu-packed-lzo.elf



Bugs and limitations
--------------------

-) It's poorly coded :-P
-) Stubs have to be in one single program header.


How to compile
--------------

  My compilation options requires libz.a and libucl.a to reside at
/usr/lib/libz.a and /usr/lib/libucl.a (I do that only to statically link
the zlib and ucl with the final software, so it will run everywhere) So, if it
doesn't match your system, change that line into the Makefile. Afterward, a
simple "make" should do the trick in order to compile everything, provided you
have the full ps2 toolchain, with the PS2SDK variable pointing to your ps2sdk
directory, and ee-gcc under your path.

  Don't look at the 'dist' target in the Makefile, it's only for me to build
the various packages.


Author
------

  Nicolas "Pixel" Noble <pixel@nobis-crew.org> - http://www.nobis-crew.org


Where to find
-------------

  The "official" webpage for this tool is at on my personal webspace:
  
    http://www.nobis-crew.org/ps2-packer/

  However, you can find the latests CVS changes into ps2dev's CVS:
  
    http://cvs.ps2dev.org/ps2-packer/

  For more informations about it, feel free to go on ps2dev's website located
at http://ps2dev.org/ and be sure to drop by #ps2dev in EfNet.


Thanks and greetings
--------------------

  They go to adresd, blackd_wd, drakonite, emoon, gorim, hiryu, herben, jenova,
linuzapp, oobles, oopo, mrbrown, nagra, neov, nik, t0mb0la, tyranid

and maybe a few other people I forgot at #ps2dev :)

  Big special thanks to LavosSpawn who helped me reducing the asm stub ;)
