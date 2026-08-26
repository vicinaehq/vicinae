import Foundation
import SoulverCore

private struct SoulverResult: Codable {
  let value: String
  let type: String
  var error: String? = nil
}

nonisolated(unsafe) private var globalCalculator: Calculator?

private func encode(_ result: SoulverResult) -> UnsafeMutablePointer<CChar>? {
  guard let data = try? JSONEncoder().encode(result),
    let json = String(data: data, encoding: .utf8)
  else { return nil }
  return strdup(json)
}

@_cdecl("soulver_initialize")
public func soulver_initialize(_ resourcesPath: UnsafePointer<CChar>) -> Bool {
  guard globalCalculator == nil else { return true }

  var customization = EngineCustomization.standard
  let currencyProvider = RaycastCurrencyProvider()
  customization.currencyRateProvider = currencyProvider
  currencyProvider.startUpdating()
  globalCalculator = Calculator(customization: customization)

  return true
}

@_cdecl("soulver_is_initialized")
public func soulver_is_initialized() -> Bool {
  return globalCalculator != nil
}

@_cdecl("soulver_evaluate")
public func soulver_evaluate(_ expression: UnsafePointer<CChar>) -> UnsafeMutablePointer<CChar>? {
  guard let calculator = globalCalculator else { return nil }

  let swiftExpression = String(cString: expression)

  if swiftExpression.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty {
    return encode(SoulverResult(value: "", type: "none"))
  }

  let result = calculator.calculate(swiftExpression)

  if result.isEmptyResult {
    return encode(SoulverResult(value: "", type: "none"))
  }

  return encode(
    SoulverResult(
      value: result.stringValue,
      type: formatResult(result: result.evaluationResult, customization: calculator.customization)))
}
