#ifndef CRWEntry_2dEffect_ParticleEffect_H
#define CRWEntry_2dEffect_ParticleEffect_H

#include "mcore.h"
#include "Type/Types.h"
#include "Format/RenderWare/Helper/BinaryStream/Entries/C2dEffect.h"
#include <string>

// todo - fucking rename to without effect in name

class mcore::CRWEntry_2dEffect_ParticleEffect : public mcore::C2dEffect
{
public:
	CRWEntry_2dEffect_ParticleEffect(void);

	void							unserialize(void);
	void							serialize(void);

	void							setParticleEffectName(std::string& strParticleEffectName) { m_strParticleEffectName = strParticleEffectName; }
	std::string&					getParticleEffectName(void) { return m_strParticleEffectName; }

private:
	std::string						m_strParticleEffectName;
};

#endif