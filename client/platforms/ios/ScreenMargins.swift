import UIKit

public struct ScreenMargins {
    public let topMargin: Double
    public let bottomMargin: Double
    public let leftMargin: Double
    public let rightMargin: Double
}

public func getScreenMargins() -> ScreenMargins {
    let window = UIApplication.shared.keyWindows.first!

    return ScreenMargins(topMargin: window.safeAreaInsets.top, bottomMargin: window.safeAreaInsets.bottom, leftMargin: window.safeAreaInsets.left, rightMargin: window.safeAreaInsets.right)
}
