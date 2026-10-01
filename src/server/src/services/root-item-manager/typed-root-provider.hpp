#pragma once
#include "command/preference-schema.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

template <TypedPreferences P> class TypedRootProvider : public RootProvider {
public:
  PreferenceList preferences() const override { return describePreferences<P>(); }
  void preferencesChanged(const PreferenceValues &values) override {
    preferencesChanged(readPreferences<P>(values));
  }
  void initialized(const PreferenceValues &values) override { initialized(readPreferences<P>(values)); }

  virtual void preferencesChanged(const P &) {}
  virtual void initialized(const P &) {}
};
