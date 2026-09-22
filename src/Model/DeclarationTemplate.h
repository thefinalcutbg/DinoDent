#pragma once
#include <string>

struct DeclarationTemplate
{
	long long rowid = 0;
	std::string header;
	std::string subheader;
	std::string body;
	std::string footer;

	static constexpr const char* nametag = "<ТРИ ИМЕНА>";
	static constexpr const char* idtag = "<ЕГН/ЛНЧ>";
	static constexpr const char* addresstag = "<АДРЕС>";
	static constexpr const char* phonetag = "<ТЕЛЕФОН>";
	static constexpr const char* emailtag = "<EMAIL>";
	static constexpr const char* birthtag = "<ДАТА НА РАЖДАНЕ>";
	static constexpr const char* currentdatetag = "<ДНЕШНА ДАТА>";
	static constexpr const char* doctorNametag = "<ЛДМ>";
	static constexpr const char* dentistLPKtag = "<УИН>";
	static constexpr const char* dentistPhonetag = "<ТЕЛЕФОН ЛДМ>";
	static constexpr const char* dentistSpecialtytag = "<СПЕЦИАЛНОСТ ЛДМ>";
};