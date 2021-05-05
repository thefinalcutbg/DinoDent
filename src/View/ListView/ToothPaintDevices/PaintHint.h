#pragma once
#include <array>

enum class ToothHint {none, normal, root, extr, extr_m, impl, impl_m,};
enum class SurfaceHint {none, normal, red, blue, green};
enum class EndoHint {none, red, darkred, blue, green};
enum class PostHint {none, blue, green};
enum class ProsthoHint {none, crown, crown_green, br_b, br_m, br_e, br_b_green, br_m_green, br_e_green};
struct PaintHint
{
	int idx;
	bool temp;
	int num;

	ToothHint tooth;
	std::array<SurfaceHint, 5> surfaces;
	EndoHint endo;
	PostHint post;
	ProsthoHint prostho;

	bool lesion;
	bool dns;
	bool perio;
	bool frac;
	int mobility;
	 
};