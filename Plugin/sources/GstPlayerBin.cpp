

#include "stdafx.h"
#include "GstPlayerBin.h"


namespace mray
{
namespace video
{


GstPlayerBin::GstPlayerBin()
{
}
GstPlayerBin::~GstPlayerBin()
{
	ClearPlayers(true);
}

void GstPlayerBin::AddPlayer(IGStreamerPlayer* player, const std::string& name)
{
	m_players[name] = player;
}
IGStreamerPlayer* GstPlayerBin::GetPlayer(const std::string& name)
{
	PlayerMap::iterator it = m_players.find(name);
	if (it == m_players.end())
		return 0;
	return it->second;
}
void GstPlayerBin::StartPlayer(const std::string& name)
{
	PlayerMap::iterator it = m_players.find(name);
	if (it == m_players.end())
		return ;
	it->second->Play();
}
void GstPlayerBin::StopPlayer(const std::string& name)
{
	PlayerMap::iterator it = m_players.find(name);
	if (it == m_players.end())
		return;
	it->second->Stop();
}
void GstPlayerBin::Play()
{
	PlayerMap::iterator it = m_players.begin();
	for (; it != m_players.end(); ++it)
	{
		it->second->Play();
	}

}
void GstPlayerBin::Stop()
{
	PlayerMap::iterator it = m_players.begin();
	for (; it != m_players.end(); ++it)
	{
		it->second->Stop();
	}

}
void GstPlayerBin::CloseAll()
{

	PlayerMap::iterator it = m_players.begin();
	for (; it != m_players.end(); ++it)
	{
		it->second->Close();
	}
}
IGStreamerPlayer* GstPlayerBin::RemovePlayer(const std::string& name,bool close)
{
	PlayerMap::iterator it = m_players.find(name);
	if (it == m_players.end())
		return 0;
	IGStreamerPlayer* ret = it->second;
	if (close)
		ret->Close();
	m_players.erase(it);
	return ret;
}
void GstPlayerBin::ClearPlayers(bool stop)
{
	if (stop)
	{
		PlayerMap::iterator it = m_players.begin();
		for (; it != m_players.end(); ++it)
		{
			it->second->Close();
		}
	}
	m_players.clear();
}

}
}


