#include <exception>
#include <string>
#include "..\sqlite\sqlite3.h"
#include "sqlite_log_writer.h"
#include "..\sdk\pfc\pfc.h"

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
		"playback_timestamp TEXT DEFAULT CURRENT_TIMESTAMP)", NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
		std::exception e(errorMessage);
		sqlite3_free(errorMessage);
		throw e;
	}

	ret_value = sqlite3_exec(db,
		"CREATE VIEW IF NOT EXISTS 'chart_artists_global'"
		" AS SELECT artist, count(*) AS play_count FROM playback_log"
		" GROUP BY artist ORDER BY play_count DESC", NULL, NULL, &errorMessage);
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
		"CREATE VIEW IF NOT EXISTS 'playback_history'"
		" AS SELECT artist, title, datetime(playback_timestamp, 'localtime')"
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

void sqlite_log_writer::add_record(const char* artist, const char* title)
{
	if (db == NULL)
	{
		return;
	}

	pfc::string8 sql_command = "INSERT INTO playback_log (artist, title) VALUES('";
	pfc::string8 artist8 = artist;
	artist8.replace_char('\'', '\'\'');
	sql_command += artist8;
	sql_command += "', '";
	pfc::string8 title8 = title;
	title8.replace_char('\'', '\'\'');
	sql_command += title8;
	sql_command += "')";
	char* errorMessage;
	int ret_value = sqlite3_exec(db, sql_command.get_ptr(), NULL, NULL, &errorMessage);
	if (ret_value != SQLITE_OK)
	{
//		std::exception e(errorMessage);
		sqlite3_free(errorMessage);
//		throw e;
	}
}
