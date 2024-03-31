// Copyright Cvxiv


#include "AbilitySystem/Data/CharacterClassInfo.h"

const FCharacterClassDefaultInfo& UCharacterClassInfo::GetClassDefaultInfo(const ECharacterClass& CharacterClass) {
	return CharacterClassInformation.FindChecked(CharacterClass);
}
