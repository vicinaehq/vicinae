#include "contribs.hpp"

static std::vector<Contributor::Contributor> CONTRIB_LIST = {
	Contributor::Contributor{.login = "aurelien-brabant", .resource = ":contribs/aurelien-brabant", .contribs = 916 },
Contributor::Contributor{.login = "cilginc", .resource = ":contribs/cilginc", .contribs = 39 },
Contributor::Contributor{.login = "dagimg-dot", .resource = ":contribs/dagimg-dot", .contribs = 19 },
Contributor::Contributor{.login = "quadratech188", .resource = ":contribs/quadratech188", .contribs = 19 },
Contributor::Contributor{.login = "Damnjelly", .resource = ":contribs/Damnjelly", .contribs = 9 },
Contributor::Contributor{.login = "ThatOneCalculator", .resource = ":contribs/ThatOneCalculator", .contribs = 8 },
Contributor::Contributor{.login = "elliotnash", .resource = ":contribs/elliotnash", .contribs = 6 },
Contributor::Contributor{.login = "RolfKoenders", .resource = ":contribs/RolfKoenders", .contribs = 4 },
Contributor::Contributor{.login = "TomRomeo", .resource = ":contribs/TomRomeo", .contribs = 3 },
Contributor::Contributor{.login = "LuizSSampaio", .resource = ":contribs/LuizSSampaio", .contribs = 2 },
Contributor::Contributor{.login = "juliogc", .resource = ":contribs/juliogc", .contribs = 2 },
Contributor::Contributor{.login = "ArjixWasTaken", .resource = ":contribs/ArjixWasTaken", .contribs = 1 },
Contributor::Contributor{.login = "UnknownCalculatedVariable", .resource = ":contribs/UnknownCalculatedVariable", .contribs = 1 }
};

std::vector<Contributor::Contributor> Contributor::getList() { return CONTRIB_LIST; };