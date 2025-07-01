import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

PaneTreeView {
    id: root

    title: qsTr("Launcher")

    model: proxyModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    StandardObjectModel {
        id: treeModel
    }
}
