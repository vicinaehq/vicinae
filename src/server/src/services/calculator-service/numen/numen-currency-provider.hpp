#pragma once
#include "environment.hpp"
#include "http-client.hpp"
#include "numen/abstract-currency-provider.hpp"
#include <unordered_map>

class NumenVicinaeCurrencyProvider : public AbstractCurrencyProvider {
public:
  std::optional<double> getRate(const std::string &code) const override;
  void updateRates() override;

  NumenVicinaeCurrencyProvider() { m_client.setBaseUrl(Environment::vicinaeApiBaseUrl()); }

private:
  struct CurrencyData {
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

  void fetchRates();
  bool loadRates(const CurrencyData &data);
  void persistOnDisk(const std::filesystem::path &path, const CurrencyData &data);

  http::Client m_client;
  std::unordered_map<std::string, double> m_rates;
  std::optional<std::chrono::time_point<std::chrono::system_clock>> m_lastFetchedAt;
};
