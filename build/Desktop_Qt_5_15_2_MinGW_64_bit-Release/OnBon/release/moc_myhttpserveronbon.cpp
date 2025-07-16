/****************************************************************************
** Meta object code from reading C++ file 'myhttpserveronbon.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../OnBon/myhttpserveronbon.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'myhttpserveronbon.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MyHttpServerOnBon_t {
    QByteArrayData data[25];
    char stringdata0[291];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MyHttpServerOnBon_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MyHttpServerOnBon_t qt_meta_stringdata_MyHttpServerOnBon = {
    {
QT_MOC_LITERAL(0, 0, 17), // "MyHttpServerOnBon"
QT_MOC_LITERAL(1, 18, 10), // "signalTest"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 8), // "jsonData"
QT_MOC_LITERAL(4, 39, 17), // "signalWrite2Kafka"
QT_MOC_LITERAL(5, 57, 5), // "topic"
QT_MOC_LITERAL(6, 63, 7), // "strJson"
QT_MOC_LITERAL(7, 71, 6), // "strKey"
QT_MOC_LITERAL(8, 78, 45), // "signalDynamicArea_AddAreaTxtD..."
QT_MOC_LITERAL(9, 124, 14), // "nBaudRateIndex"
QT_MOC_LITERAL(10, 139, 5), // "color"
QT_MOC_LITERAL(11, 145, 6), // "areaId"
QT_MOC_LITERAL(12, 152, 5), // "areaX"
QT_MOC_LITERAL(13, 158, 5), // "areaY"
QT_MOC_LITERAL(14, 164, 5), // "width"
QT_MOC_LITERAL(15, 170, 6), // "height"
QT_MOC_LITERAL(16, 177, 8), // "fontName"
QT_MOC_LITERAL(17, 186, 8), // "fontSize"
QT_MOC_LITERAL(18, 195, 7), // "content"
QT_MOC_LITERAL(19, 203, 6), // "Halign"
QT_MOC_LITERAL(20, 210, 6), // "Valign"
QT_MOC_LITERAL(21, 217, 11), // "DisplayMode"
QT_MOC_LITERAL(22, 229, 5), // "Speed"
QT_MOC_LITERAL(23, 235, 35), // "signalDynamicArea_DelArea_G5_..."
QT_MOC_LITERAL(24, 271, 19) // "signalStaticProgram"

    },
    "MyHttpServerOnBon\0signalTest\0\0jsonData\0"
    "signalWrite2Kafka\0topic\0strJson\0strKey\0"
    "signalDynamicArea_AddAreaTxtDetails_6G_serial\0"
    "nBaudRateIndex\0color\0areaId\0areaX\0"
    "areaY\0width\0height\0fontName\0fontSize\0"
    "content\0Halign\0Valign\0DisplayMode\0"
    "Speed\0signalDynamicArea_DelArea_G5_Serial\0"
    "signalStaticProgram"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyHttpServerOnBon[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    3,   42,    2, 0x06 /* Public */,
       8,   14,   49,    2, 0x06 /* Public */,
      23,    2,   78,    2, 0x06 /* Public */,
      24,   14,   83,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    5,    6,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    9,   10,   11,   12,   13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    9,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    9,   10,   11,   12,   13,   14,   15,   16,   17,   18,   19,   20,   21,   22,

       0        // eod
};

void MyHttpServerOnBon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyHttpServerOnBon *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalTest((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->signalWrite2Kafka((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: _t->signalDynamicArea_AddAreaTxtDetails_6G_serial((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< QString(*)>(_a[8])),(*reinterpret_cast< int(*)>(_a[9])),(*reinterpret_cast< QString(*)>(_a[10])),(*reinterpret_cast< int(*)>(_a[11])),(*reinterpret_cast< int(*)>(_a[12])),(*reinterpret_cast< int(*)>(_a[13])),(*reinterpret_cast< int(*)>(_a[14]))); break;
        case 3: _t->signalDynamicArea_DelArea_G5_Serial((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->signalStaticProgram((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< QString(*)>(_a[8])),(*reinterpret_cast< int(*)>(_a[9])),(*reinterpret_cast< QString(*)>(_a[10])),(*reinterpret_cast< int(*)>(_a[11])),(*reinterpret_cast< int(*)>(_a[12])),(*reinterpret_cast< int(*)>(_a[13])),(*reinterpret_cast< int(*)>(_a[14]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MyHttpServerOnBon::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyHttpServerOnBon::signalTest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MyHttpServerOnBon::*)(QString , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyHttpServerOnBon::signalWrite2Kafka)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MyHttpServerOnBon::*)(int , int , int , int , int , int , int , QString , int , QString , int , int , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyHttpServerOnBon::signalDynamicArea_AddAreaTxtDetails_6G_serial)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MyHttpServerOnBon::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyHttpServerOnBon::signalDynamicArea_DelArea_G5_Serial)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MyHttpServerOnBon::*)(int , int , int , int , int , int , int , QString , int , QString , int , int , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyHttpServerOnBon::signalStaticProgram)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MyHttpServerOnBon::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MyHttpServerOnBon.data,
    qt_meta_data_MyHttpServerOnBon,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MyHttpServerOnBon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyHttpServerOnBon::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyHttpServerOnBon.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MyHttpServerOnBon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void MyHttpServerOnBon::signalTest(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MyHttpServerOnBon::signalWrite2Kafka(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MyHttpServerOnBon::signalDynamicArea_AddAreaTxtDetails_6G_serial(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, QString _t8, int _t9, QString _t10, int _t11, int _t12, int _t13, int _t14)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t9))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t10))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t11))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t12))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t13))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t14))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MyHttpServerOnBon::signalDynamicArea_DelArea_G5_Serial(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MyHttpServerOnBon::signalStaticProgram(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, QString _t8, int _t9, QString _t10, int _t11, int _t12, int _t13, int _t14)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t9))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t10))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t11))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t12))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t13))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t14))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
