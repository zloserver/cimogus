set_target_properties(${PROJECT} PROPERTIES
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE Manual
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Developer ID Application"
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "8NJ4QZPB6B"
    XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--deep --timestamp"
    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "YES"
)
target_sources(${PROJECT} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/ZloVPN.entitlements)