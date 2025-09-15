#pragma once
#include <array>
#include <string>
#include <memory>
enum class ToothTextureHint {none, normal, root, extr, extr_m, impl, impl_m, unknown};
enum class SurfaceColor {none, red, blue, green, orange};
enum class EndoHint {none, red, darkred, blue, green};
enum class PostHint {none, blue, green};
enum class ProsthoHint {none, crown, crown_green, bridge, bridge_green, splint, splint_green, denture, denture_green};
enum class BridgeTerminal{Distal, Center, Medial};

struct SurfaceHint
{
	SurfaceColor color { SurfaceColor::none };
	SurfaceColor outline{ SurfaceColor::none };
	SurfaceColor stripes{ SurfaceColor::none };
};

class Tooth;

struct ToothPaintHint
{
	ToothPaintHint() {};
	ToothPaintHint(const Tooth& tooth, const std::string& note = std::string{});


	std::unique_ptr<ToothPaintHint> dsn;

	int idx{ -1 };
	bool temp{ false };
	bool isHyperdontic{ false };
	std::string num{ -1 };

	ToothTextureHint tooth{ 0 };
	std::array<SurfaceHint, 6> surfaces;
	EndoHint endo{ 0 };
	PostHint post{ 0 };
	ProsthoHint prostho{ 0 };
	BridgeTerminal bridgePos{ 0 };

	bool resorption{ false };
	bool calculus{ false };
	bool lesion{ false };
	bool perio{ false };
	bool frac{ false };
	bool impacted{ false };
	int mobility{ 0 };

	bool isUpper() { return idx < 16; };

	std::string toolTip;
	 
	

};