#include "QtScriptDialogProvider.h"

#include <QInputDialog>
#include <QMetaObject>
#include <qapplication.h>
#include "Core/Network/NetworkClient.h"
#include "Core/Utils/DesktopUtils.h"
#include "Core/Utils/CoreUtils.h"
#include "Core/CommonDefs.h"


QtScriptDialogProvider::QtScriptDialogProvider() {
    dialogResult_ = 0;
}

std::string QtScriptDialogProvider::askUserCaptcha(NetworkClient* nm, const std::string& url) {
    std::lock_guard<std::mutex> lk(mutex_);
    // TODO: Implement this
    DesktopUtils::ShellOpenUrl(url);
    return showDialog("Enter text from the image:", "");
}

std::string QtScriptDialogProvider::inputDialog(const std::string& text, const std::string& defaultValue) {
    std::lock_guard<std::mutex> lk(mutex_);
    return showDialog(text, defaultValue);

}

std::string QtScriptDialogProvider::showDialog(const std::string& text, const std::string& defaultValue) {
    // GUI objects must be created only in main thread
    QString defaultValueStr = U2Q(defaultValue);
    QString textStr = U2Q(text);
    QMetaObject::invokeMethod(this,"showDialogInMainThread", Qt::BlockingQueuedConnection, Q_ARG(QString, textStr),
        Q_ARG(QString, defaultValueStr));
    return value_;
}

void  QtScriptDialogProvider::showDialogInMainThread(QString text, QString defaultValue) {
    QInputDialog dlg;
    dlg.setTextValue(defaultValue);
    dlg.setLabelText(text);
    if (dlg.exec() == QDialog::Accepted) {
        this->value_ = Q2U(dlg.textValue());
    }
    else {
        this->value_ = std::string();
    }
}