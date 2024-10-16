plugins {
    id(libs.plugins.android.library.get().pluginId)
    id(libs.plugins.kotlin.android.get().pluginId)
}

kotlin {
    jvmToolchain(17)
}

android {
    namespace = "com.zloserver.vpn.protocol.awg"
}

dependencies {
    compileOnly(project(":utils"))
    compileOnly(project(":protocolApi"))
    implementation(project(":wireguard"))
}
