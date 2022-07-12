#include "stat_db_service.h"

#include <sqlite3.h>
#include <stdio.h>
#include <iostream>

namespace proxy { namespace stat {
	using namespace std;

	void createDB()
	{
		sqlite3* DB;
		int err = sqlite3_open(db_path, &DB);
		if (err != SQLITE_OK) {
			cerr << "Error occured during creation of database: " << err << "\n";
			cerr << db_path << "\n";
		}
		sqlite3_close(DB);
	}

	void createTable()
	{
		sqlite3* DB;
		char* messageError;
		try
		{
			int err = sqlite3_open(db_path, &DB);
			err = sqlite3_exec(DB, create_table_sql.c_str(), NULL, 0, &messageError);
			if (err != SQLITE_OK) {
				cerr << "Error in createTable function." << endl;
				sqlite3_free(messageError);
			}
			else
				cout << "Table created Successfully" << endl;
		}
		catch (const exception& e)
		{
			cerr << e.what();
		}

		sqlite3_close(DB);
	}

	long long create(std::string username, std::string src_addr, std::string src_port,
		std::string dst_addr, std::string dst_port)
	{
		std::lock_guard<std::mutex> guard(mutex);
		sqlite3* db;
		sqlite3_open(db_path, &db);
		sqlite3_stmt* stmt;
		int err = sqlite3_prepare_v2(db,
			create_session,
			-1, &stmt, nullptr);
		if (err != SQLITE_OK)
		{
			cerr << "Error during preparing create stmt.\n";
			return 0;
		}

		err = sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, 2, src_addr.c_str(), src_addr.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, 3, src_port.c_str(), src_port.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, 4, dst_addr.c_str(), dst_addr.length(), SQLITE_STATIC);
		err = sqlite3_bind_text(stmt, 5, dst_port.c_str(), dst_port.length(), SQLITE_STATIC);
		if (err != SQLITE_OK)
		{
			cerr << "Error during binding create stmt.\n";
			return 0;
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			cerr << "Error during executing create stmt: " << err << endl;
			return 0;
		}

		long long id = sqlite3_last_insert_rowid(db);
		cout << "Created session with id " << id << endl;
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return id;
	}

	void update(long long session_id, size_t bytes, Dest dest)
	{
		std::lock_guard<std::mutex> guard(mutex);
		Session s = selectSession(session_id);
		if (s.id == 0) {
			cerr << "No session with id " << session_id << std::endl;
			return;
		}
		long long newBytes;
		sqlite3* db;
		sqlite3_open(db_path, &db);
		sqlite3_stmt* stmt;
		int err;
		if (dest == Dest::TO_CLIENT) {
			newBytes = s.bytes_recv + bytes;
			err = sqlite3_prepare_v2(db,
				update_recv_bytes,
				-1, &stmt, nullptr);
		}
		else {
			newBytes = s.bytes_sent + bytes;
			err = sqlite3_prepare_v2(db,
				update_sent_bytes,
				-1, &stmt, nullptr);
		}
		if (err != SQLITE_OK)
		{
			cerr << "Error during preparing update stmt.\n";
			return;
		}
		err = sqlite3_bind_int64(stmt, 1, newBytes);
		err = sqlite3_bind_int64(stmt, 2, session_id);
		if (err != SQLITE_OK)
		{
			cerr << "Error during binding update stmt.\n";
			return;
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			cerr << "Error during executing update stmt.\n";
		}
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}

	void close(long long session_id)
	{
		Session s = selectSession(session_id);
		if (s.id == 0) {
			cerr << "No session with id " << session_id << std::endl;
		}
		sqlite3* db;
		sqlite3_open(db_path, &db);
		sqlite3_stmt* stmt;
		int err = sqlite3_prepare_v2(db,
			update_inactive,
			-1, &stmt, nullptr);
		if (err != SQLITE_OK)
		{
			cerr << "Error during preparing update close stmt.\n";
			return;
		}
		sqlite3_bind_int64(stmt, 1, s.id);
		if (err != SQLITE_OK)
		{
			cerr << "Error during binding update close stmt.\n";
			return;
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			cerr << "Error during executing update close stmt.\n";
		}
		sqlite3_finalize(stmt);
		sqlite3_close(db);
	}

	Session selectSession(long long session_id)
	{
		sqlite3* db;
		sqlite3_open(db_path, &db);
		sqlite3_stmt* stmt;
		int err = sqlite3_prepare_v2(db,
			select_id,
			-1, &stmt, nullptr);
		if (err != SQLITE_OK)
		{
			cerr << "Error during preparing select stmt.\n";
			return {};
		}
		sqlite3_bind_int64(stmt, 1, session_id);
		if (err != SQLITE_OK)
		{
			cerr << "Error during binding select stmt.\n";
			return {};
		}
		Session s;
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			readRow(s, stmt);
		}
		else {
			cerr << "id: " << session_id << 
				" Error during executing select stmt: " << err << endl;
		}
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return s;
	}

	vector<Session> selectAll()
	{
		int err;
		sqlite3* db;
		err = sqlite3_open(db_path, &db);
		if (err != SQLITE_OK)
		{
			cerr << "Error opening " << db_path << ": " << err << endl;
			return {};
		}
		sqlite3_stmt* stmt;
		err = sqlite3_prepare_v2(db, select_all, -1, &stmt, nullptr);
		if (err != SQLITE_OK)
		{
			cerr << "Error during preparing selectAll stmt: " << err << endl;
			sqlite3_close(db);
			return {};
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
		
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return vec;
	}

	void readRow(Session& s, sqlite3_stmt* stmt)
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
