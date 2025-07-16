/****************************************************************************
** Meta object code from reading C++ file 'onboncontrollermanagerworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../OnBon/onboncontrollermanagerworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'onboncontrollermanagerworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OnbonControllerManagerWorker_t {
    QByteArrayData data[33];
    char stringdata0[448];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OnbonControllerManagerWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OnbonControllerManagerWorker_t qt_meta_stringdata_OnbonControllerManagerWorker = {
    {
QT_MOC_LITERAL(0, 0, 28), // "OnbonControllerManagerWorker"
QT_MOC_LITERAL(1, 29, 7), // "showMsg"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 3), // "msg"
QT_MOC_LITERAL(4, 42, 28), // "signalUpdateControllerIpList"
QT_MOC_LITERAL(5, 71, 16), // "controllerIpList"
QT_MOC_LITERAL(6, 88, 26), // "signalSetControllerDetails"
QT_MOC_LITERAL(7, 115, 7), // "details"
QT_MOC_LITERAL(8, 123, 23), // "slotGetControllerIpList"
QT_MOC_LITERAL(9, 147, 5), // "times"
QT_MOC_LITERAL(10, 153, 7), // "forTest"
QT_MOC_LITERAL(11, 161, 2), // "ip"
QT_MOC_LITERAL(12, 164, 8), // "slotInit"
QT_MOC_LITERAL(13, 173, 24), // "slotGetControllerDetails"
QT_MOC_LITERAL(14, 198, 12), // "controllerIp"
QT_MOC_LITERAL(15, 211, 14), // "controllerType"
QT_MOC_LITERAL(16, 226, 27), // "slotSetControllerConfigFile"
QT_MOC_LITERAL(17, 254, 10), // "configInfo"
QT_MOC_LITERAL(18, 265, 20), // "slotClearDynamicArea"
QT_MOC_LITERAL(19, 286, 3), // "pIP"
QT_MOC_LITERAL(20, 290, 19), // "slotSendDynamicArea"
QT_MOC_LITERAL(21, 310, 5), // "color"
QT_MOC_LITERAL(22, 316, 7), // "uAreaId"
QT_MOC_LITERAL(23, 324, 6), // "uAreaX"
QT_MOC_LITERAL(24, 331, 6), // "uAreaY"
QT_MOC_LITERAL(25, 338, 6), // "uWidth"
QT_MOC_LITERAL(26, 345, 7), // "uHeight"
QT_MOC_LITERAL(27, 353, 9), // "nFontSize"
QT_MOC_LITERAL(28, 363, 17), // "strAreaTxtContent"
QT_MOC_LITERAL(29, 381, 17), // "slotClearPrograma"
QT_MOC_LITERAL(30, 399, 21), // "slotAddDefaultProgram"
QT_MOC_LITERAL(31, 421, 10), // "programNum"
QT_MOC_LITERAL(32, 432, 15) // "slotLockProgram"

    },
    "OnbonControllerManagerWorker\0showMsg\0"
    "\0msg\0signalUpdateControllerIpList\0"
    "controllerIpList\0signalSetControllerDetails\0"
    "details\0slotGetControllerIpList\0times\0"
    "forTest\0ip\0slotInit\0slotGetControllerDetails\0"
    "controllerIp\0controllerType\0"
    "slotSetControllerConfigFile\0configInfo\0"
    "slotClearDynamicArea\0pIP\0slotSendDynamicArea\0"
    "color\0uAreaId\0uAreaX\0uAreaY\0uWidth\0"
    "uHeight\0nFontSize\0strAreaTxtContent\0"
    "slotClearPrograma\0slotAddDefaultProgram\0"
    "programNum\0slotLockProgram"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OnbonControllerManagerWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    1,   82,    2, 0x06 /* Public */,
       6,    1,   85,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   88,    2, 0x0a /* Public */,
      10,    1,   91,    2, 0x0a /* Public */,
      12,    0,   94,    2, 0x0a /* Public */,
      13,    2,   95,    2, 0x0a /* Public */,
      16,    1,  100,    2, 0x0a /* Public */,
      18,    1,  103,    2, 0x0a /* Public */,
      20,    9,  106,    2, 0x0a /* Public */,
      29,    1,  125,    2, 0x0a /* Public */,
      30,    3,  128,    2, 0x0a /* Public */,
      32,    2,  135,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QStringList,    5,
    QMetaType::Void, QMetaType::QJsonObject,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   14,   15,
    QMetaType::Void, QMetaType::QJsonObject,   17,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString,   19,   21,   22,   23,   24,   25,   26,   27,   28,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString,   31,   11,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   11,   31,

       0        // eod
};

void OnbonControllerManagerWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OnbonControllerManagerWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->signalUpdateControllerIpList((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 2: _t->signalSetControllerDetails((*reinterpret_cast< QJsonObject(*)>(_a[1]))); break;
        case 3: _t->slotGetControllerIpList((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->forTest((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->slotInit(); break;
        case 6: _t->slotGetControllerDetails((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: _t->slotSetControllerConfigFile((*reinterpret_cast< QJsonObject(*)>(_a[1]))); break;
        case 8: _t->slotClearDynamicArea((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->slotSendDynamicArea((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8])),(*reinterpret_cast< QString(*)>(_a[9]))); break;
        case 10: _t->slotClearPrograma((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->slotAddDefaultProgram((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 12: _t->slotLockProgram((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OnbonControllerManagerWorker::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManagerWorker::showMsg)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManagerWorker::*)(QStringList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManagerWorker::signalUpdateControllerIpList)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (OnbonControllerManagerWorker::*)(QJsonObject );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OnbonControllerManagerWorker::signalSetControllerDetails)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject OnbonControllerManagerWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_OnbonControllerManagerWorker.data,
    qt_meta_data_OnbonControllerManagerWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OnbonControllerManagerWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OnbonControllerManagerWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OnbonControllerManagerWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OnbonControllerManagerWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void OnbonControllerManagerWorker::showMsg(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OnbonControllerManagerWorker::signalUpdateControllerIpList(QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OnbonControllerManagerWorker::signalSetControllerDetails(QJsonObject _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
