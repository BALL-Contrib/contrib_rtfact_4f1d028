#ifndef RTANYSL_OPTIONS_HPP_INCLUDED
#define RTANYSL_OPTIONS_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant/bad_visit.hpp>
#include <vector>

namespace RTAnySL
{

class RTANYSL_API OptionValue
{
public:

    enum Type
    {
        EMPTY_OPTION,
        BOOLEAN_OPTION,
        INTEGER_OPTION,
        FLOAT_OPTION,
        STRING_OPTION
    };

    typedef bool BooleanType;
    typedef long IntegerType;
    typedef double FloatType;
    typedef std::string StringType;

    OptionValue() :
        value(Empty()) { }

    OptionValue(BooleanType b) :
        value(b) { }

    OptionValue(FloatType f) :
        value(f) { }

    OptionValue(IntegerType i) :
        value(i) { }

    OptionValue(const char *s) :
        value(StringType(s)) { }

    OptionValue(const StringType &s) :
        value(s) { }

    const OptionValue &operator=(const OptionValue &other)
    {
        value = other.value;
        return *this;
    }

    Type getType() const
    {
        return static_cast<Type>(value.which());
    }

    bool isEmpty() const { return getType() == EMPTY_OPTION; }

    bool toBool() const
    {
        return boost::apply_visitor(ToBool(), value);
    }

    long toInteger() const
    {
        return boost::apply_visitor(ToInteger(), value);
    }

    double toFloat() const
    {
        return boost::apply_visitor(ToFloat(), value);
    }

    std::string toString() const
    {
        return boost::apply_visitor(ToString(), value);
    }

    void setBool(BooleanType v) { value = v; }

    void setFloat(FloatType v) { value = v; }

    void setInteger(IntegerType v) { value = v; }

    void setString(const char *v) { value = StringType(v); }

    void setString(const StringType &v) { value = v; }

    void setEmpty() { value = Empty(); }

private:

    struct Empty { };

    typedef boost::variant<Empty, bool, long, double, std::string> Value;

    struct ToBool : public boost::static_visitor<BooleanType>
    {
        BooleanType operator()(Empty) const { throw boost::bad_visit(); }

        BooleanType operator()(BooleanType v) const
        {
            return v;
        }

        BooleanType operator()(IntegerType v) const
        {
            return (v != 0);
        }

        BooleanType operator()(FloatType v) const
        {
            return (v != 0.0f);
        }

        BooleanType operator()(const StringType &v) const
        {
            if (v == "true" || v == "1" || v == "TRUE")
                return true;
            else
                return false;
        }

    };

    struct ToInteger : public boost::static_visitor<IntegerType>
    {
        IntegerType operator()(Empty) const { throw boost::bad_visit(); }

        IntegerType operator()(BooleanType v) const
        {
            return static_cast<IntegerType>(v);
        }

        IntegerType operator()(IntegerType v) const
        {
            return v;
        }

        IntegerType operator()(FloatType v) const
        {
            return static_cast<IntegerType>(v);
        }

        IntegerType operator()(const StringType &v) const
        {
            return boost::lexical_cast<IntegerType>(v);
        }
    };

    struct ToFloat : public boost::static_visitor<FloatType>
    {
        FloatType operator()(Empty) const { throw boost::bad_visit(); }

        FloatType operator()(BooleanType v) const
        {
            return static_cast<FloatType>(v);
        }

        FloatType operator()(IntegerType v) const
        {
            return static_cast<FloatType>(v);
        }

        FloatType operator()(FloatType v) const { return v; }

        FloatType operator()(const StringType &v) const
        {
            return boost::lexical_cast<FloatType>(v);
        }
    };

    struct ToString : public boost::static_visitor<StringType>
    {

        StringType operator()(Empty) const { throw boost::bad_visit(); }

        StringType operator()(BooleanType v) const
        {
            return v ? "true" : "false";
        }

        StringType operator()(IntegerType v) const
        {
            return boost::lexical_cast<StringType>(v);
        }

        StringType operator()(FloatType v) const
        {
            return boost::lexical_cast<StringType>(v);
        }

        StringType operator()(const StringType &v) const
        {
            return v;
        }
    };

    Value value;
};

class RTANYSL_API Option
{
public:

    Option() :
        name(), value()
    {
    }

    Option(const std::string &name, OptionValue::BooleanType b) :
        name(name), value(b) { }

    Option(const std::string &name, OptionValue::FloatType f) :
        name(name), value(f) { }

    Option(const std::string &name, OptionValue::IntegerType i) :
        name(name), value(i) { }

    Option(const std::string &name, const char *s) :
        name(name), value(OptionValue::StringType(s)) { }

    Option(const std::string &name, const OptionValue::StringType &s) :
        name(name), value(s) { }

    const Option &operator=(const Option &other)
    {
        name = other.name;
        value = other.value;
        return *this;
    }

    void setValueFrom(const Option &other)
    {
        value = other.value;
    }

    const std::string & getName() const { return name; }

    const OptionValue & getValue() const { return value; }

    OptionValue::Type getType() const
    {
        return value.getType();
    }

    bool isEmpty() const { return getType() == OptionValue::EMPTY_OPTION; }

    bool toBool() const
    {
        return value.toBool();
    }

    long toInteger() const
    {
        return value.toInteger();
    }

    double toFloat() const
    {
        return value.toFloat();
    }

    std::string toString() const
    {
        return value.toString();
    }

    void setBool(OptionValue::BooleanType v) { value.setBool(v); }

    void setFloat(OptionValue::FloatType v) { value.setFloat(v); }

    void setInteger(OptionValue::IntegerType v) { value.setInteger(v); }

    void setString(const char *v) { value.setString(v); }

    void setString(const OptionValue::StringType &v) { value.setString(v); }

    void setEmpty() { value.setEmpty(); }

private:

    std::string name;
    OptionValue value;
};

typedef std::vector<Option> OptionsList;

#define RTANYSL_OPTION(name, value)                                          \
    RTAnySL::Option(name, value)

#define RTANYSL_BOOLEAN_OPTION(name, value)                                  \
    RTAnySL::Option(                                                         \
        name, static_cast<RTAnySL::OptionValue::BooleanType>(value))

#define RTANYSL_INTEGER_OPTION(name, value)                                  \
    RTAnySL::Option(                                                         \
        name, static_cast<RTAnySL::OptionValue::IntegerType>(value))

#define RTANYSL_FLOAT_OPTION(name, value)                                    \
    RTAnySL::Option(name, static_cast<RTAnySL::Option::FloatType>(value))

#define RTANYSL_STRING_OPTION(name, value)                                   \
    RTAnySL::Option(name, static_cast<RTAnySL::Option::StringType>(value))

/// Default options

#define RTANYSL_PACKETIZE_OPTION_NAME "packetize"
#define RTANYSL_PACKETIZE_OPTION(value)                                      \
    RTANYSL_BOOLEAN_OPTION(RTANYSL_PACKETIZE_OPTION_NAME, value)

#define RTANYSL_SPECIALIZE_OPTION_NAME "specialize"
#define RTANYSL_SPECIALIZE_OPTION(value)                                     \
    RTANYSL_BOOLEAN_OPTION(RTANYSL_SPECIALIZE_OPTION_NAME, value)

} // namespace RTAnySL

namespace boost
{

#define _RTANYSL_HAS_NOTHROW_COPY(C)  \
  template <>                      \
  struct has_nothrow_copy< C >     \
    : mpl::true_                   \
  {                                \
  }

_RTANYSL_HAS_NOTHROW_COPY(::RTAnySL::OptionValue::Empty);

#undef _RTSG_HAS_NOTHROW_COPY

} // namespace boost

#endif
