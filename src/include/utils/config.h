/**
 * @file
 * @brief
 * @details
 *
 * @date 19.04.2020 (2020-04-19) created by Nick Egorrov
 * @author Nick Egorrov
 * @copyright Copyright (C) 2019  Nick Egorrov
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

#ifndef CONFIG_H
#define CONFIG_H

#include <QList>
#include <QLocale>
#include <QSettings>
#include <QString>
#include <QVariant>


class IGroupControl
{
public:
    virtual void push() = 0;
    virtual void pop() = 0;
    virtual void reset() = 0;
};

class Config: public IGroupControl
{
    template<typename T>friend  class ConfigItem;

public:
    Config(QSettings *settings, const QString &group = QString(""));
    Config(Config *parentNode, const QString &group = QString(""));
    virtual ~Config();

    void push();
    void pop();
    void reset();

    void begin();
    void end();

    void sync()
    {
        m_storage->sync();
    }

    template<typename T>
    inline void setValue(const QString &key, const T &value)
    {
        m_storage->setValue(key, value);
    }

    template<typename T>
    inline T value(const QString &key, const T &defaultValue) const
    {
        /*
         * It's a difficult moment for my understanding.
         * Since the template member depends on the template parameter, we must tell
         * the compiler that it's a template.
         * https://exceptionshub.com/c-template-compilation-error-expected-primary-expression-before-token.html
         */
        return m_storage->value(key, defaultValue).template value<T>();
    }

    /* Special case : QLocale stored as string */
    inline void setValue(const QString &key, const QLocale &value)
    {
        m_storage->setValue(key, QVariant(value.name()));
    }

    inline QLocale value(const QString &key, const QLocale &defaultValue) const
    {
        return QLocale(m_storage->value(key, defaultValue.name()).toString());
    }

protected:
    void add(IGroupControl *item);

    Config *m_parentNode;
    QSettings *m_storage;
    QString m_groupName;
    QList<IGroupControl *> m_items;
};

template<typename T>
class ConfigItem: public IGroupControl
{
protected:
    Config *m_config;
    QString m_name;
    bool m_groupControl;
    T m_data;
    T m_pushData;
    T m_default;

public:
    explicit ConfigItem<T>(Config *set, const QString &name, const T &defaulValue,
                           bool groupControl = true)
    {
        m_config = set;
        m_name = name;
        m_groupControl = groupControl;
        m_default = defaulValue;
        m_data = m_default;
        m_config->add(this);
        load();
    }

    void operator=(const T &data)
    {
        set(data);
    }

    T &operator()()
    {
        return get();
    }

protected:
    void set(const T &data)
    {
        if (m_data == data) {
            return;
        }

        m_data = data;
        save();
    }

    T &get()
    {
        return m_data;
    }

    virtual void load()
    {
        m_config->begin();
        m_data = m_config->value(m_name, m_default);
        m_config->end();
    }

    virtual void save()
    {
        m_config->begin();
        m_config->setValue(m_name, m_data);
        m_config->end();
    }

    void push()
    {
        if (!m_groupControl) {
            return;
        }

        m_pushData = m_data;
    }

    void pop()
    {
        if (!m_groupControl) {
            return;
        }

        set(m_pushData);
    }

    void reset()
    {
        if (!m_groupControl) {
            return;
        }

        set(m_default);
    }
};

#define _item_type(t) typedef ConfigItem<t> Cfg##t

_item_type(QBitArray);
_item_type(bool);
_item_type(QByteArray);
_item_type(QChar);
_item_type(QDate);
_item_type(QDateTime);
_item_type(double);
_item_type(QEasingCurve);
_item_type(float);
//_item_type(QHash<QString, QVariant>);
_item_type(int);
_item_type(QJsonArray);
_item_type(QJsonDocument);
_item_type(QJsonObject);
_item_type(QJsonValue);
_item_type(QLine);
_item_type(QLineF);
//_item_type(QList<QVariant>);
_item_type(QLocale);
_item_type(qlonglong);
//_item_type(QMap<QString, QVariant>);
_item_type(QModelIndex);
_item_type(QPersistentModelIndex);
_item_type(QPoint);
_item_type(QPointF);
_item_type(qreal);
_item_type(QRect);
_item_type(QRectF);
_item_type(QRegExp);
_item_type(QRegularExpression);
_item_type(QSize);
_item_type(QSizeF);
_item_type(QString);
_item_type(QStringList);
_item_type(QTime);
_item_type(uint);
_item_type(qulonglong);
_item_type(QUrl);
_item_type(QUuid);

#undef _item_type

#endif // CONFIG_H
