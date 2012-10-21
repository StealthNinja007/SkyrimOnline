#include <stdafx.h>
#include <Logic/States/ShardList.hpp>
#include <SkyrimOnline.h>
#include <Overlay/Message.h>

#include <Game/MassiveMessageManager.hpp>

namespace Skyrim
{
	namespace Logic
	{
		namespace State
		{
			//--------------------------------------------------------------------------------
			ShardList::ShardList()
			{
				mShardList = boost::make_shared<Overlay::ShardList>(SkyrimOnline::GetInstance().GetInterface().GetGui());
				mShardList->Hide();
			}
			//--------------------------------------------------------------------------------
			ShardList::~ShardList()
			{

			}
			//--------------------------------------------------------------------------------
			void ShardList::OnEnter()
			{
				SkyrimOnline::GetInstance().SetMode(false);
				mShardList->Hide();

				SkyrimOnline::GetInstance().GetInterface().GetMessage()->SetCaption("Fetching shardlist.");
				SkyrimOnline::GetInstance().GetInterface().GetMessage()->SetVisible(true);

			}
			//--------------------------------------------------------------------------------
			void ShardList::OnLeave()
			{
				mShardList->Hide();
			}
			//--------------------------------------------------------------------------------
			void ShardList::OnUpdate(uint32_t pDelta)
			{
				::Game::DisablePlayerControls(true,true,true,true,true,true,true,true,1);
				::Game::SetInChargen(true, true, false);

				mShardList->Update(0.0);
				SkyrimOnline::GetInstance().GetInterface().SetCursor(true);
			}
			//--------------------------------------------------------------------------------
			void ShardList::OnShardlistReply(std::string pReply)
			{
				std::istringstream buffer(pReply);
				std::string line;

				while (std::getline(buffer, line))
				{
					auto pos = line.find('|');
					if(pos == std::string::npos)
						continue;

					std::string name = line.substr(0, pos);
					std::string ip = line.substr(pos + 1);

					mShardList->Log(name, ip);

					std::ostringstream os;
					os << "Received shard : " << name << " on " << ip;

					System::Log::Debug(os.str());
				}

				SkyrimOnline::GetInstance().GetInterface().GetMessage()->Hide();
				mShardList->Show();
				mShardList->OnShardPick.connect(boost::bind(&ShardList::OnShardPick, this, _1));
				mShardList->OnHost.connect(boost::bind(&ShardList::OnHost, this));
			}
			//--------------------------------------------------------------------------------
			void ShardList::OnShardPick(const std::string& pShard)
			{
				System::Log::Debug(std::string("Shard picked : ") + pShard);
				SkyrimOnline::GetInstance().OnShardPick(pShard);
			}
			//--------------------------------------------------------------------------------
			void ShardList::OnHost()
			{
				System::Log::Debug("Hosting a server...");
				SkyrimOnline::GetInstance().GetInterface().GetMessage()->Show();
				SkyrimOnline::GetInstance().GetInterface().GetMessage()->SetCaption("Starting the server...");

				TheMassiveMessageMgr->BeginMultiplayer(true);
			}
			//--------------------------------------------------------------------------------
			bool ShardList::IsSwitchingAllowed()
			{
				return false;
			}
			//--------------------------------------------------------------------------------
		}
	}
}