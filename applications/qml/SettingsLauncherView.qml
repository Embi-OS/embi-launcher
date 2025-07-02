import QtQuick
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System
import L00_Launcher

Item {
    id: root

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 40
        spacing: 40

        RowLayout {
            spacing: 20
            RawButton {
                implicitWidth: 300
                implicitHeight: 120
                spacing: 20
                text: "Launch"
                font: Style.textTheme.title1
                icon.source: MaterialIcons.rocketLaunch
                onClicked: Launcher.launch()
            }
            RawButton {
                implicitWidth: 300
                implicitHeight: 120
                spacing: 20
                text: "Install"
                font: Style.textTheme.title1
                icon.source: MaterialIcons.applicationImport
                onClicked: Launcher.install()
            }
        }

        BasicLabel {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: Version.aboutQt()
            wrapMode: Text.Wrap
        }
    }

    ColumnLayout {
        id: networkAddresses
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10

        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Qt.AlignHCenter
            font: Style.textTheme.subtitle1
            text: "Networks:"
            color: Style.colorWhiteFade
        }

        Repeater {
            model: ProxyModel {
                delayed: true
                sourceModel: NetworkSettingsManager.services
                filterRoleName: "connected"
                filterValue: true
            }
            delegate: BasicLabel {
                required property string address
                required property string iface
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignRight
                font: Style.textTheme.title2
                text: ("%1: %2").arg(iface).arg(address)
                color: Style.colorWhiteFade
            }
        }
    }
}
