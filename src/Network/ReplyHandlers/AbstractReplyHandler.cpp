#include "AbstractReplyHandler.h"
#include "TinyXML/tinyxml.h"
#include "QDebug"
std::string AbstractReplyHandler::getTextNullCheck(const TiXmlElement* e)
{
    auto ptr = e->GetText();

        if ( ptr == nullptr) {
            return "";
        }

        return ptr;

}
