#pragma once

enum class ToothType { Molar, Premolar, Frontal };
enum class Surface { Occlusal, Medial, Distal, Buccal, Lingual, Cervical };
enum class Quadrant { First, Second, Third, Fourth };
enum class BridgePos { Begin, Middle, End };
enum class BridgeAppearance { Middle, Terminal, None }; // DO NOT CHANGE THE ORDER IN ANY CIRUMSTANCES!!! (casting from bridgePos)
enum class StatusCode { Temporary, Obturation, Caries, Pulpitis, ApicalLesion, EndoTreatment, Root, Fracture, Extraction, Periodontitis, Mobility1, Mobility2, Mobility3, Crown, Bridge, Implant, Dsn, };
enum class Degree { First, Second, Third };