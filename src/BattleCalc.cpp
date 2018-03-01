// BattleCalc.cpp : Defines the exported functions for the DLL application.
//

#include "combatsimulator.h"
#include <array>
#include <iostream>
#include <stdlib.h>
#include <math.h>

std::array<int32_t,5> CombatSimulator::meleeTroopTypes = { 0,1,2,3,4 };
std::array<int32_t,3> CombatSimulator::rangedTroopTypes = { 5,9,11 };
std::array<int32_t,3> CombatSimulator::mechTroopTypes = { 9,10,11 };
std::array<int32_t,2> CombatSimulator::mountedTroopTypes = { 7,8 };
std::array<int32_t,6> CombatSimulator::groundTroopTypes = { 0,1,2,3,4,5 };
int8_t CombatSimulator::troopTypes[12] = { true,true,true,true,true,false,true,true,true,false,true,false };
int32_t CombatSimulator::movementOrder[12]= { 2,7,8,3,4,1,0,6,10,5,9,11 };
int32_t CombatSimulator::nonRangedTroops[9]={ 7,8,3,4,1,0,6,2,10 };
troopStat CombatSimulator::baseStats[12] = {
    troopStat{ 100,5,50,180,10 }, // worker
    troopStat{ 200,50,50,200,20 }, // warrior
    troopStat{ 100,20,50,3000,20 }, // scout
    troopStat{ 300,150,150,300,50 }, // pike
    troopStat{ 350,100,250,275,30 }, // swords
    troopStat{ 250,120,50,250,1200 }, // archer
    troopStat{ 700,10,60,150,10 }, //transporter
    troopStat{ 500,250,180,1000,100 }, // cavalry
    troopStat{ 1000,350,350,750,80 }, // cataphract
    troopStat{ 320,450,160,100,1400 }, // ballista
    troopStat{ 5000,250,160,120,600 }, // ram
    troopStat{ 480,600,200,80,1500 } // catapult
};
troopStat CombatSimulator::baseFortificationStats[5] = {
    troopStat{ 0,0,0,0,5000 }, // trap
    troopStat{ 0,0,0,0,5000 }, // abatis
    troopStat{ 2000,300,360,0,1300 }, // archer tower
    troopStat{ 0,500,0,0,1300 }, // rolling logs
    troopStat{ 0,800,0,0,5000 } // trebuchet
};
float CombatSimulator::damageModifiers[12][17] = {
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 }, // modifiers for worker attacking other troops
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 }, // for warrior
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 }, // for scout
    { 0,0,0,0,0,0,0,1.8,1.8,0,0,0,0,0,0.3 }, // for pike
    { 0,0,0,1.1,0,0,0,0,0,0,0,0,0,0,0.3 }, // for swords
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 }, // for archer
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 }, // for transporter
    { 0,0,0,0,0,1.2,0,0,0,0,0,0,0,0,0.3 }, // for cavs
    { 0,0,0,0,0,1.2,0,0,0,0,0,0,0,0,0.3 }, // for cataphracts
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 }, // for ballista
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 }, // for ram
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3 } // for catapult
};
void CombatSimulator::modifyStats(troopStat* base, researchStats res, heroStat hero, float atk_modifier, float def_modifier, float life_modifier) {
    for (int i = 0; i < 12; i++) {
        base[i].defense = std::max((float)(1000 - ((res.iron_working * 5 + 100)/100)*((100 + hero.intel)/100)*base[i].defense*def_modifier)/1000, (float)0.5);
    }
    for (int i : groundTroopTypes) {
        base[i].life = base[i].life*(res.medicine * 5 + 100)*life_modifier / 100;
        base[i].attack = base[i].attack*atk_modifier;
        base[i].speed = base[i].speed*(res.compass * 10 + 100) / 100;
    }
    for (int i : mechTroopTypes) {
        base[i].life = base[i].life*life_modifier;
        base[i].attack = base[i].attack*atk_modifier;
        base[i].speed = base[i].speed*(res.horseback_riding * 5 + 100) / 100;
    }
    for (int i : rangedTroopTypes) {
        base[i].range = base[i].range*(res.archery * 5 + 100) / 100;
    }
    for (int i : mountedTroopTypes) {
        base[i].life = base[i].life*life_modifier;
        base[i].attack = base[i].attack*atk_modifier;
        base[i].speed = base[i].speed*(res.horseback_riding * 5 + 100) / 100;
    }
}
int8_t CombatSimulator::compareSpeed(combatTroops& x, combatTroops& y) {
    if (x.stat->speed == y.stat->speed) {
        if (x.isAttacker) return false;
        else return true;
    }
    return (x.stat->speed > y.stat->speed);
}
// id for the fortifications are reduced by 14
void CombatSimulator::fight(attacker atk, defender def,battleResult* br) {
    troopStat attackerTroopStats[12];
    std::copy(baseStats, baseStats + 12, attackerTroopStats);
    troopStat defenderTroopStats[12];
    std::copy(baseStats, baseStats + 12, defenderTroopStats);
    modifyStats(attackerTroopStats, atk.research, atk.hero, atk.attack_modifier, atk.defence_modifier, atk.life_modifier);
    modifyStats(defenderTroopStats, def.research, def.hero, def.attack_modifier, def.defence_modifier, def.life_modifier);
    
    // modifying archer tower stats
    // TODO: other wall defenses
    troopStat defenderFortificationStats[5];
    std::copy(baseFortificationStats,baseFortificationStats+5,defenderFortificationStats);
    defenderFortificationStats[2].life = (def.research.medicine * 5 + 100)*defenderFortificationStats[2].life / 100;
    defenderFortificationStats[2].attack = defenderFortificationStats[2].attack*def.attack_modifier;
    defenderFortificationStats[2].defense = std::max(1 - (def.research.iron_working * 5 + 100)*(100 + def.hero.intel)*defenderFortificationStats[2].defense*def.defence_modifier / 10000000, (float)0.5);
    defenderFortificationStats[2].range = defenderFortificationStats[2].range*(def.research.archery * 5 + def.wallLevel*4.5 + 100) / 100;
    combatTroops combatTroopsArrayAtk[12];
    combatTroops combatTroopsArrayDef[12];
    float wallhitpoints = 50000 * def.wallLevel*(def.wallLevel + 1);
    // Calculating field size
    int field_size=0;
/*    for (int i : {}) {
        if (atk.troops[i]>0 || def.troops[i]>0) {
            field_size=max(field_size,baseStats[i].speed+baseStats[i].range);
        }
    }*/
    for (int i : {0, 1, 3, 4, 6, 5, 7, 8, 9, 10, 11}) {
        if (atk.troops[i] > 0 || def.troops[i] > 0) {
            field_size = std::max(field_size, (int) (200 + baseStats[i].range));
        }
    }
    for (int i = 0; i < 5; i++) {
        if (def.fortifications[i] > 0) field_size = std::max(field_size, (int)(200 + baseFortificationStats[i].range));
    }
#if _DEBUG
    std::cout << "Field size " << field_size << "\n";
#endif
    // Creating an array with attacking troops
    combatTroops* u;
    for (int i = 0; i < 12; i++) {
        u=&combatTroopsArrayAtk[i];
        u->count = atk.troops[i];
        u->location = 0;
        u->typeId = i;
        u->stat = &attackerTroopStats[i];
        u->isAttacker = true;
        u->effectiveTroops=atk.troops[i];
    }

    // Creating an array with defending troops
    for (int i = 0; i < 12; i++) {
        u=&combatTroopsArrayDef[i];
        u->count = def.troops[i];
        u->location = field_size;
        u->typeId = i;
        u->stat = &defenderTroopStats[i];
        u->effectiveTroops=def.troops[i];
    }

    combatTroops archerTower;
    archerTower.count = def.fortifications[2];
    archerTower.stat = &defenderFortificationStats[2];
    archerTower.effectiveTroops = def.fortifications[2];
    archerTower.typeId = 14;

    int64_t trapCount = def.fortifications[0];
    int64_t abatisCount = def.fortifications[1];
    int64_t rollingLogCount = def.fortifications[3];
    int64_t rockCount = def.fortifications[4];

    int64_t trapDieRate = 0;
    if (trapCount > 0) {
        trapDieRate = trapCount * (5 + rand() % 5) / 100;
    }
    int64_t abatisDieRate = abatisCount / 10;
    int64_t logDieRate = rollingLogCount / 10;
    int64_t maxRockDieRate = rockCount / 10;

    int8_t atkerType;
    float atkValue;
    combatTroops* defenderTroop;
    float damageModifier;
    int8_t inRange;
    int64_t damage;
    int64_t damage1;
    int8_t attackerAlive;
    int8_t defenderAlive;
    int32_t maxRange;
    int round;
    for (round = 0; round < 100; ++round) {

        // move the troops according to the movement order
        for (int i : movementOrder) {
            combatTroops* pq = &combatTroopsArrayDef[i];

            // first moving the defender troop of that type
            if (i!=2 && i!=6 && pq->count > 0) {
                int32_t nearestPosition = 0;
                inRange=false;
                maxRange = pq->location - pq->stat->range;
                // checking if some troop is already in its range
                for (combatTroops& xp : combatTroopsArrayAtk) {
                    if (xp.count > 0 ) {
                        if (xp.location >= maxRange) {
                            inRange=true;
                            break;
                        }
                        nearestPosition=std::max(nearestPosition,xp.location);
                    }
                }

                // if no troop is in range, move
                if (!inRange) {
                    pq->location=std::max(nearestPosition,(int32_t)(pq->location-pq->stat->speed));
//                    else pq->location = max(min(pq->location,nearestPosition+pq->stat->range),pq->location-pq->stat->speed);
#if _DEBUG
                    std::cout << "Defender troops of type " << pq->typeId << " moves to " << pq->location << "\n";
#endif
                }
            }
            
            if (i==2 || i==6) {
                bool scoutOnly=true;
                for (int i: {0,1,3,4,5,7,8,9,10,11}) {
                    if (combatTroopsArrayAtk[i].count>0) {
                        scoutOnly=false;
                        break;
                    }
                }
                if (!scoutOnly) continue;
            }
            pq = &combatTroopsArrayAtk[i];

            // next moving the attacker troop of that type
            if (pq->count > 0) {
                int32_t nearestPosition = field_size;
                inRange=false;
                maxRange = pq->location + pq->stat->range;
                // checking if some troop is already in its range
                for (combatTroops& xp : combatTroopsArrayDef) {
                    if (xp.count > 0 ) {
                        if (xp.location <= maxRange) {
                            inRange=true;
                            break;
                        }
                        nearestPosition=std::min(nearestPosition,xp.location);
                    }
                }

                // if no troop is in range, move
                if (!inRange) {
                    pq->location=std::min(nearestPosition,(int32_t)(pq->location+pq->stat->speed));
//                    else pq->location = min(max(pq->location,nearestPosition-pq->stat->range),pq->location+pq->stat->speed);
#if _DEBUG
                    std::cout << "Attacker troops of type " << pq->typeId << " moves to " << pq->location << "\n";
#endif
                }
            }
        }

        // perform attacks according to the movement order
        for (int i : { 2, 7, 8, 3, 4, 5, 1, 0, 6, 10, 9, 11 }) {

            // first defender troop of that type attacks

            combatTroops* pq=&combatTroopsArrayDef[i];
            if (pq->count > 0) {

                // choose which troop to attack
                defenderTroop=nullptr;
                atkValue=0;
                maxRange=pq->location - pq->stat->range;
                int32_t mindistance=-1;
                if (troopTypes[i]) {
                    for (combatTroops& lp:combatTroopsArrayAtk) {
                        if (lp.effectiveTroops> 0 && lp.location>=maxRange) {
                            if (mindistance==-1||mindistance>(pq->location-lp.location)||(mindistance==(pq->location-lp.location)&&(lp.stat->attack*lp.effectiveTroops>atkValue))) {
                                defenderTroop=&lp;
                                atkValue=lp.stat->attack*lp.effectiveTroops;
                                mindistance=pq->location-lp.location;
                            }
                        }
                    }
                }
                else {
                    // checking if there is a ranged troop in range
                    for (int j : rangedTroopTypes) {
                        combatTroops& lp=combatTroopsArrayAtk[j];
                        if (lp.effectiveTroops>0) {
                            if (lp.location >= maxRange) {
                                if ((lp.stat->attack*lp.effectiveTroops)>atkValue) {
                                    atkValue=lp.stat->attack*lp.effectiveTroops;
                                    defenderTroop=&lp;
                                }
                            }
                        }
                    }

                    // checking for fastest melee troop if no ranged troop is in range
                    float bestSpeed=-1;
                    if (defenderTroop==nullptr) {
                        for (int j : nonRangedTroops) {
                            combatTroops& lp=combatTroopsArrayAtk[j];
                            if (lp.effectiveTroops> 0 && lp.location>=maxRange && lp.stat->speed>bestSpeed) {
                                defenderTroop=&lp;
                                bestSpeed=lp.stat->speed;
                            }
                        }
                    }
                }

                // attack if there is a troop to attack
                if (defenderTroop!=nullptr) {
                    int64_t killed = 0;
                    if (troopTypes[i]) {
                        damageModifier=damageModifiers[pq->typeId][defenderTroop->typeId];
                        if (damageModifier==0) damageModifier=1;
                        // calculating damage by the attacker
#if _DEBUG
                        std::cout << "floor " << damageModifier << "*" << pq->count << "*" << pq->stat->attack << "*" << defenderTroop->stat->defense << "/" << defenderTroop->stat->life << "\n";
#endif
                        killed=(float)(damageModifier*pq->count*pq->stat->attack*defenderTroop->stat->defense) / defenderTroop->stat->life;
                    }
                    else {
                        int64_t distance=pq->location-defenderTroop->location;
                        if (distance <= defenderTroop->stat->range && troopTypes[defenderTroop->typeId]) damageModifier=0.25;
                        else if (distance > ((pq->stat->range)/2)) damageModifier=0.5;
                        else damageModifier=1;
#if _DEBUG                        
                        std::cout << "ceil " << pq->stat->attack << "*" << defenderTroop->stat->defense << "*" << pq->count << "*" << damageModifier << "/" << defenderTroop->stat->life << "\n";
#endif                        
                        killed=ceil(pq->stat->attack*defenderTroop->stat->defense*pq->count*damageModifier / defenderTroop->stat->life);
                    }
#if _DEBUG
                    std::cout << "Defender troops of type " << pq->typeId << " kills " << killed << " troops of type " << defenderTroop->typeId << "\n";
#endif
                    if (killed>defenderTroop->effectiveTroops) defenderTroop->effectiveTroops=0;
                    else defenderTroop->effectiveTroops-=killed;
                }
            }

            // now doing the same for attacker troop of that type
            pq=&combatTroopsArrayAtk[i];
            if (pq->count>0) {
                // choose which troop to attack
                defenderTroop=nullptr;
                atkValue=0;
                maxRange=pq->location + pq->stat->range;
                int32_t mindistance=-1;

                // if attacker is a melee troop
                if (troopTypes[i]) {
                    for (int j=0;j<12;j++) {
                        combatTroops& lp=combatTroopsArrayDef[j];
                        if (lp.effectiveTroops> 0 && lp.location<=maxRange) {
                            if (mindistance==-1||mindistance>(lp.location-pq->location)||(mindistance==(lp.location-pq->location)&&(lp.stat->attack*lp.effectiveTroops>atkValue))) {
                                defenderTroop=&lp;
                                atkValue=lp.stat->attack*lp.effectiveTroops;
                                mindistance=lp.location-pq->location;
                            }
                        }
                    }
                    // checking if the archer towers are in range
                    if (defenderTroop == nullptr) {
                        // checking if the archer towers are in range
                        if (archerTower.effectiveTroops > 0 && field_size <= maxRange) {
                            if ((archerTower.stat->attack*archerTower.effectiveTroops) > atkValue) {
                                defenderTroop = &archerTower;
                            }
                        }
                    }
                }
                // if attacker is a ranged troop
                else {
                    // checking if there is a ranged troop in range
                    for (int j : rangedTroopTypes) {
                        combatTroops& lp=combatTroopsArrayDef[j];
                        if (lp.effectiveTroops>0) {
                            if (lp.location <= maxRange) {
                                if ((lp.stat->attack*lp.effectiveTroops)>atkValue) {
                                    atkValue=lp.stat->attack*lp.effectiveTroops;
                                    defenderTroop=&lp;
                                }
                            }
                        }
                    }

                    // checking if the archer towers are in range
                    if (archerTower.effectiveTroops > 0 && field_size <= maxRange) {
                        if ((archerTower.stat->attack*archerTower.effectiveTroops) > atkValue) {
                            defenderTroop = &archerTower;
                        }
                    }
                    // checking for fastest melee troop if no ranged troop is in range
                    float bestSpeed=-1;
                    if (defenderTroop==nullptr) {
                        for (int j : nonRangedTroops) {
                            combatTroops& lp=combatTroopsArrayDef[j];
                            if (lp.effectiveTroops> 0 && lp.location<=maxRange && lp.stat->speed>bestSpeed) {
                                defenderTroop=&lp;
                                bestSpeed=lp.stat->speed;
                            }
                        }
                    }
                }

                // attack if there is a troop to attack
                if (defenderTroop!=nullptr) {
                    int64_t killed=0;
                    if (troopTypes[i]) {
                        damageModifier=damageModifiers[pq->typeId][defenderTroop->typeId];
                        if (damageModifier==0) damageModifier=1;
                        // calculating damage by the attacker
#if _DEBUG
                        std::cout << "floor " << damageModifier << "*" << pq->count << "*" << pq->stat->attack << "*" << defenderTroop->stat->defense << "/" << defenderTroop->stat->life << "\n";
#endif
                        killed=(float)(damageModifier*pq->count*pq->stat->attack*defenderTroop->stat->defense) / defenderTroop->stat->life;
                    }
                    else {
                        int64_t distance=defenderTroop->location-pq->location;
                        if (distance <= defenderTroop->stat->range && troopTypes[defenderTroop->typeId]) damageModifier=0.25;
                        else if (distance > ((pq->stat->range)/2) ) damageModifier=0.5;
                        else damageModifier=1;
#if _DEBUG
                        std::cout << "ceil " << pq->stat->attack << "*" << defenderTroop->stat->defense << "*" << pq->count << "*" << damageModifier << "/" << defenderTroop->stat->life << "\n";
#endif
                        killed=ceil(pq->stat->attack*defenderTroop->stat->defense*pq->count*damageModifier / defenderTroop->stat->life);
                    }
#if _DEBUG
                    std::cout << "Attacker troops of type " << pq->typeId << " kills " << killed << " troops of type " << defenderTroop->typeId << "\n";
#endif
                    if (killed>defenderTroop->effectiveTroops) defenderTroop->effectiveTroops=0;
                    else defenderTroop->effectiveTroops-=killed;
                }
                // attack wall if no troop is in range
                else if (wallhitpoints > 0 && maxRange >= field_size) {
                    wallhitpoints = std::max((float)(wallhitpoints - floor(pq->count*pq->stat->attack)), (float)0);
                }
            }
        }

        // now attack using traps
        if (trapCount > 0) {
            int totalTroops = 0;
            int pos = field_size - 5000;
            for (combatTroops& lp : combatTroopsArrayAtk) {
                if (lp.effectiveTroops > 0 && lp.location >= pos && lp.location<field_size) totalTroops++;
            }
            if (totalTroops > 0) {
                int maxTraps = std::min(trapCount, trapDieRate);
                int kills = (float)(maxTraps*0.5*def.trapKillPower) / totalTroops;
                for (combatTroops& lp : combatTroopsArrayAtk) {
                    if (lp.effectiveTroops > 0 && lp.location >= pos && lp.location<field_size) {
                        lp.effectiveTroops -= kills;
                        if (lp.effectiveTroops < 0) lp.effectiveTroops = 0;
                    }
                }
                trapCount -= maxTraps;
            }
        }

        // now attack using abatis
        if (abatisCount > 0) {
            // if there are cavs attack them
            int pos = field_size - 5000;
            combatTroops* lp;
            if ((lp=&combatTroopsArrayAtk[7])->effectiveTroops > 0 && lp->location>=pos && lp->location<field_size) {
                int64_t maxAbatisKill = std::min(abatisCount, abatisDieRate);
                int64_t kills = maxAbatisKill * 0.535;
                lp->effectiveTroops = std::max(lp->effectiveTroops - kills, (int64_t)0);
            }
            // attack phracts if no cav is there
            else if ((lp=&combatTroopsArrayAtk[8])->effectiveTroops > 0 && lp->location>=pos && lp->location<field_size) {
                int64_t maxAbatisKill = std::min(abatisCount, abatisDieRate);
                int64_t kills = maxAbatisKill * 0.535;
                lp->effectiveTroops = std::max(lp->effectiveTroops - kills, (int64_t)0);
            }
        }

        // now attack using rolling logs
        if (rollingLogCount > 0) {
            int pos = field_size - defenderFortificationStats[3].range;
            float maxAtkValue = 0;
            combatTroops* defenderTroop = nullptr;
            combatTroops* lp;
            // choose which troop to attack
            for (int i : {0, 1, 2, 3, 4, 5}) {
                if ((lp = &combatTroopsArrayAtk[i])->effectiveTroops > 0 && lp->location >=pos && lp->location < field_size && (lp->effectiveTroops*lp->stat->attack > maxAtkValue)) {
                    defenderTroop = &combatTroopsArrayAtk[i];
                    maxAtkValue = lp->effectiveTroops*lp->stat->attack;
                }
            }
            if (defenderTroop != nullptr) {
                int64_t maxLogKill = std::min(rollingLogCount, logDieRate);
                defenderTroop->effectiveTroops = std::max((int64_t)(defenderTroop->effectiveTroops - floor(maxLogKill*def.logKillPower*defenderFortificationStats[3].attack*defenderTroop->stat->defense)), (int64_t)0);
                rollingLogCount -= maxLogKill;
            }
        }

        // now attack using trebs
        if (rockCount > 0) {
            int pos = field_size - defenderFortificationStats[4].range;
            float maxAtkValue = 0;
            combatTroops* defenderTroop = nullptr;
            combatTroops* lp;
            // choose which troop to attack
            for (int i = 0; i < 12; i++) {
                if ((lp = &combatTroopsArrayAtk[i])->effectiveTroops > 0 && lp->location >= pos && lp->location < field_size && (lp->effectiveTroops*lp->stat->attack > maxAtkValue)) {
                    defenderTroop = &combatTroopsArrayAtk[i];
                    maxAtkValue = lp->effectiveTroops*lp->stat->attack;
                }
            }
            if (defenderTroop != nullptr) {
                double totalDefValue = defenderTroop->effectiveTroops * defenderTroop->stat->life*defenderTroop->stat->defense;
                double totalAtkValue = maxRockDieRate*def.rockKillPower*defenderFortificationStats[4].attack;
                if (totalDefValue > totalAtkValue) {
                    rockCount -= maxRockDieRate;
                    defenderTroop->effectiveTroops = std::max((int64_t)(defenderTroop->effectiveTroops - floor((float)(maxRockDieRate*defenderFortificationStats[4].attack*def.rockKillPower) / (float)(defenderTroop->stat->defense*defenderTroop->stat->life))), (int64_t)0);
                }
                else {
                    rockCount -= ceil((float)(defenderTroop->stat->defense*defenderTroop->stat->life*defenderTroop->effectiveTroops) / (float)(def.rockKillPower*defenderFortificationStats[4].attack));
                    if (rockCount < 0) rockCount = 0;
                }
            }
        }

        // now attack using archer towers
        archerTower.count = archerTower.effectiveTroops;
        if (archerTower.count > 0) {
            // checking if there is a ranged troop in range
            defenderTroop = nullptr;
            atkValue = 0;
            maxRange = field_size - archerTower.stat->range;
            for (int j : rangedTroopTypes) {
                combatTroops& lp = combatTroopsArrayAtk[j];
                if (lp.effectiveTroops>0) {
                    if (lp.location >= maxRange) {
                        if ((lp.stat->attack*lp.effectiveTroops)>atkValue) {
                            atkValue = lp.stat->attack*lp.effectiveTroops;
                            defenderTroop = &lp;
                        }
                    }
                }
            }

            // checking for fastest melee troop if no ranged troop is in range
            float bestSpeed = -1;
            if (defenderTroop == nullptr) {
                for (int j : nonRangedTroops) {
                    combatTroops& lp = combatTroopsArrayAtk[j];
                    if (lp.effectiveTroops> 0 && lp.location >= maxRange && lp.stat->speed>bestSpeed) {
                        defenderTroop = &lp;
                        bestSpeed = lp.stat->speed;
                    }
                }
            }
            if (defenderTroop != nullptr) {
                int64_t killed;
                int64_t distance = field_size - defenderTroop->location;
                if (distance > ((archerTower.stat->range) / 2)) damageModifier = 0.5;
                else damageModifier = 1;
#if _DEBUG                        
                std::cout << "ceil " << archerTower.stat->attack << "*" << defenderTroop->stat->defense << "*" << archerTower.count << "*" << damageModifier << "/" << defenderTroop->stat->life << "\n";
#endif                        
                killed = (float) archerTower.stat->attack*archerTower.count*damageModifier / (float)(defenderTroop->stat->defense * defenderTroop->stat->life);
#if _DEBUG
                std::cout << "Defender archer towers kills " << killed << " troops of type " << defenderTroop->typeId << "\n";
#endif
                if (killed > defenderTroop->effectiveTroops) defenderTroop->effectiveTroops = 0;
                else defenderTroop->effectiveTroops -= killed;
            }
        }
#if _DEBUG
        std::cout << "==== Round finished ====\n";
#endif

        // calculating total damage for all the troops and reducing troop count
        attackerAlive = false;
        defenderAlive = false;
        for (combatTroops& xp:combatTroopsArrayAtk) {
#if _DEBUG
            if (xp.effectiveTroops<xp.count) {
                std::cout << "Attacker troops of type " << xp.typeId << " killed total " << floor(xp.count-xp.effectiveTroops) << "\n";
            }
#endif
            xp.count=xp.effectiveTroops;
            if (xp.count>0) attackerAlive=true;
        }
        for (combatTroops& xp:combatTroopsArrayDef) {
#if _DEBUG
            if (xp.effectiveTroops<xp.count) {
                std::cout << "Defender troops of type " << xp.typeId << " killed total " << floor(xp.count-xp.effectiveTroops) << "\n";
            }
#endif
            xp.count=xp.effectiveTroops;
            if (xp.count>0) defenderAlive=true;
        }
        if (wallhitpoints > 0) defenderAlive = true;
#if _DEBUG
        std::cout << "==== ROUND " << round << " END ==== \n";
#endif
        if (!attackerAlive || !defenderAlive) break;
    }
    std::copy(atk.troops, atk.troops + 12, br->attackerTroops);
    std::copy(def.troops, def.troops + 12, br->defenderTroops);
    for (combatTroops& uh : combatTroopsArrayAtk) {
        br->attackerTroops[uh.typeId] = uh.count;
    }
    for (combatTroops& uh : combatTroopsArrayDef) {
        br->defenderTroops[uh.typeId] = uh.count;
    }
    br->fortification[0] = trapCount;
    br->fortification[1] = abatisCount;
    br->fortification[2] = archerTower.count;
    br->fortification[3] = rollingLogCount;
    br->fortification[4] = rockCount;
    br->result = defenderAlive;
    br->totalRounds = std::min(round + 1,100);
#if _DEBUG
    std::cout << "==== Remaining Troops ====\n";
    for (combatTroops& hu : combatTroopsArrayAtk) {
        if (hu.count > 0) {
            std::cout << "Attacker troop of type " << hu.typeId << " count " << hu.count << "\n";
        }
    }
    for (combatTroops& hu : combatTroopsArrayDef) {
        if (hu.count > 0) {
            std::cout << "Defender troop of type " << hu.typeId << " count " << hu.count << "\n";
        }
    }
    if (trapCount > 0) std::cout << "Traps survived " << trapCount << "\n";
    if (abatisCount > 0) std::cout << "Abatis survived " << abatisCount << "\n";
    if (archerTower.count > 0) std::cout << "Archer towers survived " << archerTower.count << "\n";
    if (rollingLogCount > 0) std::cout << "Rolling logs survived " << rollingLogCount << "\n";
    if (rockCount > 0) std::cout << "Trebuchets survived " << rockCount << "\n";
    if (wallhitpoints > 0) std::cout << "Wall hitpoints left " << wallhitpoints << "\n";
#endif
}