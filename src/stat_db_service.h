#ifndef _STAT_DB_SERVICE_H_
#define _STAT_DB_SERVICE_H_
#include "proxy_common.h"

struct sqlite3_stmt;
namespace proxy { namespace stat {
	using std::string;
	using std::vector;
	enum Dest
	{
		TO_SERVER,
		TO_CLIENT
	};

	struct Session {
		long long id;
		string user;
		char is_active;
		string src_addr;
		string src_port;
		string dst_addr;
		string dst_port;
		long long bytes_sent;
		long long bytes_recv;
	};

	void createDB();
	void createTable();
	long long create(std::string username, std::string src_add, std::string src_port,
		std::string dst_add, std::string dst_port);
	void update(long long session_id, size_t bytes, Dest dest);
	void close(long long session_id);
	Session selectSession(long long session_id);
	vector<Session> selectAll();
	void readRow(Session& s, sqlite3_stmt* stmt);

	namespace {
		std::mutex mutex;
		const char* db_path; // todo move to config?
		const string create_table_sql = "CREATE TABLE IF NOT EXISTS sessions("
			"id			INTEGER PRIMARY KEY AUTOINCREMENT, "
			"user		TEXT, "
			"is_active	INTEGER DEFAULT(1), "
			"src_addr	TEXT NOT NULL, "
			"src_port	VARCHAR(5) NOT NULL, "
			"dst_addr	TEXT NOT NULL, "
			"dst_port	VARCHAR(5) NOT NULL, "
			"bytes_sent INTEGER DEFAULT(0), "
			"bytes_recv INTEGER DEFAULT(0));";

		const char* create_session = "INSERT INTO SESSIONS "
			"(user, src_addr, src_port, dst_addr, dst_port)"
			" VALUES (?, ?, ?, ?, ?)";

		const char* update_sent_bytes = "UPDATE sessions SET bytes_sent = ? WHERE id = ?";
		const char* update_recv_bytes = "UPDATE sessions SET bytes_recv = ? WHERE id = ?";
		const char* update_inactive   = "UPDATE sessions SET is_active = 0 WHERE id = ?";
		const char* select_all = "SELECT * FROM sessions";
		const char* select_id = "SELECT * FROM sessions WHERE id = ?";
	}
}}
#endif
