#pragma once
#include <objbase.h>

// Tolerates an existing COM init (S_FALSE) and a foreign apartment mode (RPC_E_CHANGED_MODE).
// Declare before any ComPtr so interfaces are released before the apartment closes.
class ScopedCom {
public:
  explicit ScopedCom(DWORD model = COINIT_APARTMENTTHREADED) {
    const HRESULT hr = CoInitializeEx(nullptr, model);
    m_owns = hr == S_OK || hr == S_FALSE;
  }
  ~ScopedCom() {
    if (m_owns) CoUninitialize();
  }
  ScopedCom(const ScopedCom &) = delete;
  ScopedCom &operator=(const ScopedCom &) = delete;

private:
  bool m_owns = false;
};
