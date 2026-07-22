#pragma once

UENUM(BlueprintType)
enum class AudioSpatialization : uint8
{
	TwoD,
	ThreeDs
};
UENUM(BlueprintType)
enum class TPP_AudioTargetFollowMode : uint8
{
	StartLocation,
	TargetLocation
};
UENUM(BlueprintType)
enum class TPP_Slider : uint8
{
	Master,
	Music,
	Env,
	SFX
};