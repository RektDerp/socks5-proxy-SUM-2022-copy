#include "stat_handlers.h"
#include "string_utils.h"
#include <sqlite3.h>

namespace proxy { namespace stat {
	using string_utils::concat;

	db_connection::db_connection(const std::string& path)
		: _con(nullptr)
	{
		int err = sqlite3_open(path.c_str(), &_con);
		if (err != SQLITE_OK) {
			throw db_exception("Error during opening connection: " + std::string(sqlite3_errstr(err)));
		}
	}

	db_connection::~db_connection()
	{
		sqlite3_close(_con);
	}

	db_stmt::~db_stmt()
	{
		sqlite3_finalize(_stmt);
	}

}} // namespace proxy stat