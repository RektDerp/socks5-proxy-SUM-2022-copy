#include "stat_db_service.h"
#include "stat_handlers.h"
#include "string_utils.h"
#include "Logger.h"
#include <sqlite3.h>
#include <iostream>
#include <string.h>

namespace proxy {
	using std::string;
	using std::lock_guard;
	using std::mutex;
	using std::vector;
	using string_utils::concat;
	using string_utils::to_string;

	std::unique_ptr<DatabaseService> DatabaseService::_instance = nullptr;
	mutex DatabaseService::_mutex;

	DatabaseService& DatabaseService::getInstance(const string& db_path)
	{
		lock_guard<mutex> guard(_mutex);
		if (_instance == nullptr)
		{
			_instance = std::unique_ptr<DatabaseService>(new DatabaseService(db_path));
		}
		return *_instance;
	}

	void DatabaseService::createDB()
	{
		DatabaseConnection db(_db_path);
		log(DEBUG_LOG) << "Database created successfully.";
	}

	void DatabaseService::createTable()
	{
		DatabaseConnection DB(_db_path);
		int err = sqlite3_exec(DB, create_table_sql, NULL, 0, NULL);
		if (err != SQLITE_OK) {
			throw DatabaseException("createTable: error during executing stmt: " + string(sqlite3_errstr(err)));
		}
		err = sqlite3_exec(DB, set_all_inactive, NULL, 0, NULL);
		if (err != SQLITE_OK) {
			throw DatabaseException("createTable: error during executing stmt: " + string(sqlite3_errstr(err)));
		}
		log(DEBUG_LOG) << "Table created successfully.";
	}

	long long DatabaseService::create(const Session s)
	{
		lock_guard<mutex> guard(_mutex);
		DatabaseConnection db(_db_path);
		DatabaseStatement stmt;
		int err = sqlite3_prepare_v2(db, create_session, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw DatabaseException("Create: error during preparing stmt: " + string(sqlite3_errstr(err)));
		}

		int index = 0;
		err = sqlite3_bind_text(stmt, ++index, s.user.c_str(), -1, SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.src_addr.c_str(), -1, SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.src_port.c_str(), -1, SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.dst_addr.c_str(), -1, SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.dst_port.c_str(), -1, SQLITE_STATIC);
		if (err != SQLITE_OK)
		{
			throw DatabaseException("Create: error during binding stmt: " + string(sqlite3_errstr(err)));
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			throw DatabaseException("Create: error during executing stmt: " + string(sqlite3_errstr(err)));
		}

		long long id = sqlite3_last_insert_rowid(db);
		log(TRACE_LOG) << "Created session with id " << id;
		return id;
	}
	
	void DatabaseService::update(long long session_id, size_t bytes, Dest dest)
	{
		lock_guard<mutex> guard(_mutex);
		Session s = selectSession(session_id);
		if (s.id == 0)
		{
			throw DatabaseException(concat("Update: no session with id: ", session_id));
		}
		long long newBytes;
		DatabaseConnection db(_db_path);
		DatabaseStatement stmt;
		int err;
		if (dest == Dest::TO_CLIENT) {
			newBytes = s.bytes_recv + bytes;
			err = sqlite3_prepare_v2(db, update_recv_bytes, -1, stmt, nullptr);
		}
		else {
			newBytes = s.bytes_sent + bytes;
			err = sqlite3_prepare_v2(db, update_sent_bytes, -1, stmt, nullptr);
		}
		if (err != SQLITE_OK)
		{
			throw DatabaseException("Update: error during preparing stmt: " + string(sqlite3_errstr(err)));
		}
		sqlite3_bind_int64(stmt, 1, newBytes);
		sqlite3_bind_int64(stmt, 2, session_id);
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			throw DatabaseException("Update: error during executing stmt: " + string(sqlite3_errstr(err)));
		}
	}

	void DatabaseService::close(long long session_id)
	{
		lock_guard<mutex> guard(_mutex);
		Session s = selectSession(session_id);
		if (s.id == 0) {
			throw DatabaseException(concat("Close: no session with id: ", session_id));
		}
		DatabaseConnection db(_db_path);
		DatabaseStatement stmt;
		int err = sqlite3_prepare_v2(db, update_inactive, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw DatabaseException("Close: error during preparing stmt: " + string(sqlite3_errstr(err)));
		}
		sqlite3_bind_int64(stmt, 1, s.id);
		if (err != SQLITE_OK)
		{
			throw DatabaseException("Close: error during binding stmt: " + string(sqlite3_errstr(err)));
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			throw DatabaseException("Close: error during executing stmt: " + string(sqlite3_errstr(err)));
		}
	}

	Session DatabaseService::selectSession(long long session_id)
	{
		DatabaseConnection db(_db_path);
		DatabaseStatement stmt;
		int err = sqlite3_prepare_v2(db, select_id, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw DatabaseException("Select: error during preparing stmt: " + string(sqlite3_errstr(err)));
		}
		sqlite3_bind_int64(stmt, 1, session_id);
		if (err != SQLITE_OK)
		{
			throw DatabaseException("Select: error during binding stmt: " + string(sqlite3_errstr(err)));
		}
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Session s;
			readRow(s, stmt);
			return s;
		}
		else {
			throw DatabaseException("Select: error during executing stmt: " + string(sqlite3_errstr(err)));
		}
	}

	vector<Session> DatabaseService::selectAll()
	{
		int err;
		DatabaseConnection db(_db_path);
		DatabaseStatement stmt;
		err = sqlite3_prepare_v2(db, select_all, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw DatabaseException("SelectAll: error during preparing stmt: " + string(sqlite3_errstr(err)));
		}
		vector<Session> vec;

		for (;;) {
			err = sqlite3_step(stmt);
			if (err != SQLITE_ROW)
				break;

			Session s;
			readRow(s, stmt);
			vec.push_back(s);
		}
		return vec;
	}

	void DatabaseService::readRow(Session& s, sqlite3_stmt* stmt)
	{
		int index = 0;
		s.id			= sqlite3_column_int(stmt, index++);
		s.user			= to_string(sqlite3_column_text(stmt, index++));
		s.create_date	= to_string(sqlite3_column_text(stmt, index++));
		s.update_date	= to_string(sqlite3_column_text(stmt, index++));
		s.is_active		= sqlite3_column_int(stmt, index++);
		s.src_addr		= to_string(sqlite3_column_text(stmt, index++));
		s.src_port		= to_string(sqlite3_column_text(stmt, index++));
		s.dst_addr		= to_string(sqlite3_column_text(stmt, index++));
		s.dst_port		= to_string(sqlite3_column_text(stmt, index++));
		s.bytes_sent	= sqlite3_column_int(stmt, index++);
		s.bytes_recv	= sqlite3_column_int(stmt, index++);
	}
} // namespace proxy
