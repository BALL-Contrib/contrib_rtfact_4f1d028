/*
 * TypeName.hpp
 *
 *  Created on: Dec 4, 2009
 *      Author: rubinstein
 */
#ifndef CXXSHADER_TYPEINFO_HPP_INCLUDED
#define CXXSHADER_TYPEINFO_HPP_INCLUDED

#include "commontypes.hpp"

enum ParamSemanticID {
    SEMANTIC_UNKNOWN          = '\0',
    SEMANTIC_INTEGER          = 'i',
    SEMANTIC_FLOAT            = 'f',
    SEMANTIC_VECTOR           = 'v',
    SEMANTIC_NORMAL           = 'n',
    SEMANTIC_POINT            = 'p',
    SEMANTIC_COLOR            = 'c',
    SEMANTIC_MATRIX           = 'm',
    SEMANTIC_STRING           = 's', // zero-ended string
    SEMANTIC_TEXTURE          = 't',
    SEMANTIC_BOOLEAN          = 'b'
};

enum ElementTypeID {
    TYPE_UNKNOWN            = '\0',
    TYPE_SIGNED_CHAR        = 'b',
    TYPE_UNSIGNED_CHAR      = 'B',
    TYPE_SIGNED_SHORT       = 'h',
    TYPE_UNSIGNED_SHORT     = 'H',
    TYPE_SIGNED_INT         = 'i',
    TYPE_UNSIGNED_INT       = 'I',
    TYPE_SIGNED_LONG        = 'l',
    TYPE_UNSIGNED_LONG      = 'L',
    TYPE_SIGNED_LONG_LONG   = 'q',
    TYPE_UNSIGNED_LONG_LONG = 'Q',
    TYPE_FLOAT              = 'f',
    TYPE_DOUBLE             = 'd',
    TYPE_POINTER            = 'P'
};

/** TypeInfo struct is used for producing type IDs and human readable names
 *  from types.
 *  Specialize this struct for new types.
 */
template <class T> struct TypeInfo
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_UNKNOWN;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_UNKNOWN;
    }

    static int getNumElements() { return 1; }

    static const char * getName()
    {
        return typeid(T).name();
    }
};

/// getTypeInfo returns name of the type of the given argument
template <class T>
const char * getTypeName(const T &)
{
    return TypeInfo<T>::getName();
}

/// getTypeNameAndSet set a dest argument to the new value and
/// returns type name of the dest argument
template <class T, class U>
const char * getTypeNameAndSet(T & dest, const U &newValue)
{
    dest = newValue;
    return TypeInfo<T>::getName();
}

template <class T>
ParamSemanticID getTypeSemanticID(const T &)
{
    return TypeInfo<T>::getSemanticID();
}

template <class T>
ElementTypeID getTypeElementTypeID(const T &)
{
    return TypeInfo<T>::getElementTypeID();
}

template <class T>
int getTypeNumElements(const T &)
{
    return TypeInfo<T>::getNumElements();
}

template <class T>
TypeInfo<T> getTypeInfo(const T &)
{
    return TypeInfo<T>();
}

/// getTypeInfoAndSet set a dest argument to the new value and
/// returns type name of the dest argument
template <class T, class U>
const char * getTypeInfoAndSet(T & dest, const U &newValue)
{
    dest = newValue;
    return TypeInfo<T>::getName();
}

template <> struct TypeInfo<const char *>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_STRING;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_POINTER;
    }

    static int getNumElements() { return 1; }

    static const char * getName()
    {
        return "const char *";
    }
};

template <> struct TypeInfo<bool>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_BOOLEAN;
    }

    static ElementTypeID getElementTypeID()
    {
        // don't use switch because of possible duplicate cases
        // e.g. when sizeof(int) == sizeof(long)
        const size_t bsz = sizeof(bool);
        if (bsz == sizeof(char))
            return TYPE_SIGNED_CHAR;
        else if (bsz == sizeof(short))
            return TYPE_SIGNED_SHORT;
        else if (bsz == sizeof(int))
            return TYPE_SIGNED_INT;
        else if (bsz == sizeof(long))
            return TYPE_SIGNED_LONG;
        return TYPE_UNKNOWN;
    }

    static int getNumElements() { return 1; }

    static const char *getName()
    {
        return "bool";
    }
};

template <> struct TypeInfo<int>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_INTEGER;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_SIGNED_INT;
    }

    static int getNumElements() { return 1; }

    static const char * getName()
    {
        return "int";
    }
};

template <> struct TypeInfo<unsigned int>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_INTEGER;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_UNSIGNED_INT;
    }

    static int getNumElements() { return 1; }

    static const char *getName()
    {
        return "unsigned int";
    }
};


template <> struct TypeInfo<char>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_INTEGER;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_SIGNED_CHAR;
    }

    static int getNumElements() { return 1; }

    static const char *getName()
    {
        return "char";
    }
};

template <> struct TypeInfo<unsigned char>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_INTEGER;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_UNSIGNED_CHAR;
    }

    static int getNumElements() { return 1; }

    static const char *getName()
    {
        return "unsigned char";
    }
};

template <> struct TypeInfo<float>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_FLOAT;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_FLOAT;
    }

    static int getNumElements() { return 1; }

    static const char *getName()
    {
        return "float";
    }
};

template <> struct TypeInfo<double>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_FLOAT;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_DOUBLE;
    }

    static int getNumElements() { return 1; }

    static const char *getName()
    {
        return "double";
    }
};

template <> struct TypeInfo<Vector>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_VECTOR;
    }

    static ElementTypeID getElementTypeID()
    {
        Vector v;
        return getTypeInfo(v.x).getElementTypeID();
    }

    static int getNumElements()
    {
        Vector v;
        return sizeof(v)/sizeof(v.x);
    }

    static const char *getName()
    {
        return "Vector";
    }
};

template <> struct TypeInfo<Texture>
{
    static ParamSemanticID getSemanticID()
    {
        return SEMANTIC_TEXTURE;
    }

    static ElementTypeID getElementTypeID()
    {
        return TYPE_POINTER;
    }

    static int getNumElements() { return 1; }

    static const char *getName()
    {
        return "Texture";
    }
};

#endif
