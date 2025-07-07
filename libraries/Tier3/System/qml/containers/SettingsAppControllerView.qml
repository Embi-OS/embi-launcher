import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

PaneTreeView {
    id: root

    title: qsTr("Gestion du controlleur d'application")

    model: proxyModel

    editable: applicationController.hasAppController

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    ApplicationController {
        id: applicationController
        onErrorOccurred: (error) => DialogManager.showError(error)
    }

    StandardObjectModel {
        id: treeModel
        InfoTreeDelegate {
            text: qsTr("Application par défaut")
            info: applicationController.currentDefault
        }
        SeparatorTreeDelegate {}
        FormButtonDelegate {
            enabled: root.editable
            highlighted: true
            label: qsTr("Mettre à jour")
            icon: MaterialIcons.update
            onClicked: applicationController.update()
        }
        FormButtonDelegate {
            enabled: root.editable
            label: qsTr("Changer l'application par défaut")
            icon: MaterialIcons.update
            onClicked: applicationController.makeDefault()
        }
        FormButtonDelegate {
            enabled: root.editable
            label: qsTr("Réinitialiser l'application par défaut")
            icon: MaterialIcons.update
            onClicked: applicationController.removeDefault()
        }
        FormButtonDelegate {
            enabled: root.editable
            label: qsTr("Lancer une application")
            icon: MaterialIcons.rocketLaunch
            onClicked: applicationController.launch()
        }
    }
}
