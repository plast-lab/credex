#
# libredex: the optimizer's guts
#
noinst_LTLIBRARIES = libredex.la

libredex_la_SOURCES = \
	@top_srcdir@/redex/liblocator/locator.cpp \
	@top_srcdir@/redex/libredex/AnnoUtils.cpp \
	@top_srcdir@/redex/libredex/ApkManager.cpp \
	@top_srcdir@/redex/libredex/CallGraph.cpp \
	@top_srcdir@/redex/libredex/CFGInliner.cpp \
	@top_srcdir@/redex/libredex/ClassHierarchy.cpp \
	@top_srcdir@/redex/libredex/ConfigFiles.cpp \
	@top_srcdir@/redex/libredex/Creators.cpp \
	@top_srcdir@/redex/libredex/ControlFlow.cpp \
	@top_srcdir@/redex/libredex/Debug.cpp \
	@top_srcdir@/redex/libredex/DexAnnotation.cpp \
	@top_srcdir@/redex/libredex/DexClass.cpp \
	@top_srcdir@/redex/libredex/DexDebugInstruction.cpp \
	@top_srcdir@/redex/libredex/DexEncoding.cpp \
	@top_srcdir@/redex/libredex/DexIdx.cpp \
	@top_srcdir@/redex/libredex/DexLoader.cpp \
	@top_srcdir@/redex/libredex/DexInstruction.cpp \
	@top_srcdir@/redex/libredex/DexMemberRefs.cpp \
	@top_srcdir@/redex/libredex/DexOpcode.cpp \
	@top_srcdir@/redex/libredex/DexOutput.cpp \
	@top_srcdir@/redex/libredex/DexPosition.cpp \
	@top_srcdir@/redex/libredex/DexStore.cpp \
	@top_srcdir@/redex/libredex/DexUtil.cpp \
	@top_srcdir@/redex/libredex/DexStoreUtil.cpp \
	@top_srcdir@/redex/libredex/FieldOpTracker.cpp \
	@top_srcdir@/redex/libredex/ImmutableSubcomponentAnalyzer.cpp \
	@top_srcdir@/redex/libredex/Inliner.cpp \
	@top_srcdir@/redex/libredex/InstructionLowering.cpp \
	@top_srcdir@/redex/libredex/IRAssembler.cpp \
	@top_srcdir@/redex/libredex/IRCode.cpp \
	@top_srcdir@/redex/libredex/IRInstruction.cpp \
	@top_srcdir@/redex/libredex/IRList.cpp \
	@top_srcdir@/redex/libredex/IRMetaIO.cpp \
	@top_srcdir@/redex/libredex/IROpcode.cpp \
	@top_srcdir@/redex/libredex/IRTypeChecker.cpp \
	@top_srcdir@/redex/libredex/JarLoader.cpp \
	@top_srcdir@/redex/libredex/Match.cpp \
	@top_srcdir@/redex/libredex/MethodDevirtualizer.cpp \
	@top_srcdir@/redex/libredex/MethodOverrideGraph.cpp \
	@top_srcdir@/redex/libredex/Mutators.cpp \
	@top_srcdir@/redex/libredex/OptData.cpp \
	@top_srcdir@/redex/libredex/PassManager.cpp \
	@top_srcdir@/redex/libredex/PassRegistry.cpp \
	@top_srcdir@/redex/libredex/PluginRegistry.cpp \
	@top_srcdir@/redex/libredex/PointsToSemantics.cpp \
	@top_srcdir@/redex/libredex/PointsToSemanticsUtils.cpp \
	@top_srcdir@/redex/libredex/PrintSeeds.cpp \
	@top_srcdir@/redex/libredex/ProguardLexer.cpp \
	@top_srcdir@/redex/libredex/ProguardMap.cpp \
	@top_srcdir@/redex/libredex/ProguardMatcher.cpp \
	@top_srcdir@/redex/libredex/ProguardParser.cpp \
	@top_srcdir@/redex/libredex/ProguardPrintConfiguration.cpp \
	@top_srcdir@/redex/libredex/ProguardRegex.cpp \
	@top_srcdir@/redex/libredex/ProguardReporting.cpp \
	@top_srcdir@/redex/libredex/Reachability.cpp \
	@top_srcdir@/redex/libredex/ReachableClasses.cpp \
	@top_srcdir@/redex/libredex/RedexContext.cpp \
	@top_srcdir@/redex/libredex/Resolver.cpp \
	@top_srcdir@/redex/libredex/Show.cpp \
	@top_srcdir@/redex/libredex/SimpleReflectionAnalysis.cpp \
	@top_srcdir@/redex/libredex/Timer.cpp \
	@top_srcdir@/redex/libredex/Trace.cpp \
	@top_srcdir@/redex/libredex/Transform.cpp \
	@top_srcdir@/redex/libredex/IRTypeChecker.cpp \
	@top_srcdir@/redex/libredex/TypeSystem.cpp \
	@top_srcdir@/redex/libredex/Vinfo.cpp \
	@top_srcdir@/redex/libredex/VirtualScope.cpp \
	@top_srcdir@/redex/libredex/Warning.cpp \
	@top_srcdir@/redex/libresource/FileMap.cpp \
	@top_srcdir@/redex/libresource/RedexResources.cpp \
	@top_srcdir@/redex/libresource/ResourceTypes.cpp \
	@top_srcdir@/redex/libresource/Serialize.cpp \
	@top_srcdir@/redex/libresource/SharedBuffer.cpp \
	@top_srcdir@/redex/libresource/Static.cpp \
	@top_srcdir@/redex/libresource/String16.cpp \
	@top_srcdir@/redex/libresource/String8.cpp \
	@top_srcdir@/redex/libresource/TypeWrappers.cpp \
	@top_srcdir@/redex/libresource/Unicode.cpp \
	@top_srcdir@/redex/libresource/VectorImpl.cpp \
	@top_srcdir@/redex/shared/DexDefs.cpp \
	@top_srcdir@/redex/shared/file-utils.cpp \
	@top_srcdir@/redex/util/CommandProfiling.cpp \
	@top_srcdir@/redex/util/JemallocUtil.cpp \
	@top_srcdir@/redex/util/Sha1.cpp

libredex_la_LIBADD = \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_IOSTREAMS_LIB) \
	$(BOOST_THREAD_LIB) \
	-lpthread

