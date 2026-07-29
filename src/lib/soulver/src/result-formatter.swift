import Foundation
import SoulverCore

func formatResult(result: EvaluationResult, customization: EngineCustomization) -> String {
  switch result {
  case .unitExpression(let unitExpression):
    let unit = unitExpression.unit
    return customization.longFormNameFor(unitIdentifier: unit.identifier)?.capitalized ?? unit.symbol
  case .unit(let scUnit):
    return customization.longFormNameFor(unitIdentifier: scUnit.identifier)?.capitalized ?? scUnit.symbol
  case .customUnit(let customUnit):
    return customUnit.name
  case .unitRate:
    return "Rate"
  case .unitRange:
    return "Range"

  case .decimal:
    return "Number"
  case .percentage:
    return "Percentage"
  case .fraction:
    return "Fraction"
  case .scientificNotation:
    return "Scientific Notation"
  case .binary:
    return "Binary"
  case .octal:
    return "Octal"
  case .hex:
    return "Hexadecimal"
  case .multiplier:
    return "Multiplier"

  case .date:
    return "Date"
  case .datespan:
    return "Date Range"
  case .iso8601:
    return "Date"
  case .timestamp:
    return "Timestamp"
  case .timespan:
    return "Duration"
  case .laptime:
    return "Lap Time"
  case .frametime:
    return "Frame Time"
  case .pace:
    return "Pace"
  case .periodicity:
    return "Periodicity"
  case .cadence:
    return "Cadence"

  case .gpsCoordinates:
    return "Coordinates"
  case .degreesMinutesSeconds:
    return "DMS Angle"
  case .place, .dynamicPlace:
    return "Place"

  case .boolean:
    return "Boolean"
  case .resolution:
    return "Resolution"
  case .salesTax:
    return "Sales Tax"
  case .pitch:
    return "Pitch"
  case .substance:
    return "Substance"
  case .list:
    return "List"
  case .variable:
    return "Variable"
  case .statisticType:
    return "Statistic"

  case .addingContext(_, let toResult):
    return formatResult(result: toResult, customization: customization)
  case .rawString:
    return "Text"
  case .error:
    return "Error"
  case .pending:
    return "Pending"
  case .failed:
    return "Failed"
  case .none:
    return "Empty"

  case .decimalRate,
    .percentageRate,
    .customType:
    return String(describing: result.equivalentTokenType)

  @unknown default:
    return "Unknown"
  }
}
