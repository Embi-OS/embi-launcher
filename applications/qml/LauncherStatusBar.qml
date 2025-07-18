pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Files
import Eco.Tier3.System

LauncherInfosBar {
    id: root

    readonly property LocaleFilterModel langModel: LocaleFilterModel {
        filters: [
            "C",
            "fr_CH",
            "de_CH",
            "it_CH",
            "en_GB"
        ]
    }

    signal aboutClicked()
    signal powerClicked()
    signal langChanged(string newLang)

    function changeLanguage(language: string) {
        if(!LocaleSettings.setLocale(language))
            return;
        root.langChanged(language);
    }

    TapHandler {
        parent: root.rightLayout
        target: root.rightLayout
        cursorShape: Qt.PointingHandCursor
        onTapped: menu.open()
    }

    BasicMenu {
        id: menu
        ExtraPosition.position: ItemPositions.BottomRight
        BasicMenu {
            id: diskMenu
            title: qsTr("Disques / USB")
            onOpened: Filesystem.drives.markDirty()

            Repeater {
                model: ProxyModel {
                    delayed: true
                    enabled: diskMenu.opened
                    sourceModel: Filesystem.drives
                }
                delegate: BasicMenuItem {
                    required property FilesystemDrive qtObject
                    required property string name
                    required property string driveDevice
                    required property string fileUrl
                    hint: qsTr("Ejecter")
                    text: ("%1 (%2)").arg(name).arg(driveDevice)
                    icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl)
                    onTriggered: Filesystem.drives.eject(qtObject)
                }
            }
        }
        BasicMenu {
            id: langMenu
            title: qsTr("Langue")
            width: 200

            Repeater {
                model: ProxyModel {
                    delayed: true
                    enabled: langMenu.opened
                    sourceModel: root.langModel
                }
                delegate: BasicMenuItem {
                    required property string name
                    outlined: name===root.language
                    text: name
                    onTriggered: root.changeLanguage(text)
                }
            }
        }
        BasicMenuSeparator {}
        BasicMenuItem {icon.source: MaterialIcons.informationOutline; onTriggered: root.aboutClicked(); text: qsTr("A propos")}
        BasicMenuItem {icon.source: MaterialIcons.power; onTriggered: root.powerClicked(); text: qsTr("Eteindre / Redémarrer")}
    }
}
