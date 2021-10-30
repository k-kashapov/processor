#pragma once

const int Signature = 'KEEK';
const int Version = 8;
const int	RAM_MEM = 1024;
const int REG_NUM =   26;

struct Header_t
{
	int sign = Signature;
	int ver = Version;
	int char_num = 0;
	int xRes = 0x24;
	int yRes = 0x10;
};
