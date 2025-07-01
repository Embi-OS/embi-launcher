pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.System
import L00_Launcher

Item {
    id: root

    Background {
        anchors.fill: parent
    }

    DialogPower {
        id: powerPopup
        quitVisible: false
    }

    LauncherStatusBar {
        id: barStatus
        ExtraAnchors.topRightCorner: parent
        edge: Qt.RightEdge | Qt.TopEdge

        onAboutClicked: AxionHelper.showAbout()
        onPowerClicked: powerPopup.open()
        onLangChanged: AxionHelper.warningRestart()
    }

    PaneSplitView {
        anchors.top: barStatus.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 32
        drawFrame: true
        model: LauncherSettingsModel {}
    }

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants globaux
//──────────────────────────────────────────────────────────────────────

    LoaderDialogManager {
        anchors.fill: parent
    }

    LoaderSnackbarManager {
        visible: root.visible && !isEmpty
        position: ItemPositions.Top
    }
}
