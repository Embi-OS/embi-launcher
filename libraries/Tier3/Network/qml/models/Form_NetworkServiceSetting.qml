import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Network

FormObjectModel {
    id: root

    required property NetworkSettingsService networkService
    readonly property NetworkSettingsIPv4 ipv4: networkService.ipv4
    readonly property NetworkSettingsIPv6 ipv6: networkService.ipv6
    readonly property NetworkSettingsProxy proxy: networkService.proxy
    readonly property NetworkSettingsWireless wirelessConfig: networkService.wirelessConfig

    FormInfoDelegate {
        label: qsTr("Nom")
        target: root.networkService
        targetProperty: "name"
    }
    FormInfoDelegate {
        label: qsTr("Adresse IPv4")
        target: root.ipv4
        targetProperty: "address"
    }
    FormInfoDelegate {
        label: qsTr("Adresse IPv6")
        target: root.ipv6
        targetProperty: "address"
    }

    SeparatorTreeDelegate {}

    FormSwitchDelegate {
        enabled: root.networkService.type===NetworkSettingsType.Wifi
        label: qsTr("Connexion automatique")
        target: root.networkService
        targetProperty: "autoConnect"
    }

    FormNestDelegate {
        text: qsTr("IPv4")

        FormComboBoxDelegate {
            id: methodIPv4
            label: qsTr("Méthode IPv4")
            options: StandardObjectModel {
                StandardObject {text: qsTr("Automatique (DHCP)"); value: NetworkSettingsIPv4.Dhcp }
                StandardObject {text: qsTr("Manuel"); value: NetworkSettingsIPv4.Manual }
                StandardObject {text: qsTr("Désactiver"); value: NetworkSettingsIPv4.Off }
            }
            textRole: "text"
            valueRole: "value"
            target: root.ipv4
            targetProperty: "method"
        }
        FormTextFieldDelegate {
            visible: methodIPv4.currentValue===NetworkSettingsIPv4.Manual
            label: qsTr("Adresse")
            validator: Ipv4Validator {}
            target: root.ipv4
            targetProperty: "address"
        }
        FormTextFieldDelegate {
            visible: methodIPv4.currentValue===NetworkSettingsIPv4.Manual
            label: qsTr("Masque de réseau")
            validator: Ipv4Validator {}
            target: root.ipv4
            targetProperty: "mask"
        }
        FormTextFieldDelegate {
            visible: methodIPv4.currentValue===NetworkSettingsIPv4.Manual
            label: qsTr("Passerelle")
            validator: Ipv4Validator {}
            target: root.ipv4
            targetProperty: "gateway"
        }
    }
}
