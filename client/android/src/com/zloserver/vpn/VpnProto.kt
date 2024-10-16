package com.zloserver.vpn

import com.zloserver.vpn.protocol.Protocol
import com.zloserver.vpn.protocol.awg.Awg
import com.zloserver.vpn.protocol.cloak.Cloak
import com.zloserver.vpn.protocol.openvpn.OpenVpn
import com.zloserver.vpn.protocol.wireguard.Wireguard
import com.zloserver.vpn.protocol.xray.Xray

enum class VpnProto(
    val label: String,
    val processName: String,
    val serviceClass: Class<out AmneziaVpnService>
) {
    WIREGUARD(
        "WireGuard",
        "com.zloserver.vpn:amneziaAwgService",
        AwgService::class.java
    ) {
        override fun createProtocol(): Protocol = Wireguard()
    },

    AWG(
        "AmneziaWG",
        "com.zloserver.vpn:amneziaAwgService",
        AwgService::class.java
    ) {
        override fun createProtocol(): Protocol = Awg()
    },

    OPENVPN(
        "OpenVPN",
        "com.zloserver.vpn:amneziaOpenVpnService",
        OpenVpnService::class.java
    ) {
        override fun createProtocol(): Protocol = OpenVpn()
    },

    CLOAK(
        "Cloak",
        "com.zloserver.vpn:amneziaOpenVpnService",
        OpenVpnService::class.java
    ) {
        override fun createProtocol(): Protocol = Cloak()
    },

    XRAY(
        "XRay",
        "com.zloserver.vpn:amneziaXrayService",
        XrayService::class.java
    ) {
        override fun createProtocol(): Protocol = Xray.instance
    },

    SSXRAY(
        "SSXRay",
        "com.zloserver.vpn:amneziaXrayService",
        XrayService::class.java
    ) {
        override fun createProtocol(): Protocol = Xray.instance
    };

    private var _protocol: Protocol? = null
    val protocol: Protocol
        get() {
            if (_protocol == null) _protocol = createProtocol()
            return _protocol ?: throw AssertionError("Set to null by another thread")
        }

    protected abstract fun createProtocol(): Protocol

    companion object {
        fun get(protocolName: String): VpnProto = VpnProto.valueOf(protocolName.uppercase())
    }
}