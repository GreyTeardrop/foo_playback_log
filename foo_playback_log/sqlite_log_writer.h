#ifndef __SQLITE_LOG_WRITER_H__
#define __SQLITE_LOG_WRITER_H__

struct sqlite3;

class sqlite_log_writer
{
private:
	sqlite3* db;
public:
	sqlite_log_writer();
	~sqlite_log_writer();

	void init(const char* db_path);
	void term();
	void add_record(const char* artist, const char* title, const char* album,
		const char* date);
};

#endif
