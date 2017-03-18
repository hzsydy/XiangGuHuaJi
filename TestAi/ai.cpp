#include "user_toolbox.h"  // 直接用 user_toolbox 快速构建ai !


TMoney birthplacePrice(BaseMap* map) { 
    return 3; // 返回3元
}

TPosition birthplace(vector<TPosition>& posSelected, BaseMap* map) { 
    // 先考虑固定出生点
    //TPosition birthplace_list[] = {{15, 24}, /* 李兆基 */ {17, 4}, /* 紫荆桃李 */  };
    //for (TPosition p : birthplace_list) if (check_birth_pos_validity(posSelected, p)) return p;
    // 否则随缘吧
    return random_birthplace(posSelected, map);
}

void player_ai(Info& info) {
    // 外交 DiplomaticCommandList ------------------------------------------------------------------
    for (int i=0; i<info.playerSize; ++i) 
		info.DiplomaticCommandList[i] = KeepNeutral; // 中庸
    
    // 选择新的首都 newCapital ------------------------------------------------------------------
    info.newCapital = auto_capital(info);
    
    // 军事 MilitaryCommandList ------------------------------------------------------------------
    TMaskMap mask = get_border_TMaskMap(info); // 获取边界区域。TMaskMap 是一个bool型的二维数组 vector<vector<bool> >
    //if (info.playerInfo[info.id].warList.size()!=0) 
    //    mask[info.newCapital.x][info.newCapital.y] = true;  // 可以稍微调整一下mask
    info.MilitaryCommandList = auto_military(info,  // 自动军事
        mask,                            // 指定在mask上
        info.playerInfo[info.id].saving, // 花所有的钱
        MILITARY_COUNT_LIMIT);           // 举办这么多活动
    
    // 如果不满意，可以继续对军事列表做微调
    //TMilitaryCommand tmc = {info.newCapital, 50}; 
    //for (TMilitaryCommand& tmc : info.MilitaryCommandList) tmc.bomb_size -= 50/info.MilitaryCommandList.size();
    //info.MilitaryCommandList.push_back(tmc); 
}