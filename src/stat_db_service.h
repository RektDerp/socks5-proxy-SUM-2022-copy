#ifndef _STAT_DB_SERVICE_H_
#define _STAT_DB_SERVICE_H_

#include "proxy_exceptions.h"
#include <string>
#include <vector>
#include <mutex>

struct sqlite3_stmt;

namespace proxy {
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
		string create_date;
		string update_date;
		char is_active;
		string src_addr;
		string src_port;
		string dst_addr;
		string dst_port;
		long long bytes_sent;
		long long bytes_recv;
	};

	class DatabaseService {
	private:
		static std::unique_ptr<DatabaseService> _instance;
		static std::mutex _mutex;
	public:
		static DatabaseService& getInstance(const string& db_path = "./sessions_stat.db");

		~DatabaseService() = default;
		void createDB();
		void createTable();
		long long create(const Session s);
		void update(long long session_id, int bytes, Dest dest);
		void close(long long session_id);
		Session selectSession(long long session_id);
		vector<Session> selectAll();
	private:
		const string _db_path;

		DatabaseService(const std::string& db_path) :
			_db_path(db_path)
		{
			createDB();
			createTable();
		}

		void readRow(Session& s, sqlite3_stmt* stmt);

		DatabaseService(const DatabaseService&) = delete;
		DatabaseService& operator=(const DatabaseService&) = delete;
	};

	namespace {
		const char* create_table_sql = "CREATE TABLE IF NOT EXISTS sessions("
			"id			INTEGER PRIMARY KEY AUTOINCREMENT, "
			"user		TEXT, "
			"create_date TEXT DEFAULT(datetime()), "
			"update_date TEXT DEFAULT(datetime()), "
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

		const char* update_sent_bytes = "UPDATE sessions SET update_date = datetime(), bytes_sent = ? WHERE id = ?";
		const char* update_recv_bytes = "UPDATE sessions SET update_date = datetime(), bytes_recv = ? WHERE id = ?";
		const char* update_inactive   = "UPDATE sessions SET update_date = datetime(), is_active = 0 WHERE id = ?";
		const char* set_all_inactive   = "UPDATE sessions SET is_active = 0";
		const char* select_all = "SELECT * FROM sessions";
		const char* select_id = "SELECT * FROM sessions WHERE id = ?";
	}
} // namespace proxy 
#endif // _STAT_DB_SERVICE_H_
