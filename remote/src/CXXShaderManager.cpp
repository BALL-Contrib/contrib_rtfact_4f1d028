#include "Precompiled.hpp"

#define RTFACT_REMOTE_LIB
#define RTANYSL_LIB
#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>
#include <RTremote/Config/Common.hpp>
#include "CXXShaderManager.hpp"
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

RTFACT_REMOTE_NAMESPACE_BEGIN

#define JITRT_CXX_SHADER_BUILDER_ENVVAR "JITRT_CXX_SHADER_BUILDER"

//TODO: where should this define be set best?
#define ANYSL_USE_COMMON_ORG

typedef void (*RTfactCXXGenericShadeFunction)
(
    void * context,
    const void * aRayPacket,
    const void * aRayMask,
    void * aIntersection,
    void * oResult
);

// CXXShaderInstance

CXXShaderInstance::CXXShaderInstance(
    const RTAnySL::ShaderPtr &shader) :
    RTAnySL::ShaderInstance(shader),
    buildModule(),
    packetize(false),
    specialize(true),
    needsUpdate(true),
    needsRebuild(true),
    parameters(),
    shaderContext(),
    dataEps(0),
    dataInf(0)
{
    RTANYSL_ASSERT(shader.get() != 0, "shader should not be NULL");
    RTANYSL_ASSERT(shader->getModule() != 0, "shader's module should not be NULL");

    resetModule();

    rebuildModule();
}

CXXShaderInstance::~CXXShaderInstance()
{
    RTANYSL_DEBUG("Destroyed CXXShaderInstance");

    if (dataEps)
        free(dataEps);

    if (dataInf)
        free(dataInf);
}

const RTAnySL::ShaderEntryTable & CXXShaderInstance::getEntryTableImpl()
{
    return shaderContext.entryTable;
}

const RTAnySL::ShaderInstanceContext & CXXShaderInstance::getContextImpl()
{
    return shaderContext;
}

void * CXXShaderInstance::getGenericShadeFuncImpl()
{
    return reinterpret_cast<void*>(&genericShadeFunc);
}

void CXXShaderInstance::init()
{
}

namespace {

/// TODO: following code should be common for CXX shaders and CXXShaderManager

struct CXXParamInfo
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

void setAnySLElemTypeFromCXX(
    const CXXParamInfo &src, RTAnySL::ParameterInfo &dest)
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


void CXXShaderInstance::resetModule()
{
    // reset build module to the fresh (non specialized and non packetized)
    // version
    llvm::Module * sourceModule = getShader()->getModule();
    buildModule.reset(jitRT::cloneModule(sourceModule));

	RTANYSL_IFDEBUG( jitRT::writeModuleToFile(buildModule.get(), "TEST0.ll"); );

    const bool initialize = (parameters.size() == 0);

    if (initialize)
    {
        // Because we generate shaders with -m32 flag (to circumvent llvm bug 6194)
        // we have to be sure that the execution engine does not generate i386
        // code on i386-64.
        // As we only need this for CXX shaders, it is better not to push that
        // into 'getExecutionEngine'
        jitRT::setTargetData(buildModule.get(), "", "");

        // create parameters list
        const int numParams =
            parameters.initFromModule(buildModule.get());

        RTANYSL_DEBUG("CXXShaderInstance: Found "
                      <<numParams<<" parameters in shader");

        RTANYSL_IFDEBUG( parameters.dump(std::cout); )
    }

    // create initializers and so replace external linkage of global variables
    // (i.e. our parameters)
    parameters.createZeroInitializersForGlobalVariables(buildModule.get());

    if (initialize)
    {
        // copy default parameters into parameter cache

        llvm::ExecutionEngine * engine =
            jitRT::getExecutionEngine(buildModule.get());

        // allocate memory for global variables by setting them to zero
        // (zeros are coming from default value of parameters after
        //  call to ParameterCache::initFromModule)
        parameters.setModuleGlobalVariables(engine, buildModule.get());

        //execute initialization of global variables of the new module
        if (llvm::Function * getParamInfoFn = jitRT::getFunction("cxx_get_param_info_and_init", buildModule.get()))
        {
            void * fnPtr = jitRT::getPointerToFunction(engine, getParamInfoFn);

            typedef const CXXParamInfo * (*FnType)();
            const CXXParamInfo * params = ((FnType)fnPtr)();

            // we need to synchronize indices of parameter cache and
            // parameter infos so we can use index map in parameter cache
            // for searching
            parameterInfos.resize(parameters.size());

            if (params)
            {
                while (params->name != 0 && params->typeName != 0)
                {
                    RTANYSL_DEBUG("PARAM NAME: "<<params->name);
                    RTANYSL_DEBUG("PARAM SEMANTIC ID: "<<params->semanticID);
                    RTANYSL_DEBUG("PARAM ELEMENT TYPE ID: "<<params->elementTypeID);
                    RTANYSL_DEBUG("PARAM NUM ELEMENTS: "<<params->numElements);
                    RTANYSL_DEBUG("PARAM TYPE NAME: "<<(params->typeName ? params->typeName : "NONE")<<" PTR "<<(void*)params->typeName);
                    RTANYSL_DEBUG("PARAM DESCR: "<<(params->descr ? params->descr : "NONE"));

                    int index = parameters.getParameterIndex(params->name);

                    assert(index >= 0 && index < parameterInfos.size());

                    // encode
                    setAnySLElemTypeFromCXX(*params, parameterInfos[index]);

                    ++params;
                }
            }

            jitRT::freeMachineCodeForFunction(buildModule.get(), getParamInfoFn);
        }

        // read back all initialized module variables
        parameters.getModuleGlobalVariables(engine, buildModule.get());

        RTANYSL_IFDEBUG(
			parameters.dump(std::cout);

			std::cout<<"-- infos --"<<std::endl;
			for (RTAnySL::ParameterInfoVector::const_iterator
					 it = parameterInfos.begin(); it != parameterInfos.end(); ++it)
			{
				std::cout<<*it<<std::endl;
			}
			std::cout<<"-- end of infos --"<<std::endl;
		)
    }

	RTANYSL_IFDEBUG( jitRT::writeModuleToFile(buildModule.get(), "TEST1.ll"); );
}

void CXXShaderInstance::rebuildModule()
{
    RTANYSL_ASSERT(boost::dynamic_pointer_cast<CXXShaderManager>(getShader()->getManager()) != 0, "manager must be an instance of CXXShaderManager class");

    // get modules from shader manager
    CXXShaderManagerPtr sm = boost::static_pointer_cast<CXXShaderManager>(getShader()->getManager());

	RTANYSL_ASSERT(buildModule.get(), "build module must be initialized!");
	RTANYSL_IFDEBUG( jitRT::writeModuleToFile(buildModule.get(), "TEST2.ll"); );

	// optimize the scalar code (required in any case because taSize = 1 also requires scalar shader)
    jitRT::optimizeFunction("masked_shade", buildModule.get());
	jitRT::inlineFunctionInModule("masked_shade", buildModule.get());


    // packetize & link against RTfact

    RTAnySL::ModuleAutoPtr clonedShaderGlueModule;

    if (packetize && !specialize)
    {
        // This deactivation (see next condition (packetize && specialize) )
        // is currently required as in some cases
        // (only happened with BrickShader.bc) Packetizer will crash
        // when packetization is enabled but specialization not.

        RTANYSL_WARNING("WARNING: Deactivated packetization due to the problems in non fully specialized mode");
    }

    if (packetize && specialize)
    {
		llvm::Module * shaderGlueModule = sm->shaderGlueModule.get();
        RTANYSL_ASSERT(shaderGlueModule, "shaderGlueModule is NULL");
        clonedShaderGlueModule.cloneFrom(shaderGlueModule);

#ifdef RTFACT_HAVE_SSE41
        Packetizer::Packetizer * packetizer = Packetizer::getPacketizer(true);
#else
        Packetizer::Packetizer * packetizer = Packetizer::getPacketizer(false);
#endif
        Packetizer::addFunctionToPacketizer(packetizer, "masked_shade", "masked_shade_4", 4);
#ifdef ANYSL_USE_COMMON_ORG
		Packetizer::addFunctionToPacketizer(packetizer, "masked_shade", "masked_shade_4_commonOrigin", 4);
#endif

	// add native functions

        struct ScalarToVecFuncMap
        {
            const char * scalarCall;
            const char * vecCall;
            int maskPos;
        };

		static ScalarToVecFuncMap scalarToVecFuncMap [] = {
			{"RTFACT_getLC_1", "RTFACT_getLC_4", 1},
			{"RTFACT_trace_1", "RTFACT_trace_4", 1},
			{"RTFACT_sampleLight_1", "RTFACT_sampleLight_4", 1},
			{"RTFACT_sampleDT_1", "RTFACT_sampleDT_4", 1},
			{"RTFACT_sampleDT_id_1", "RTFACT_sampleDT_id_4", 1},
			{"RTFACT_getNumLight", "RTFACT_getNumLight", -1},
			{0, 0, 0}
		};

        ScalarToVecFuncMap * cur = scalarToVecFuncMap;

        while (cur->scalarCall != 0)
        {
            const char * funcName = cur->vecCall;
            llvm::Function * apiFunc = jitRT::getFunction(funcName, buildModule.get());

            if (apiFunc) {
                RTANYSL_DEBUG("added packetized API function " << funcName);
				const bool forcePacketization = false; //if the call does not have to be packetized, don't force it
                Packetizer::addNativeFunctionToPacketizer(packetizer, cur->scalarCall, cur->maskPos, apiFunc, forcePacketization);
            } else {
                RTANYSL_WARNING("could not find packetized API function '" << funcName << "'!");
            }

            ++cur;
        }

        // packetize
        Packetizer::runPacketizer(packetizer, buildModule.get());

        llvm::Function * shadeFuncSSE = jitRT::getFunction("masked_shade_4", buildModule.get());
        RTANYSL_ASSERT(shadeFuncSSE, "Could not find SSE shade function in build module!");
		jitRT::inlineFunctionInModule("masked_shade_4", buildModule.get());

#ifdef ANYSL_USE_COMMON_ORG
        llvm::Function * shadeFuncCommonOrg = jitRT::getFunction("masked_shade_4_commonOrigin", buildModule.get());
        RTANYSL_ASSERT(shadeFuncCommonOrg, "Could not find SSE common origin shade function in build module!");
		jitRT::inlineFunctionInModule("masked_shade_4_commonOrigin", buildModule.get());
#endif

    } else {
		// no packetization: link scalar wrapper

		llvm::Module * scalarWrapperModule = sm->scalarWrapperModule.get();
        RTANYSL_ASSERT(scalarWrapperModule, "scalarWrapperModule is NULL");

        clonedShaderGlueModule.cloneFrom(scalarWrapperModule);
    }

    RTANYSL_ASSERT(clonedShaderGlueModule.get(), "clonedShaderGlueModule is NULL");

	RTANYSL_IFDEBUG( jitRT::writeModuleToFile(clonedShaderGlueModule.get(), "TEST3.ll"); );

	//
    // now link shader glue into buildModule
	//

    // We need to release buildModule because there are some issues with
    // packetization and usage of native functions like llvm.sqrt.f32.
    // Following assertion will happen when we packetize and don't release
    // buildModule:
    // virtual llvm::Value::~Value():
    // Assertion `use_empty() &&
    // "Uses remain when a value is destroyed!"' failed.
    //std::vector<llvm::Module*> moduleVec;
    //moduleVec.push_back(buildModule.release());
    //buildModule.reset(jitRT::linkInModules(clonedShaderGlueModule.get(), moduleVec));

	// since ApiCalls.bc was removed, this here seems to be enough:
	jitRT::link(buildModule.get(), clonedShaderGlueModule.get());

//    assert(buildModule.get() == clonedShaderGlueModule.get());

    // linkInModules returns it's first argument i.e. the target module.
    // In order to avoid double deletion (buildModule and clonedShaderGlueModule
    // refer to the same module) we release clonedShaderGlueModule.
    clonedShaderGlueModule.release();

    RTANYSL_ASSERT(buildModule.get(), "buildModule is NULL");
    //moduleVec.clear();

	//
    // link against RTfact Shader API
	//

    // t_Context is currently configured RTremote context and is declared in
    // RTremotePrivateConfig.hpp file
    RTAnySL::Utils::linkAPIFuncTable(buildModule.get(), ShaderAPI<t_Context>::getAPIFuncTable());

//    jitRT::optimizeModule(buildModule.get());

//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_true1", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_false1", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_true4", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_false4", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_true16", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_false16", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_true64", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_false64", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_true256", buildModule.get()));
//    jitRT::inlineFunctionCalls(jitRT::getFunction("generic_masked_shade_false256", buildModule.get()));
    jitRT::optimizeFunction("generic_masked_shade_true1", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_false1", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_true4", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_false4", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_true16", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_false16", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_true64", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_false64", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_true256", buildModule.get());
    jitRT::optimizeFunction("generic_masked_shade_false256", buildModule.get());

    //--------------------------------------------------------------------//
    // temporary workaround for the "undefined globals" bug
    //--------------------------------------------------------------------//
	// Because we generate shaders with -m32 flag (to circumvent llvm bug 6194)
	// we have to be sure that the execution engine does not generate i386
	// code on i386-64.
	// As we only need this for CXX shaders, it is better not to push that
	// into 'getExecutionEngine'
	jitRT::setTargetData(buildModule.get(), "", "");

    llvm::ExecutionEngine * engine =
        jitRT::createExecutionEngine(buildModule.get());

    llvm::GlobalValue * packetEps = jitRT::getNamedGlobalValue(
        buildModule.get(), "_ZN6RTfact6PacketILj1EfE9C_RAY_EPSE");
    llvm::GlobalValue * packetInf = jitRT::getNamedGlobalValue(
        buildModule.get(), "_ZN6RTfact6PacketILj1EfE10C_INFINITYE");

    if (packetEps && packetInf)
    {
        const size_t bytes = sizeof(RTfact::Packet<1u, float>::C_RAY_EPS);

        if (!dataEps)
            dataEps = malloc(bytes);

        memcpy(dataEps, &RTfact::Packet<1u, float>::C_RAY_EPS, bytes);
        jitRT::addGlobalMapping(engine, packetEps, dataEps);

        if (!dataInf)
            dataInf = malloc(bytes);

        memcpy(dataInf, &RTfact::Packet<1u, float>::C_INFINITY, bytes);
        jitRT::addGlobalMapping(engine, packetInf, dataInf);
    }
    //--------------------------------------------------------------------//

    RTANYSL_IFDEBUG( jitRT::verifyModule(buildModule.get()); )
    RTANYSL_IFDEBUG( jitRT::writeModuleToFile(buildModule.get(), "shader_dump.ll"); )

    // build entry table
    shaderContext.entryTable.setModule(buildModule.get());

    RTAnySL::Singleton<RTfactShaderEntryTableMaker>::getInstance().
        makeEntryTable(&shaderContext.entryTable);

    // because module is recompiled and so changed,
    // set all parameters from parameter cache
    parameters.setModuleGlobalVariables(engine, buildModule.get());

    //RTANYSL_DEBUG("!!! In rebuild module parameters are set to:");
    //RTANYSL_IFDEBUG(parameters.dump(std::cout);)

	RTANYSL_IFDEBUG( jitRT::writeModuleToFile(buildModule.get(), "TESTF.ll"); );
}

// -- options support --

RTAnySL::OptionValue CXXShaderInstance::getOptionValueImpl(
        const std::string &name) const
{
    if (name == RTANYSL_PACKETIZE_OPTION_NAME)
        return RTAnySL::OptionValue(packetize);
    else if (name == RTANYSL_SPECIALIZE_OPTION_NAME)
        return RTAnySL::OptionValue(specialize);
    else
        return RTAnySL::OptionValue();
}

bool CXXShaderInstance::setOptionValueImpl(
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

void CXXShaderInstance::getOptionsImpl(RTAnySL::OptionsList &options)
{
    options.push_back(RTANYSL_PACKETIZE_OPTION(packetize));
    options.push_back(RTANYSL_SPECIALIZE_OPTION(specialize));
}

// -- parameters support --

int CXXShaderInstance::getParamIndexImpl(const std::string &name) const
{
    return parameters.getParameterIndex(name);
}

RTAnySL::ParameterInfo CXXShaderInstance::getParamInfoImpl(int index) const
{
    if (parameterInfos.empty())
    {
        RTAnySL::Parameter * p = parameters.getParameter(index);

        return RTAnySL::ParameterInfo(
            "", // TODO
            "",
            RTAnySL::ParameterType::fromType<unsigned char>(p->getSize()),
            RTAnySL::ParameterInfo::NO_SEMANTIC);
    }
    return parameterInfos[index];
}

void CXXShaderInstance::beginParamsImpl()
{
}

void CXXShaderInstance::endParamsImpl()
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
			jitRT::setTargetData(buildModule.get(), "", "");

            llvm::ExecutionEngine * engine =
                jitRT::getExecutionEngine(buildModule.get());
            parameters.setModuleGlobalVariables(engine, buildModule.get());
        }
    }

    needsUpdate = false;
    needsRebuild = false;
}

bool CXXShaderInstance::getParamImpl(int index, void *data, int byteSize)
{
    if (index < 0 || index >= static_cast<int>(parameters.size()))
        return false;

    RTAnySL::Parameter * param = parameters.getParameter(index);

    RTANYSL_ASSERT(param != 0, "parameter is NULL");

    if (param->getSize() != byteSize)
        return false; // throw Exception instead ?

    param->copyValueTo(data);
    needsUpdate = true;

    return true;
}

bool CXXShaderInstance::setParamImpl(int index, const void *data, int byteSize)
{
    if (index < 0 || index >= static_cast<int>(parameters.size()))
    {
        return false;
    }

    RTANYSL_DEBUG("setParamImpl:before");
    RTANYSL_IFDEBUG( parameters.dumpParameter(index, std::cout); )

    RTAnySL::Parameter * param = parameters.getParameter(index);

    RTANYSL_ASSERT(param != 0, "parameter is NULL");

    if (param->getSize() != byteSize)
    {
        return false; // throw Exception instead ?
    }

    param->copyValueFrom(data);
    needsUpdate = true;

    RTANYSL_DEBUG("setParamImpl:after");
    RTANYSL_IFDEBUG( parameters.dumpParameter(index, std::cout); )

    return true;
}

void CXXShaderInstance::setParamsFromImpl(
    const RTAnySL::ShaderInstancePtr &source)
{
    CXXShaderInstancePtr other =
        boost::dynamic_pointer_cast<CXXShaderInstance>(source);

    if (other != 0)
    {
        parameters.copyValuesFrom(other->parameters);

        needsUpdate = true;

        //RTANYSL_DEBUG("!!!! setParamsFromImpl ACTIVE !!!");
        //RTANYSL_DEBUG("parameters set to:");
        //RTANYSL_IFDEBUG( parameters.dump(std::cout); )
        //RTANYSL_DEBUG("-----------------");
    }
    else
    {
        // TODO : do generic copy of parameters
    }
}

void CXXShaderInstance::getParamInfosImpl(RTAnySL::ParameterInfoVector &infos)
{
    if (parameterInfos.empty())
    {
        typedef RTAnySL::ParameterCache::IndexMapConstIterator Iter;
        for (Iter it = parameters.indexMapBegin(), E = parameters.indexMapEnd();
             it != E; ++it)
        {
            RTAnySL::Parameter *p = parameters.getParameter(it->second);

            infos.push_back(RTAnySL::ParameterInfo(
                                it->first,
                                "",
                                RTAnySL::ParameterType::fromType<unsigned char>(p->getSize()),
                                RTAnySL::ParameterInfo::NO_SEMANTIC));
        }
    }
    else
    {
        infos = parameterInfos;
    }
}

bool CXXShaderInstance::specializeParam(const std::string & paramName,
                                        const void * value)
{
    const char * rawData = static_cast<const char*>(value);
    const llvm::GlobalValue * global = jitRT::getGlobal(buildModule.get(),
                                                        paramName);
    if (global)
    {
        const llvm::Type * type = jitRT::getType(global);
        llvm::Constant * constant = jitRT::createConstant(type, rawData);
        jitRT::specializeGlobal(paramName, constant, buildModule.get());
        return true;
    }
    else
    {
        RTANYSL_ERROR("Could not find global value '"<<paramName<<"' for specialization!");
        return false;
    }
}

void CXXShaderInstance::genericShadeFunc(
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

    RTfactCXXGenericShadeFunction shadeFunc =
        static_cast<const CXXContext*>(shaderInstanceContext)->
        entryTable.getEntryFuncPtr<RTfactCXXGenericShadeFunction>(
            RTfactShaderEntryTableMaker::getShadeFuncIndex(taCommonOrg,
                                                           taSize));

    assert(shadeFunc && "trying to call uninitialized shader shadeFunc=NULL");

    (*shadeFunc)(aContext, aRayPacket, aRayMask, aIntersection, oResult);
}

// CXXShader

CXXShader::~CXXShader()
{
    RTANYSL_DEBUG("Destroyed CXXShader");
}

CXXShader::CXXShader(const RTAnySL::ShaderManagerPtr &shaderManager,
                     const RTAnySL::ShaderPtr &parentShader,
                     llvm::Module * module) :
    RTAnySL::Shader(shaderManager, parentShader, module)
{
}

RTAnySL::ShaderInstancePtr CXXShader::createInstance(
    const RTAnySL::OptionsList *options)
{
    RTANYSL_ASSERT(boost::dynamic_pointer_cast<CXXShaderManager>(getManager()) != 0, "manager must be an instance of CXXShaderManager class");

    return boost::static_pointer_cast<CXXShaderManager>(getManager())->
        createShaderInstance(shared_from_this(), options);
}

// CXXShaderManager

#define _CXX_SHMAN_SHADER_GLUE_MODULE "ShaderGlue.bc"
#define _CXX_SHMAN_SCALAR_WRAPPER_MODULE "ScalarWrapper.bc"

CXXShaderManager::CXXShaderManager()
{
    // initialize
    RTANYSL_DEBUG("Initializing RTfact/CXXShaderManager");

    const RTAnySL::Utils::PathFinder &pf =
        RTAnySL::Utils::getBitcodePathFinder();

    std::string shaderGlueModuleFile =
        pf.findPath(_CXX_SHMAN_SHADER_GLUE_MODULE);
    std::string scalarWrapperModuleFile =
        pf.findPath(_CXX_SHMAN_SCALAR_WRAPPER_MODULE);

    assert(shaderGlueModuleFile.length() != 0 && "Could not find shader glue bitcode file");
    assert(scalarWrapperModuleFile.length() != 0 && "Could not find scalar wrapper bitcode file");

    // ### Loading gadgets ###
    shaderGlueModule.reset(
        jitRT::createModuleFromFile(shaderGlueModuleFile));
    scalarWrapperModule.reset(
        jitRT::createModuleFromFile(scalarWrapperModuleFile));

    jitRT::setModuleIdentifier(shaderGlueModule.get(), "Shader Glue");
    jitRT::setModuleIdentifier(scalarWrapperModule.get(), "Scalar Wrapper");

    RTANYSL_IFDEBUG( jitRT::verifyModule(shaderGlueModule.get()); )
    RTANYSL_IFDEBUG( jitRT::verifyModule(scalarWrapperModule.get()); )

    // ### Link container wrapper loop against scalar SSE wrapper module ###

    // clone SSEWrapperModule() and link scalarAdapter into it
    RTAnySL::ModuleAutoPtr clonedShaderGlueModule = shaderGlueModule.clone();

    assert(clonedShaderGlueModule.get() != 0);

    // TODO: It is not clear if jitRT::linkInModule will destroy
    //       source argument.
    //       We assume here that we need to destroy it. When not, call
    //       scalarWrapperModule.release() function instead of
    //       scalarWrapperModule.get() function.
    scalarWrapperModule.reset(
        jitRT::linkInModule(clonedShaderGlueModule.get(),
                            scalarWrapperModule.get()));

    assert(scalarWrapperModule.get() != 0);

    // linkInModule returns it's first argument i.e. the target module.
    // In order to avoid double deletion we release clonedShaderGlueModule.
    clonedShaderGlueModule.release();

    RTANYSL_DEBUG("RTfact/CXXShaderManager ready!");
}

CXXShaderManager::~CXXShaderManager()
{
    jitRT::flush();
    RTANYSL_DEBUG("Destroyed RTfact/CXXShaderManager !");
}

std::string CXXShaderManager::getShadingLanguage() const
{
    return "C++";
}

bool CXXShaderManager::isModuleSupported(llvm::Module *module) const
{
    // check if required entry points (shade functions) does exist
    // in the module
    return (jitRT::getFunction("shade", module) != 0 &&
            jitRT::getFunction("masked_shade", module) != 0 &&
            jitRT::getFunction("cxx_shader_id", module) != 0);
}

RTAnySL::ShaderPtr CXXShaderManager::createShader(llvm::Module *module)
{
    CXXShaderPtr s = CXXShaderPtr(
        new CXXShader(shared_from_this(), RTAnySL::ShaderPtr(), module));

    return s;
}

RTAnySL::ShaderPtr CXXShaderManager::buildShader(
    const std::string &shaderScript,
    RTAnySL::ShaderCompilerResult * result)
{
    if (shaderScript.length() != 0)
    {
        std::string bcModuleFileName;
        bool ok = buildCXXShader(shaderScript, bcModuleFileName, result);

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

bool CXXShaderManager::buildCXXShader(
    const std::string &shaderScript,
    std::string &bcModuleFileName,
    RTAnySL::ShaderCompilerResult * result)
{
    const char * path = ::getenv(JITRT_CXX_SHADER_BUILDER_ENVVAR);

    if (path == 0 || ::strlen(path) == 0)
    {
        RTANYSL_WARNING("no shader builder specified, please set "
                        <<JITRT_CXX_SHADER_BUILDER_ENVVAR
                        <<" environment variable.");
        return false;
    }

    std::string cxxShaderBuilder(path);

    std::string tmpShaderFileName =
        RTAnySL::Utils::makeTempFileName(
            RTfact::IO::appendPath(RTAnySL::Utils::getBuildDir(), "cxxshader"));
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
        cxxShaderBuilder+" "+
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

RTAnySL::ShaderInstancePtr CXXShaderManager::createShaderInstance(
    const CXXShaderPtr &shader,
    const RTAnySL::OptionsList *options)
{
    RTAnySL::ShaderInstancePtr shaderInstance(new CXXShaderInstance(shader));

    if (options)
        shaderInstance->setOptions(*options);

    return shaderInstance;
}

RTANYSL_STATIC_INIT_FUNC
{
    RTANYSL_REGISTER_SHADER_MANAGER(CXXShaderManager);
}

RTFACT_REMOTE_NAMESPACE_END
