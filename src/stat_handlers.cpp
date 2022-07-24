#include "stat_handlers.h"
#include "string_utils.h"
#include <sqlite3.h>

namespace proxy { namespace stat {
	using string_utils::concat;

	DatabaseConnection::DatabaseConnection(const std::string& path)
		: _con(nullptr)
	{
		int err = sqlite3_open(path.c_str(), &_con);
		if (err != SQLITE_OK) {
			throw DatabaseException("Error during opening connection: " + std::string(sqlite3_errstr(err)));
		}
	}

	DatabaseConnection::~DatabaseConnection()
	{
		sqlite3_close(_con);
	}

	DatabaseStatement::~DatabaseStatement()
	{
		sqlite3_finalize(_stmt);
	}

}} // namespace proxy stat