#pragma once
#include "mime-apps-list-file.hpp"

namespace xdgpp {

/**
 * Collect all the mimeapps.list files that could be found on the system, with respect
 * to their specification-defined precedence.
 * Files that are not present on the system will still be added to the list
 * as empty files, as required by the specification.
 */
std::vector<xdgpp::MimeAppsListFile> getAllMimeAppsLists();
}; // namespace xdgpp
