#ifndef _STAT_DB_SERVICE_H_
#define _STAT_DB_SERVICE_H_
#include "proxy_common.h"
#include "proxy_exceptions.h"

struct sqlite3_stmt;

namespace proxy { namespace stat {
	using std::string;
	using std::vector;

	enum Dest
	{
		TO_SERVER,
		TO_CLIENT
	};

	struct session {
		long long id;
		string user;
		char is_active;
		string type;
		string src_addr;
		string src_port;
		string dst_addr;
		string dst_port;
		long long bytes_sent;
		long long bytes_recv;
	};

	class db_service {
	private:
		static db_service* _instance;
		static std::mutex _mutex;
	public:
		static db_service& getInstance(const string& db_path = R"(./sessions_stat.db)") throw(db_exception);

		~db_service() = default;
		void createDB() throw(db_exception);
		void createTable() throw(db_exception);
		long long create(const session s) throw(db_exception);
		void update(long long session_id, int bytes, Dest dest) throw(db_exception);
		void close(long long session_id) throw(db_exception);
		session selectSession(long long session_id) throw(db_exception);
		vector<session> selectAll() throw(db_exception);
	private:
		const string _db_path; // todo move to config?

		db_service(const std::string& db_path) throw(db_exception) :
			_db_path(db_path)
		{
			createDB();
			createTable();
		}

		void readRow(session& s, sqlite3_stmt* stmt);

		db_service(const db_service&) = delete;
		db_service& operator=(const db_service&) = delete;
	};

	namespace {
		const string create_table_sql = "CREATE TABLE IF NOT EXISTS sessions("
			"id			INTEGER PRIMARY KEY AUTOINCREMENT, "
			"user		TEXT, "
			"is_active	INTEGER DEFAULT(1), "
			"type		TEXT, "
			"src_addr	TEXT NOT NULL, "
			"src_port	VARCHAR(5) NOT NULL, "
			"dst_addr	TEXT NOT NULL, "
			"dst_port	VARCHAR(5) NOT NULL, "
			"bytes_sent INTEGER DEFAULT(0), "
			"bytes_recv INTEGER DEFAULT(0));";

		const char* create_session = "INSERT INTO SESSIONS "
			"(user, type, src_addr, src_port, dst_addr, dst_port)"
			" VALUES (?, ?, ?, ?, ?, ?)";

		const char* update_sent_bytes = "UPDATE sessions SET bytes_sent = ? WHERE id = ?";
		const char* update_recv_bytes = "UPDATE sessions SET bytes_recv = ? WHERE id = ?";
		const char* update_inactive   = "UPDATE sessions SET is_active = 0 WHERE id = ?";
		const char* select_all = "SELECT * FROM sessions";
		const char* select_id = "SELECT * FROM sessions WHERE id = ?";
	}
}} // namespace proxy stat
#endif // _STAT_DB_SERVICE_H_
