

#ifndef GstPlayerBin_h__
#define GstPlayerBin_h__

#include "IGStreamerPlayer.h"
#include <string>
#include <map>

namespace mray
{
namespace video
{
	
class GstPlayerBin
{
protected:
	typedef std::map<std::string, IGStreamerPlayer*> PlayerMap;
	PlayerMap m_players;
public:
	GstPlayerBin();
	virtual ~GstPlayerBin();

	void AddPlayer(IGStreamerPlayer* player, const std::string& name);
	IGStreamerPlayer* GetPlayer(const std::string& name);

	void Play();
	void Stop();
	void CloseAll();

	void StartPlayer(const std::string& name);
	void StopPlayer(const std::string& name);
	IGStreamerPlayer* RemovePlayer(const std::string& name, bool close);
	void ClearPlayers(bool stop);
};

}
}

#endif // GstPlayerBin_h__
