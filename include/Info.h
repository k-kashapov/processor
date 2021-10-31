#pragma once

const int Signature   = 0x4B45454B;
const int Version     =          8;
const int	RAM_MEM     =       1024;
const int REG_NUM     =         26;
const int XRESOLUTION =       0x24;
const int YRESOLUTION =       0x10;

struct Header_t
{
	unsigned long sign     =   Signature;
	unsigned long ver      =     Version;
	unsigned long char_num =           0;
	long xRes              = XRESOLUTION;
	long yRes              = YRESOLUTION;
};
