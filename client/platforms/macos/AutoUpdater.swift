import Foundation
import Sparkle

class UpdaterDelegate: NSObject, SPUUpdaterDelegate {
  let feedUrl: String

  public init(_ feedUrl: String) {
    self.feedUrl = feedUrl
  }

  public func feedURLString(for updater: SPUUpdater) -> String? {
    return self.feedUrl
  }
}

public class MacAutoUpdater {
  let updaterController: SPUStandardUpdaterController
  let updaterDelegate: UpdaterDelegate

  public init(feedUrl: String) {
    self.updaterDelegate = UpdaterDelegate(feedUrl)
    self.updaterController = SPUStandardUpdaterController(startingUpdater: true, updaterDelegate: self.updaterDelegate, userDriverDelegate: nil)
  }

  public func checkForUpdates() {
    self.updaterController.checkForUpdates(self)
  }

  public func canCheckForUpdates() -> Bool {
    self.updaterController.updater.canCheckForUpdates
  }
}
