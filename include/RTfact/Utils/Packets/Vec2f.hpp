#ifndef RTFACT__VEC2F_HPP
#define RTFACT__VEC2F_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Math/CompileTimeMathUtils.hpp>
#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Detail/Constants.hpp>

#pragma region Constants utility macros

#define RTFACT__VEC2F_SPECS (1)(4)

#define RTFACT_VEC2F_DECLARE_CONST_AND_INITIALIZER(aName)                      \
    static const t_This aName;                                                 \
    static const t_This getConstant_##aName(float aX, float aY)                \
    {                                                                          \
        return replicate(Vec2f<1>(aX, aY));                                    \
    }

#define RTFACT_VEC2F_INIT_CONST(aSize, aName, aX, aY)                          \
    RTFACT_PACKET_PUT_TEMPLATE(aSize, RTFACT__VEC2F_SPECS)                     \
    const Vec2f<aSize> Vec2f<aSize>::aName                                     \
        = Vec2f<aSize>::getConstant_##aName(aX, aY);

#pragma endregion // Constants utility macros

#include <RTfact/Utils/Packets/Detail/Vec2f1.hpp>
#include <RTfact/Utils/Packets/Detail/Vec2f4.hpp>
#include <RTfact/Utils/Packets/Detail/Vec2fGeneric.hpp>

#include <RTfact/Utils/Math/Detail/Vec2fMathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

typedef Vec2f<1> Vec2f1;
typedef Vec2f<4> Vec2fC;

RTFACT_NAMESPACE_END

#endif // RTFACT__VEC2F_HPP
