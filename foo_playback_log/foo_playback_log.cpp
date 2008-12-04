#include "..\sdk\foobar2000\sdk\foobar2000.h"
#include "..\sdk\foobar2000\sdk\console.h"
#include "..\sdk\foobar2000\helpers\helpers.h"
#include "sqlite_log_writer.h"

#define coalesce(a) (a!=NULL)?a:""

sqlite_log_writer log_writer;

class initquit_playback_log : public initquit
{
public:
	void on_init()
	{
		std::string db_path = core_api::get_profile_path();
		if (db_path.length() > 7 &&
			db_path[0] == 'f' &&
			db_path[1] == 'i' &&
			db_path[2] == 'l' &&
			db_path[3] == 'e' &&
			db_path[4] == ':' &&
			db_path[5] == '/' &&
			db_path[6] == '/')
		{
			db_path = db_path.substr(7, db_path.length() - 1);
		}
		db_path += "\\playback_log.sqlite";
		try
		{
			log_writer.init(db_path.c_str());
		}
		catch (std::exception& e)
		{
			console::error("Playback log: unable to open database");
			if (e.what() != NULL)
			{
				console::error(e.what());
			}
		}
		console::info("Playback log: Loaded");
	}

	void on_quit()
	{
		log_writer.term();
	}
};


class playback_log_collector : public playback_statistics_collector
{
public:
	virtual void on_item_played(metadb_handle_ptr p_item)
	{
		file_info_impl track_info;
		if (p_item->get_info(track_info))
		{
			const char* track_artist = track_info.meta_get("artist", 0);
			const char* track_title = track_info.meta_get("title", 0);
			const char* track_album = track_info.meta_get("album", 0);
			if (track_album == NULL)
				track_album = "";
			const char* track_date = track_info.meta_get("date", 0);
			if (track_date == NULL)
				track_date = "";
			if ((track_artist != NULL) && (track_title != NULL))
			{
				log_writer.add_record(track_artist, track_title, track_album, track_date);
			}
		}
	}
};

static initquit_factory_t<initquit_playback_log>
	initquit_playback_log_instance;
static playback_statistics_collector_factory_t<playback_log_collector>
	playback_log_collector_instance;
DECLARE_COMPONENT_VERSION("Playback log", "0.2.1", "Keeps history of played tracks in sqlite database.")
