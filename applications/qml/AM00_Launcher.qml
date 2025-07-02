pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.System
import L00_Launcher

Page {
    id: root

    background: Background {}

    header: LauncherStatusBar {
        onAboutClicked: AxionHelper.showAbout()
        onPowerClicked: powerPopup.open()
        onLangChanged: AxionHelper.warningRestart()
        onMenuClicked: drawer.open()
    }

    contentItem: SettingsLauncherView {

    }

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants globaux
//──────────────────────────────────────────────────────────────────────

    BasicDrawer {
        id: drawer
        edge: Qt.TopEdge
        width: root.width
        height: root.height-root.header.height
        interactive: false
        contentItem: PaneSplitView {
            id: pane
            padding: 10
            // drawFrame: true
            proxyModel.enabled: pane.visible
            active: drawer.position>=1.0
            headerPadding: 10
            footerPadding: 10
            drawFooterSeparator: true
            model: LauncherSettingsModel {}

            footer: RowContainer {
                topInset: pane.footerPadding
                bottomInset: pane.footerPadding
                leftInset: pane.footerPadding
                rightInset: pane.footerPadding

                ClickableIcon {
                    icon: MaterialIcons.close
                    color: Style.colorWhite
                    size: 32
                    onClicked: drawer.close()
                }

                LabelWithCaption {
                    Layout.fillWidth: true
                    horizontalAlignment: Qt.AlignRight
                    textFont: Style.textTheme.body1
                    text: qsTr("Adresse IPv4 Ethernet: %1").arg(NetworkSettingsManager.currentWiredConnection?.ipv4.address ?? "N/A")
                    captionFont: Style.textTheme.body1
                    caption: qsTr("Adresse IPv4 Wifi: %1").arg(NetworkSettingsManager.currentWifiConnection?.ipv4.address ?? "N/A")
                }
            }
        }
    }

    DialogPower {
        id: powerPopup
        quitVisible: false
    }

    LoaderDialogManager {
        anchors.fill: parent
    }

    LoaderSnackbarManager {
        visible: root.visible && !isEmpty
        position: ItemPositions.Top
    }
}
