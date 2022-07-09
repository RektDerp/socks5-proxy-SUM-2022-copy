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
			cerr << "Error occured during creation of database\n";
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
			cerr << "Error during executing create stmt.\n";
			return 0;
		}

		long long id = sqlite3_last_insert_rowid(db);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return id;
	}

	void update(long long session_id, size_t bytes, Dest dest)
	{
		Session s = selectSession(session_id);
		if (s.id == 0) {
			cerr << "No session with id " << session_id << std::endl;
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
		sqlite3_bind_int64(stmt, 1, newBytes);
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
			cerr << "Error during preparing create stmt.\n";
			return;
		}
		sqlite3_bind_int64(stmt, 1, s.id);
		if (err != SQLITE_OK)
		{
			cerr << "Error during binding create stmt.\n";
			return;
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			cerr << "Error during executing update stmt.\n";
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
			cerr << "Error during preparing create stmt.\n";
			return {};
		}
		sqlite3_bind_int64(stmt, 1, session_id);
		if (err != SQLITE_OK)
		{
			cerr << "Error during binding create stmt.\n";
			return {};
		}
		err = sqlite3_step(stmt);
		if (err != SQLITE_DONE)
		{
			cerr << "Error during executing update stmt.\n";
			return {};
		}
		Session s;
		readRow(&s, stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return s;
	}

	vector<Session> selectAll()
	{
		sqlite3* db;
		sqlite3_open(db_path, &db);
		sqlite3_stmt* stmt;
		int err = sqlite3_prepare_v2(db,
			select_id,
			-1, &stmt, nullptr);
		if (err != SQLITE_OK)
		{
			cerr << "Error during preparing create stmt.\n";
			return {};
		}
		
		if (err != SQLITE_DONE)
		{
			cerr << "Error during executing update stmt.\n";
			return {};
		}
		vector<Session> vec;

		for (;;) {
			err = sqlite3_step(stmt);
			if (err != SQLITE_ROW)
				break;

			Session s;
			readRow(&s, stmt);
			vec.push_back(s);
		}
		
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return vec;
	}

	void readRow(Session* s, sqlite3_stmt* stmt)
	{
		s->id = sqlite3_column_int(stmt, 0);
		const char* user = (const char*) sqlite3_column_text(stmt, 1);
		s->user = string(user, strlen(user));
		s->is_active = sqlite3_column_int(stmt, 2);
		const char* src_addr = (const char*) sqlite3_column_text(stmt, 3);
		const char* src_port = (const char*) sqlite3_column_text(stmt, 4);
		const char* dst_addr = (const char*) sqlite3_column_text(stmt, 5);
		const char* dst_port = (const char*) sqlite3_column_text(stmt, 6);
		s->src_addr = string(src_addr, strlen(src_addr));
		s->src_port = string(src_port, strlen(src_port));
		s->dst_addr = string(dst_addr, strlen(dst_addr));
		s->dst_port = string(dst_port, strlen(dst_port));
		s->bytes_sent = sqlite3_column_int(stmt, 7);
		s->bytes_recv = sqlite3_column_int(stmt, 8);
	}

	int insertData()
	{
		sqlite3* DB;
		char* messageError;

		string sql("INSERT INTO GRADES (NAME, LNAME, AGE, ADDRESS, GRADE) VALUES('Alice', 'Chapa', 35, 'Tampa', 'A');"
			"INSERT INTO GRADES (NAME, LNAME, AGE, ADDRESS, GRADE) VALUES('Bob', 'Lee', 20, 'Dallas', 'B');"
			"INSERT INTO GRADES (NAME, LNAME, AGE, ADDRESS, GRADE) VALUES('Fred', 'Cooper', 24, 'New York', 'C');");

		int exit = sqlite3_open(db_path, &DB);
		/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK) {
			cerr << "Error in insertData function." << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "Records inserted Successfully!" << endl;

		return 0;
	}

	int updateData()
	{
		sqlite3* DB;
		char* messageError;

		string sql("UPDATE GRADES SET GRADE = 'A' WHERE LNAME = 'Cooper'");

		int exit = sqlite3_open(db_path, &DB);
		/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK) {
			cerr << "Error in updateData function." << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "Records updated Successfully!" << endl;

		return 0;
	}

	int deleteData()
	{
		sqlite3* DB;
		char* messageError;

		string sql = "DELETE FROM GRADES;";

		int exit = sqlite3_open(db_path, &DB);
		/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
		exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
		if (exit != SQLITE_OK) {
			cerr << "Error in deleteData function." << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "Records deleted Successfully!" << endl;

		return 0;
	}

	int selectData()
	{
		sqlite3* DB;
		char* messageError;

		string sql = "SELECT * FROM GRADES;";

		int exit = sqlite3_open(db_path, &DB);
		/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
		exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);

		if (exit != SQLITE_OK) {
			cerr << "Error in selectData function." << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "Records selected Successfully!" << endl;

		return 0;
	}

	// retrieve contents of database used by selectData()
	/* argc: holds the number of results, argv: holds each value in array, azColName: holds each column returned in array, */
	static int callback(void* NotUsed, int argc, char** argv, char** azColName)
	{
		cout << "callback\n";
		for (int i = 0; i < argc; i++) {
			// column name and value
			cout << azColName[i] << ": " << argv[i] << endl;
		}

		cout << endl;

		return 0;
	}
}}