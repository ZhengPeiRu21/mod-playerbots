/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "AiFactory.h"
#include "BattlegroundMgr.h"
#include "Playerbots.h"
#include "Engine.h"
#include "Group.h"
#include "DKAiObjectContext.h"
#include "PriestAiObjectContext.h"
#include "MageAiObjectContext.h"
#include "WarlockAiObjectContext.h"
#include "WarriorAiObjectContext.h"
#include "ShamanAiObjectContext.h"
#include "PaladinAiObjectContext.h"
#include "DruidAiObjectContext.h"
#include "HunterAiObjectContext.h"
#include "RogueAiObjectContext.h"

AiObjectContext* AiFactory::createAiObjectContext(Player* player, PlayerbotAI* botAI)
{
    switch (player->getClass())
    {
        case CLASS_PRIEST:
            return new PriestAiObjectContext(botAI);
        case CLASS_MAGE:
            return new MageAiObjectContext(botAI);
        case CLASS_WARLOCK:
            return new WarlockAiObjectContext(botAI);
        case CLASS_WARRIOR:
            return new WarriorAiObjectContext(botAI);
        case CLASS_SHAMAN:
            return new ShamanAiObjectContext(botAI);
        case CLASS_PALADIN:
            return new PaladinAiObjectContext(botAI);
        case CLASS_DRUID:
            return new DruidAiObjectContext(botAI);
        case CLASS_HUNTER:
            return new HunterAiObjectContext(botAI);
        case CLASS_ROGUE:
            return new RogueAiObjectContext(botAI);
        case CLASS_DEATH_KNIGHT:
            return new DKAiObjectContext(botAI);
    }

    return new AiObjectContext(botAI);
}

uint8 AiFactory::GetPlayerSpecTab(Player* bot)
{
    std::map<uint8, uint32> tabs = GetPlayerSpecTabs(bot);

    if (bot->getLevel() >= 10 && ((tabs[0] + tabs[1] + tabs[2]) > 0))
    {
        std::map<uint8, uint32> tabs = GetPlayerSpecTabs(bot);

        int8 tab = -1;
        uint32 max = 0;
        for (uint32 i = 0; i < uint32(3); i++)
        {
            if (tab == -1 || max < tabs[i])
            {
                tab = i;
                max = tabs[i];
            }
        }

        return tab;
    }
    else
    {
        uint8 tab = 0;

        switch (bot->getClass())
        {
            case CLASS_MAGE:
                tab = 1;
                break;
            case CLASS_PALADIN:
                tab = 0;
                break;
            case CLASS_PRIEST:
                tab = 1;
                break;
        }

        return tab;
    }
}

std::map<uint8, uint32> AiFactory::GetPlayerSpecTabs(Player* bot)
{
    std::map<uint8, uint32> tabs;
    for (uint32 i = 0; i < 3; i++)
        tabs[i] = 0;

    uint32 classMask = bot->getClassMask();
    for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if ((classMask & talentTabInfo->ClassMask) == 0)
            continue;

        uint32 maxRank = 0;
        for (int32 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            if (!talentInfo->RankID[rank])
                continue;

            uint32 spellid = talentInfo->RankID[rank];
            if (spellid && bot->HasSpell(spellid))
                maxRank = rank + 1;

        }

        tabs[talentTabInfo->tabpage] += maxRank;
    }

    return tabs;
}

BotRoles AiFactory::GetPlayerRoles(Player* player)
{
    BotRoles role = BOT_ROLE_NONE;
    uint8 tab = GetPlayerSpecTab(player);

    switch (player->getClass())
    {
        case CLASS_PRIEST:
            if (tab == 2)
                role = BOT_ROLE_DPS;
            else
                role = BOT_ROLE_HEALER;
            break;
        case CLASS_SHAMAN:
            if (tab == 2)
                role = BOT_ROLE_HEALER;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                role = BOT_ROLE_TANK;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_PALADIN:
            if (tab == 0)
                role = BOT_ROLE_HEALER;
            else if (tab == 1)
                role = BOT_ROLE_TANK;
            else if (tab == 2)
                role = BOT_ROLE_DPS;
            break;
        case CLASS_DRUID:
            if (tab == 0)
                role = BOT_ROLE_DPS;
            else if (tab == 1)
                role = (BotRoles)(BOT_ROLE_TANK | BOT_ROLE_DPS);
            else if (tab == 2)
                role = BOT_ROLE_HEALER;
            break;
        default:
            role = BOT_ROLE_DPS;
            break;
    }

    return role;
}

std::string AiFactory::GetPlayerSpecName(Player* player)
{
    std::string specName;
    int tab = GetPlayerSpecTab(player);
    switch (player->getClass())
    {
        case CLASS_PRIEST:
            if (tab == 2)
                specName = "shadow";
            else if (tab == 1)
                specName = "holy";
            else
                specName = "disc";
            ;        break;
        case CLASS_SHAMAN:
            if (tab == 2)
                specName = "resto";
            else if (tab == 1)
                specName = "enhance";
            else
                specName = "elem";
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                specName = "prot";
            else if (tab == 1)
                specName = "fury";
            else
                specName = "arms";
            break;
        case CLASS_PALADIN:
            if (tab == 0)
                specName = "holy";
            else if (tab == 1)
                specName = "prot";
            else if (tab == 2)
                specName = "retrib";
            break;
        case CLASS_DRUID:
            if (tab == 0)
                specName = "balance";
            else if (tab == 1)
                specName = "feraldps";
            else if (tab == 2)
                specName = "resto";
            break;
        case CLASS_ROGUE:
            if (tab == 0)
                specName = "assas";
            else if (tab == 1)
                specName = "combat";
            else if (tab == 2)
                specName = "subtle";
            break;
        case CLASS_HUNTER:
            if (tab == 0)
                specName = "beast";
            else if (tab == 1)
                specName = "marks";
            else if (tab == 2)
                specName = "surv";
            break;
        case CLASS_DEATH_KNIGHT:
            if (tab == 0)
                specName = "blooddps";
            else if (tab == 1)
                specName = "frostdps";
            else if (tab == 2)
                specName = "unholydps";
            break;
        case CLASS_MAGE:
            if (tab == 0)
                specName = "arcane";
            else if (tab == 1)
                specName = "fire";
            else if (tab == 2)
                specName = "frost";
            break;
        case CLASS_WARLOCK:
            if (tab == 0)
                specName = "afflic";
            else if (tab == 1)
                specName = "demo";
            else if (tab == 2)
                specName = "destro";
            break;
        default:
            break;
    }

    return std::move(specName);
}

void AiFactory::AddDefaultCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* engine)
{
    uint8 tab = GetPlayerSpecTab(player);

    if (!player->InBattleground())
    {
        engine->addStrategies("racials", "chat", "default", "potions", "cast time", "duel", nullptr);
    }

    switch (player->getClass())
    {
        case CLASS_PRIEST:
            if (tab == 2)
            {
                engine->addStrategies("dps", "shadow debuff", "shadow aoe", "threat", nullptr);
            }
            else if (tab == 0)
            {
                engine->addStrategies("holy", "shadow debuff", "shadow aoe", "threat", nullptr);
                //if (player->getLevel() >= 4)
                   //engine->addStrategy("dps debuff");
            }
            else
                engine->addStrategies("heal", "threat", nullptr);

            engine->addStrategies("dps assist", "flee", "cure", "ranged", "cc", nullptr);
            break;
        case CLASS_MAGE:
            if (tab == 0)
                engine->addStrategies("arcane", "threat", nullptr);
            else if (tab == 1)
                engine->addStrategies("fire", "fire aoe", "threat", nullptr);
            else
                engine->addStrategies("frost", "frost aoe", "threat", "dps aoe", nullptr);

            engine->addStrategies("dps", "dps assist", "flee", "cure", "ranged", "cc", nullptr);
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                engine->addStrategies("tank", "tank assist", "aoe", "close", "mark rti", nullptr);
            else if (player->getLevel() < 30 || tab == 0)
                engine->addStrategies("arms", "aoe", "dps assist", "threat", "close", "behind", nullptr);
            else
                engine->addStrategies("fury", "aoe", "dps assist", "threat", "close", "behind", nullptr);
            break;
        case CLASS_SHAMAN:
            if (tab == 0)
                engine->addStrategies("caster", "caster aoe", "bmana", "threat", "flee", "ranged", nullptr);
            else if (tab == 2)
                engine->addStrategies("heal", "bmana", "flee", "ranged", nullptr);
            else
                engine->addStrategies("dps", "melee aoe", "bdps", "threat", "close", nullptr);

            engine->addStrategies("dps assist", "cure", "totems", nullptr);
            break;
        case CLASS_PALADIN:
            if (tab == 1)
                engine->addStrategies("tank", "tank assist", "bthreat", "cure", "barmor", "bstats", "close", "cc", nullptr);
            else if (tab == 0)
                engine->addStrategies("heal", "bmana", "dps assist", "cure", "flee", "barmor", nullptr);
            else
                engine->addStrategies("dps", "bdps", "dps assist", "cure", "baoe", "close", "cc", nullptr);

            if (player->getLevel() < 14)
            {
                engine->addStrategy("bdps");
            }

            if (player->getLevel() < 16)
            {
                engine->addStrategy("barmor");
            }
            break;
        case CLASS_DRUID:
            if (tab == 0)
            {
                engine->addStrategies("caster", "cure", "caster aoe", "threat", "flee", "dps assist", "ranged", "cc", nullptr);
                if (player->getLevel() > 19)
                    engine->addStrategy("caster debuff");
            }
            else if (tab == 2)
                engine->addStrategies("heal", "cure", "flee", "dps assist", "ranged", "cc", nullptr);
            else
            {
                engine->removeStrategy("ranged");
                engine->addStrategies("bear", "tank assist", "flee", "close", nullptr);
            }
            break;
        case CLASS_HUNTER:
            engine->addStrategies("dps", "bdps", "threat", "dps assist", "ranged", "pet", "cc", nullptr);
            if (player->getLevel() > 19)
                engine->addStrategy("dps debuff");
            break;
        case CLASS_ROGUE:
            engine->addStrategies("dps", "threat", "dps assist", "aoe", "close", "cc", "behind", "stealth", nullptr);
            break;
        case CLASS_WARLOCK:
            if (player->getLevel() > 19)
                engine->addStrategy("dps debuff");

            engine->addStrategies("dps assist", "dps", "flee", "ranged", "cc", "pet", "threat", nullptr);
            break;
        case CLASS_DEATH_KNIGHT:
            if (tab == 0)
                engine->addStrategies("blood", nullptr);
            else if (tab == 1)
                engine->addStrategies("frost", "frost aoe", "dps assist", "threat", nullptr);
            else
                engine->addStrategies("unholy", "unholy aoe", "dps assist", "threat", nullptr);

            engine->addStrategies("dps assist", "flee", "close", "cc", nullptr);

            break;
    }

    if (facade->IsRealPlayer() || sRandomPlayerbotMgr->IsRandomBot(player))
    {
        if (!player->GetGroup())
        {
            engine->ChangeStrategy(sPlayerbotAIConfig->randomBotCombatStrategies);

            engine->addStrategy("flee");
            engine->addStrategy("boost");

            if (player->getClass() == CLASS_WARLOCK)
            {
                engine->removeStrategy("ranged");
            }

            if (player->getClass() == CLASS_DRUID && tab == 2)
            {
                engine->addStrategies("caster", "caster aoe", nullptr);
            }

            if (player->getClass() == CLASS_DRUID && tab == 1 && urand(0, 100) > 50 && player->getLevel() > 19)
            {
                engine->addStrategy("dps");
            }

            if (player->getClass() == CLASS_PRIEST && tab == 1)
            {
                engine->removeStrategy("heal");
                engine->addStrategies("holy", "shadow debuff", "shadow aoe", "threat", nullptr);
            }

            if (player->getClass() == CLASS_SHAMAN && tab == 2)
            {
                engine->addStrategies("caster", "caster aoe", nullptr);
            }

            if (player->getClass() == CLASS_PALADIN && tab == 0)
            {
                engine->addStrategies("dps", "close", nullptr);
            }
        }
    }
    else
    {
        engine->ChangeStrategy(sPlayerbotAIConfig->combatStrategies);
    }

    // Battleground switch
    if (player->InBattleground() && player->GetBattleground())
    {
        BattlegroundTypeId bgType = player->GetBattlegroundTypeId();
        if (bgType == BATTLEGROUND_RB)
            bgType = player->GetBattleground()->GetBgTypeID(true);

        if (bgType == BATTLEGROUND_WS)
            engine->addStrategy("warsong");

        if (bgType == BATTLEGROUND_AB)
            engine->addStrategy("arathi");

        if (bgType == BATTLEGROUND_AV)
            engine->addStrategy("alterac");

        if (bgType == BATTLEGROUND_EY)
            engine->addStrategy("eye");

        if (bgType == BATTLEGROUND_IC)
            engine->addStrategy("isle");

        if (player->InArena())
        {
            engine->addStrategy("arena");
        }

        engine->addStrategies("boost", "racials", "chat", "default", "aoe", "potions", "conserve mana", "cast time", "dps assist", nullptr);
        engine->removeStrategy("custom::say");
        engine->removeStrategy("flee");
        engine->removeStrategy("threat");
        engine->addStrategy("boost");

        if ((player->getClass() == CLASS_DRUID && tab == 2) || (player->getClass() == CLASS_SHAMAN && tab == 2))
            engine->addStrategies("caster", "caster aoe", nullptr);

        if (player->getClass() == CLASS_DRUID && tab == 1)
            engine->addStrategies("behind", "dps", nullptr);

        if (player->getClass() == CLASS_ROGUE)
            engine->addStrategies("behind", "stealth", nullptr);
    }
}

Engine* AiFactory::createCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* aiObjectContext)
{
	Engine* engine = new Engine(facade, aiObjectContext);
    AddDefaultCombatStrategies(player, facade, engine);
    return engine;
}

void AiFactory::AddDefaultNonCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* nonCombatEngine)
{
    uint8 tab = GetPlayerSpecTab(player);

    switch (player->getClass())
    {
        case CLASS_PRIEST:
            nonCombatEngine->addStrategies("dps assist", "cure", nullptr);
            break;
        case CLASS_PALADIN:
            if (tab == 1)
                nonCombatEngine->addStrategies("bthreat", "tank assist", "barmor", "bstats", nullptr);
            else if (tab == 0)
                nonCombatEngine->addStrategies("dps assist", "barmor", "bmana", nullptr);
            else
                nonCombatEngine->addStrategies("dps assist", "baoe", "bdps", nullptr);

            nonCombatEngine->addStrategies("cure", nullptr);

            if (player->getLevel() < 14)
                nonCombatEngine->addStrategies("bdps", nullptr);

            if (player->getLevel() < 16)
                nonCombatEngine->addStrategies("barmor", nullptr);
            break;
        case CLASS_HUNTER:
            nonCombatEngine->addStrategies("bdps", "dps assist", nullptr);
            break;
        case CLASS_SHAMAN:
            if (tab == 0 || tab == 2)
                nonCombatEngine->addStrategy("bmana");
            else
                nonCombatEngine->addStrategy("bdps");

            nonCombatEngine->addStrategies("dps assist", "cure", nullptr);
            break;
        case CLASS_MAGE:
            if (tab == 1)
                nonCombatEngine->addStrategy("bdps");
            else
                nonCombatEngine->addStrategy("bmana");

            nonCombatEngine->addStrategies("dps assist", "cure", nullptr);
            break;
        case CLASS_DRUID:
            if (tab == 1)
                nonCombatEngine->addStrategy("tank assist");
            else
                nonCombatEngine->addStrategies("dps assist", "cure", nullptr);
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                nonCombatEngine->addStrategy("tank assist");
            else
                nonCombatEngine->addStrategy("dps assist");
            break;
        case CLASS_WARLOCK:
            nonCombatEngine->addStrategies("pet", "dps assist", nullptr);
            break;
        case CLASS_DEATH_KNIGHT:
            if (tab == 0)
                nonCombatEngine->addStrategy("tank assist");
            else
                nonCombatEngine->addStrategy("dps assist");
            break;
        default:
            nonCombatEngine->addStrategy("dps assist");
            break;
    }

    if (!player->InBattleground())
    {
        nonCombatEngine->addStrategies("nc", "food", "chat", "follow",
            "default", "quest", "loot", "gather", "duel", "emote", "buff", "mount", nullptr);
    }

    if ((facade->IsRealPlayer() || sRandomPlayerbotMgr->IsRandomBot(player)) && !player->InBattleground())
    {
        Player* master = facade->GetMaster();

         // let 25% of free bots start duels.
        if (!urand(0, 3))
            nonCombatEngine->addStrategy("start duel");

        if (!player->GetGroup() || player->GetGroup()->GetLeaderGUID() == player->GetGUID())
        {
            // let 25% of random not grouped (or grp leader) bots help other players
            if (!urand(0, 3))
                nonCombatEngine->addStrategy("attack tagged");

            nonCombatEngine->addStrategy("pvp");
            nonCombatEngine->addStrategy("collision");
            nonCombatEngine->addStrategy("grind");
            nonCombatEngine->addStrategy("group");
            nonCombatEngine->addStrategy("guild");

            if (sPlayerbotAIConfig->autoDoQuests)
            {
                nonCombatEngine->addStrategy("travel");
                nonCombatEngine->addStrategy("rpg");
            }

            if (sPlayerbotAIConfig->randomBotJoinLfg)
                nonCombatEngine->addStrategy("lfg");

            if (sPlayerbotAIConfig->randomBotJoinBG)
                nonCombatEngine->addStrategy("bg");

            if (!master || GET_PLAYERBOT_AI(master))
                nonCombatEngine->addStrategy("maintenance");

            nonCombatEngine->ChangeStrategy(sPlayerbotAIConfig->randomBotNonCombatStrategies);
        }
        else
        {
            if (facade)
            {
                if (master)
                {
                    PlayerbotAI* masterBotAI = GET_PLAYERBOT_AI(master);
                    if (masterBotAI || sRandomPlayerbotMgr->IsRandomBot(player))
                    {
                        nonCombatEngine->addStrategy("pvp");
                        nonCombatEngine->addStrategy("collision");
                        nonCombatEngine->addStrategy("grind");
                        nonCombatEngine->addStrategy("group");
                        nonCombatEngine->addStrategy("guild");

                        if (sPlayerbotAIConfig->autoDoQuests)
                        {
                            nonCombatEngine->addStrategy("travel");
                            nonCombatEngine->addStrategy("rpg");
                        }

                        if (masterBotAI)
                            nonCombatEngine->addStrategy("maintenance");

                        nonCombatEngine->ChangeStrategy(sPlayerbotAIConfig->randomBotNonCombatStrategies);
                    }
                    else
                        nonCombatEngine->ChangeStrategy(sPlayerbotAIConfig->nonCombatStrategies);
                }
            }
        }
    }
    else
    {
        nonCombatEngine->ChangeStrategy(sPlayerbotAIConfig->nonCombatStrategies);
    }

    // Battleground switch
    if (player->InBattleground() && player->GetBattleground())
    {
        nonCombatEngine->addStrategies("nc", "chat", "default", "buff", "food", "mount", "pvp", "collision", "dps assist", "attack tagged", "emote", nullptr);
        nonCombatEngine->removeStrategy("custom::say");
        nonCombatEngine->removeStrategy("travel");
        nonCombatEngine->removeStrategy("rpg");
        nonCombatEngine->removeStrategy("grind");

        BattlegroundTypeId bgType = player->GetBattlegroundTypeId();
        if (bgType == BATTLEGROUND_RB)
            bgType = player->GetBattleground()->GetBgTypeID(true);

        if (bgType <= BATTLEGROUND_EY || bgType == BATTLEGROUND_IC) // do not add for not supported bg
            nonCombatEngine->addStrategy("battleground");

        if (bgType == BATTLEGROUND_WS)
            nonCombatEngine->addStrategy("warsong");

        if (bgType == BATTLEGROUND_AV)
            nonCombatEngine->addStrategy("alterac");

        if (bgType == BATTLEGROUND_AB)
            nonCombatEngine->addStrategy("arathi");

        if (bgType == BATTLEGROUND_EY)
            nonCombatEngine->addStrategy("eye");

        if (bgType == BATTLEGROUND_IC)
            nonCombatEngine->addStrategy("isle");

        if (player->InArena())
        {
            nonCombatEngine->addStrategy("arena");
            nonCombatEngine->removeStrategy("mount");
        }
    }
}

Engine* AiFactory::createNonCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* aiObjectContext)
{
	Engine* nonCombatEngine = new Engine(facade, aiObjectContext);

    AddDefaultNonCombatStrategies(player, facade, nonCombatEngine);
	return nonCombatEngine;
}

void AiFactory::AddDefaultDeadStrategies(Player* player, PlayerbotAI* const facade, Engine* deadEngine)
{
    deadEngine->addStrategies("dead", "stay", "chat", "default", "follow", nullptr);

    if (sRandomPlayerbotMgr->IsRandomBot(player) && !player->GetGroup())
    {
        deadEngine->removeStrategy("follow");
    }
}

Engine* AiFactory::createDeadEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext)
{
    Engine* deadEngine = new Engine(facade, AiObjectContext);
    AddDefaultDeadStrategies(player, facade, deadEngine);
    return deadEngine;
}
