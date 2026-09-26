import QtQuick
import Vicinae

ViciImage {
    property url imageUrl
    source: DocumentIntegration.imageSource(imageUrl.toString())
    fillMode: Image.PreserveAspectFit
}
