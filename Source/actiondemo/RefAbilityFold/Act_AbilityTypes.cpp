#include "Act_AbilityTypes.h"

#include "InstalledPlatformInfo.h"

int FAct_AbilityTypes::GetAbilityListSize()
{

	return AbilityList.Len();
}

TCHAR FAct_AbilityTypes::GetAbilityListContentByIndex(int32 index,bool GetEnd)
{	TCHAR Content;
	if (GetEnd)
	{
		Content=AbilityList[AbilityList.Len()-1];
	}
	else
	{
		Content=AbilityList[index];
	}
	return Content;
}
