/**
 * @file
 * @brief The ConfigDialog class is used to manage settings.
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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QSharedPointer>


class QStackedLayout;
class QTreeWidgetItem;
class ConfigPage;

namespace Ui {
class ConfigDialog;
}

/**
 * @brief The ConfigDialog class, in conjunction with ConfigPage, is used to manage settings.
 *
 * ConfigDialog has a TreeView widget for selecting a page, a StackViev widget containing page
 * widgets and three buttons: "OK", "Cancel" and "Default". Pages are listed in the order they
 * were added.
 */
class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);

    explicit ConfigDialog(QSharedPointer<ConfigPage> page, QWidget *parent = 0);

    explicit ConfigDialog(QList<QSharedPointer<ConfigPage>> pages, QWidget *parent = 0);

    ~ConfigDialog();

    void addPage(QSharedPointer<ConfigPage> page);

    void addPages(QList<QSharedPointer<ConfigPage>> pages);

    /* wrap to emit signal "started" */
    void open();
    /* wrap to emit signal "started" */
    int  exec();

protected slots:
    /**
     * @brief Sends onReset() signal to active page.
     *
     * Typically this slot activated from the “default” button.
     */
    virtual void reset();

public slots:
    /**
     * @brief Update translation of UI.
     *
     * Also, sends onRetranslateUI() to all pages.
     */
    void retranslateUI();

signals:
    /**
     * @brief Sended before displaying dialog.
     */
    void onStart();

    /**
     * @brief Sended while reset() function called.
     */
    void onReset();

    /**
     * @brief Sended while retranslateUI() function called.
     */
    void onRetranslateUI();

protected:
    Ui::ConfigDialog *mForm;
    QList<QSharedPointer<ConfigPage>> mPages;
    QStackedLayout *mStackedLayout;
    QHash<QTreeWidgetItem *, QSharedPointer<ConfigPage>> mItemPageMap;

    QTreeWidgetItem *doItem(QSharedPointer<ConfigPage> pagePtr);

protected slots:
    void selectedItemChange(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};

#endif // CONFIGDIALOG_H
