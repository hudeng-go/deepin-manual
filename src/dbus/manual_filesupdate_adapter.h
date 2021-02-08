/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp com.deepin.Manual.FilesUpdate.xml -a manual_filesupdate_adapter -c ManualFilesUpdateAdapter
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef MANUAL_FILESUPDATE_ADAPTER_H
#define MANUAL_FILESUPDATE_ADAPTER_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface local.ManualFilesUpdateProxy
 */
class ManualFilesUpdateAdapter: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.ManualFilesUpdateProxy")
    Q_CLASSINFO("D-Bus Introspection", ""
                                       "  <interface name=\"local.ManualFilesUpdateProxy\">\n"
                                       "    <method name=\"OnFilesUpdate\">\n"
                                       "      <arg direction=\"in\" type=\"as\" name=\"list\"/>\n"
                                       "    </method>\n"
                                       "  </interface>\n"
                                       "")
public:
    explicit ManualFilesUpdateAdapter(QObject *parent);
    virtual ~ManualFilesUpdateAdapter();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void OnFilesUpdate(const QStringList &list);
Q_SIGNALS: // SIGNALS
};

#endif
