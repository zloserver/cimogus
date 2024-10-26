package com.zloserver.vpn

import android.Manifest.permission
import android.annotation.SuppressLint
import android.app.Notification
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import androidx.core.app.NotificationChannelCompat.Builder
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationCompat.Action
import androidx.core.app.NotificationManagerCompat
import androidx.core.content.ContextCompat
import com.zloserver.vpn.protocol.ProtocolState
import com.zloserver.vpn.protocol.ProtocolState.CONNECTED
import com.zloserver.vpn.protocol.ProtocolState.DISCONNECTED
import com.zloserver.vpn.util.Log
import com.zloserver.vpn.util.net.TrafficStats.TrafficData

private const val TAG = "ServiceNotification"

private const val OLD_NOTIFICATION_CHANNEL_ID: String = "com.zloserver.vpn.notification"
private const val NOTIFICATION_CHANNEL_ID: String = "com.zloserver.vpn.notifications"
const val NOTIFICATION_ID = 1337

private const val GET_ACTIVITY_REQUEST_CODE = 0
private const val CONNECT_REQUEST_CODE = 1
private const val DISCONNECT_REQUEST_CODE = 2

class ServiceNotification(private val context: Context) {

    private val upDownSymbols = when (Build.BRAND) {
        "Infinix" -> '˅' to '˄'
        else -> '↓' to '↑'
    }

    private val notificationManager = NotificationManagerCompat.from(context)

    private val notificationBuilder = NotificationCompat.Builder(context, NOTIFICATION_CHANNEL_ID)
        .setShowWhen(false)
        .setOngoing(true)
        .setPriority(NotificationCompat.PRIORITY_DEFAULT)
        .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
        .setForegroundServiceBehavior(NotificationCompat.FOREGROUND_SERVICE_IMMEDIATE)
        .setCategory(NotificationCompat.CATEGORY_SERVICE)
        .setContentIntent(
            PendingIntent.getActivity(
                context,
                GET_ACTIVITY_REQUEST_CODE,
                Intent(context, AmneziaActivity::class.java),
                PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
            )
        )

    private val zeroSpeed: String = with(TrafficData.ZERO) {
        formatSpeedString(rxString, txString)
    }

    fun buildNotification(serverName: String?, protocol: String?, state: ProtocolState): Notification {
        val speedString = if (state == CONNECTED) zeroSpeed else null

        Log.d(TAG, "Build notification: $serverName, $state")

        return notificationBuilder
            .setSmallIcon(R.drawable.ic_status)
            .setContentTitle((serverName ?: "ZloVPN") + (protocol?.let { " $it" } ?: ""))
            .setContentText(context.getString(state))
            .setSubText(speedString)
            .setWhen(System.currentTimeMillis())
            .clearActions()
            .apply {
                getAction(state)?.let {
                    addAction(it)
                }
            }
            .build()
    }

    private fun buildNotification(speed: TrafficData): Notification =
        notificationBuilder
            .setWhen(System.currentTimeMillis())
            .setSubText(getSpeedString(speed))
            .build()

    fun isNotificationEnabled(): Boolean {
        if (!context.isNotificationPermissionGranted()) return false
        if (!notificationManager.areNotificationsEnabled()) return false
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            return notificationManager.getNotificationChannel(NOTIFICATION_CHANNEL_ID)
                ?.let { it.importance != NotificationManager.IMPORTANCE_NONE } ?: true
        }
        return true
    }

    @SuppressLint("MissingPermission")
    fun updateNotification(serverName: String?, protocol: String?, state: ProtocolState) {
        if (context.isNotificationPermissionGranted()) {
            Log.d(TAG, "Update notification: $serverName, $state")
            notificationManager.notify(NOTIFICATION_ID, buildNotification(serverName, protocol, state))
        }
    }

    @SuppressLint("MissingPermission")
    fun updateSpeed(speed: TrafficData) {
        if (context.isNotificationPermissionGranted()) {
            notificationManager.notify(NOTIFICATION_ID, buildNotification(speed))
        }
    }

    private fun getSpeedString(traffic: TrafficData) =
        if (traffic == TrafficData.ZERO) zeroSpeed
        else formatSpeedString(traffic.rxString, traffic.txString)

    private fun formatSpeedString(rx: String, tx: String) = with(upDownSymbols) { "$first $rx  $second $tx" }

    private fun getAction(state: ProtocolState): Action? {
        return when (state) {
            CONNECTED -> {
                Action(
                    0, context.getString(R.string.disconnect),
                    PendingIntent.getBroadcast(
                        context,
                        DISCONNECT_REQUEST_CODE,
                        Intent(ACTION_DISCONNECT).apply {
                            setPackage(context.packageName)
                        },
                        PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
                    )
                )
            }

            DISCONNECTED -> {
                Action(
                    0, context.getString(R.string.connect),
                    PendingIntent.getBroadcast(
                        context,
                        CONNECT_REQUEST_CODE,
                        Intent(ACTION_CONNECT).apply {
                            setPackage(context.packageName)
                        },
                        PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
                    )
                )
            }

            else -> null
        }
    }

    companion object {
        fun createNotificationChannel(context: Context) {
            with(NotificationManagerCompat.from(context)) {
                deleteNotificationChannel(OLD_NOTIFICATION_CHANNEL_ID)
                createNotificationChannel(
                    Builder(NOTIFICATION_CHANNEL_ID, NotificationManagerCompat.IMPORTANCE_DEFAULT)
                        .setShowBadge(false)
                        .setSound(null, null)
                        .setVibrationEnabled(false)
                        .setLightsEnabled(false)
                        .setName("ZloVPN")
                        .setDescription(context.resources.getString(R.string.notificationChannelDescription))
                        .build()
                )
            }
        }
    }
}

fun Context.isNotificationPermissionGranted(): Boolean =
    Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU ||
        ContextCompat.checkSelfPermission(this, permission.POST_NOTIFICATIONS) ==
        PackageManager.PERMISSION_GRANTED