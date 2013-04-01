#include "shortcut_selector.hpp"
#include "actions.hpp"

#include <QTimer>


ShortcutSelector::ShortcutSelector(Actions *actions, QWidget *parent)
    : QPushButton(parent)
    , mActions(0)
    , mShortcutTimer(new QTimer(this))
{
    init();
    setActions(actions);
}

ShortcutSelector::ShortcutSelector(QWidget *parent)
    : QPushButton(parent)
    , mActions(0)
    , mShortcutTimer(new QTimer(this))
{
    init();
}

void ShortcutSelector::init()
{
    setCheckable(true);

    mShortcutTimer->setInterval(1000);
    mShortcutTimer->setSingleShot(false);

    connect(this, SIGNAL(clicked()), this, SLOT(grabShortcut()));

    connect(mShortcutTimer, SIGNAL(timeout()), this, SLOT(shortcutTimer_timeout()));
}

void ShortcutSelector::setActions(Actions *actions)
{
    if (mActions)
    {
        return;
    }
    mActions = actions;
    connect(mActions, SIGNAL(grabShortcutCancelled()), this, SLOT(grabShortcut_fail()));
    connect(mActions, SIGNAL(grabShortcutTimedout()), this, SLOT(grabShortcut_fail()));
    connect(mActions, SIGNAL(grabShortcutFailed()), this, SLOT(grabShortcut_fail()));
    connect(mActions, SIGNAL(shortcutGrabbed(QString)), this, SLOT(newShortcutGrabbed(QString)));
}

void ShortcutSelector::grabShortcut(int timeout)
{
    if (!mActions)
    {
        return;
    }

    if (!isChecked())
    {
        mActions->cancelShortutGrab();
        return;
    }

    mTimeoutCounter = timeout;
    mOldShortcut = text();
    setText(QString::number(mTimeoutCounter));
    mShortcutTimer->start();
    mActions->grabShortcut(mTimeoutCounter * mShortcutTimer->interval());
}

void ShortcutSelector::shortcutTimer_timeout()
{
    --mTimeoutCounter;
    setText(QString::number(mTimeoutCounter));
    if (!mTimeoutCounter)
    {
        setChecked(false);
        mShortcutTimer->stop();
    }
}

void ShortcutSelector::grabShortcut_fail()
{
    setChecked(false);
    mShortcutTimer->stop();
    setText(mOldShortcut);
    emit shortcutGrabbed(mOldShortcut);
}

void ShortcutSelector::newShortcutGrabbed(const QString &newShortcut)
{
    setChecked(false);
    mShortcutTimer->stop();
//    setText(newShortcut);
    setText(QString());
    emit shortcutGrabbed(newShortcut);
}
