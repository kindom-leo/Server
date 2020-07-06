#pragma once
#include "define.h"
#include "CAdmin.h"
#include <mysql.h>
class CWorker {
	MYSQL m_sql;
	CAdmin m_admin = m_sql;
	BOOL OnPrint(CSockLeo& sock);
	BOOL OnInput(CSockLeo& sock);
	BOOL OnDelete(CSockLeo& sock);
	BOOL OnModify(CSockLeo& sock);
	BOOL OnReceive(CSockLeo& sock);
	BOOL OnFindByNumb(CSockLeo& sock);
	BOOL OnFindByName(CSockLeo& sock);
	BOOL OnFindBySalary(CSockLeo& sock);
	BOOL OnFindByDate(CSockLeo& sock);

	bool Start();
	bool ConnectDB();
	static THREAD_RET theProc(void*);

public:
	int Main();
	BOOL Select(LPCSTR lpQueryCmd, CSockLeo& sock);
};