#pragma once

const int Signature   = 'KEEK';
const int Version     =      8;
const int	RAM_MEM     =   1024;
const int REG_NUM     =     26;
const int XRESOLUTION =   0x24;
const int YRESOLUTION =   0x10;

struct Header_t
{
	int sign     =   Signature;
	int ver      =     Version;
	int char_num =           0;
	int xRes     = XRESOLUTION;
	int yRes     = YRESOLUTION;
};
