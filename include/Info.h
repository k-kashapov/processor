#pragma once

const int Signature = 'KEEK';
const int Version = 6;
const int	RAM_MEM = 20;
const int REG_NUM = 4;

struct Header_t
{
	int sign = Signature;
	int ver = Version;
	int char_num = 0;
};
