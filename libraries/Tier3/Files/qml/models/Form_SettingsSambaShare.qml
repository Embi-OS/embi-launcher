import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Files

FormObjectModel {
    id: root

    FormTextFieldDelegate {
        id: hostName
        label: qsTr("Nom d'hôte")
        placeholder: "//<server>/<share>"
        validator: StringValidator {
            pattern: /^\/\/[ -~]+(\/[ -~]+)+$/
        }
        targetProperty: "hostName"
    }
    FormTextFieldDelegate {
        label: qsTr("Port")
        validator: SocketPortValidator{}
        targetProperty: "port"
    }
    FormTextFieldPathDelegate {
        label: qsTr("Chemin de montage")
        placeholder: ("/mnt/smb/%1").arg(name.currentText || name.placeholder)
        infos: qsTr("Peut être laissé vide")
        targetProperty: "mountPath"
    }
    FormTextFieldDelegate {
        id: name
        label: qsTr("Nom")
        placeholder: hostName.currentText.replace(/\//g, "_")
        infos: qsTr("Peut être laissé vide")
        targetProperty: "name"
    }
    FormSwitchDelegate {
        label: qsTr("Nofail")
        infos: qsTr("Permet au système de démarrer même si la connexion échoue")
        targetProperty: "nofail"
    }
    FormComboBoxDelegate {
        label: qsTr("Version")
        options: StandardObjectModel {
            StandardObject {text: "SMB 1.0"; value: SMBVersions.Vers_1_0}
            StandardObject {text: "SMB 2.0"; value: SMBVersions.Vers_2_0}
            StandardObject {text: "SMB 2.1"; value: SMBVersions.Vers_2_1}
            StandardObject {text: "SMB 3.0"; value: SMBVersions.Vers_3_0}
        }
        textRole: "text"
        valueRole: "value"
        targetProperty: "smbVersion"
    }
    FormComboBoxDelegate {
        id: identification
        label: qsTr("Identification")
        options: StandardObjectModel {
            StandardObject {text: qsTr("Mot de passe"); value: SMBIdentifications.Password}
            StandardObject {text: qsTr("Invité"); value: SMBIdentifications.Guest}
            StandardObject {text: qsTr("Anonyme"); value: SMBIdentifications.Anonym}
        }
        textRole: "text"
        valueRole: "value"
        targetProperty: "smbIdentification"
    }
    FormTextFieldDelegate {
        visible: identification.currentValue===SMBIdentifications.Password
        label: qsTr("Nom d'utilisateur")
        targetProperty: "userName"
    }
    FormTextFieldPasswordDelegate {
        visible: identification.currentValue===SMBIdentifications.Password
        label: qsTr("Mot de passe")
        targetProperty: "password"
    }
    FormTextFieldDelegate {
        visible: identification.currentValue===SMBIdentifications.Password
        label: qsTr("Domaine")
        targetProperty: "domain"
    }
    FormTextFieldDelegate {
        label: qsTr("Options Avancées")
        infos: qsTr("Liste d'options de montage séparée par des virgules")
        targetProperty: "advancedOptions"
    }
    FormInfoDelegate {
        label: qsTr("Crédences")
        targetProperty: "credentials"
    }
}
