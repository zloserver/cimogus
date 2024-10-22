import CxxStdlib
import ServiceManagement

var service = SMAppService.daemon(plistName: "ZloVPN-service.plist")

public func firstSetupNeeded() -> Bool {
  do {
    try service.register()
  } catch let err as NSError {
    if err.code == kSMErrorAlreadyRegistered && service.status == .enabled {
      return false
    }
    return true
  }

  return service.status != .enabled
}

public struct FirstSetupResponse {
  public let isError: Bool
  public let requiresApproval: Bool
  public let errorString: std.string
}

public func doFirstSetup() -> FirstSetupResponse {
  do {
    try service.register()
  } catch let err as NSError {
    if err.code == kSMErrorAlreadyRegistered {
      return FirstSetupResponse(isError: false, requiresApproval: false, errorString: "")
    }

    if service.status == .requiresApproval {
      SMAppService.openSystemSettingsLoginItems()
    }

    return FirstSetupResponse(isError: true, requiresApproval: service.status == .requiresApproval, errorString: std.string(err.localizedDescription))
  }

  return FirstSetupResponse(isError: false, requiresApproval: false, errorString: "")
}

public func restartService() {
  if service.status == .enabled {
    _ = try? service.unregister()
  }

  _ = try? service.register()
}
