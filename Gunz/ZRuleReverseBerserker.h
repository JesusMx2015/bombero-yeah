#ifndef _ZRULE_REVERSE_BERSERKER_H
#define _ZRULE_REVERSE_BERSERKER_H

#include "ZRule.h"



class ZRuleReverseBerserker : public ZRule
{
private:
	MUID	m_uidBerserker;
	float	m_fElapsedHealthUpdateTime;
	void AssignBerserker(MUID& uidBerserker);
	virtual void OnUpdate(float fDelta);
	void BonusHealth(ZCharacter* pBerserker);
	void AddHealth(ZCharacter* Uid);
public:
	ZRuleReverseBerserker(ZMatch* pMatch);
	virtual ~ZRuleReverseBerserker();
	virtual bool OnCommand(MCommand* pCommand);
	virtual void OnResponseRuleInfo(MTD_RuleInfo* pInfo);
	MUID GetBerserkerUID() const { return m_uidBerserker; }
};

#define BERSERKER_DAMAGE_RATIO			2.0f		// ����Ŀ�� �Ǹ� �Ŀ��� 2��� �ȴ�.

#endif