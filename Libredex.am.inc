#
# libredex: the optimizer's guts
#
noinst_LTLIBRARIES = libredex.la

libredex_la_SOURCES = \
	redex/liblocator/locator.cpp \
	redex/libredex/AnnoUtils.cpp \
	redex/libredex/ApkManager.cpp \
	redex/libredex/CallGraph.cpp \
	redex/libredex/CFGInliner.cpp \
	redex/libredex/ClassHierarchy.cpp \
	redex/libredex/ConfigFiles.cpp \
	redex/libredex/Creators.cpp \
	redex/libredex/ControlFlow.cpp \
	redex/libredex/Debug.cpp \
	redex/libredex/DexAnnotation.cpp \
	redex/libredex/DexClass.cpp \
	redex/libredex/DexDebugInstruction.cpp \
	redex/libredex/DexEncoding.cpp \
	redex/libredex/DexIdx.cpp \
	redex/libredex/DexLoader.cpp \
	redex/libredex/DexInstruction.cpp \
	redex/libredex/DexMemberRefs.cpp \
	redex/libredex/DexOpcode.cpp \
	redex/libredex/DexOutput.cpp \
	redex/libredex/DexPosition.cpp \
	redex/libredex/DexStore.cpp \
	redex/libredex/DexUtil.cpp \
	redex/libredex/DexStoreUtil.cpp \
	redex/libredex/FieldOpTracker.cpp \
	redex/libredex/ImmutableSubcomponentAnalyzer.cpp \
	redex/libredex/Inliner.cpp \
	redex/libredex/InstructionLowering.cpp \
	redex/libredex/IRAssembler.cpp \
	redex/libredex/IRCode.cpp \
	redex/libredex/IRInstruction.cpp \
	redex/libredex/IRList.cpp \
	redex/libredex/IRMetaIO.cpp \
	redex/libredex/IROpcode.cpp \
	redex/libredex/IRTypeChecker.cpp \
	redex/libredex/JarLoader.cpp \
	redex/libredex/Match.cpp \
	redex/libredex/MethodDevirtualizer.cpp \
	redex/libredex/MethodOverrideGraph.cpp \
	redex/libredex/Mutators.cpp \
	redex/libredex/OptData.cpp \
	redex/libredex/PassManager.cpp \
	redex/libredex/PassRegistry.cpp \
	redex/libredex/PluginRegistry.cpp \
	redex/libredex/PointsToSemantics.cpp \
	redex/libredex/PointsToSemanticsUtils.cpp \
	redex/libredex/PrintSeeds.cpp \
	redex/libredex/ProguardLexer.cpp \
	redex/libredex/ProguardMap.cpp \
	redex/libredex/ProguardMatcher.cpp \
	redex/libredex/ProguardParser.cpp \
	redex/libredex/ProguardPrintConfiguration.cpp \
	redex/libredex/ProguardRegex.cpp \
	redex/libredex/ProguardReporting.cpp \
	redex/libredex/Reachability.cpp \
	redex/libredex/ReachableClasses.cpp \
	redex/libredex/RedexContext.cpp \
	redex/libredex/Resolver.cpp \
	redex/libredex/Show.cpp \
	redex/libredex/SimpleReflectionAnalysis.cpp \
	redex/libredex/Timer.cpp \
	redex/libredex/Trace.cpp \
	redex/libredex/Transform.cpp \
	redex/libredex/IRTypeChecker.cpp \
	redex/libredex/TypeSystem.cpp \
	redex/libredex/Vinfo.cpp \
	redex/libredex/VirtualScope.cpp \
	redex/libredex/Warning.cpp \
	redex/libresource/FileMap.cpp \
	redex/libresource/RedexResources.cpp \
	redex/libresource/ResourceTypes.cpp \
	redex/libresource/Serialize.cpp \
	redex/libresource/SharedBuffer.cpp \
	redex/libresource/Static.cpp \
	redex/libresource/String16.cpp \
	redex/libresource/String8.cpp \
	redex/libresource/TypeWrappers.cpp \
	redex/libresource/Unicode.cpp \
	redex/libresource/VectorImpl.cpp \
	redex/shared/DexDefs.cpp \
	redex/shared/file-utils.cpp \
	redex/util/CommandProfiling.cpp \
	redex/util/JemallocUtil.cpp \
	redex/util/Sha1.cpp

libredex_la_LIBADD = \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_IOSTREAMS_LIB) \
	$(BOOST_THREAD_LIB) \
	-lpthread

