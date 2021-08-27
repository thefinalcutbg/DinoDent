#pragma once

enum class ToothType { Molar, Premolar, Frontal };

enum class Quadrant { First, Second, Third, Fourth };
enum class BridgePos { Begin, Middle, End };

constexpr int statusCount = 19;

namespace StatusCode
{
	enum StatusCode { Temporary, Obturation, Caries, Pulpitis, ApicalLesion, EndoTreatment, Post, Root, Fracture, Extraction, Periodontitis, Mobility1, Mobility2, Mobility3, Crown, Bridge, Implant, Dsn, Impacted};
}

constexpr int surfaceCount = 6;

namespace Surface
{
	enum Surface { Occlusal, Medial, Distal, Buccal, Lingual, Cervical };
}


enum class Degree { First, Second, Third };