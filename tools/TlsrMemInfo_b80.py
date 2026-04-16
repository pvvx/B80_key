#!/usr/bin/env python
 
### TlsrMemInfo.py ###
###  Autor: pvvx   ###
 
import sys
import signal
import struct
import platform
import time
import argparse
import subprocess
import os
import io

__progname__ = "TLSR8208 MemInfo"
__filename__ = "TlsrMemInfo"
__version__ = "07.04.26"

SRAM_BASE_ADDR = 0x840000

class FatalError(RuntimeError):
	def __init__(self, message):
		RuntimeError.__init__(self, message)

	@staticmethod
	def WithResult(message, result):
		message += " (result was %s)" % hexify(result)
		return FatalError(message)

def signal_handler(signal, frame):
	print()
	print('Keyboard Break!')
	sys.exit(0)
	
def arg_auto_int(x):
	return int(x, 0)
	
class ELFFile:

	def __init__(self, name, tool_nm):
		self.name = name
		self.tool_nm = tool_nm
		self.symbols = {}
		try:
			#tool_nm = "tc32-elf-nm"
			#if sys.platform == 'linux2':
			#	tool_nm = "tc32-elf-nm"
			proc = subprocess.Popen([self.tool_nm, self.name], stdout=subprocess.PIPE)
		except OSError:
			print("Error calling " + self.tool_nm + ", do you have toolchain in PATH?")
			sys.exit(1)
		for l in proc.stdout:
			fields = l.strip().split()
			try:
				if fields[0] == b"U":
					print("Warning: Undefined symbol '%s'!" %(fields[1].decode('ASCII')))
					continue
				if fields[0] == b"w":
					continue  # can skip weak symbols
				self.symbols[fields[2]] = int(fields[0], 16)
			except ValueError:
				raise FatalError("Failed to strip symbol output from nm: %s" % fields)

	def get_symbol_addr(self, sym, default = 0):
		try:
			x = self.symbols[sym]
		except:
			return default
		return x
'''
	def load_section(self, section):
		tool_objcopy = "e:/Telink/SDK/opt/tc32/bin/tc32-elf-objcopy.exe"
		if sys.platform == 'linux2':
			tool_nm = "tc32-elf-objcopy"
		tmpsection = tempfile.mktemp(suffix=".section")
		try:
			subprocess.check_call([tool_objcopy, "--only-section", section, "-Obinary", self.name, tmpsection])
			with open(tmpsection, "rb") as f:
				data = f.read()
		finally:
			os.remove(tmpsection)
		return data
'''

def main():

	signal.signal(signal.SIGINT, signal_handler)
	parser = argparse.ArgumentParser(description='%s version %s' % (__progname__, __version__), prog=__filename__)
	parser.add_argument(
		"--size", "-s",
		help="Chip Retention SRAM Size (default: 16384)",
		type=arg_auto_int, 
		default=16384)
	parser.add_argument('-t','--tools', help='Path and name tc32-elf-nm', default = 'tc32-elf-nm');
	parser.add_argument('elffname', help='Name of elf file')
	args = parser.parse_args()

	print('%s version %s' % (__progname__, __version__))
	#            0                   1                 2                     3                  4                 5			      6               7                      8			       9              10                     11
	sec_name = ["ramcode",           "data",           "nc",                 "ictag",           "icdata",         "irq_stk",	  "bss",          "rldata",              "stack",	       "text",        "rodata",              "flash"]
	sec_des = ["Resident Code SRAM", "Init Data SRAM", "Wasteful Area SRAM", "Cache Table SRAM","Cache Data SRAM","BSS Data SRAM","BSS Data SRAM","Reload SRAM",         "CPU Stack SRAM", "Code Flash",  "Read Only Data Flash","Bin Size Flash"]
	sec_start = [b"__start",         b"_start_data_",  b"_end_data_",        b"_ictag_start_",  b"_ictag_end_",   b"irq_stk",     b"_start_bss_", b"_data_reload_start_",b"_ram_use_end_", b"_start_text_",b"_start_rodata_",     b"__start"]
	sec_end   = [b"_ramcode_size_",  b"_end_data_",    b"_ictag_start_",     b"_ictag_end_",    b"_ictag_end_",   b"IRQ_STK_SIZE",b"_end_bss_",   b"_data_reload_end_",  b"__SRAM_SIZE",   b"_end_text_",  b"_end_rodata_",       b"_bin_size_"]
	sec_start_add = [SRAM_BASE_ADDR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
	sec_end_add =   [SRAM_BASE_ADDR, 0, 0, 0, 0x800, 0, 0, 0, 0, 0, 0, 0]
	sec_size = []

	e = ELFFile(args.elffname, args.tools);
	chip_sram_size = e.get_symbol_addr(b"__SRAM_SIZE") - SRAM_BASE_ADDR;
	chip_retram_size = args.size;
	if e.get_symbol_addr(b"irq_stk"):
		sec_end_add[5] = e.get_symbol_addr(b"irq_stk") # = _start_bss_
	if e.get_symbol_addr(b"IRQ_STK_SIZE"):
		sec_start_add[6] = e.get_symbol_addr(b"IRQ_STK_SIZE")
	load_sram = e.get_symbol_addr(b"_dstored_");
	ictag = e.get_symbol_addr(b"_ictag_start_");
	print("===================================================================")
	print("{0:>8}|{1:>21}|{2:>12}|{3:>12}|{4:>8}".format("Section", "Description", "Start (hex)", "End (hex)", "Used space"))
	print("-------------------------------------------------------------------")

	for i in range(len(sec_name)):
		#ss = e.get_symbol_addr(sec_start[i], sec_start_def[i]) + sec_start_add[i]
		ss = e.get_symbol_addr(sec_start[i], 0) + sec_start_add[i]
		#se = e.get_symbol_addr(sec_end[i], sec_end_def[i]) + sec_end_add[i]
		se = e.get_symbol_addr(sec_end[i], 0) + sec_end_add[i]
		sec_size.append(int(se - ss))
		print("{0:>8}|{1:>21}|{2:>12X}|{3:>12X}|{4:>8d}".format(sec_name[i], sec_des[i], ss, se, sec_size[i]))
	print("-------------------------------------------------------------------")

	retram_used = e.get_symbol_addr(b"_end_data_")  - SRAM_BASE_ADDR
	ram_used =  e.get_symbol_addr(b"_ram_use_end_") - SRAM_BASE_ADDR
	print("{0} : {1:d} {2}{3:X}{4}".format("Start Load SRAM", load_sram, "(ICtag: 0x", ictag,")"))
	print("{0} : {1:d} {2} {3}".format("Total Used RRAM", retram_used, "from", chip_retram_size))
	print("{0} : {1:d} {2} {3}".format("Total Used SRAM", ram_used, "from", chip_sram_size))
	print("{0} : {1:d}{2}{3}{4}{5}".format("Total Free SRAM", sec_size[2], " + stack[", sec_size[8], '] = ',  sec_size[2] + sec_size[8]))
	if sec_size[8] < 600:
		print("Warning! Stack is low!")
	if chip_retram_size < retram_used:
		print()
		print("Error: Retention SRAM Overflow (%i bytes)!" % (retram_used - chip_retram_size))
		sys.exit(1);
	if sec_size[11] > 0xBFFC:
		print()
		print("Error: FirmWare size Overflow (%i bytes)!" % (sec_size[11] - 0xBFFC))
		sys.exit(1);
	else:
		print("{0} : {1:d} {2} {3}".format("Total Free  OTA", 0xBFFC - sec_size[11] , "from", 0xBFFC))
	sys.exit(0);
	

if __name__ == '__main__':
	main()
