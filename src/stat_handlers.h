#ifndef _STAT_HANDLERS_H_
#define _STAT_HANDLERS_H_

#include "proxy_exceptions.h"

struct sqlite3;
struct sqlite3_stmt;

namespace proxy { namespace stat {

	class DatabaseConnection {
	private:
		sqlite3* _con;
	public:
		DatabaseConnection(const std::string& path);

		~DatabaseConnection();

		operator sqlite3* ()
		{
			return _con;
		}

		operator sqlite3**()
		{
			return &_con;
		}
	};

	class DatabaseStatement {
	private:
		sqlite3_stmt* _stmt;
	public:
		DatabaseStatement() : _stmt(nullptr)
		{}

		~DatabaseStatement();

		operator sqlite3_stmt* ()
		{
			return _stmt;
		}

		operator sqlite3_stmt** ()
		{
			return &_stmt;
		}
	};

}} // proxy stat

#endif // _STAT_HANDLERS_H_
