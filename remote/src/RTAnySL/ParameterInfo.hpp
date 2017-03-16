#ifndef RTANYSL_PARAMETERINFO_HPP_INCLUDED
#define RTANYSL_PARAMETERINFO_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <boost/type_traits.hpp>

namespace RTAnySL
{

struct ParameterType
{
    enum ElementType
    {
        VOID_TYPE = 0,          // void (size = 0)
        SIGNED_INTEGER_TYPE,    // char, int, short, long, long long, ...
        UNSIGNED_INTEGER_TYPE,  // unsigned char, unsigned int, ...
        FLOAT_TYPE              // float or double
    };

    ElementType elementType;
    bool isPointerType;
    size_t elementTypeSize;     /// size of a single array element in bytes
    int numElements;            /// for fixed size array > 1

    ParameterType() :
        elementType(VOID_TYPE),
        isPointerType(false),
        elementTypeSize(0),
        numElements(0)
    {
    }

    ParameterType(
        ElementType elementType, bool isPointerType,
        size_t elementTypeSize, int numElements) :
        elementType(elementType),
        isPointerType(isPointerType),
        elementTypeSize(elementTypeSize),
        numElements(numElements)
    {
    }

    // size of the single element
    size_t getElementSize() const
    {
        return isPointerType ? sizeof(void*) : elementTypeSize;
    }

    // full size of the type
    size_t getSize() const
    {
        return getElementSize() * numElements;
    }

    // if numElements == 0, use auto detection
    template <class T> void setToType(unsigned int numElements = 0);

    void setToRawDataType(unsigned int byteSize)
    {
        elementType = UNSIGNED_INTEGER_TYPE;
        isPointerType = false;
        elementTypeSize = 1;
        numElements = byteSize;
    }

    template <class T> bool isElementOfType() const
    {
        ParameterType other;
        other.setToType<T>();

        return (elementType == other.elementType &&
                isPointerType == other.isPointerType &&
                elementTypeSize == other.elementTypeSize);
    }

    template <class T> bool isOfType() const
    {
        ParameterType other;
        other.setToType<T>();

        return (elementType == other.elementType &&
                isPointerType == other.isPointerType &&
                elementTypeSize == other.elementTypeSize &&
                numElements == other.numElements);
    }

    template <class T> static ParameterType fromType(
        unsigned int numElements = 0)
    {
        ParameterType pt;
        pt.setToType<T>(numElements);
        return pt;
    }

};

struct ParameterInfo
{
public:


    enum ParameterSemantic
    {
        NO_SEMANTIC = 0,
        DEFAULT_SEMANTIC,
        STRING_SEMANTIC,
        BOOLEAN_SEMANTIC,
        TEXTURE_SEMANTIC,
        COLOR_SEMANTIC,
        VECTOR_SEMANTIC,
        NORMAL_SEMANTIC,
        POINT_SEMANTIC,
        MATRIX_SEMANTIC
    };

    std::string name;
    std::string description;
    ParameterType type;
    ParameterSemantic semantic;

    ParameterInfo() :
        name(), description(), type(), semantic(NO_SEMANTIC)
    {
    }

    ParameterInfo(const std::string &name, const std::string &description,
                  const ParameterType &type, ParameterSemantic semantic) :
        name(name), description(description), type(type),
        semantic(semantic)
    {
    }

    ParameterInfo(const std::string &name, const std::string &description,
                  ParameterType::ElementType elementType, bool isPointerType,
                  size_t elementTypeSize, int numElements,
                  ParameterSemantic semantic) :
        name(name), description(description),
        type(elementType, isPointerType, elementTypeSize, numElements),
        semantic(semantic)
    {
    }

    int getNumArrayElements() const
    {
        return type.numElements;
    }

    // size of the single element
    size_t getElementSize() const
    {
        return type.getElementSize();
    }

    // full size of the parameter
    size_t getParameterSize() const
    {
        return type.getSize();
    }

    template <class T> void setToType(unsigned int numElements = 0)
    {
        type.setToType<T>(numElements);
    }

    template <class T> bool isElementOfType() const
    {
        return type.isElementOfType<T>();
    }

    template <class T> bool isOfType() const
    {
        return type.isOfType<T>();
    }

};

inline std::ostream &operator<<(std::ostream &o, const ParameterType &pt)
{
    switch (pt.elementType)
    {
        case ParameterType::VOID_TYPE:
            o<<"void"; break;
        case ParameterType::SIGNED_INTEGER_TYPE:
            o<<"int"<<(pt.elementTypeSize*8); break;
        case ParameterType::UNSIGNED_INTEGER_TYPE:
            o<<"uint"<<(pt.elementTypeSize*8); break;
        case ParameterType::FLOAT_TYPE:
            if (pt.elementTypeSize == sizeof(double))
                o<<"double";
            else
            {
                o<<"float";
                if (pt.elementTypeSize != sizeof(float))
                    o<<(pt.elementTypeSize*8);
            }
            break;
        default: break;
    };

    if (pt.isPointerType)
    {
        o<<"*";
    }

    if (pt.numElements != 1)
    {
        o<<" ["<<pt.numElements<<"]";
    }

    o<<" elementSize="<<pt.getElementSize()
     <<" typeSize="<<pt.getSize();

    return o;
}

inline std::ostream &operator<<(std::ostream &o, const ParameterInfo &pi)
{
    o << "param name="<<pi.name<<" type="<<pi.type<<" semantic=";

    switch (pi.semantic)
    {
        case ParameterInfo::NO_SEMANTIC: o<<"NO_SEMANTIC"; break;
        case ParameterInfo::DEFAULT_SEMANTIC: o<<"DEFAULT_SEMANTIC"; break;
        case ParameterInfo::STRING_SEMANTIC: o<<"STRING_SEMANTIC"; break;
        case ParameterInfo::BOOLEAN_SEMANTIC: o<<"BOOLEAN_SEMANTIC"; break;
        case ParameterInfo::TEXTURE_SEMANTIC: o<<"TEXTURE_SEMANTIC"; break;
        case ParameterInfo::COLOR_SEMANTIC: o<<"COLOR_SEMANTIC"; break;
        case ParameterInfo::VECTOR_SEMANTIC: o<<"VECTOR_SEMANTIC"; break;
        case ParameterInfo::NORMAL_SEMANTIC: o<<"NORMAL_SEMANTIC"; break;
        case ParameterInfo::POINT_SEMANTIC: o<<"POINT_SEMANTIC"; break;
        case ParameterInfo::MATRIX_SEMANTIC: o<<"MATRIX_SEMANTIC"; break;
    };

    o << " descr=" << pi.description;
    return o;
}

typedef std::vector<ParameterInfo> ParameterInfoVector;

/// sizeofType<T>() is used to avoid warnings about
/// sizeof(void) and sizeof(const void)
template <class T> inline size_t sizeofType()
{
    return sizeof(T);
}

template <> inline size_t sizeofType<void>()
{
    return 0;
}

template <> inline size_t sizeofType<const void>()
{
    return 0;
}

/// isSignedType<T> returns true if the type is signed (int, short, etc.)
template <class T> inline bool isSignedType()
{
    return false;
}

template <> bool inline isSignedType<char>()
{
    return true;
}

template <> bool inline isSignedType<int>()
{
    return true;
}

template <> bool inline isSignedType<short>()
{
    return true;
}

template <> bool inline isSignedType<long>()
{
    return true;
}

template <> bool inline isSignedType<long long>()
{
    return true;
}

template <> bool inline isSignedType<const char>()
{
    return true;
}

template <> bool inline isSignedType<const int>()
{
    return true;
}

template <> bool inline isSignedType<const short>()
{
    return true;
}

template <> bool inline isSignedType<const long>()
{
    return true;
}

template <> bool inline isSignedType<const long long>()
{
    return true;
}

template <class T> inline ParameterType::ElementType getElementType()
{
    if (boost::is_integral<T>::value)
        return (isSignedType<T>() ?
                ParameterType::SIGNED_INTEGER_TYPE :
                ParameterType::UNSIGNED_INTEGER_TYPE);

    if (boost::is_floating_point<T>::value)
        return ParameterType::FLOAT_TYPE;

    return ParameterType::VOID_TYPE;
}

template <class T> void ParameterType::setToType(unsigned int _numElements)
{
    numElements = _numElements;

    if (boost::is_pointer<T>::value)
    {
        typedef typename boost::remove_pointer<T>::type NoPtrT;

        isPointerType = true;
        elementType = getElementType<NoPtrT>();
        elementTypeSize = sizeofType<NoPtrT>();
    }
    else if (boost::is_array<T>::value)
    {
        typedef typename boost::remove_all_extents<T>::type NoArrayT;

        if (boost::is_pointer<NoArrayT>::value)
        {
            typedef typename boost::remove_pointer<NoArrayT>::type NoArrayNoPtrT;

            isPointerType = true;
            elementType = getElementType<NoArrayNoPtrT>();
            elementTypeSize = sizeofType<NoArrayNoPtrT>();
            if (numElements == 0)
                numElements = static_cast<int>(sizeofType<T>() / sizeof(void*));
        }
        else
        {
            isPointerType = false;
            elementType = getElementType<NoArrayT>();
            elementTypeSize = sizeofType<NoArrayT>();
            if (numElements == 0)
                numElements = static_cast<int>(sizeofType<T>() / elementTypeSize);
        }
    }
    else if (boost::is_integral<T>::value || boost::is_floating_point<T>::value)
    {
        isPointerType = false;
        elementType = getElementType<T>();
        elementTypeSize = sizeofType<T>();
    }
    else
    {
        isPointerType = false;
        elementType = VOID_TYPE;
        elementTypeSize = sizeofType<T>();
    }

    if (numElements == 0)
        numElements = 1;
}

} // namespace RTAnySL

#endif
