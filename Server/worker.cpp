#include "worker.h"

CWorker w;
BOOL CWorker::OnPrint(CSockLeo& sock) {
	int n = 0;
	if (sock.Receive(&n, sizeof(n)) < sizeof(n)) {
		return FALSE;
	}
	const char* ps[] = {"t_id","t_name","t_sala","t_date",nullptr};
	char cmd[64] = { "SELECT * FROM employee" };
	if (n > -1 && n < 4) {
		strcat(cmd, " ORDER BY ");
		strcat(cmd, ps[n]);
	}
	
	return Select(cmd,sock);
}

BOOL CWorker::OnInput(CSockLeo& sock) {
	SInfo data;
	if (sock.Receive(&data, sizeof(data)) < sizeof(data)) {
		return FALSE;
	}
	char cmd[256] = { 0 };
	sprintf(cmd, "INSERT INTO emplyoee VALUES(%d,'%s',%f,'%s')", data.nId, data.name, data.salary, data.date);
	int n = mysql_query(&m_sql, cmd);
	if (n) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
	}
	return !n;
}

BOOL CWorker::OnDelete(CSockLeo& sock) {
	int nid = 0;
	if (sock.Receive(&nid, sizeof(nid)) < sizeof(nid)) {
		return FALSE;
	}
	char cmd[64] = { 0 };
	sprintf(cmd, "DELETE FROM emplyoee WHERE t_id=%d", nid);
	int ret = mysql_query(&m_sql, cmd);
	if (ret) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
	}
	return !ret	;
}

BOOL CWorker::OnModify(CSockLeo& sock) {
	SInfo data;
	if (sock.Receive(&data, sizeof(data)) < sizeof(data)) {
		return FALSE;
	}
	char cmd[256] = { 0 };
	sprintf(cmd, "UPDATE emplyoee SET t_name='%s',t_sala=%f,t_data='%s' "
		"WHERE tid=%d", data.name,data.salary,data.date,data.nId);
	int ret = mysql_query(&m_sql, cmd);
	if (ret) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
	}
	return !ret;
}

BOOL CWorker::OnReceive(CSockLeo& sock) {
	int cmd;
	if (sock.Receive(&cmd, sizeof(cmd)) < sizeof(cmd)) {
		return FALSE;
	}
	switch (cmd) {
	case WK_INPUT:
		return OnInput(sock);
	case WK_PRINT:
		return OnPrint(sock);
	case WK_DELETE:
		return OnDelete(sock);
	case WK_MODIFY:
		return OnModify(sock);
	case WK_FIND_NUMB:
		return OnFindByNumb(sock);
	case WK_FIND_NAME:
		return OnFindByName(sock);
	case WK_FIND_SALA:
		return OnFindBySalary(sock);
	case WK_FIND_DATE:
		return OnFindByDate(sock);

	case US_BROWSE:
		return m_admin.OnPrint(sock);
	case US_CHPASS:
		return m_admin.OnChangePass(sock);
	case US_DELETE:
		return m_admin.OnDelete(sock);
	case US_FIND:
		return m_admin.OnCheck(sock);
	case US_INPUT:
		return m_admin.OnInput(sock);
	case US_LOGIN:
		return m_admin.OnLogin(sock);
	}
	return FALSE;
}

BOOL CWorker::OnFindByNumb(CSockLeo& sock) {
	int nid;
	if (sock.Receive(&nid, sizeof(nid)) < sizeof(nid)) {
		return FALSE;
	}
	char cmd[64];
	sprintf(cmd, "SELECT * FROM emplyoee WHERE t_id=%d", nid);
	return Select(cmd,sock);
}

BOOL CWorker::OnFindByName(CSockLeo& sock) {
	char name[32];
	int len;
	if ((len = sock.Receive(name, sizeof(name))) <= 0) {
		return FALSE;
	}
	name[len] = 0;
	char cmd[256];
	sprintf(cmd, "SELECT * FROM employee WHERE t_name LIKE '%%%s%%'", name);
	return Select(cmd,sock);
}

BOOL CWorker::OnFindBySalary(CSockLeo& sock) {
	float sala_min,sala_max;
	if (sock.Receive(&sala_min, sizeof(sala_min)) < sizeof(sala_min)) {
		return FALSE;
	}
	if (sock.Receive(&sala_max, sizeof(sala_max)) < sizeof(sala_max)) {
		return FALSE;
	}
	char cmd[256];
	sprintf(cmd, "SELECT * FROM emplyoee WHERE t_sala>=%f AND t_sala<=%f",sala_min,sala_max);
	return Select(cmd,sock);
}

BOOL CWorker::OnFindByDate(CSockLeo& sock) {
	char data_min[32], data_max[32];
	int n = sock.Receive(data_min, sizeof(data_min));
	if (n <= 0) {
		return FALSE;
	}
	sock.Send(&n, sizeof(n));
	n = sock.Receive(data_max, sizeof(data_max));
	if (n <= 0) {
		return FALSE;
	} 
	char cmd[256];
	sprintf(cmd, "SELECT * FROM emplyoee WHERE t_date>='%s' AND t_date<='%s'", data_min, data_max);
	return Select(cmd,sock);
}

bool CWorker::Start() {
	CSockLeo sock;
	if (!sock.Create(TEXT("127.0.0.1"), SERV_PORT, SOCK_STREAM)) {
		return false;
	}
	sock.Listen();
	CSockLeo* pNewSock = new CSockLeo;
	TCHAR bufAddr[1024] = { 0 };
	UINT port = 0;
	while (sock.Accept(*pNewSock, bufAddr, &port)) {
		std::cout << bufAddr << ":" << port << "has comming" << std::endl;
#ifdef _WIN32
		_beginthread(theProc, 0, pNewSock);
#else
		pthread_t tid;
		if (pthread_create(&tid, NULL, theProc, pNewSock) < 0) {
			fprintf(stderr, "pthread_create error:%s\n", strerror(errno));
			return false;
		}
#endif
		pNewSock = new CSockLeo;
	}
	delete pNewSock;
	return true;
}

bool CWorker::ConnectDB() {
	if (!mysql_init(&m_sql)) {
		printf("mysql_init failed!\n");
		return false;
	}
	if (!mysql_real_connect(&m_sql, "127.0.0.1", "root", "ljt111104", "worker", 3306, NULL, 0)) {
		printf("mysql_real_connect() failed! errno:%d, error: %s\n",mysql_errno(&m_sql),mysql_error(&m_sql));
		return false;
	}
	mysql_query(&m_sql, "set names 'gbk'");
	return true;
}

THREAD_RET CWorker::theProc(void* sock) {
	CSockLeo* psock = (CSockLeo*)sock;
	while (w.OnReceive(*psock))
		;
	delete psock;
#ifndef _WIN32
	return NULL;
#endif
}

int CWorker::Main() {
	if (!ConnectDB()) {
		return -1;
	}
	Start();
	return 0;
}

BOOL CWorker::Select(LPCSTR lpQueryCmd, CSockLeo& sock) {
	int ret = mysql_query(&m_sql, lpQueryCmd);
	if (ret) {
		std::cout << mysql_errno(&m_sql) << "===> " << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}

	MYSQL_RES* result = mysql_store_result(&m_sql);
	if (!result) {
		std::cout << mysql_errno(&m_sql) << "===> " << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}

	auto number = mysql_num_rows(result);
	sock.Send(&number, sizeof(number));
	SInfo data;
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result)) {
		data.nId = atoi(*row++);
		strcpy(data.name, *row++);
		data.salary = (float)atof(*row++);
		strcpy(data.date, *row);
		sock.Send(&data, sizeof(data));
	}

	mysql_free_result(result);
	return TRUE;
}



