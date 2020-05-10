/**
 * @file
 * @brief The ConfigPage class is used to manage settings.
 *
 * @date 04.05.2020 (2020-05-04) created by Nick Egorrov
 * @author Nick Egorrov
 * @copyright Copyright (C) 2020  Nick Egorrov
 * @copyright
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 * @copyright
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.*
 * @copyright
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H

#include <QObject>
#include <QSharedPointer>

class QString;
class QWidget;

/**
 * @brief The ConfigPage class is used in conjunction with ConfigDialog to manage settings.
 *
 * A minimal implementation of a class descendant must include the doWidget() and text()
 * functions, however, a user-friendly implementation needs the start(), accept(), and reject()
 * functions.
 *
 * @code
 * class CustomPage: public ConfigPage
 * {
 * public:
 *     const QString &text();
 *
 *     void start();
 *
 *     void accept();
 *
 *     reject();
 *
 * protected:
 *     QWidget *doWidget();
 * }
 * @endcode
 */
class ConfigPage: public QObject
{
    Q_OBJECT

public:
    virtual ~ConfigPage() {}

    QWidget *widget();

    /**
     * @brief Displayed page name.
     *
     * @return The string can be localized.
     */
    virtual const QString &text() = 0;

    /**
     * @brief Additional page tooltip.
     *
     * @return The string can be localized.
     */
    virtual const QString &toolTip();

    /**
     * @brief Returns list of childs pages.
     *
     * @see add(QSharedPointer<ConfigPage>) add(ConfigPage *)
     */
    QList<QSharedPointer<ConfigPage>> pages();

    /**
     * @brief Add child page.
     */
    void add(QSharedPointer<ConfigPage> child);

protected:
    /**
     * @brief Add child page.
     */
    void add(ConfigPage *child);

public slots:
    /**
     * @brief Update translation of UI.
     */
    virtual void retranslateUI() {}

    /**
     * @brief Actions when starting the settings dialog.
     *
     * For example, you can load the current settings into a widget. Also, you can make
     * a copy of the settings to @link reject() roll back@endlink changes made by the user.
     */
    virtual void start() {}

    /**
     * @brief Actions when confirm the changes made.
     *
     * This is happend by pressing the "OK" button.
     */
    virtual void accept() {}

    /**
     * @brief Actions when rejecting the changes made.
     *
     * This is happend by pressing the "Cancel" button. If you have @link start() backup@endlink
     * your settings, it's time to apply them.
     */
    virtual void reject() {}

    /**
     * @brief Actions when requesting default settings.
     *
     * This is happend by pressing the "Default" button. If you implement this feature,
     * override the hasReset() function so that it returns @c true.
     */
    virtual void reset() {}

public:
    /**
     * @brief Indicates whether the page has a reset() function.
     *
     * @return By default, it returns @c false.
     */
    virtual bool hasReset()
    {
        return false;
    }

protected:
    ConfigPage(QObject *parent = 0): QObject(parent) {}

    /**
     * @brief Make widget for this page.
     */
    virtual QWidget *doWidget();

    QWidget *mWidget = 0;
    QList<QSharedPointer<ConfigPage>> mChild;

private slots:
    void widgetDestroed(QObject *object);
};

#endif // CONFIGPAGE_H
