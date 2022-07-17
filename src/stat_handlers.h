#ifndef _STAT_HANDLERS_H_
#define _STAT_HANDLERS_H_

#include "proxy_exceptions.h"

struct sqlite3;
struct sqlite3_stmt;

namespace proxy { namespace stat {

	class db_connection {
	private:
		sqlite3* _con;
	public:
		db_connection(const std::string& path);

		~db_connection();

		operator sqlite3* ()
		{
			return _con;
		}

		operator sqlite3**()
		{
			return &_con;
		}
	};

	class db_stmt {
	private:
		sqlite3_stmt* _stmt;
	public:
		db_stmt() : _stmt(nullptr)
		{}

		~db_stmt();

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
