#pragma once

const int Signature = 'KEEK';
const int Version = 5;
const int	RAM_MEM = 20;

struct Header_t
{
	int sign = signature;
	int ver = version;
	int char_num = 0;
};
