#pragma once

#include "Tile.h"
#include "structs.h"
#include "Hero.h"

class ValleyData
{
public:
    ValleyData();
    ~ValleyData(void);
    void Reset(bool force);

    Hero* m_temphero;
    uint64_t m_lastUpdated;
    stTroops m_troops;
    Tile* m_tile;
};
