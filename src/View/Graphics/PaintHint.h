#pragma once
#include <array>

enum class ToothTextureHint {none, normal, root, extr, extr_m, impl, impl_m, unknown};
enum class SurfaceColor {none, red, blue, green};
enum class EndoHint {none, red, darkred, blue, green};
enum class PostHint {none, blue, green};
enum class ProsthoHint {none, crown, crown_green, bridge, bridge_green, splint, splint_green, denture};
enum class BridgeTerminal{Distal, Center, Medial};

struct SurfaceHint
{
	SurfaceColor color { SurfaceColor::none };
	bool outline{ false };
};

struct ToothPaintHint
{
	int idx{ -1 };
	bool temp{ false };
	int num{ -1 };

	ToothTextureHint tooth{ 0 };
	std::array<SurfaceHint, 6> surfaces;
	EndoHint endo{ 0 };
	PostHint post{ 0 };
	ProsthoHint prostho{ 0 };
	BridgeTerminal bridgePos{ 0 };

	bool lesion{ false };
	bool dns{ false };
	bool dnsToothImplacted{ false };
	bool perio{ false };
	bool frac{ false };
	bool impacted{ false };
	int mobility{ 0 };

	bool isUpper() { return idx < 16; };

	std::string toolTip;
	 
};