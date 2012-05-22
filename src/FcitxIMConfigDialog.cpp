#include <fcitx/module/dbus/dbusstuff.h>
#include <fcitx/module/ipc/ipc.h>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <KComboBox>

#include "FcitxIMConfigDialog.h"
#include "ConfigDescManager.h"
#include "FcitxConfigPage.h"

Fcitx::FcitxIMConfigDialog::FcitxIMConfigDialog(const QString& imName, const FcitxAddon* addon, QWidget* parent): KDialog(parent)
    ,m_connection(QDBusConnection::sessionBus())
    ,m_imName(imName)
    ,m_keyboard(0)
    ,m_layoutCombobox(0)
    ,m_configPage(0)
{
    m_keyboard = new org::fcitx::Fcitx::Keyboard(
        QString("%1-%2").arg(FCITX_DBUS_SERVICE).arg(fcitx_utils_get_display_number()),
        "/keyboard",
        m_connection,
        this
    );

    QWidget* widget = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(this);
    widget->setLayout(l);

    if (!imName.startsWith("fcitx-keyboard")) {
        QDBusPendingReply< FcitxLayoutList > layoutList = m_keyboard->GetLayouts();
        {
            QEventLoop loop;
            QDBusPendingCallWatcher watcher(layoutList);
            loop.connect(&watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(quit()));
            loop.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
        }

        m_keyboard->GetLayoutForIM(imName);

        if (!layoutList.isError()) {
            m_layoutList = layoutList.value();
            m_layoutCombobox = new KComboBox(this);

            QDBusPendingReply< QString, QString > res = m_keyboard->GetLayoutForIM(imName);
            {
                QEventLoop loop;
                QDBusPendingCallWatcher watcher(res);
                loop.connect(&watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(quit()));
                loop.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
            }
            QString imLayout = qdbus_cast<QString>(res.argumentAt(0));
            QString imVariant =  qdbus_cast<QString>(res.argumentAt(1));

            QLabel* label;
            label = new QLabel("<b>Keyboard Layout:</b>");

            int idx = 1;
            int select = 0;
            m_layoutCombobox->addItem(i18n("Default"));

            foreach (const FcitxLayout& layout, layoutList.value()) {
                if (imLayout == layout.layout() && imVariant == layout.variant())
                    select = idx;
                m_layoutCombobox->addItem(layout.name());
                idx ++;
            }
            m_layoutCombobox->setCurrentIndex(select);

            l->addWidget(label);
            l->addWidget(m_layoutCombobox);
            label = new QLabel("<b>Input Method Setting:</b>");
            l->addWidget(label);
        }
    }

    FcitxConfigFileDesc* cfdesc = ConfigDescManager::instance()->GetConfigDesc(QString::fromUtf8(addon->name).append(".desc"));

    if (cfdesc ||  strlen(addon->subconfig) != 0) {
        m_configPage = new FcitxConfigPage(
            this,
            cfdesc,
            QString::fromUtf8("conf"),
            QString::fromUtf8(addon->name).append(".config") ,
            QString::fromUtf8(addon->subconfig)
        );
        l->addWidget(m_configPage);
        connect(this, SIGNAL(buttonClicked(KDialog::ButtonCode)), this, SLOT(onButtonClicked(KDialog::ButtonCode)));
    }
    setWindowIcon(KIcon("fcitx"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Default);
    setMainWidget(widget);
}

void Fcitx::FcitxIMConfigDialog::onButtonClicked(KDialog::ButtonCode code)
{
    if (m_layoutCombobox) {
        if (code == KDialog::Ok) {
            int idx = m_layoutCombobox->currentIndex();
            if (idx == 0)
                m_keyboard->SetLayoutForIM(m_imName, "", "");
            else
                m_keyboard->SetLayoutForIM(m_imName, m_layoutList.at(idx - 1).layout(), m_layoutList.at(idx - 1).variant());
        }
        else if (code == KDialog::Default)
            m_layoutCombobox->setCurrentIndex(0);
    }

    if (m_configPage)
        m_configPage->buttonClicked(code);
}
