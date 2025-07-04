import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

PaneTreeView {
    id: root

    title: "U-Boot"

    model: proxyModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    UBootEnvModel {
        id: ubootEnvs
    }

    StandardObjectModel {
        id: treeModel
        FormComboBoxDelegate {
            editable: root.editable
            visible: UBootSettings.canSetEnv
            label: "fdtfile"
            infos: "Main device-tree"
            options: FolderTreeModel {
                path: "/boot"
                nameFilters: ["*.dtb"]
                showDirs: false
            }
            textRole: "text"
            mandatory: false

            value: UBootSettings.printEnv("fdtfile")
            onAccepted: (value) => {
                if(value!=='undefined')
                    UBootSettings.setEnv("fdtfile", value);
                else
                    UBootSettings.clearEnv("fdtfile");
                ubootEnvs.select();
                AxionHelper.warningReboot();
            }
        }
        SeparatorTreeDelegate {}
        FormButtonDelegate {
            label: "Refresh envs"
            icon: MaterialIcons.refresh
            onClicked: ubootEnvs.select()
        }
        SeparatorTreeDelegate {}
        property Instantiator instantiator: Instantiator {
            model: ubootEnvs
            delegate: InfoTreeDelegate {
                required property string key
                required value
                text: key
                info: value
            }
            onObjectAdded: (index, object) => treeModel.append(object)
            onObjectRemoved: (index, object) => treeModel.remove(object)
        }
    }
}
