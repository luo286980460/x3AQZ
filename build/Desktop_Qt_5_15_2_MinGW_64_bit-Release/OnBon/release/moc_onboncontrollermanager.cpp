/****************************************************************************
** Meta object code from reading C++ file 'onboncontrollermanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../OnBon/onboncontrollermanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'onboncontrollermanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OnbonControllerManager_t {
    QByteArrayData data[33];
    char stringdata0[460];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OnbonControllerManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OnbonControllerManager_t qt_meta_stringdata_OnbonControllerManager = {
    {
QT_MOC_LITERAL(0, 0, 22), // "OnbonControllerManager"
QT_MOC_LITERAL(1, 23, 7), // "showMsg"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 3), // "msg"
QT_MOC_LITERAL(4, 36, 25), // "signalGetControllerIpList"
QT_MOC_LITERAL(5, 62, 5), // "times"
QT_MOC_LITERAL(6, 68, 7), // "forTest"
QT_MOC_LITERAL(7, 76, 2), // "ip"
QT_MOC_LITERAL(8, 79, 10), // "signalInit"
QT_MOC_LITERAL(9, 90, 28), // "signalUpdateControllerIpList"
QT_MOC_LITERAL(10, 119, 16), // "controllerIpList"
QT_MOC_LITERAL(11, 136, 26), // "signalGetControllerDetails"
QT_MOC_LITERAL(12, 163, 12), // "controllerIp"
QT_MOC_LITERAL(13, 176, 14), // "controllerType"
QT_MOC_LITERAL(14, 191, 26), // "signalSetControllerDetails"
QT_MOC_LITERAL(15, 218, 7), // "details"
QT_MOC_LITERAL(16, 226, 29), // "signalSetControllerConfigFile"
QT_MOC_LITERAL(17, 256, 10), // "configInfo"
QT_MOC_LITERAL(18, 267, 22), // "signalClearDynamicArea"
QT_MOC_LITERAL(19, 290, 3), // "pIP"
QT_MOC_LITERAL(20, 294, 21), // "signalSendDynamicArea"
QT_MOC_LITERAL(21, 316, 5), // "color"
QT_MOC_LITERAL(22, 322, 7), // "uAreaId"
QT_MOC_LITERAL(23, 330, 6), // "uAreaX"
QT_MOC_LITERAL(24, 337, 6), // "uAreaY"
QT_MOC_LITERAL(25, 344, 6), // "uWidth"
QT_MOC_LITERAL(26, 351, 7), // "uHeight"
QT_MOC_LITERAL(27, 359, 9), // "nFontSize"
QT_MOC_LITERAL(28, 369, 17), // "strAreaTxtContent"
QT_MOC_LITERAL(29, 387, 19), // "signalClearPrograma"
QT_MOC_LITERAL(30, 407, 23), // "signalAddDefaultProgram"
QT_MOC_LITERAL(31, 431, 10), // "programNum"
QT_MOC_LITERAL(32, 442, 17) // "signalLockProgram"

    },
    "OnbonControllerManager\0showMsg\0\0msg\0"
    "signalGetControllerIpList\0times\0forTest\0"
    "ip\0signalInit\0signalUpdateControllerIpList\0"
    "controllerIpList\0signalGetControllerDetails\0"
    "controllerIp\0controllerType\0"
    "signalSetControllerDetails\0details\0"
    "signalSetControllerConfigFile\0configInfo\0"
    "signalClearDynamicArea\0pIP\0"
    "signalSendDynamicArea\0color\0uAreaId\0"
    "uAreaX\0uAreaY\0uWidth\0uHeight\0nFontSize\0"
    "strAreaTxtContent\0signalClearPrograma\0"
    "signalAddDefaultProgram\0programNum\0"
    "signalLockProgram"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OnbonControllerManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    1,   82,    2, 0x06 /* Public */,
       6,    1,   85,    2, 0x06 /* Public */,
       8,    0,   88,    2, 0x06 /* Public */,
       9,    1,   89,    2, 0x06 /* Public */,
      11,    2,   92,    2, 0x06 /* Public */,
      14,    1,   97,    2, 0x06 /* Public */,
      16,    1,  100,    2, 0x06 /* Public */,
      18,    1,  103,    2, 0x06 /* Public */,
      20,    9,  106,    2, 0x06 /* Public */,
      29,    1,  125,    2, 0x06 /* Public */,
      30,    3,  128,    2, 0x06 /* Public */,
      32,    2,  135,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   12,   13,
    QMetaType::Void, QMetaType::QJsonObject,   15,
    QMetaType::Void, QMetaType::QJsonObject,   17,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString,   19,   21,   22,   23,   24,   25,   26,   27,   28,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString,   31,    7,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    7,   31,

       0        // eod
};

void OnbonControllerManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OnbonControllerManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->signalGetControllerIpList((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->forTest((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->signalInit(); break;
        case 4: _t->signalUpdateControllerIpList((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 5: _t->signalGetControllerDetails((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->signalSetControllerDetails((*reinterpret_cast< QJsonObject(*)>(_a[1]))); break;
        case 7: _t->signalSetControllerConfigFile((*reinterpret_cast< QJsonObject(*)>(_a[1]))); break;
        case 8: _t->signalClearDynamicArea((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->signalSendDynamicArea((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8])),(*reinterpret_cast< QString(*)>(_a[9]))); break;
        case 10: _t->signalClearPrograma((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->signalAddDefaultProgram((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 12: _t->signalLockProgram((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OnbonControllerManager::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::showMsg)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalGetControllerIpList)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::forTest)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalInit)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QStringList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalUpdateControllerIpList)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalGetControllerDetails)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QJsonObject );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalSetControllerDetails)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QJsonObject );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalSetControllerConfigFile)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalClearDynamicArea)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QString , int , int , int , int , int , int , int , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalSendDynamicArea)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalClearPrograma)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(int , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalAddDefaultProgram)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManager::*)(QString , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManager::signalLockProgram)) {
                *result = 12;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject OnbonControllerManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_OnbonControllerManager.data,
    qt_meta_data_OnbonControllerManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OnbonControllerManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OnbonControllerManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OnbonControllerManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OnbonControllerManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void OnbonControllerManager::showMsg(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OnbonControllerManager::signalGetControllerIpList(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OnbonControllerManager::forTest(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void OnbonControllerManager::signalInit()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void OnbonControllerManager::signalUpdateControllerIpList(QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void OnbonControllerManager::signalGetControllerDetails(QString _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void OnbonControllerManager::signalSetControllerDetails(QJsonObject _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void OnbonControllerManager::signalSetControllerConfigFile(QJsonObject _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void OnbonControllerManager::signalClearDynamicArea(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void OnbonControllerManager::signalSendDynamicArea(QString _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, int _t8, QString _t9)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t9))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void OnbonControllerManager::signalClearPrograma(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void OnbonControllerManager::signalAddDefaultProgram(int _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void OnbonControllerManager::signalLockProgram(QString _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
