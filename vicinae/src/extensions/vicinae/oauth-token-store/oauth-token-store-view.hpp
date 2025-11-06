#pragma once
#include "ui/views/list-view.hpp"

class OAuthTokenStoreView : public SearchableListView {
public:
  OAuthTokenStoreView();
  Data initData() const override;
  QString sectionName() const override { return "OAuth Token Sets ({count})"; }
  QString initialSearchPlaceholderText() const override;
};
