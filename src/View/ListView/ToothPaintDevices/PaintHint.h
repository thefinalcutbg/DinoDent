#pragma once
#include <array>

enum class ToothTextureHint {none, normal, root, extr, extr_m, impl, impl_m,};
enum class SurfaceColor {none, red, blue, green};
enum class EndoHint {none, red, darkred, blue, green};
enum class PostHint {none, blue, green};
enum class ProsthoHint {none, crown, crown_green, bridge, bridge_green};
enum class BridgeAppearance { Middle, Terminal, None }; // DO NOT CHANGE THE ORDER IN ANY CIRUMSTANCES!!! (casting from bridgePos)

struct BridgesPaintHint
{
	
	std::array<std::tuple<BridgeAppearance, bool>, 32> paintHints; //the bool stores whether the bridge is made by the current user, or not
};

struct SurfaceHint
{
	SurfaceColor color { SurfaceColor::none };
	bool outline{ false };
};

struct ToothPaintHint
{
	int idx;
	bool temp;
	int num;

	ToothTextureHint tooth;
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