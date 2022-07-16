#ifndef _STAT_HANDLERS_H_
#define _STAT_HANDLERS_H_
#include "proxy_common.h"
#include "proxy_exceptions.h"
#include "string_utils.h"
#include <sqlite3.h>

namespace proxy { namespace stat {
	using string_utils::concat;

	class db_connection {
	private:
		sqlite3* _con;
	public:
		db_connection(const std::string& path) throw (db_exception)
			: _con(nullptr)
		{
			int err = sqlite3_open(path.c_str(), &_con);
			if (err != SQLITE_OK) {
				throw db_exception(
					concat("Error during opening connection: %1%", err)
				);
			}
		}

		~db_connection()
		{
			sqlite3_close(_con);
		}

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

		~db_stmt()
		{
			sqlite3_finalize(_stmt);
		}

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
