#ifndef RTFACT__PARAMUTILS_HPP
#define RTFACT__PARAMUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Foundations/ReferenceCounted.hpp>
#include <RTfact/Utils/Containers/HashMap.hpp>
#include <RTfact/Foundations/Strings/StringAlgorithms.hpp>
#include <RTfact/Foundations/Strings/StringConverter.hpp>

RTFACT_NAMESPACE_BEGIN

#pragma region Basic parameter type definitions

enum ParamType
{
    PARAM_BOOL            = 0,
    PARAM_INT             = 1,
    PARAM_UINT            = 2,
    PARAM_FLOAT           = 3,
    //PARAM_VEC2F           = 4,
    //PARAM_VEC3F           = 5,
    //PARAM_BBOX            = 6,
    //PARAM_MATRIX3F        = 7,
    //PARAM_MATRIX4F        = 8,
    //PARAM_TRANSFORM2D     = 9,
    //PARAM_TRANSFORM3D     = 10,
    //PARAM_IMAGE2D         = 11,
    //PARAM_FILTER_MODE_2D  = 12,
    //PARAM_WRAP_MODE       = 13,
    //PARAM_MAP_MODE_2D     = 14,
    //PARAM_TEXTURE_POINTER = 15,
    PARAM_STRING          = 16
};

enum ParamAccess
{
    PARAM_GET     = 1,
    PARAM_SET     = 2,
    PARAM_GET_SET = 3
};

struct Param
{
    ParamType   type;
    std::string name;
    void* data;
};

class ParamInfo
{
    ParamAccess mAccess;
    ParamType   mType;
    std::string mName;
    std::string mDescription;
    std::string mSemantic;

public:

    ParamInfo()
    {}

    ParamInfo(
        const ParamAccess aAccess,
        const ParamType aType,
        const std::string& aName,
        const std::string& aDescription,
        const std::string& aSemantic
    ) :
        mAccess(aAccess),
        mType(aType),
        mName(aName),
        mDescription(aDescription),
        mSemantic(aSemantic)
    {}

    ParamAccess getAccess() const
    {
        return mAccess;
    }

    ParamType getType() const
    {
        return mType;
    }

    const std::string& getName() const
    {
        return mName;
    }

    const std::string& getDescription() const
    {
        return mDescription;
    }

    const std::string& getSemantic() const
    {
        return mSemantic;
    }
};

typedef HashMap<std::string, ParamInfo> ParamInfos;

template<
    class tType>
struct ParamTraits;

template<
    class tType>
RTFACT_INLINE ParamType getParamType()
{
    return ParamTraits<tType>::type;
}

template<
    class tType>
RTFACT_INLINE void checkParamType(
    const Param& aParam)
{
    RTFACT_USER_ERROR_IF_F(
        ParamTraits<tType>::type != aParam.type,
        "Invalid type supplied for parameter \"%1%\"", aParam.name);
}

#define RTFACT_DEFINE_PARAM_TRAIT(aTypeName, aParamType)                       \
    template<>                                                                 \
        struct ParamTraits<aTypeName>                                          \
    {                                                                          \
        static const ParamType type = aParamType;                              \
    };

RTFACT_DEFINE_PARAM_TRAIT(bool, PARAM_BOOL);
RTFACT_DEFINE_PARAM_TRAIT(int, PARAM_INT);
RTFACT_DEFINE_PARAM_TRAIT(uint, PARAM_UINT);
RTFACT_DEFINE_PARAM_TRAIT(float, PARAM_FLOAT);
//RTFACT_DEFINE_PARAM_TRAIT(Vec2f1, PARAM_VEC2F);
//RTFACT_DEFINE_PARAM_TRAIT(Vec3f1, PARAM_VEC3F);
//RTFACT_DEFINE_PARAM_TRAIT(BBox, PARAM_BBOX);
//RTFACT_DEFINE_PARAM_TRAIT(Matrix3f, PARAM_MATRIX3F);
//RTFACT_DEFINE_PARAM_TRAIT(Matrix4f, PARAM_MATRIX4F);
//RTFACT_DEFINE_PARAM_TRAIT(Transform2D, PARAM_TRANSFORM2D);
//RTFACT_DEFINE_PARAM_TRAIT(Transform3D, PARAM_TRANSFORM3D);
//RTFACT_DEFINE_PARAM_TRAIT(Image2D, PARAM_IMAGE2D);
//RTFACT_DEFINE_PARAM_TRAIT(FilterMode2D, PARAM_FILTER_MODE_2D);
//RTFACT_DEFINE_PARAM_TRAIT(WrapMode, PARAM_WRAP_MODE);
//RTFACT_DEFINE_PARAM_TRAIT(MappingMode2D, PARAM_MAP_MODE_2D);
//RTFACT_DEFINE_PARAM_TRAIT(Traits<Texture>::Ptr, PARAM_TEXTURE_POINTER);
RTFACT_DEFINE_PARAM_TRAIT(std::string, PARAM_STRING);

#pragma endregion // Basic parameter type definitions

#pragma region Functions for getting/setting Param data

template<
    class tType>
RTFACT_INLINE const tType getParamData(
    const Param& aParam)
{
    if(aParam.type == PARAM_STRING)
    {
        return fromString<tType>(*reinterpret_cast<const std::string*>(aParam.data));
    }

    checkParamType<tType>(aParam);

    return *reinterpret_cast<const tType*>(aParam.data);
}

template<
    class tType>
RTFACT_INLINE void setParamData(
    const tType& aData,
    Param& oParam)
{
    oParam.type = getParamType<tType>();
    oParam.data = reinterpret_cast<void*>(const_cast<tType*>(&aData));
}

template<
    class tType>
RTFACT_INLINE const Param createParam(
    const std::string& aName,
    const tType& aData)
{
    Param res;

    res.name = aName;
    setParamData(aData, res);

    return res;
}

#pragma endregion // Functions for getting/setting Param data

#pragma region Parameterizable class definition

class Parameterizable
{
    template<
        class tType>
    struct InvalidParamHandler
    {
        static tType get(
            const std::string& aParamName)
        {
            RTFACT_PROGRAM_ERROR_F(
                "Parameter \"%1%\" does not support get operation", aParamName);
        }

        static void set(
            const std::string& aParamName,
            const tType& aValue)
        {
            RTFACT_PROGRAM_ERROR_F(
                "Parameter \"%1%\" does not support set operation", aParamName);
        }
    };

    struct ParamHandler :
        public ReferenceCounted
    {
        virtual void get(
            Param& oParam) const = 0;

        virtual void set(
            const Param& aParam) = 0;
    };

    template<
        class tParamType,
        class tGetCallBack,
        class tSetCallBack>
    class TypedParamHandler :
        public ParamHandler
    {
        tGetCallBack mGetCallBack;
        tSetCallBack mSetCallBack;

    public:

        TypedParamHandler(
            const tGetCallBack& aGetCallBack,
            const tSetCallBack& aSetCallBack
        ) :
            mGetCallBack(aGetCallBack),
            mSetCallBack(aSetCallBack)
        {}

        virtual void get(
            Param& oParam) const
        {
            RTfact::setParamData(mGetCallBack(), oParam);
        }

        virtual void set(
            const Param& aParam)
        {
            mSetCallBack(RTfact::getParamData<tParamType>(aParam));
        }
    };

    typedef HashMap<std::string, Traits<ParamHandler>::Ptr > ParamHandlerMap;

    ParamInfos mParamInfos;
    ParamHandlerMap mParamHandlers;
    std::string mName;
    std::string mType;

protected:

    virtual void dummy() const {}

    template<
        class tParamType,
        class tClass,
        class tGetResult,
        class tSetResult,
        class tSetArgument>
    void registerGetSetParam(
        const std::string& aName,
        tGetResult(tClass::*aGetCallBack)() const,
        tSetResult(tClass::*aSetCallBack)(tSetArgument),
        tClass* aObject,
        const std::string& aDescription = "",
        const std::string& aSemantic = "")
    {
        mParamInfos[aName] = ParamInfo(
            PARAM_GET_SET, getParamType<tParamType>(), aName, aDescription, aSemantic);

        mParamHandlers[aName] =
            new TypedParamHandler<
                tParamType,
                boost::function<tGetResult()>,
                boost::function<tSetResult(tSetArgument)> >(
                boost::bind(aGetCallBack, aObject),
                boost::bind(aSetCallBack, aObject, _1));
    }

    template<
        class tParamType,
        class tClass,
        class tGetResult>
    void registerGetParam(
        const std::string& aName,
        tGetResult(tClass::*aGetCallBack)() const,
        tClass* aObject,
        const std::string& aDescription = "",
        const std::string& aSemantic = "")
    {
        mParamInfos[aName] = ParamInfo(
            PARAM_GET, getParamType<tParamType>(), aName, aDescription, aSemantic);

        mParamHandlers[aName] =
            new TypedParamHandler<
                tParamType,
                boost::function<tGetResult()>,
                boost::function<void(const tParamType&)> >(
                boost::bind(aGetCallBack, aObject),
                boost::bind(&InvalidParamHandler<tParamType>::set, aName, _1));
    }

    template<
        class tParamType,
        class tClass,
        class tSetResult,
        class tSetArgument>
    void registerSetParam(
        const std::string& aName,
        tSetResult(tClass::*aSetCallBack)(tSetArgument),
        tClass* aObject,
        const std::string& aDescription = "",
        const std::string& aSemantic = "")
    {
        mParamInfos[aName] = ParamInfo(
            PARAM_SET, getParamType<tParamType>(), aName, aDescription, aSemantic);

        mParamHandlers[aName] =
            new TypedParamHandler<
                tParamType,
                boost::function<tParamType()>,
                boost::function<tSetResult(tSetArgument)> >(
                boost::bind(&InvalidParamHandler<tParamType>::get, aName),
                boost::bind(aSetCallBack, aObject, _1));
    }

public:

    Parameterizable() :
        mName("[unnamed]"),
        mType("[unknown]")
    {}

    const std::string& getName() const
    {
        return mName;
    }

    void setName(
        const std::string& aName)
    {
        mName = aName;
    }

    const std::string& getType() const
    {
        return mType;
    }

    void setType(
        const std::string& aType)
    {
        mType = aType;
    }

    const ParamInfos& getParamInfos() const
    {
        return mParamInfos;
    }

    bool paramExists(
        const std::string& aName) const
    {
        return mParamInfos.find(aName) != mParamInfos.end();
    }

    const ParamInfo& getParamInfo(
        const std::string& aName) const
    {
        ParamInfos::const_iterator it = mParamInfos.find(aName);

        RTFACT_USER_ERROR_IF_F(it == mParamInfos.end(),
            "Unknown parameter \"%1%\"", aName);

        return it->second;
    }

    void getParam(
        Param& oParam) const
    {
        ParamHandlerMap::const_iterator it = mParamHandlers.find(oParam.name);

        RTFACT_USER_ERROR_IF_F(it == mParamHandlers.end(),
            "Unknown parameter \"%1%\"", oParam.name);

        it->second->get(oParam);
    }

    void setParam(
        const Param& aParam)
    {
        ParamHandlerMap::iterator it = mParamHandlers.find(aParam.name);

        RTFACT_USER_ERROR_IF_F(it == mParamHandlers.end(),
            "Unknown parameter \"%1%\"", aParam.name);

        it->second->set(aParam);
    }

    template<
        class tType>
    void setParam(
        const std::string& aName,
        const tType& aData)
    {
        setParam(createParam(aName, aData));
    }

    void setParam(
        const std::string& aName,
        const char* aData)
    {
        setParam(aName, std::string(aData));
    }

    template<
        class tType>
    const tType getParamData(
        const std::string& aName) const
    {
        Param param;
        param.name = aName;

        getParam(param);

        return RTfact::getParamData<tType>(param);
    }
};

#pragma endregion // Parameterizable class definition

#pragma region Parameter declaration helper classes

namespace Detail {

template<
    class tType>
class ParamInit
{
public:

    typedef void (*RegisterParamCallBack)(tType*);

private:

    static Vector<RegisterParamCallBack> mCallbacks;

public:

    static void addCallback(
        RegisterParamCallBack aCallback)
    {
        mCallbacks.push_back(aCallback);
    }

    static void performInitialization(
        tType* aInstance)
    {
        for(uint i = 0; i < mCallbacks.size(); ++i)
        {
            mCallbacks[i](aInstance);
        }
    }
};

template<
    class tType,
    class tInitializer>
struct ParamInitHelper
{
    int dummy;

    ParamInitHelper()
    {
        poke();
        ParamInit<tType>::addCallback(&tInitializer::init);
    }

    void poke()
    {
        dummy = 0;
    }
};

template<
    class tType,
    class tInitializer>
struct ParamInitHelperContainer
{
    static ParamInitHelper<tType, tInitializer> helper;
};

template<
    class tType>
Vector<typename ParamInit<tType>::RegisterParamCallBack> ParamInit<tType>::mCallbacks =
    Vector<typename ParamInit<tType>::RegisterParamCallBack>();

template<
    class tType,
    class tInitializer>
ParamInitHelper<tType, tInitializer> ParamInitHelperContainer<tType, tInitializer>::helper =
    ParamInitHelper<tType, tInitializer>();

} // namespace Detail

#pragma endregion // Parameter declaration helper classes

RTFACT_NAMESPACE_END

#pragma region Utility macros

#define RTFACT_PARAM_INIT(aClassName)                                          \
    private:                                                                   \
        typedef aClassName t__RTfact_This;                                     \
        void initParams()                                                      \
        {                                                                      \
            ::RTfact::Detail::ParamInit<t__RTfact_This>::performInitialization(this);\
        }                                                                      \

#define RTFACT__PARAM_BASE(aName, aType, aRegistrar, aDeclaration, aInitialization)\
    protected:                                                                 \
        aDeclaration                                                           \
        struct RTfact__##aName##_ParamInit                                     \
        {                                                                      \
            static void poke()                                                 \
            {                                                                  \
                ::RTfact::Detail::ParamInitHelperContainer<                    \
                    t__RTfact_This, RTfact__##aName##_ParamInit>::helper.poke();\
            }                                                                  \
                                                                               \
            static void init(                                                  \
                t__RTfact_This* aInstance)                                     \
            {                                                                  \
                aInitialization                                                \
                aRegistrar;                                                    \
            }                                                                  \
            RTfact__##aName##_ParamInit()                                      \
            {                                                                  \
                RTfact__##aName##_ParamInit::poke();                           \
            }                                                                  \
        };                                                                     \
        static RTfact__##aName##_ParamInit rtfact__##aName##_ParamInit;               \
    public:                                                                    \
        void poke(const RTfact__##aName##_ParamInit& aDummy)                   \
        {                                                                      \
            RTfact__##aName##_ParamInit::poke();                               \
        }                                                                      \

#define RTFACT_PARAM_GS_GS(aName, aType, aDefaultValue, aDescription, aSemantic)\
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET_SET(aName, aType, aDescription, aSemantic),          \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \
    RTFACT_DEFAULT_PARAM_GETTER_SETTER(aName, aType)                           \

#define RTFACT_PARAM_GS_G(aName, aType, aDefaultValue, aDescription, aSemantic)\
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET_SET(aName, aType, aDescription, aSemantic),          \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \
    RTFACT_DEFAULT_PARAM_GETTER(aName, aType)                                  \

#define RTFACT_PARAM_GS_S(aName, aType, aDefaultValue, aDescription, aSemantic)\
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET_SET(aName, aType, aDescription, aSemantic),          \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \
    RTFACT_DEFAULT_PARAM_SETTER(aName, aType)                                  \

#define RTFACT_PARAM_GS(aName, aType, aDefaultValue, aDescription, aSemantic)  \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET_SET(aName, aType, aDescription, aSemantic),          \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \

#define RTFACT_PARAM_GS_N(aName, aType, aDescription, aSemantic)               \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET_SET(aName, aType, aDescription, aSemantic),          \
        RTFACT__DECLARE_PARAM_VAR_DUMMY,                                       \
        RTAFCT__INIT_PARAM_VAR_DUMMY)                                          \

#define RTFACT_PARAM_G_G(aName, aType, aDefaultValue, aDescription, aSemantic) \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET(aName, aType, aDescription, aSemantic),              \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \
    RTFACT_DEFAULT_PARAM_GETTER(aName, aType)                                  \

#define RTFACT_PARAM_G(aName, aType, aDefaultValue, aDescription, aSemantic)   \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET(aName, aType, aDescription, aSemantic),              \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \

#define RTFACT_PARAM_G_N(aName, aType, aDescription, aSemantic)                \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_GET(aName, aType, aDescription, aSemantic),              \
        RTFACT__DECLARE_PARAM_VAR_DUMMY,                                       \
        RTAFCT__INIT_PARAM_VAR_DUMMY)                                          \

#define RTFACT_PARAM_S_S(aName, aType, aDefaultValue, aDescription, aSemantic) \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_SET(aName, aType, aDescription, aSemantic),              \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \
    RTFACT_DEFAULT_PARAM_SETTER(aName, aType)                                  \

#define RTFACT_PARAM_S(aName, aType, aDefaultValue, aDescription, aSemantic)   \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_SET(aName, aType, aDescription, aSemantic),              \
        RTFACT__DECLARE_PARAM_VAR(aName, aType),                               \
        RTAFCT__INIT_PARAM_VAR(aName, aType, aDefaultValue))                   \

#define RTFACT_PARAM_S_N(aName, aType, aDescription, aSemantic)                \
    RTFACT__PARAM_BASE(aName, aType,                                           \
        RTFACT__PARAM_SET(aName, aType, aDescription, aSemantic),              \
        RTFACT__DECLARE_PARAM_VAR_DUMMY,                                       \
        RTAFCT__INIT_PARAM_VAR_DUMMY)                                          \

#define RTFACT__PARAM_GET_SET(aName, aType, aDescription, aSemantic)           \
    aInstance->template registerGetSetParam<aType>(                                     \
        RTFACT_STRINGIZE(aName),                                               \
        BOOST_PP_CAT(&t__RTfact_This::get, aName),                             \
        BOOST_PP_CAT(&t__RTfact_This::set, aName),                             \
        aInstance, aDescription, aSemantic)                                    \

#define RTFACT__PARAM_GET(aName, aType, aDescription, aSemantic)               \
    aInstance->template registerGetParam<aType>(                                        \
        RTFACT_STRINGIZE(aName),                                               \
        BOOST_PP_CAT(&t__RTfact_This::get, aName),                             \
        aInstance, aDescription, aSemantic)                                    \

#define RTFACT__PARAM_SET(aName, aType, aDescription, aSemantic)               \
    aInstance->template registerSetParam<aType>(                                        \
        RTFACT_STRINGIZE(aName),                                               \
        BOOST_PP_CAT(&t__RTfact_This::set, aName),                             \
        aInstance, aDescription, aSemantic)                                    \

#define RTFACT_PARAM_GETTER(aName, aType)                                      \
    const aType& get##aName() const                                            \

#define RTFACT_PARAM_SETTER(aName, aType, aValueName)                          \
    void set##aName(const aType& aValueName)                                   \

#define RTFACT_DEFAULT_PARAM_GETTER(aName, aType)                              \
    public:                                                                    \
        RTFACT_PARAM_GETTER(aName, aType)                                      \
        { return m##aName; }                                                   \

#define RTFACT_DEFAULT_PARAM_SETTER(aName, aType)                              \
    public:                                                                    \
        RTFACT_PARAM_SETTER(aName, aType, aValue)                              \
        { m##aName = aValue; }                                                 \

#define RTFACT_DEFAULT_PARAM_GETTER_SETTER(aName, aType)                       \
    RTFACT_DEFAULT_PARAM_GETTER(aName, aType)                                  \
    RTFACT_DEFAULT_PARAM_SETTER(aName, aType)                                  \

#define RTFACT__DECLARE_PARAM_VAR(aName, aType)                                \
    aType m##aName;                                                            \

#define RTAFCT__INIT_PARAM_VAR(aName, aType, aValue)                           \
    aInstance->m##aName = aType(aValue);                                       \

#define RTFACT__DECLARE_PARAM_VAR_DUMMY
#define RTAFCT__INIT_PARAM_VAR_DUMMY

#pragma endregion // Utility macros

#endif // RTFACT__PARAMUTILS_HPP
