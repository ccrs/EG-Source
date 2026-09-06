#include "ScriptMgr.h"
#include "AnticheatMgr.h"
#include "World.h"


class EG_Anticheat : public PlayerScript
{
    public:
        EG_Anticheat() : PlayerScript("EG_Anticheat") { }

        void OnLogin(Player* player, bool /*firstLogin*/) override
        {
            sAnticheatMgr->HandlePlayerLogin(player);
        }

        void OnLogout(Player* player) override
        {
            sAnticheatMgr->HandlePlayerLogout(player);
        }
};

void AddSC_EG_player_scripts()
{
    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        new EG_Anticheat();
}
