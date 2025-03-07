#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

enum class Alignment { LEFT, CENTER, RIGHT };

struct ColumnConfig {
  std::string name;
  int width = 0; // Auto-calculated if 0
  Alignment align = Alignment::LEFT;
};

class TableFormatter {
private:
  std::vector<ColumnConfig> columns;
  std::vector<std::vector<std::string>> rows;
  char horizontalChar = '-';
  char verticalChar = '|';
  char intersectionChar = '+';
  bool showHeader = true;
  bool showBorder = true;
  int padding = 1;

  // Calculate the actual width needed for each column
  void calculateColumnWidths() {
    // Initialize with header widths
    std::vector<int> calculatedWidths(columns.size(), 0);
    for (size_t i = 0; i < columns.size(); ++i) {
      calculatedWidths[i] = columns[i].name.length();
    }

    // Check data widths
    for (const auto &row : rows) {
      for (size_t i = 0; i < row.size() && i < columns.size(); ++i) {
        calculatedWidths[i] = std::max(calculatedWidths[i], static_cast<int>(row[i].length()));
      }
    }

    // Apply calculated widths where auto-width (0) was specified
    for (size_t i = 0; i < columns.size(); ++i) {
      if (columns[i].width == 0) { columns[i].width = calculatedWidths[i]; }
    }
  }

  // Format a cell according to alignment and width
  std::string formatCell(const std::string &content, int width, Alignment align) const {
    std::string paddingStr(padding, ' ');
    int contentWidth = width;
    std::string result;

    switch (align) {
    case Alignment::LEFT:
      result = content + std::string(contentWidth - content.length(), ' ');
      break;
    case Alignment::RIGHT:
      result = std::string(contentWidth - content.length(), ' ') + content;
      break;
    case Alignment::CENTER: {
      int leftPad = (contentWidth - content.length()) / 2;
      int rightPad = contentWidth - content.length() - leftPad;
      result = std::string(leftPad, ' ') + content + std::string(rightPad, ' ');
      break;
    }
    }

    return paddingStr + result + paddingStr;
  }

  // Draw a horizontal border line
  void drawHorizontalLine() const {
    if (!showBorder) return;

    std::cout << intersectionChar;
    for (const auto &col : columns) {
      std::cout << std::string(col.width + padding * 2, horizontalChar) << intersectionChar;
    }
    std::cout << std::endl;
  }

public:
  // Add a column definition
  void addColumn(const std::string &name, int width = 0, Alignment align = Alignment::LEFT) {
    columns.push_back({name, width, align});
  }

  // Add a row of data
  void addRow(const std::vector<std::string> &row) { rows.push_back(row); }

  // Set table style
  void setStyle(char horizontal = '-', char vertical = '|', char intersection = '+') {
    horizontalChar = horizontal;
    verticalChar = vertical;
    intersectionChar = intersection;
  }

  // Set padding (space between content and borders)
  void setPadding(int pad) { padding = pad; }

  // Toggle header visibility
  void setShowHeader(bool show) { showHeader = show; }

  // Toggle border visibility
  void setShowBorder(bool show) { showBorder = show; }

  // Render the table to stdout
  void render() {
    if (columns.empty()) return;

    calculateColumnWidths();

    // Draw top border
    drawHorizontalLine();

    // Draw header
    if (showHeader) {
      std::cout << verticalChar;
      for (size_t i = 0; i < columns.size(); ++i) {
        std::cout << formatCell(columns[i].name, columns[i].width, Alignment::CENTER) << verticalChar;
      }
      std::cout << std::endl;

      // Draw header separator
      drawHorizontalLine();
    }

    // Draw rows
    for (const auto &row : rows) {
      if (showBorder) std::cout << verticalChar;
      for (size_t i = 0; i < columns.size(); ++i) {
        std::string cellContent = (i < row.size()) ? row[i] : "";
        std::cout << formatCell(cellContent, columns[i].width, columns[i].align);
        if (showBorder) std::cout << verticalChar;
      }
      std::cout << std::endl;
    }

    // Draw bottom border
    drawHorizontalLine();
  }
};
