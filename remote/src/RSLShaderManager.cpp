#include "Precompiled.hpp"

#define RTFACT_REMOTE_LIB
#define RTANYSL_LIB
#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>
#include <RTremote/Config/Common.hpp>
#include "RSLShaderManager.hpp"
#include "RTremotePrivateConfig.hpp"
#include "Utils.hpp"

#include <RTAnySL/RTAnySL.hpp>
#include <RTAnySL/StaticInit.hpp>
#include <RTAnySL/ShaderManagerRegistry.hpp>
#include <RTAnySL/ParameterCache.hpp>
#include <RTAnySL/Options.hpp>
#include <RTAnySL/Exception.hpp>

#include <cassert>
#include <iostream>

#include "RTAnySLShaderAPI.hpp"
#include "RTfactShaderEntryTableMaker.hpp"

#include <RTfact/Utils/IO/PathUtils.hpp>
#include <cctype>

RTFACT_REMOTE_NAMESPACE_BEGIN

#define ANYSL_RSL_SHADER_BUILDER_ENVVAR "ANYSL_RSL_SHADER_BUILDER"

//#define USE_DERIVATIVES

typedef void (*RTfactRSLGenericShadeFunction)
(
    void * rtfactContext,
    void * paramPtr,
    void * textureTable,
    float time,
    const void * aRayPacket,
    const void * aRayMask,
    void * aIntersection,
    void * oResult
);


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
// RSLShaderInstance
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
namespace
{

void checkRSLParamName(const char *paramName)
{
    const unsigned int MAX_LENGTH = 1024;

    const char *nc = paramName;

    // check for valid paramName
    for (; *nc != '\0' && (nc-paramName < MAX_LENGTH); ++nc)
    {
        RTANYSL_ASSERT(isprint(*nc), "RSL parameter name contains non-printable characters");
    }
    RTANYSL_ASSERT( (nc-paramName) != 0, "RSL parameter name is empty");
    RTANYSL_ASSERT( (nc-paramName) < MAX_LENGTH, "RSL parameter name is too big");
}

} // unnamed namespace


RSLShaderInstance::RSLShaderInstance(
    const RTAnySL::ShaderPtr &shader) :
    RTAnySL::ShaderInstance(shader),
    packetize(false),
    specialize(true),
    needsUpdate(true),
    needsRebuild(true),
    targetData(0),
    engine(0),
    parameterInfos(),
    parameterOffsets(),
    numTextures(0),
    shaderContext()
{
    RTANYSL_ASSERT(shader.get() != 0, "shader should not be NULL");
    RTANYSL_ASSERT(shader->getModule() != 0, "shader's module should not be NULL");

    resetModule();

    // the shader parameters information never change for one instance
    // so we only need to generate parameters and textures only once
    rebuildModule(true);
}

RSLShaderInstance::~RSLShaderInstance()
{
    RTANYSL_DEBUG("Destroyed RSLShaderInstance");

    freeShaderParams(shaderContext.paramPtr);
    freeTextureTable(shaderContext.texturePtr);
}

const RTAnySL::ShaderEntryTable & RSLShaderInstance::getEntryTableImpl()
{
    return shaderContext.entryTable;
}

const RTAnySL::ShaderInstanceContext & RSLShaderInstance::getContextImpl()
{
    return shaderContext;
}

void * RSLShaderInstance::getGenericShadeFuncImpl()
{
    return reinterpret_cast<void*>(&genericShadeFunc);
}


namespace {

/// TODO: following code should be common for RSL shaders and RSLShaderManager

struct RSLParamInfo
{
    const char * name;         /// parameter name
    char semanticID;           /// parameter semantic (see TypeInfo.hpp)
    char elementTypeID;        /// parameter element type
    unsigned int numElements;  /// number of elements in parameter
    const char * typeName;
    const char * descr; /// optional parameter description (might be 0)
};

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

void setAnySLElemTypeFromRSL(
    const RSLParamInfo &src, RTAnySL::ParameterInfo &dest)
{
    dest.name = src.name;
    dest.type.numElements = src.numElements;

    switch (src.semanticID)
    {
        case SEMANTIC_UNKNOWN:
            dest.semantic = RTAnySL::ParameterInfo::NO_SEMANTIC;
            break;
        case SEMANTIC_INTEGER:
        case SEMANTIC_FLOAT:
        case SEMANTIC_STRING:
            dest.semantic = RTAnySL::ParameterInfo::DEFAULT_SEMANTIC;
            break;
        case SEMANTIC_VECTOR:
            dest.semantic = RTAnySL::ParameterInfo::VECTOR_SEMANTIC;
            break;
        case SEMANTIC_NORMAL:
            dest.semantic = RTAnySL::ParameterInfo::NORMAL_SEMANTIC;
            break;
        case SEMANTIC_POINT:
            dest.semantic = RTAnySL::ParameterInfo::POINT_SEMANTIC;
            break;
        case SEMANTIC_COLOR:
            dest.semantic = RTAnySL::ParameterInfo::COLOR_SEMANTIC;
            break;
        case SEMANTIC_MATRIX:
            dest.semantic = RTAnySL::ParameterInfo::MATRIX_SEMANTIC;
            break;
        case SEMANTIC_TEXTURE:
            dest.semantic = RTAnySL::ParameterInfo::TEXTURE_SEMANTIC;
            break;
        case SEMANTIC_BOOLEAN:
            dest.semantic = RTAnySL::ParameterInfo::BOOLEAN_SEMANTIC;
            break;
        default:
            dest.semantic = RTAnySL::ParameterInfo::NO_SEMANTIC;
            break;
    }

    switch (src.elementTypeID)
    {
        case TYPE_UNKNOWN:
            dest.type.setToRawDataType(src.numElements);
            break;
        case TYPE_SIGNED_CHAR:
            dest.setToType<char>(src.numElements);
            break;
        case TYPE_UNSIGNED_CHAR:
            dest.setToType<unsigned char>(src.numElements);
            break;
        case TYPE_SIGNED_SHORT:
            dest.setToType<short>(src.numElements);
            break;
        case TYPE_UNSIGNED_SHORT:
            dest.setToType<unsigned short>(src.numElements);
            break;
        case TYPE_SIGNED_INT:
            dest.setToType<int>(src.numElements);
            break;
        case TYPE_UNSIGNED_INT:
            dest.setToType<unsigned int>(src.numElements);
            break;
        case TYPE_SIGNED_LONG:
            dest.setToType<long>(src.numElements);
            break;
        case TYPE_UNSIGNED_LONG:
            dest.setToType<unsigned long>(src.numElements);
            break;
        case TYPE_SIGNED_LONG_LONG:
            dest.setToType<long long>(src.numElements);
            break;
        case TYPE_UNSIGNED_LONG_LONG:
            dest.setToType<unsigned long long>(src.numElements);
            break;
        case TYPE_FLOAT:
            dest.setToType<float>(src.numElements);
            break;
        case TYPE_DOUBLE:
            dest.setToType<double>(src.numElements);
            break;
        case TYPE_POINTER:
            dest.setToType<void*>(src.numElements);
            break;
        default:
            dest.setToType<void>();
            break;
    }
}

} // unnamed namespace

void RSLShaderInstance::resetModule()
{
}

void RSLShaderInstance::rebuildModule(bool initParamsAndTextures)
{
    RTANYSL_ASSERT(boost::dynamic_pointer_cast<RSLShaderManager>(getShader()->getManager()) != 0, "manager must be an instance of RSLShaderManager class");

    RSLShaderManagerPtr sm =
            boost::static_pointer_cast<RSLShaderManager>(getShader()->getManager());

    buildModulePtr = getRSLShader()->getModuleAutoPtr().clone();
    RTANYSL_IFDEBUG ( jitRT::writeModuleToFile(buildModulePtr.get(), "TEST0.ll"); )

    //------------------------------------------------------------------------//
    // resolve threaded code
    // - function-calls in shader are resolved by linking in the runtime module
    // - abstract types in shader are resolved by generating alloca's of correct types
    //------------------------------------------------------------------------//
#define ANYSL_USE_COMMON_ORG false
#define USE_DERIVATIVES false
#ifdef RTFACT_HAVE_SSE41
	const bool use_sse41 = true;
#else
	const bool use_sse41 = false;
#endif
	const llvm::Module* glue = packetize ? sm->packetGlueModule.get() : sm->glueModule.get();
    std::map<const std::string, const unsigned> textureMap;
	std::map<std::string, const llvm::Type*> typeMap;
	
	// be sure to name the functions correctly:
	// inner_shade, inner_shade_4, inner_shade_4_commonOrigin
	jitRT::resolveThreadedRSLCode(buildModulePtr.get(), sm->runtimeModule.get(), glue, typeMap, textureMap, ANYSL_USE_COMMON_ORG, USE_DERIVATIVES, packetize, use_sse41);

    llvm::Module * module = buildModulePtr.get();
    RTANYSL_ASSERT(module != 0, "linked shader module has to exist!");
	
	engine = jitRT::getExecutionEngine(module);
	targetData = jitRT::getTargetData(module);
	initFn = jitRT::getFunction("init", module);

    // optimize loop-wrapper after inlining
    // - remove all load/stores related to unused predefs,
    // - remove opacity-related branch/trace-call if not used,
    // - etc.
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_true1", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_false1", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_true4", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_false4", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_true16", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_false16", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_true64", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_false64", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_true256", module));
    jitRT::optimizeFunction(jitRT::getFunction("generic_masked_shade_false256", module));

    //
    // replace extern calls to RTFACT API functions by calls to those
    // implemented in RTAnySLShaderAPI.cpp
    // NOTE: t_Context is currently configured RTremote context and is declared in
    // RTremotePrivateConfig.hpp file
    //
    RTAnySL::Utils::linkAPIFuncTable(module, ShaderAPI<t_Context>::getAPIFuncTable());


    // now module is ready for shading, prepare entry table
    
	//
	// inline calls to parameter accessor functions
	//
	llvm::Function * paramGetNameFn =
		jitRT::getFunction("ar_param_get_name", module);
	llvm::Function * paramGetTypeFn =
		jitRT::getFunction("ar_param_get_type", module);
	llvm::Function * paramGetNFn =
		jitRT::getFunction("ar_param_get_n", module);
	llvm::Function * paramGetAddrFn =
		jitRT::getFunction("ar_param_get_addr", module);

	if (paramGetNameFn) jitRT::inlineFunctionCalls(paramGetNameFn);
	if (paramGetTypeFn) jitRT::inlineFunctionCalls(paramGetTypeFn);
	if (paramGetNFn) jitRT::inlineFunctionCalls(paramGetNFn);
	if (paramGetAddrFn) jitRT::inlineFunctionCalls(paramGetAddrFn);

#define JITRT_GETPARAMADDR_BUG
#ifndef JITRT_GETPARAMADDR_BUG
	if (paramGetAddrFn)
		jitRT::replaceGEPs(paramGetAddrFn, typeMap);
#endif

	
    shaderContext.entryTable.setModule(module);

    RTAnySL::Singleton<RTfactShaderEntryTableMaker>::getInstance().
        makeEntryTable(&shaderContext.entryTable);

    if (initParamsAndTextures)
    {

        // determine all shader parameters

        if (paramGetNameFn && paramGetTypeFn && paramGetNFn && paramGetAddrFn)
        {
            typedef const char * (*ParamGetNameFunc)(int index);
            typedef char (*ParamGetTypeFunc)(int index);
            typedef int (*ParamGetN)();
            typedef void * (*ParamGetAddr)(void * paramStruct, int index);

            void * dummyParams = allocateAndInitShaderParams();

            ParamGetNameFunc paramGetName =
                (ParamGetNameFunc)jitRT::getPointerToFunction(
                    engine, paramGetNameFn);

            ParamGetTypeFunc paramGetType =
                (ParamGetTypeFunc)jitRT::getPointerToFunction(
                    engine, paramGetTypeFn);

            ParamGetN paramGetN =
                (ParamGetN)jitRT::getPointerToFunction(
                    engine, paramGetNFn);

#ifndef JITRT_GETPARAMADDR_BUG
            ParamGetAddr paramGetAddr =
                (ParamGetAddr)jitRT::getPointerToFunction(
                    engine, paramGetAddrFn);
#else
            size_t paramOffset = 0;
#endif

            int numParams = paramGetN();
            for (int i = 0; i < numParams; i++)
            {
                const char * paramName = paramGetName(i);
                char paramType = paramGetType(i);

                RTANYSL_IFDEBUG( checkRSLParamName(paramName); )

                RTANYSL_DEBUG("RSLShader::RSLShader: paramName=\""<<paramName<<"\"");
                RTANYSL_DEBUG("RSLShader::RSLShader: paramType=\""<<paramType<<"\"");

#ifndef JITRT_GETPARAMADDR_BUG
                void * paramPtr = paramGetAddr(dummyParams, i);
                // compute parameter offset in a parameter struct
                size_t paramOffset =
                    reinterpret_cast<char*>(paramPtr) -
                    reinterpret_cast<char*>(dummyParams);
#else
#endif

                RTAnySL::ParameterInfo pinfo;
                pinfo.name = paramName;
                pinfo.semantic = RTAnySL::ParameterInfo::DEFAULT_SEMANTIC;

                bool validParam = true;

                switch (paramType)
                {
                    case 'f': // float
                        pinfo.setToType<float>();
                        break;
                    case 'v': // vector
                        pinfo.setToType<float[3]>();
                        pinfo.semantic = RTAnySL::ParameterInfo::VECTOR_SEMANTIC;
                        break;
                    case 'n': // normal
                        pinfo.setToType<float[3]>();
                        pinfo.semantic = RTAnySL::ParameterInfo::NORMAL_SEMANTIC;
                        break;
                    case 'p': // point
                        pinfo.setToType<float[3]>();
                        pinfo.semantic = RTAnySL::ParameterInfo::POINT_SEMANTIC;
                        break;
                    case 'c': // color
                        pinfo.setToType<float[3]>();
                        pinfo.semantic = RTAnySL::ParameterInfo::COLOR_SEMANTIC;
                        break;
                    case 'm': // matrix
                        pinfo.setToType<float[4*4]>();
                        pinfo.semantic = RTAnySL::ParameterInfo::MATRIX_SEMANTIC;
                        break;
                    case 's': // string
                        pinfo.setToType<char*>();
                        pinfo.semantic = RTAnySL::ParameterInfo::STRING_SEMANTIC;
                        break;
                    default:
                        RTANYSL_ERROR("Unkown RSL type : "<<paramType);
                        validParam = false;
                        break;
                }

                if (validParam)
                {
                    parameterInfos.push_back(pinfo);
                    parameterOffsets.push_back(paramOffset);
#ifdef JITRT_GETPARAMADDR_BUG
                    paramOffset += pinfo.getParameterSize();
#endif
                }
            }

            freeShaderParams(dummyParams);
        }

        // process textures in order to create texture parameters
        numTextures = static_cast<int>(textureMap.size());

        // parameterInfos will contain all RenderMan parameters plus all textures
        unsigned int texParamIndex = static_cast<unsigned int>(parameterInfos.size());
        parameterInfos.resize(parameterInfos.size()+numTextures);

        typedef std::map<const std::string, const unsigned>::const_iterator Iter;
        for (Iter it = textureMap.begin();
             it != textureMap.end(); ++it, ++texParamIndex)
        {
            // names that we have from LLVM are string constants, and LLVM
            // provides them completely i.e. with the leading zero
            // so we need to trim.
            std::string textureName;
            if (it->first.length() != 0 && it->first[it->first.length()-1] == '\0')
            {
                textureName = it->first.substr(0, it->first.length()-1);
            }
            else
            {
                // this should actually not happen
                RTANYSL_ASSERT(false, "this case should not happen. check !!!");
                textureName = it->first;
            }

            std::cout<<"TEX "<<textureName<<" index "<<it->second<<std::endl;
            RTAnySL::ParameterInfo & pinfo = parameterInfos[texParamIndex];
            pinfo.name = textureName;
            pinfo.description = "";
            pinfo.semantic = RTAnySL::ParameterInfo::TEXTURE_SEMANTIC;
            // We accept pointers to internal RTfact texture data
            pinfo.setToType<void*>();
        }

        RTANYSL_IFDEBUG( RTANYSL_DEBUG("RSL params: "<<getNumRSLParameters()); )
        RTANYSL_IFDEBUG( RTANYSL_DEBUG("RSL textures: "<<getNumTextures()); )

        // allocate and initialize shader parameters (stored in returned void*)
        shaderContext.paramPtr = allocateAndInitShaderParams();
        // allocate and initialize texture table
        shaderContext.texturePtr = allocateAndInitTextureTable();
    }

    DEBUG_ANYSL( jitRT::writeModuleToFile(module, "TESTF.ll"); )
}

// -- options support --

RTAnySL::OptionValue RSLShaderInstance::getOptionValueImpl(
        const std::string &name) const
{
    if (name == RTANYSL_PACKETIZE_OPTION_NAME)
        return RTAnySL::OptionValue(packetize);
    else if (name == RTANYSL_SPECIALIZE_OPTION_NAME)
        return RTAnySL::OptionValue(specialize);
    else
        return RTAnySL::OptionValue();
}

bool RSLShaderInstance::setOptionValueImpl(
        const std::string &name, const RTAnySL::OptionValue &value)
{
    if (name == RTANYSL_PACKETIZE_OPTION_NAME)
    {
        if (packetize != value.toBool())
            needsRebuild = true;
        packetize = value.toBool();
        RTANYSL_DEBUG("Found option : packetize = "<<packetize);
        return true;
    }
    if (name == RTANYSL_SPECIALIZE_OPTION_NAME)
    {
        if (specialize != value.toBool())
            needsRebuild = true;
        specialize = value.toBool();
        RTANYSL_DEBUG("Found option : specialize = "<<specialize);
        return true;
    }
    return false;
}

void RSLShaderInstance::getOptionsImpl(RTAnySL::OptionsList &options)
{
    options.push_back(RTANYSL_PACKETIZE_OPTION(packetize));
    options.push_back(RTANYSL_SPECIALIZE_OPTION(specialize));
}

// -- parameters support --

int RSLShaderInstance::getParamIndexImpl(const std::string &name) const
{
    return findParameterByName(name);
}

RTAnySL::ParameterInfo RSLShaderInstance::getParamInfoImpl(int index) const
{
    return getParameterInfo(index);
}

void RSLShaderInstance::beginParamsImpl()
{
}

void RSLShaderInstance::endParamsImpl()
{
    RTANYSL_DEBUG("endParams :: needsRebuild="<<needsRebuild
                  <<" | needsUpdate="<<needsUpdate
                  <<" | specialize="<<specialize
                  <<" | packetize="<<packetize);

    if (!needsRebuild && !needsUpdate)
        return;

    if (specialize)
    {
        //in any case, recompile a fully specialized shader
        RTANYSL_LOG("Recompiling in specialized mode");
        RTANYSL_LOG("WARNING: specialization currently needs to be disabled for texture support!");

        resetModule();

		typedef RTAnySL::ParameterCache::IndexMapConstIterator Iter;
		for (Iter it = parameters.indexMapBegin(), E = parameters.indexMapEnd(); it != E; ++it)
        {
			const std::string name = it->first;
            const int paramIndex = it->second;
            const void * data = parameters.getParameterValuePtr(paramIndex);

            specializeParam(name, data);
        }

        rebuildModule();
    }
    else  //set global values
    {
        // if we need a rebuild, recompile the shader without specialized globals
        // if we only need a parameter-update we do nothing
        if (needsRebuild)
        {
            RTANYSL_DEBUG("Recompiling in dynamic mode");

            resetModule();
            rebuildModule();
        }
		else if (needsUpdate)
        {
            // if parameters are changed, copy them from cache to the module

			// Because we generate shaders with -m32 flag (to circumvent llvm bug 6194)
			// we have to be sure that the execution engine does not generate i386
			// code on i386-64.
			// As we only need this for CXX shaders, it is better not to push that
			// into 'getExecutionEngine'
			jitRT::setTargetData(buildModulePtr.get(), "", "");

            llvm::ExecutionEngine * engine =
                jitRT::getExecutionEngine(buildModulePtr.get());
            parameters.setModuleGlobalVariables(engine, buildModulePtr.get());
        }
    }

    needsUpdate = false;
    needsRebuild = false;
}

bool RSLShaderInstance::getParamImpl(int index, void *data, int byteSize)
{
    if (index < 0 || index >= static_cast<int>(getNumParameters()))
        return false;

    const RTAnySL::ParameterInfo &pinfo = getParameterInfo(index);

    if (pinfo.getParameterSize() != byteSize)
        return false; // throw Exception instead ?

    // when parameter is a texture we need to use texture table
    if (pinfo.semantic == RTAnySL::ParameterInfo::TEXTURE_SEMANTIC)
    {
        RTANYSL_ASSERT(byteSize == sizeof(void*), "texture is not encoded as a pointer");
        RTANYSL_ASSERT(index >= getNumRSLParameters(), "parameter must be texture");

        const unsigned int textureIndex = index - getNumRSLParameters();
        const size_t offset = textureIndex * sizeof(void*);

        const void * srcPtr =
                reinterpret_cast<char*>(shaderContext.texturePtr)+offset;

        ::memcpy(data, srcPtr, byteSize);
    }
    else
    {
        RTANYSL_ASSERT(index < getNumRSLParameters(), "parameter must be no texture");

        // get pointer into shader parameter struct
        const size_t offset = getParameterOffset(index);

        const void * srcPtr =
                reinterpret_cast<char*>(shaderContext.paramPtr)+offset;

        ::memcpy(data, srcPtr, byteSize);
    }

    return true;
}

bool RSLShaderInstance::setParamImpl(int index, const void *data, int byteSize)
{
    if (index < 0 || index >= static_cast<int>(getNumParameters()))
        return false;

    const RTAnySL::ParameterInfo &pinfo = getParameterInfo(index);

    if (pinfo.getParameterSize() != byteSize)
        return false; // throw Exception instead ?

    // when parameter is a texture we need to use texture table
    if (pinfo.semantic == RTAnySL::ParameterInfo::TEXTURE_SEMANTIC)
    {
        RTANYSL_ASSERT(byteSize == sizeof(void*), "texture is not encoded as a pointer");
        RTANYSL_ASSERT(index >= getNumRSLParameters(), "parameter must be texture");

        const unsigned int textureIndex = index - getNumRSLParameters();
        const size_t offset = textureIndex * sizeof(void*);

        void * destPtr = reinterpret_cast<char*>(shaderContext.texturePtr)+offset;

        ::memcpy(destPtr, data, byteSize);
    }
    else
    {
        RTANYSL_ASSERT(index < getNumRSLParameters(), "parameter must be no texture");

        // get pointer into shader parameter struct
        const size_t offset = getParameterOffset(index);

        void * destPtr = reinterpret_cast<char*>(shaderContext.paramPtr)+offset;

        ::memcpy(destPtr, data, byteSize);
    }

    needsUpdate = true;

    return true;
}

void RSLShaderInstance::setParamsFromImpl(
    const RTAnySL::ShaderInstancePtr &source)
{
    RSLShaderInstancePtr other =
        boost::dynamic_pointer_cast<RSLShaderInstance>(source);

    // TODO : do generic copy of parameters
}

void RSLShaderInstance::getParamInfosImpl(RTAnySL::ParameterInfoVector &infos)
{
    infos = parameterInfos;
}


void * RSLShaderInstance::allocateAndInitShaderParams()
{
    // allocate and initialize shader parameters (stored in returned void*)
    return jitRT::executeVoidPtrFunction(initFn, engine);
}

void RSLShaderInstance::freeShaderParams(void * paramPtr)
{
    ::free(paramPtr);
}

void * RSLShaderInstance::allocateAndInitTextureTable()
{
    // allocate and initialize texture table (stored in returned void*)
    if (getNumTextures() != 0)
    {
        const size_t tableSize = sizeof(void*) * getNumTextures();
        void * table = ::malloc(tableSize);
        ::memset(table, 0, tableSize);
        return table;
    }
    return 0;
}

void RSLShaderInstance::freeTextureTable(void * texturePtr)
{
    ::free(texturePtr);
}


int RSLShaderInstance::findParameterByName(const std::string &name) const
{
    for (int i = 0; i < parameterInfos.size(); i++)
    {
        if (parameterInfos[i].name == name)
            return i;
    }
    return -1;
}

bool RSLShaderInstance::specializeParam(const std::string & paramName,
                                        const void * value)
{
	// TODO: this has to be implemented different from CXX shaders
	//       (parameters are no global values but stored in params-struct)

//    const char * rawData = static_cast<const char*>(value);
//    const llvm::GlobalValue * global = jitRT::getGlobal(buildModulePtr.get(),
//                                                        paramName);
//    if (global)
//    {
//        const llvm::Type * type = jitRT::getType(global);
//        llvm::Constant * constant = jitRT::createConstant(type, rawData);
//        jitRT::specializeGlobal(paramName, constant, buildModulePtr.get());
//        return true;
//    }
//    else
//    {
//        RTANYSL_ERROR("Could not find global value '"<<paramName<<"' for specialization!");
//        return false;
//    }
	return true;
}

void RSLShaderInstance::genericShadeFunc(
    const RTAnySL::ShaderInstanceContext * shaderInstanceContext,
    bool taCommonOrg,
    uint taSize,
    const void * aRayPacket,
    const void * aRayMask,
    void * aIntersection,
    void * aContext,
    void * oResult)
{
    RTANYSL_ASSERT(shaderInstanceContext != 0, "context is NULL");

    const RTfactRSLGenericShadeFunction shadeFunc =
        static_cast<const RSLSIContext*>(shaderInstanceContext)->
        entryTable.getEntryFuncPtr<RTfactRSLGenericShadeFunction>(
            RTfactShaderEntryTableMaker::getShadeFuncIndex(taCommonOrg,
                                                           taSize));
    void * paramPtr =
        static_cast<const RSLSIContext*>(shaderInstanceContext)->paramPtr;
    void * texturePtr =
        static_cast<const RSLSIContext*>(shaderInstanceContext)->texturePtr;

    assert(shadeFunc && "trying to call uninitialized shader shadeFunc=NULL");

    (*shadeFunc)(aContext, paramPtr, texturePtr, ((t_Context*)aContext)->time, aRayPacket, aRayMask, aIntersection, oResult);
}



//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
// RSLShader
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

RSLShader::~RSLShader()
{
    RTANYSL_DEBUG("Destroyed RSLShader");
}

RSLShader::RSLShader(const RTAnySL::ShaderManagerPtr &shaderManager,
                     const RTAnySL::ShaderPtr &parentShader,
                     llvm::Module * _module) :
    RTAnySL::Shader(shaderManager, parentShader, _module)
{
}

RTAnySL::ShaderInstancePtr RSLShader::createInstance(
    const RTAnySL::OptionsList *options)
{
    RTANYSL_ASSERT(boost::dynamic_pointer_cast<RSLShaderManager>(getManager()) != 0, "manager must be an instance of RSLShaderManager class");

    return boost::static_pointer_cast<RSLShaderManager>(getManager())->
        createShaderInstance(shared_from_this(), options);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
// RSLShaderManager
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

#define _RSL_SHMAN_RUNTIME_MODULE "runtime.bc"
#define _RSL_SHMAN_GLUE_MODULE "RSLGlue.bc"
#define _RSL_SHMAN_PACKET_GLUE_MODULE "RSLPacketGlue.bc"

RSLShaderManager::RSLShaderManager()
{
    // initialize
    RTANYSL_DEBUG("Initializing RTfact/RSLShaderManager");

    const RTAnySL::Utils::PathFinder &pf =
        RTAnySL::Utils::getBitcodePathFinder();

    std::string runtimeModuleFile =
        pf.findPath(_RSL_SHMAN_RUNTIME_MODULE);
    std::string glueModuleFile =
        pf.findPath(_RSL_SHMAN_GLUE_MODULE);
	std::string packetGlueModuleFile =
        pf.findPath(_RSL_SHMAN_PACKET_GLUE_MODULE);

    assert(runtimeModuleFile.length() != 0 && "Could not find runtime bitcode file");
    assert(glueModuleFile.length() != 0 && "Could not find glue bitcode file");
    assert(packetGlueModuleFile.length() != 0 && "Could not find packet glue bitcode file");

    // ### Loading gadgets ###
    runtimeModule.reset(
        jitRT::createModuleFromFile(runtimeModuleFile));
    glueModule.reset(
        jitRT::createModuleFromFile(glueModuleFile));
    packetGlueModule.reset(
        jitRT::createModuleFromFile(packetGlueModuleFile));

    jitRT::setModuleIdentifier(runtimeModule.get(), "RSL Runtime");
    jitRT::setModuleIdentifier(glueModule.get(), "RSL Glue");
    jitRT::setModuleIdentifier(packetGlueModule.get(), "RSL Packet Glue");

    RTANYSL_IFDEBUG( jitRT::verifyModule(runtimeModule.get()); )
    RTANYSL_IFDEBUG( jitRT::verifyModule(glueModule.get()); )
    RTANYSL_IFDEBUG( jitRT::verifyModule(packetGlueModule.get()); )

    RTANYSL_DEBUG("RTfact/RSLShaderManager ready!");
}

RSLShaderManager::~RSLShaderManager()
{
    RTANYSL_DEBUG("Destroyed RTfact/RSLShaderManager !");
}

std::string RSLShaderManager::getShadingLanguage() const
{
    return "RSL";
}

bool RSLShaderManager::isModuleSupported(llvm::Module *module) const
{
    // check if required entry points (shade functions) do exist
    // in the module
    return (jitRT::getFunction("ar_param_get_name", module) != 0 &&
            jitRT::getFunction("ar_param_get_n", module) != 0);
}

RTAnySL::ShaderPtr RSLShaderManager::createShader(llvm::Module *module)
{
    RSLShaderPtr s = RSLShaderPtr(
        new RSLShader(shared_from_this(), RTAnySL::ShaderPtr(), module));

    return s;
}

RTAnySL::ShaderPtr RSLShaderManager::buildShader(
    const std::string &shaderScript,
    RTAnySL::ShaderCompilerResult * result)
{
    if (shaderScript.length() != 0)
    {
        std::string bcModuleFileName;
        bool ok = buildRSLShader(shaderScript, bcModuleFileName, result);

        if (ok)
        {
            RTANYSL_LOG("Compilation successful"
                        " (BC module in '"<<bcModuleFileName<<"' file)");

            RTAnySL::ModuleAutoPtr module =
                jitRT::createModuleFromFile(bcModuleFileName);

            if (module.get())
            {
                RTANYSL_LOG("Successfully loaded shader module"
                            " from '"<<bcModuleFileName<<"' file");

                RTAnySL::Utils::removeFile(bcModuleFileName);
            }

            RTAnySL::ShaderPtr s = createShader(module.get());

            module.release();

            return s;
        }
        else
        {
            RTANYSL_WARNING(
                "Compilation of shader script '"<<shaderScript<<"' failed");
        }
    }
    else
    {
        if (result)
            result->set(1, "no shader source code");
    }
    return RTAnySL::ShaderPtr();
}

bool RSLShaderManager::buildRSLShader(
    const std::string & shaderScript,
    std::string & bcModuleFileName,
    RTAnySL::ShaderCompilerResult * result)
{
    const char * path = ::getenv(ANYSL_RSL_SHADER_BUILDER_ENVVAR);

    if (path == 0 || ::strlen(path) == 0)
    {
        RTANYSL_WARNING("no shader builder for RenderMan shading language"
                        " specified, please set "
                        <<ANYSL_RSL_SHADER_BUILDER_ENVVAR
                        <<" environment variable.");
        return false;
    }

    std::string rslShaderBuilder(path);

    std::string tmpShaderFileName =
        RTAnySL::Utils::makeTempFileName(
            RTfact::IO::appendPath(RTAnySL::Utils::getBuildDir(), "rslshader"));
    std::string tmpBCModuleFileName =
        RTAnySL::Utils::makeTempFileName(
            RTfact::IO::appendPath(RTAnySL::Utils::getBuildDir(), "bcmodule"));

    std::string tmpOutFileName;
    if (result)
    {
        tmpOutFileName =
                RTAnySL::Utils::makeTempFileName(
                        RTfact::IO::appendPath(
                                    RTAnySL::Utils::getBuildDir(), "cmplout"));
    }

    std::ofstream out(tmpShaderFileName.c_str());
    out<<shaderScript;
    out.close();

    RTANYSL_DEBUG("shader script in file '"<<tmpShaderFileName<<"'");

    std::string cmdLine =
        rslShaderBuilder+" "+
        tmpShaderFileName+" "+
        tmpBCModuleFileName;

    if (result)
        cmdLine += " " + tmpOutFileName;

    int ret = ::system(cmdLine.c_str());

    if (result)
    {
        result->setErrorCode(ret);
        std::ifstream ifs(tmpOutFileName.c_str());
        std::ostringstream oss;
        oss<<"shaderscript:"<<tmpShaderFileName<<"\n";
        oss<<ifs.rdbuf();
        ifs.close();

        result->setOutput(oss.str());
        RTAnySL::Utils::removeFile(tmpOutFileName);
    }

    RTAnySL::Utils::removeFile(tmpShaderFileName);

    RTANYSL_DEBUG("Compilation result: "<<ret);

    if (ret == 0 && RTAnySL::Utils::isFileExists(tmpBCModuleFileName))
    {
        bcModuleFileName = tmpBCModuleFileName;
        return true;
    }

    return false;
}

RTAnySL::ShaderInstancePtr RSLShaderManager::createShaderInstance(
    const RSLShaderPtr &shader,
    const RTAnySL::OptionsList *options)
{
    RTAnySL::ShaderInstancePtr shaderInstance(new RSLShaderInstance(shader));

    if (options)
        shaderInstance->setOptions(*options);

    return shaderInstance;
}

RTANYSL_STATIC_INIT_FUNC
{
    RTANYSL_REGISTER_SHADER_MANAGER(RSLShaderManager);
}

#undef _RSL_SHMAN_RUNTIME_MODULE
#undef _RSL_SHMAN_GLUE_MODULE
#undef _RSL_SHMAN_PACKET_GLUE_MODULE

RTFACT_REMOTE_NAMESPACE_END
