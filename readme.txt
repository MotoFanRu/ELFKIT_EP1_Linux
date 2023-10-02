This patch updates the ADS 1.2 to build 848. (The standard ADS 1.2 CD
provides build 805).

These patch files are supplied as part of ADS 1.2.1.  If you are an 
existing ADS 1.2 user, and would like to update to ADS 1.2.1, please 
contact your supplier.


Installation Instructions
=========================
The patch is provided as a ZIP file.  To install the patch open the ZIP file
with your ZIP utility program and manually copy the contents to the appropriate
directories.



ADS 1.2 build 848 contains fixes for the following issues:

armar
=====

Build 837 fixed the "malformed archive" error which could occur if the User 
ID field overflows.

armasm
======

Build 848 fixes a problem with the AREA ALIGN attribute causing unnecessary 
padding to be added to the end of the section.

armlink
=======

Build 844 fixes a problem with outputting dynamic relocations which 
resulted in "Fatal error: L6003U: Could not write to file" for large projects.

Build 842 fixes a linker memory leak, suppresses linker warning L6333W, now 
calculates correctly whether an ER/LR exceeds the 32bit limit, and now 
eliminates names of common sections from symdefs files.

Build 837 fixed some faults with -callgraph, $Sub$$/$Super$$ handling, 
processing of SDT AOF objects (though mixing SDT and ADS objects is still 
not recommended), the scatterfile ".ANY" directive, and generation of 
interworking/long branch veneers.


ARMulator
=========

Build 837 fixed some minor instruction execution and cycle counting faults 
in ARM7, ARM9 (including ARM926EJ-S) and ARM10-family ARMulators, and the 
XScale ARMulator.
Some VFP issues are also resolved. It also supports the new ARM7EJ-S and 
ARM720rev4.

After installing the patch, when the debugger (e.g. AXD) is restarted, the 
ARMulator may not appear on the list of Targets.  If so, simply Add the 
ARMulate.dll back into the list, using the Options->Configure Target dialog.


AXD/armsd
=========

Build 848 fixes a fault with the display of the VFP FPEXC register.

Build 844 now allows all the global variables for multiple 
(non-overlapping) images to appear in AXD's globals tab all the same time.

Build 842 fixes a fault with setting breakpoints in Thumb function pointer 
($p) code.

Build 837 allows AXD to correctly format FPE or FPU registers as 
"printf...", print from the CLI using a pointer to a structure, (for Unix) 
identify source files with absolute filenames in a debug image.  It also 
fixed a fault where a new log file sometimes contained old log data.

Updated armperip.xml, armboard.xml, armperip.dtd and XScale.dsc files are 
provided.  These are used by AXD to display properties of the target 
(either ARMulator or a real target via Multi-ICE).

After installing the patch, the OCX file will need registering.  To do 
this, run register.bat (register.sh for Unix platforms)


C/C++ Compilers
===============

Build 848 fixes problems with access to nonstatic member functions in C++, 
functions with pointer-to-function parameters in C++, register stacking in 
Thumb code, ordering of arguments of shifted compares, float/double stores 
incorrectly being optimized away, a rare peephole optimization at -O2, 
boolean conversion in C++ code, incorrect division due to register 
allocation, evaluation of shift operations, corruption of SB in RWPI code, 
and stores to malloc'd pointers following memcpy. The C++ compiler now also 
allows multiple declaration specifications in conditions.

Build 844 added a new switch -Wc to suppress the #pragma warning C2813W.

Build 842 fixes some problems with code generation differences between 
platforms, Thumb branch/literal pool issues, code optimization, '#pragma 
arm section', "Internal faults" 0xd288 and 0x2696, and initializing nested 
structures in C++

Build 837 fixed faults in the areas of generation of browse information, 
__packed bitfield initialization, code optimization, DWARF2 line tables, 
__packed typedefs, #pragma preprocessing, "Internal faults" 0x49de and 
0x55af, and memory leaks when
compiling with CodeWarrior.


C Libraries
===========

Build 848 fixes a problem with __user_libspace corrupting R9 in RWPI 
library variants.

Build 837 features optimized __main copy/init code using LDM/STM instead of 
LDR/STR instructions.  It also fixes minor faults with the __rt_entry 
symbol (which did not have its size set, affecting profiling results) and 
backspace() (which did not interact correctly with ungetc()).
Note: the libraries are the same for Unix and Windows.
To install, unzip the patch file into the ADS 1.2\Lib\armlib directory.


fromelf
=======

Build 842 fixes a problem with long symbol names, and the creation of an 
output file where a directory already exists with the same name, or if the 
existing output file is held open by another application.  fromelf now also 
correctly releases its license when used from CodeWarrior.
