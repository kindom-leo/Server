#pragma once
#include <iostream>
#include "CSockLeo.h"

#define SERV_PORT 6666

enum {
	WK_INPUT = 0x1000,
	WK_PRINT,
	WK_DELETE,
	WK_MODIFY,
	WK_FIND_NUMB,
	WK_FIND_NAME,
	WK_FIND_SALA,
	WK_FIND_DATE,

	US_LOGIN = 0x2000,
	US_BROWSE,
	US_INPUT,
	US_DELETE,
	US_CHPASS,
	US_FIND,
};

struct SInfo {
	int nId;
	char name[32];
	float salary;
	char date[32];
};

struct SAdmin {
	int		priority;
	char	name[32];
	char	password[32];
};