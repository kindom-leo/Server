#include "define.h"
#include <mysql.h>

class CAdmin {
	MYSQL& m_sql;
public:
	CAdmin(MYSQL&);
	BOOL OnCheck(CSockLeo& sock);
	BOOL OnPrint(CSockLeo& sock);
	BOOL OnLogin(CSockLeo& sock);
	BOOL OnInput(CSockLeo& sock);
	BOOL OnDelete(CSockLeo& sock);
	BOOL OnChangePass(CSockLeo& sock);
};
