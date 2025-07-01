import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

PaneTreeView {
    id: root

    title: "U-Boot"

    model: proxyModel

    header: RowContainer {
        topPadding: root.headerPadding
        leftPadding: root.headerPadding
        rightPadding: root.headerPadding
        alignment: Qt.AlignTop

        LabelWithCaption {
            Layout.fillWidth: true
            text: root.title
            textFont: Style.textTheme.title1
            caption: root.comment
            captionFont: Style.textTheme.subtitle1
        }

        BasicBusyIndicator {
            Layout.alignment: Qt.AlignVCenter
            visible: sshController.processing
            size: Style.textTheme.title1.pixelSize
        }
    }

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    SystemServiceController {
        id: sshController
        service: DeviceInfo.isBoot2Qt ? "dropbear.socket" : "ssh.service"
    }

    StandardObjectModel {
        id: treeModel
        FormSwitchDelegate {
            editable: root.editable && sshController.serviceExists
            label: qsTr("Chargé")
            leftLabel: qsTr("Off")
            rightLabel: qsTr("On")
            value: sshController.loaded
            onAccepted: (value) => {
                if(value)
                    sshController.enable()
                else
                    sshController.disable()
            }
        }
        FormSwitchDelegate {
            editable: root.editable && sshController.serviceExists
            enabled: sshController.loaded
            label: qsTr("Actif")
            leftLabel: qsTr("Off")
            rightLabel: qsTr("On")
            value: sshController.active
            onAccepted: (value) => {
                if(value)
                    sshController.start()
                else
                    sshController.stop()
            }
        }
        SeparatorTreeDelegate {}
        FormButtonDelegate {
            label: "Refresh status"
            icon: MaterialIcons.refresh
            onClicked: sshController.refreshStatus()
        }
        SeparatorTreeDelegate {}
        LabelDelegate {
            font: Style.textTheme.subtitle1
            text: sshController.status
        }
    }
}
