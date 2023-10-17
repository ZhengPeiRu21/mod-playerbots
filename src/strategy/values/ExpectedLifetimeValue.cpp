#include "ExpectedLifetimeValue.h"
#include "AiFactory.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "SharedDefines.h"

float ExpectedLifetimeValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);
    if (!target || !target->IsAlive()) {
        return 0.0f;
    }
    float dps = AI_VALUE(float, "expected group dps");
    float res = target->GetHealth() / dps;
    // bot->Say(target->GetName() + " lifetime: " + std::to_string(res), LANG_UNIVERSAL);
    return res;
}

float ExpectedGroupDpsValue::Calculate()
{
    float dps_num;
    Group* group = bot->GetGroup();
    if (!group) {
        dps_num = 1;
    } else {
        dps_num = group->GetMembersCount() * 0.7;
    }
    uint32 mixedGearScore = PlayerbotAI::GetMixedGearScore(bot, false, false, 12);
    // efficiency record based on rare gear level, is there better calculation method?
    // float dps_efficiency = 1;
    float basic_dps;
    int32 basic_gs;
    int32 level = bot->GetLevel();
    
    if (level <= 15) {
        basic_dps = 5 + level * 1;
    } else if (level <= 30) {
        basic_dps = 20 + (level - 15) * 2;
    } else if (level <= 40) {
        basic_dps = 50 + (level - 30) * 3;
    } else if (level <= 55) {
        basic_dps = 80 + (level - 45) * 8;
    } else if (level <= 60) {
        basic_dps = 200 + (level - 55) * 30;
    } else if (level <= 70) {
        basic_dps = 350 + (level - 60) * 40;
    } else {
        basic_dps = 750 + (level - 70) * 100;
    }

    if (level <= 8) {
        basic_gs = (level + 5) * 2;
    } else if (level <= 15) {
        basic_gs = (level + 5) * 3;
    } else if (level <= 60) {
        basic_gs = (level + 5) * 4;
    } else if (level <= 70) {
        basic_gs = (85 + (level - 60) * 3) * 4;
    } else if (level <= 80) {
        basic_gs = (155 + (level - 70) * 4) * 4;
    }

    float gs_modifier = (float)mixedGearScore / basic_gs;
    if (gs_modifier < 0.5) gs_modifier = 0.5;
    if (gs_modifier > 3) gs_modifier = 3;

    return dps_num * basic_dps * gs_modifier;
}