#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <iostream>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include "numen-currency-provider.hpp"
#include "numen/abstract-currency-provider.hpp"
#include "vicinae.hpp"

namespace fs = std::filesystem;

namespace {
constexpr auto CACHE_TTL = std::chrono::minutes{30};

fs::path persistPath() { return Omnicast::cacheDir() / "numen-rates.json"; }

std::string normalizeCurrencyId(std::string_view id) {
  std::string s{id};
  std::ranges::transform(id, s.begin(), [](char c) { return std::tolower(c); });
  return s;
}

} // namespace

std::optional<numen::ExchangeRate> NumenVicinaeCurrencyProvider::getRate(std::string_view code) const {
  if (auto it = m_rates.find(std::string{code}); it != m_rates.end()) {
    return numen::ExchangeRate{.rate = it->second};
  }

  // we use $<ticker> as disambiguation for crypto tickers
  // for instance "sol" maps to the Peruvian sol, while
  // $SOL would map to the solana crypto token.
  if (code.starts_with("$")) return getRate(code.substr(1));

  return std::nullopt;
}

bool NumenVicinaeCurrencyProvider::loadRates(const NumenVicinaeCurrencyData &data) {
  for (const auto &[symbol, rate] : data.fiat.rates) {
    m_rates[normalizeCurrencyId(symbol)] = rate;
  }

  for (const auto &[symbol, rate] : data.crypto.prices) {
    m_rates[normalizeCurrencyId(symbol)] = 1 / rate;
  }

  return true;
}

void NumenVicinaeCurrencyProvider::updateRates() { fetchRates(); }

void NumenVicinaeCurrencyProvider::fetchRates() {
  std::error_code ec;
  auto path = persistPath();

  if (!m_lastFetchedAt && fs::is_regular_file(path, ec)) {
    if (std::chrono::file_clock::now() - fs::last_write_time(path) < CACHE_TTL) {
      NumenVicinaeCurrencyData data;
      std::ifstream ifs{path};
      std::stringstream buffer;
      buffer << ifs.rdbuf();

      if (auto res = glz::read_json(data, buffer.str()); !res) { loadRates(data); }

      return;
    }
  }

  if (m_lastFetchedAt && std::chrono::system_clock::now() - *m_lastFetchedAt < CACHE_TTL) return;

  auto watcher = new http::Client::Watcher<http::Client::Result<NumenVicinaeCurrencyData>>(nullptr);

  watcher->setFuture(m_client.get<NumenVicinaeCurrencyData>("/currencies"));

  QObject::connect(watcher, &QFutureWatcherBase::finished, [this, watcher]() {
    watcher->deleteLater();
    if (watcher->isCanceled()) return;
    if (auto r = watcher->result()) {
      if (loadRates(r.value())) {
        m_lastFetchedAt = std::chrono::system_clock::now();
        persistOnDisk(persistPath(), r.value());
      }
    } else {
      qWarning() << "Failed to fetch rates from the vicinae API" << r.error();
    }
  });
}

void NumenVicinaeCurrencyProvider::persistOnDisk(const std::filesystem::path &path,
                                                 const NumenVicinaeCurrencyData &data) {
  std::string buf{};

  if (auto res = glz::write_file_json(data, path.string(), buf)) {
    qWarning() << "Failed to persist exchange rates to" << path << glz::format_error(res);
  }
}
