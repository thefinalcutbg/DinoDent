#pragma once
#include <array>

enum class ToothTextureHint {none, normal, root, extr, extr_m, impl, impl_m};
enum class SurfaceColor {none, red, blue, green};
enum class EndoHint {none, red, darkred, blue, green};
enum class PostHint {none, blue, green};
enum class ProsthoHint {none, crown, crown_green, bridge, bridge_green, splint, splint_green};
enum class BridgeAppearance { Middle, Terminal, None }; // DO NOT CHANGE THE ORDER IN ANY CIRUMSTANCES!!! (casting from bridgePos)
enum class BridgeTerminal{Distal, Center, Medial};

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
	BridgeTerminal bridgePos;

	bool lesion;
	bool dns;
	bool dnsToothImplacted = false;
	bool perio;
	bool frac;
	bool impacted;
	int mobility;

	bool isUpper() { return idx < 16; };

	std::string toolTip;
	 
};