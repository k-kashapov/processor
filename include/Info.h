#pragma once

const int Signature = 'KEEK';
const int Version = 7;
const int	RAM_MEM = 0x1000;
const int REG_NUM =      4;

struct Header_t
{
	int sign = Signature;
	int ver = Version;
	int char_num = 0;
	int video_mem = 0x800;
};
