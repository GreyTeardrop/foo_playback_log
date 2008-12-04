#include <exception>
#include <string>
#include "..\sqlite\sqlite3.h"
#include "sqlite_log_writer.h"
#include "..\sdk\pfc\pfc.h"
#include <boost/algorithm/string/replace.hpp>

sqlite_log_writer::sqlite_log_writer() : db(NULL)
{
}

sqlite_log_writer::~sqlite_log_writer()
{
	if (db != NULL)
	{
		term();
	}
}

void sqlite_log_writer::init(const char* db_path)
{
	int ret_value = sqlite3_open(db_path, &db);
	if (ret_value != SQLITE_OK)
	{
		db = NULL;
		std::exception e;
		throw e;
	}

	char* errorMessage;
	ret_value = sqlite3_exec(db,
		"CREATE TABLE IF NOT EXISTS playback_log ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"artist TEXT,"
		"title TEXT,"
		"album TEXT,"
		"date TEXT,"
		"playback_timestamp TEXT DEFAULT CURRENT_TIMESTAMP)", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		std::exception e(errorMessage);
		sqlite3_free(errorMessage);
		throw e;
	}

	// Global charts
	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_artists_global'"
		" AS SELECT artist, count(*) AS play_count FROM playback_log"
		" GROUP BY artist ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albums_global'"
		" AS SELECT artist, album, count(*) AS play_count FROM playback_log"
		" GROUP BY artist, album ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_tracks_global'"
		" AS SELECT artist, title, count(*) AS play_count FROM playback_log"
		" GROUP BY artist, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albumtracks_global'"
		" AS SELECT artist, album, title, count(*) AS play_count FROM playback_log"
		" GROUP BY artist, album, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	// Last day charts
	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_artists_day'"
		" AS SELECT artist, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 day'))"
		" GROUP BY artist ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albums_day'"
		" AS SELECT artist, album, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 day'))"
		" GROUP BY artist, album ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_tracks_day'"
		" AS SELECT artist, title, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 day'))"
		" GROUP BY artist, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albumtracks_day'"
		" AS SELECT artist, album, title, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 day'))"
		" GROUP BY artist, album, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	// Last week charts
	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_artists_week'"
		" AS SELECT artist, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-7 day'))"
		" GROUP BY artist ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albums_week'"
		" AS SELECT artist, album, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-7 day'))"
		" GROUP BY artist, album ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_tracks_week'"
		" AS SELECT artist, title, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-7 day'))"
		" GROUP BY artist, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albumtracks_week'"
		" AS SELECT artist, album, title, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-7 day'))"
		" GROUP BY artist, album, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	// Last month charts
	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_artists_month'"
		" AS SELECT artist, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 month'))"
		" GROUP BY artist ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albums_month'"
		" AS SELECT artist, album, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 month'))"
		" GROUP BY artist, album ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_tracks_month'"
		" AS SELECT artist, title, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 month'))"
		" GROUP BY artist, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_albumtracks_month'"
		" AS SELECT artist, album, title, count(*) AS play_count FROM playback_log"
		" WHERE (playback_timestamp >= datetime('now','-1 month'))"
		" GROUP BY artist, album, title ORDER BY play_count DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}

	// Playback history
	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'playback_history'"
		" AS SELECT artist, date, album, title, datetime(playback_timestamp, 'localtime')"
		" AS play_time FROM playback_log ORDER BY play_time DESC", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}
}

void sqlite_log_writer::term()
{
	if (db != NULL)
	{
		sqlite3_close(db);
		db = NULL;
	}
}

void sqlite_log_writer::add_record(const char* artist, const char* title,
	const char* album, const char* date)
{
	if (db == NULL)
	{
		return;
	}

	pfc::string8 sql_command = "INSERT INTO playback_log (artist, title, album, date) VALUES('";
	std::string artist_str = artist;
	boost::algorithm::replace_all(artist_str, "'", "''");
	sql_command += artist_str.c_str();
	sql_command += "', '";
	std::string title_str = title;
	boost::algorithm::replace_all(title_str, "'", "''");
	sql_command += title_str.c_str();
	sql_command += "', '";
	std::string album_str = album;
	boost::algorithm::replace_all(album_str, "'", "''");
	sql_command += album_str.c_str();
	sql_command += "', '";
	std::string date_str = date;
	boost::algorithm::replace_all(date_str, "'", "''");
	sql_command += date_str.c_str();
	sql_command += "')";

	char* errorMessage;
	int ret_value = sqlite3_exec(db, sql_command.get_ptr(), NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		sqlite3_free(errorMessage);
	}
}
