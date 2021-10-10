#pragma once

struct Header_t
{
	int signature = 'KEEK';
	int version = 4;
	int char_num = 0;
};

enum ASM_CODES
{
	BGN_CODE,
	PUSH_CODE,
	IN_CODE,
	POP_CODE,
	ADD_CODE,
	SUB_CODE,
	MUL_CODE,
	DIV_CODE,
	OUT_CODE,
	HLT_CODE,
};
