#include "CAdmin.h"

CAdmin::CAdmin(MYSQL& sql):m_sql(sql) {
}

BOOL CAdmin::OnCheck(CSockLeo& sock) {
	char name[32];
	int n;
	if ((n = sock.Receive(name, sizeof(name), 0)) <= 0) {
		return FALSE;
	}
	name[n] = 0;
	char cmd[256] = { 0 };
	sprintf(cmd, "SELECT * FROM admin WHERE username='%s'", name);
	n = mysql_query(&m_sql, cmd);
	if (n) {
		std::cout << mysql_errno(&m_sql) << "--->" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}

	MYSQL_RES* result = mysql_store_result(&m_sql);
	if (result) {
		std::cout << mysql_errno(&m_sql) << "--->" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	n = (int)mysql_num_rows(result);
	sock.Send(&n, sizeof(n), 0);
	SAdmin admin;
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result)) {
		strcpy(admin.name, *row++);
		strcpy(admin.password, *row++);
		admin.priority = atoi(*row);
		sock.Send(&admin, sizeof(admin));
	}

	mysql_free_result(result);
	return TRUE;
}

BOOL CAdmin::OnPrint(CSockLeo& sock) {
	int n = mysql_query(&m_sql, "SELECT * FROM admin");
	if (n) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	MYSQL_RES* result = mysql_store_result(&m_sql);
	if (result) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	n = (int)mysql_num_rows(result);
	sock.Send(&n, sizeof(n));
	SAdmin admin;
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result)) {
		strcpy(admin.name, *row++);
		strcpy(admin.password, *row++);
		admin.priority = atoi(*row);
		sock.Send(&admin, sizeof(admin));
	}
	mysql_free_result(result);
	return TRUE;
}

BOOL CAdmin::OnLogin(CSockLeo& sock) {
	SAdmin admin;
	int n = sizeof(admin) - sizeof(int);
	if (sock.Receive(&admin, n) < n) {
		return FALSE;
	}
	char cmd[256] = { 0 };
	sprintf(cmd, "SELECT * FROM admin WHERE username='%s' and password='%s'", admin.name, admin.password);

	n = mysql_query(&m_sql, cmd);
	if (n) {
		std::cout << mysql_errno(&m_sql) << "==>" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	MYSQL_RES* result = mysql_store_result(&m_sql);
	if (!result) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	n = (int)mysql_num_rows(result);
	if (n > 0) {
		auto row = mysql_fetch_row(result);
		row++;
		row++;
		admin.priority = atoi(*row);
		sock.Send(&admin.priority, sizeof(admin.priority));
	} else {
		admin.priority = -1;
		return FALSE;
	}
	mysql_free_result(result);
	return TRUE;
}

BOOL CAdmin::OnInput(CSockLeo& sock) {
	SAdmin admin;
	if (sock.Receive(&admin, sizeof(admin)) < sizeof(admin)) {
		return FALSE;
	}
	char cmd[256] = { 0 };
	sprintf(cmd, "INSERT INTO admin VALUES('%s','%s',%d", admin.name, admin.password, admin.priority);
	int n = mysql_query(&m_sql, cmd);
	if (n) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	n = (int)mysql_affected_rows(&m_sql);
	sock.Send(&n, sizeof(n));
	return TRUE;
}

BOOL CAdmin::OnDelete(CSockLeo& sock) {
	char name[32] = { 0 };
	if (sock.Receive(name, sizeof(name)) <= 0) {
		return FALSE;
	}

	char cmd[256] = { 0 };
	sprintf(cmd, "DELETE FROM admin WHERE username='%s'", name);
	int n = mysql_query(&m_sql, cmd);
	if (n) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	n = (int)mysql_affected_rows(&m_sql);
	sock.Send(&n, sizeof(n));
	return TRUE;
}

BOOL CAdmin::OnChangePass(CSockLeo& sock) {
	SAdmin admin;
	int n;
	if ((n = sock.Receive(admin.name, sizeof(admin.name))) <= 0) {
		return FALSE;
	}
	admin.name[n] = 0;
	int ACK = 0;
	sock.Send(&ACK, sizeof(ACK));
	if ((n = sock.Receive(admin.password, sizeof(admin.password))) <= 0) {
		return FALSE;
	}
	admin.password[n] = 0;

	char cmd[256] = { 0 };
	sprintf(cmd, "UPDATE admin SET password='%s' WHERE username='%s'", admin.password,admin.name);
	n = mysql_query(&m_sql, cmd);
	if (n) {
		std::cout << mysql_errno(&m_sql) << "===>" << mysql_error(&m_sql) << std::endl;
		return FALSE;
	}
	n = (int)mysql_affected_rows(&m_sql);
	sock.Send(&n, sizeof(n));
	return TRUE;
}
