#include "utils/utils.hpp"
#include <filesystem>
#include <qlocale.h>
#include <unordered_map>
#include <optional>
#include "relevancy-scorer.hpp"

// clang-format off
static const std::unordered_map<std::string, double> FILE_TYPE_WEIGHTS = {
	// Documents - high relevance
	{".txt", 1.0}, {".md", 1.0}, {".tex", 1.0},  {".doc", 1.0}, {".docx", 1.0}, 
	{".pdf", 1.0}, {".rtf", 1.0}, {".odt", 1.0}, {".csv", 1.0},
	{".ppt", 1.0}, {".pptx", 1.0}, {".xls", 1.0}, {".xlsx", 1.0},

	// OpenDocument (LibreOffice/OpenOffice)
	{"odt", 1.0},    
    {"ods", 1.0},    
    {"odp", 1.0},    
    {"odg", 1.0},    
    {"odf", 1.0},    
    {"odb", 1.0}, 

	// Code files - high relevance
	{".cpp", 0.9}, {".hpp", 0.9}, {".c", 0.9}, {".h", 0.9},
	{".py", 0.9}, {".js", 0.9}, {".ts", 0.9}, {".java", 0.9},
	{".rs", 0.9}, {".go", 0.9}, {".rb", 0.9}, {".php", 0.9},
	{".html", 0.9}, {".htm", 0.9}, {".css", 0.9}, {".php", 0.9},
	
	// Config files - medium-high relevance
	{".json", 0.8}, {".yaml", 0.8}, {".yml", 0.8}, {".xml", 0.8},
	{".ini", 0.8}, {".conf", 0.8}, {".config", 0.8},
	
	// Images - medium relevance
	{".jpg", 0.6}, {".jpeg", 0.6}, {".png", 0.6}, {".gif", 0.6},
	{".bmp", 0.6}, {".svg", 0.6}, {".webp", 0.6},
	
	// Archives - low relevance
	{".zip", 0.4}, {".tar", 0.4}, {".gz", 0.4}, {".rar", 0.4},
	{".7z", 0.4}, {".bz2", 0.4},
	
	// System/binary files - very low relevance
	{".so", 0.2}, {".dll", 0.2}, {".exe", 0.2}, {".bin", 0.2},
	{".obj", 0.2}, {".o", 0.2}, {".lib", 0.2}, {".a", 0.2}
};
// clang-format on

double RelevancyScorer::computeLocationMultiplier(const std::filesystem::path &path) {
  if (isInHomeDirectory(path)) { return 2.0; }

  return 1.0;
}

double RelevancyScorer::computeFileTypeMultiplier(const std::filesystem::path &path) {
  std::string extension = path.extension().string();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
  auto it = FILE_TYPE_WEIGHTS.find(extension);

  if (it != FILE_TYPE_WEIGHTS.end()) { return it->second; }

  // No extension files (often scripts or configs)
  if (extension.empty()) { return 0.8; }

  return 0.5; // Unknown file types get lower score
}

double RelevancyScorer::computeHiddenFileMultiplier(const std::filesystem::path &path) {
  if (!isHiddenPath(path)) { return 1.0; }

  return 0.3;
}

double RelevancyScorer::computePathDepthMultiplier(const std::filesystem::path &path) {
  size_t depth = std::distance(path.begin(), path.end());

  if (depth <= 3) return 1.0;
  if (depth <= 6) return 1.0 - (depth - 3) * 0.05; // small penalty for moderately long paths
  return 0.7;
}

double RelevancyScorer::computeRecencyMultiplier(std::filesystem::file_time_type lastModified) {
  auto now = std::filesystem::file_time_type::clock::now();
  auto duration = now - lastModified;
  auto days = std::chrono::duration_cast<std::chrono::days>(duration).count();

  if (days <= 7) return 1.3;
  if (days <= 30) return 1.1;
  if (days <= 90) return 1.0;
  if (days <= 365) return 0.9;

  return 0.8;
}

double RelevancyScorer::computeScore(const std::filesystem::path &path,
                                     std::optional<std::filesystem::file_time_type> lastModified) {
  double score = 1.0;

  // 1. Location-based scoring
  score *= computeLocationMultiplier(path);

  // 2. File type scoring
  score *= computeFileTypeMultiplier(path);

  // 3. Hidden file handling
  score *= computeHiddenFileMultiplier(path);

  // 4. Path depth penalty
  score *= computePathDepthMultiplier(path);

  // 5. Recency bonus (if modification time available)
  if (lastModified.has_value()) { score *= computeRecencyMultiplier(lastModified.value()); }

  return std::max(0.1, score); // Minimum score of 0.1
}
