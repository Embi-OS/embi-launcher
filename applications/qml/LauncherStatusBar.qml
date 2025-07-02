pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Files
import Eco.Tier3.System

RowContainer {
    id: root

    spacing: 20
    radius: 10
    font: Style.barInfoFont

    readonly property string time: DateTimeUtils.formatTime(DateTimeUtils.systemDateTime, Locale.ShortFormat)
    readonly property string date: DateTimeUtils.formatDate(DateTimeUtils.systemDateTime)

    readonly property string language: LocaleSettings.locale
    readonly property LocaleFilterModel langModel: LocaleFilterModel {
        filters: [
            "C",
            "fr_CH",
            "de_CH",
            "it_CH",
            "en_GB"
        ]
    }

    signal menuClicked()
    signal aboutClicked()
    signal powerClicked()
    signal langChanged(string newLang)

    function changeLanguage(language: string) {
        if(!LocaleSettings.setLocale(language))
            return;
        root.langChanged(language);
    }

    ClickableIcon {
        icon: MaterialIcons.menu
        color: Style.colorWhite
        size: 32
        onClicked: root.menuClicked()
    }

    BasicLabel {
        font: Style.textTheme.title1
        text: root.time
    }

    BasicLabel {
        font: Style.textTheme.headline6
        text: root.date
    }

    LayoutSpring {}

    RowLayout {
        id: rightLayout
        FpsIndicator {
            showMinimum: false
            showMaximum: false
            showJitter: false
        }

        BasicBusyIndicator {
            visible: Filesystem.drives.processing
            size: 24
            lineWidth: 0
        }

        SvgColorImage {
            visible: !Filesystem.drives.isEmpty
            icon: MaterialIcons.usb
            color: Style.colorWhite
            size: 24
        }

        SvgColorImage {
            visible: NetworkSettingsManager.currentWifiConnection?.connected ?? false
            readonly property int signalStrength: NetworkSettingsManager.currentWifiConnection?.wirelessConfig?.signalStrength ?? 0
            icon: signalStrength < 0 ? MaterialIcons.wifiStrengthOffOutline :
                  signalStrength <= 10 ? MaterialIcons.wifiStrengthOutline :
                  signalStrength <= 30 ? MaterialIcons.wifiStrength1 :
                  signalStrength <= 55 ? MaterialIcons.wifiStrength2 :
                  signalStrength <= 80 ? MaterialIcons.wifiStrength3 :
                  signalStrength <= 100 ? MaterialIcons.wifiStrength4 : MaterialIcons.wifiStrengthOffOutline
            color: Style.colorWhite
            size: 24
        }

        SvgColorImage {
            visible: NetworkSettingsManager.currentWiredConnection?.connected ?? false
            icon: MaterialIcons.ethernet
            color: Style.colorWhite
            size: 24
        }

        BasicLabel {
            font: root.font
            text: root.language
        }

        Item {
            Layout.fillHeight: true
            implicitWidth: 10
        }

        BasicLabel {
            font: Style.textTheme.title1
            text: "Embi OS"
        }

        SvgColorImage {
            icon: Style.icon
            color: Style.colorWhite
            size: 32
        }
    }

    TapHandler {
        parent: rightLayout
        // margin: rightLayout.padding
        target: rightLayout
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
