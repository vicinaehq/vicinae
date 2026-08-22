#pragma once
#include "environment.hpp"
#include "http-client.hpp"
#include "numen/abstract-currency-provider.hpp"
#include <unordered_map>

struct NumenVicinaeCurrencyData {
  std::string base;
  struct {
    std::string updatedAt;
    std::unordered_map<std::string, double> rates;
  } fiat;
  struct {
    std::string updatedAt;
    std::unordered_map<std::string, double> prices;
  } crypto;
};

class NumenVicinaeCurrencyProvider : public numen::AbstractCurrencyProvider {
public:
  std::optional<numen::ExchangeRate> getRate(const std::string_view code) const override;
  void updateRates() override;

  NumenVicinaeCurrencyProvider() { m_client.setBaseUrl(Environment::vicinaeApiBaseUrl()); }

private:
  void fetchRates();
  bool loadRates(const NumenVicinaeCurrencyData &data);
  void persistOnDisk(const std::filesystem::path &path, const NumenVicinaeCurrencyData &data);

  http::Client m_client;
  std::unordered_map<std::string, double> m_rates;
  std::optional<std::chrono::time_point<std::chrono::system_clock>> m_lastFetchedAt;
};
