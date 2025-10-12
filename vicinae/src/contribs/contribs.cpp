#include "contribs.hpp"

static std::vector<Contributor::Contributor> CONTRIB_LIST = {
	Contributor::Contributor{.login = "aurelien-brabant", .resource = ":contribs/aurelien-brabant", .contribs = 1145 },
Contributor::Contributor{.login = "cilginc", .resource = ":contribs/cilginc", .contribs = 40 },
Contributor::Contributor{.login = "quadratech188", .resource = ":contribs/quadratech188", .contribs = 25 },
Contributor::Contributor{.login = "dagimg-dot", .resource = ":contribs/dagimg-dot", .contribs = 24 },
Contributor::Contributor{.login = "Damnjelly", .resource = ":contribs/Damnjelly", .contribs = 11 },
Contributor::Contributor{.login = "ThatOneCalculator", .resource = ":contribs/ThatOneCalculator", .contribs = 8 },
Contributor::Contributor{.login = "elliotnash", .resource = ":contribs/elliotnash", .contribs = 6 },
Contributor::Contributor{.login = "schromp", .resource = ":contribs/schromp", .contribs = 5 },
Contributor::Contributor{.login = "TomRomeo", .resource = ":contribs/TomRomeo", .contribs = 5 },
Contributor::Contributor{.login = "RolfKoenders", .resource = ":contribs/RolfKoenders", .contribs = 4 },
Contributor::Contributor{.login = "juliogc", .resource = ":contribs/juliogc", .contribs = 2 },
Contributor::Contributor{.login = "LuizSSampaio", .resource = ":contribs/LuizSSampaio", .contribs = 2 },
Contributor::Contributor{.login = "koutselakismanos", .resource = ":contribs/koutselakismanos", .contribs = 2 },
Contributor::Contributor{.login = "falood", .resource = ":contribs/falood", .contribs = 2 },
Contributor::Contributor{.login = "ArjixWasTaken", .resource = ":contribs/ArjixWasTaken", .contribs = 1 },
Contributor::Contributor{.login = "bendi-github", .resource = ":contribs/bendi-github", .contribs = 1 },
Contributor::Contributor{.login = "cluther", .resource = ":contribs/cluther", .contribs = 1 },
Contributor::Contributor{.login = "chmanie", .resource = ":contribs/chmanie", .contribs = 1 },
Contributor::Contributor{.login = "Kricheldorf", .resource = ":contribs/Kricheldorf", .contribs = 1 },
Contributor::Contributor{.login = "soymadip", .resource = ":contribs/soymadip", .contribs = 1 },
Contributor::Contributor{.login = "UnknownCalculatedVariable", .resource = ":contribs/UnknownCalculatedVariable", .contribs = 1 },
Contributor::Contributor{.login = "dawnofmidnight", .resource = ":contribs/dawnofmidnight", .contribs = 1 },
Contributor::Contributor{.login = "dawsers", .resource = ":contribs/dawsers", .contribs = 1 },
Contributor::Contributor{.login = "kaan-w", .resource = ":contribs/kaan-w", .contribs = 1 }
};

std::vector<Contributor::Contributor> Contributor::getList() { return CONTRIB_LIST; };