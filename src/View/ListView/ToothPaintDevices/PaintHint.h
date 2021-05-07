#pragma once
#include <array>

enum class ToothHint {none, normal, root, extr, extr_m, impl, impl_m,};
enum class SurfaceHint {none, normal, red, blue, green};
enum class EndoHint {none, red, darkred, blue, green};
enum class PostHint {none, blue, green};
enum class ProsthoHint {none, crown, crown_green, bridge, bridge_green};
enum class BridgeAppearance { Middle, Terminal, None };// DO NOT CHANGE THE ORDER IN ANY CIRUMSTANCES!!! (casting from bridgePos)
//the bool stores whether the bridge is made by the current user, or not
typedef std::tuple<BridgeAppearance, bool> BridgeAppearenceTuple;

struct PaintHint
{
	int idx;
	bool temp;
	int num;

	ToothHint tooth;
	std::array<SurfaceHint, 6> surfaces;
	EndoHint endo;
	PostHint post;
	ProsthoHint prostho;

	bool lesion;
	bool dns;
	bool perio;
	bool frac;
	int mobility;
	 
};