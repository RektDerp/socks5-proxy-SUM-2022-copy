#include "stat_db_service.h"
#include "stat_handlers.h"
#include "string_utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <iostream>

namespace proxy { namespace stat {
	using namespace std;
	using boost::format;
	using boost::str;
	using string_utils::concat;

	db_service* db_service::_instance = nullptr;
	mutex db_service::_mutex;

	db_service& db_service::getInstance(const string& db_path) throw(db_exception)
	{
		lock_guard<mutex> guard(_mutex);
		if (_instance == nullptr)
		{
			_instance = new db_service(db_path);
		}
		return *_instance;
	}

	void db_service::createDB() throw(db_exception)
	{
		db_connection db(_db_path);
	}

	void db_service::createTable() throw(db_exception)
	{
		db_connection DB(_db_path);
		char* messageError;
		int err = sqlite3_exec(DB, create_table_sql.c_str(), NULL, 0, &messageError);
		if (err != SQLITE_OK) {
			db_exception ex(concat("createTable: error during executing stmt: %1%", messageError));
			sqlite3_free(messageError);
			throw ex;
		}
		else
			cout << "Table created Successfully" << endl;
	}

	long long db_service::create(const session s) throw(db_exception)
	{
		lock_guard<mutex> guard(_mutex);
		db_connection db(_db_path);
		db_stmt stmt;
		int err = sqlite3_prepare_v2(db, create_session, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("Create: error during preparing stmt: %1%", err));
		}

		int index = 0;
		err = sqlite3_bind_text(stmt, ++index, s.user.c_str(), s.user.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.type.c_str(), s.type.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.src_addr.c_str(), s.src_addr.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.src_port.c_str(), s.src_port.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.dst_addr.c_str(), s.dst_addr.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, ++index, s.dst_port.c_str(), s.dst_port.length(), SQLITE_STATIC);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("Create: error during binding stmt: %1%", err));
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			throw db_exception(concat("Create: error during executing stmt: %1%", err));
		}

		long long id = sqlite3_last_insert_rowid(db);
		cout << "Created session with id " << id << endl;
		return id;
	}
	
	void db_service::update(long long session_id, size_t bytes, Dest dest) throw(db_exception)
	{
		lock_guard<mutex> guard(_mutex);
		session s = selectSession(session_id);
		if (s.id == 0)
		{
			throw db_exception(concat("Update: no session with id: %1%", session_id));
		}
		long long newBytes;
		db_connection db(_db_path);
		db_stmt stmt;
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
			throw db_exception(concat("Update: error during preparing stmt: %1%", err));
		}
		err = sqlite3_bind_int64(stmt, 1, newBytes);
		err = sqlite3_bind_int64(stmt, 2, session_id);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("Update: error during binding stmt: %1%", err));
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			throw db_exception(concat("Update: error during executing stmt: %1%", err));
		}
	}

	void db_service::close(long long session_id) throw(db_exception)
	{
		lock_guard<mutex> guard(_mutex);
		session s = selectSession(session_id);
		if (s.id == 0) {
			throw db_exception(concat("Close: no session with id: %1%", session_id));
		}
		db_connection db(_db_path);
		db_stmt stmt;
		int err = sqlite3_prepare_v2(db, update_inactive, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("Close: error during preparing stmt: %1%", err));
		}
		sqlite3_bind_int64(stmt, 1, s.id);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("Close: error during binding stmt: %1%", err));
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			throw db_exception(concat("Close: error during executing stmt: %1%", err));
		}
	}

	session db_service::selectSession(long long session_id) throw(db_exception)
	{
		db_connection db(_db_path);
		db_stmt stmt;
		int err = sqlite3_prepare_v2(db, select_id, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("Select: error during preparing stmt: %1%", err));
		}
		sqlite3_bind_int64(stmt, 1, session_id);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("Select: error during binding stmt: %1%", err));
		}
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			session s;
			readRow(s, stmt);
			return s;
		}
		else {
			throw db_exception(concat("Select: error during executing stmt: %1%", err));
		}
	}

	vector<session> db_service::selectAll() throw(db_exception)
	{
		int err;
		db_connection db(_db_path);
		db_stmt stmt;
		err = sqlite3_prepare_v2(db, select_all, -1, stmt, nullptr);
		if (err != SQLITE_OK)
		{
			throw db_exception(concat("SelectAll: error during preparing stmt: %1%", err));
		}
		vector<session> vec;

		for (;;) {
			err = sqlite3_step(stmt);
			if (err != SQLITE_ROW)
				break;

			session s;
			readRow(s, stmt);
			vec.push_back(s);
		}
		return vec;
	}

	void db_service::readRow(session& s, sqlite3_stmt* stmt)
	{
		s.id = sqlite3_column_int(stmt, 0);
		const char* user = (const char*) sqlite3_column_text(stmt, 1);
		if (user == nullptr)
		{
			s.user = "[NO AUTH]";
		}
		else {
			s.user = string(user, strlen(user));
		}
		s.is_active = sqlite3_column_int(stmt, 2);
		const char* src_addr = (const char*) sqlite3_column_text(stmt, 3);
		const char* src_port = (const char*) sqlite3_column_text(stmt, 4);
		const char* dst_addr = (const char*) sqlite3_column_text(stmt, 5);
		const char* dst_port = (const char*) sqlite3_column_text(stmt, 6);
		if (src_addr != nullptr) s.src_addr = string(src_addr, strlen(src_addr));
		if (src_port != nullptr) s.src_port = string(src_port, strlen(src_port));
		if (dst_addr != nullptr) s.dst_addr = string(dst_addr, strlen(dst_addr));
		if (dst_port != nullptr) s.dst_port = string(dst_port, strlen(dst_port));
		s.bytes_sent = sqlite3_column_int(stmt, 7);
		s.bytes_recv = sqlite3_column_int(stmt, 8);
	}
}}
