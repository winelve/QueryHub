#include "T_BasePage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToolButton.h"
T_BasePage::T_BasePage(QWidget* parent)
    : ElaScrollPage(parent)
{
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=]() {
        if (!parent)
        {
            update();
        }
    });
}

T_BasePage::~T_BasePage()
{
}

void T_BasePage::createCustomWidget(QString desText)
{
    QWidget* customWidget = new QWidget(this);

    ElaToolButton* themeButton = new ElaToolButton(this);
    themeButton->setFixedSize(35, 35);
    themeButton->setIsTransparent(false);
    themeButton->setElaIcon(ElaIconType::MoonStars);
    connect(themeButton, &ElaToolButton::clicked, this, [=]() {
        eTheme->setThemeMode(eTheme->getThemeMode() == ElaThemeType::Light ? ElaThemeType::Dark : ElaThemeType::Light);
    });

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(themeButton);
    buttonLayout->addSpacing(15);

    ElaText* descText = new ElaText(this);
    descText->setText(desText);
    descText->setTextPixelSize(13);

    QVBoxLayout* topLayout = new QVBoxLayout(customWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addLayout(buttonLayout);
    topLayout->addSpacing(5);
    topLayout->addWidget(descText);
    setCustomWidget(customWidget);
}
