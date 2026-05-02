#pragma once
#include "section-source.hpp"
#include <filesystem>
#include <functional>

class SearchFilesSection : public SectionSource {
public:
  void setFiles(std::vector<std::filesystem::path> files, const QString &sectionName);

  QString sectionName() const override { return m_sectionName; }
  int count() const override { return static_cast<int>(m_files.size()); }

  const std::filesystem::path &fileAt(int i) const { return m_files.at(i); }

  void setOnFileSelected(std::function<void(const std::filesystem::path &)> cb) {
    m_onFileSelected = std::move(cb);
  }

  void onSelected(int i) override {
    if (m_onFileSelected) m_onFileSelected(m_files.at(i));
  }

protected:
  QString itemTitle(int i) const override;
  QString itemIconSource(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::vector<std::filesystem::path> m_files;
  QString m_sectionName;
  std::function<void(const std::filesystem::path &)> m_onFileSelected;
};
