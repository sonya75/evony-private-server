#include "Valley.h"
#include "defines.h"
#include "Utils.h"
#include <cmath>

ValleyData::ValleyData(): m_temphero(nullptr), m_lastUpdated(0)
{

}

void ValleyData::Reset(bool force) {
    if (!force && (Utils::time() - m_lastUpdated) < 360000) return;
    if (m_temphero==nullptr) m_temphero=new Hero;
    m_temphero->m_level=1+(rand()%20);
    m_temphero->m_power=30+(rand()%30);
    m_temphero->m_poweradded=m_temphero->m_level;
    m_temphero->m_management=30+(rand()%30);
    m_temphero->m_stratagem=30+(rand()%30);
    m_temphero->m_status=DEF_HEROIDLE;
    m_temphero->m_name="TestName"+std::to_string(rand()%100);
    memset(&m_troops, 0, sizeof(stTroops));
    std::vector<int8_t> trooptypes;
    if ((rand() % 3) > 0) trooptypes.push_back(1);
    if ((rand() % 3) > 0) trooptypes.push_back(3);
    if (m_tile->m_level > 2 && (rand() % 3) > 0) trooptypes.push_back(4);
    if (m_tile->m_level > 4 && (rand() % 3) > 0) trooptypes.push_back(5);
    if (m_tile->m_level > 6 && (rand() % 3) > 0) trooptypes.push_back(7);
    if (trooptypes.size() == 0) trooptypes.push_back(1);
    int x = pow(2,m_tile->m_level - 1) * 10000;
    double totalexp = (x + (rand() % x))/trooptypes.size();
    int64_t* y = (int64_t*)&m_troops;
    for (int8_t z : trooptypes) {
        *(y + z) = (int64_t)(ceil(totalexp / Utils::TROOP_EXPS[z]));
    }
    m_lastUpdated = Utils::time();
}

ValleyData::~ValleyData(void) {
    if (m_temphero!=nullptr) {
        delete m_temphero;
        m_temphero=nullptr;
    }
}