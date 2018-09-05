AUTOMAKE_OPTIONS = foreign

SUBDIRS = . test

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = --std=gnu++14 -O3 -Wall -g

# comment right before redex_srcdir
redex_srcdir= $(srcdir)/redex

#
# Include paths
#
AM_CPPFLAGS = \
	-I$(redex_srcdir)/liblocator \
	-I$(redex_srcdir)/libredex \
	-I$(redex_srcdir)/libresource \
	-I$(redex_srcdir)/libresource/android \
	-I$(redex_srcdir)/libresource/androidfw \
	-I$(redex_srcdir)/libresource/cutils \
	-I$(redex_srcdir)/libresource/system \
	-I$(redex_srcdir)/libresource/utils \
	-I$(redex_srcdir)/opt \
	-I$(redex_srcdir)/opt/access-marking \
	-I$(redex_srcdir)/opt/add_redex_txt_to_apk \
	-I$(redex_srcdir)/opt/analysis_ref_graph \
	-I$(redex_srcdir)/opt/annoclasskill \
	-I$(redex_srcdir)/opt/annokill \
	-I$(redex_srcdir)/opt/basic-block \
	-I$(redex_srcdir)/opt/bridge \
	-I$(redex_srcdir)/opt/check_breadcrumbs \
	-I$(redex_srcdir)/opt/constant-propagation \
	-I$(redex_srcdir)/opt/dead-code-elimination \
	-I$(redex_srcdir)/opt/dedup_blocks \
	-I$(redex_srcdir)/opt/delinit \
	-I$(redex_srcdir)/opt/delsuper \
	-I$(redex_srcdir)/opt/final_inline \
	-I$(redex_srcdir)/opt/hotness-score \
	-I$(redex_srcdir)/opt/inlineinit \
	-I$(redex_srcdir)/opt/instrument \
	-I$(redex_srcdir)/opt/interdex \
	-I$(redex_srcdir)/opt/local-dce \
	-I$(redex_srcdir)/opt/obfuscate \
	-I$(redex_srcdir)/opt/original_name \
	-I$(redex_srcdir)/opt/outliner \
	-I$(redex_srcdir)/opt/peephole \
	-I$(redex_srcdir)/opt/print-members \
	-I$(redex_srcdir)/opt/rebindrefs \
	-I$(redex_srcdir)/opt/redundant_move_elimination \
	-I$(redex_srcdir)/opt/regalloc \
	-I$(redex_srcdir)/opt/remove-builders \
	-I$(redex_srcdir)/opt/remove-unreachable \
	-I$(redex_srcdir)/opt/remove_empty_classes \
	-I$(redex_srcdir)/opt/remove_gotos \
	-I$(redex_srcdir)/opt/remove_unused_args \
	-I$(redex_srcdir)/opt/renameclasses \
	-I$(redex_srcdir)/opt/reorder-interfaces \
	-I$(redex_srcdir)/opt/shorten-srcstrings \
	-I$(redex_srcdir)/opt/simpleinline \
	-I$(redex_srcdir)/opt/singleimpl \
	-I$(redex_srcdir)/opt/static-sink \
	-I$(redex_srcdir)/opt/staticrelo \
	-I$(redex_srcdir)/opt/string_concatenator \
	-I$(redex_srcdir)/opt/string_simplification \
	-I$(redex_srcdir)/opt/strip-debug-info \
	-I$(redex_srcdir)/opt/synth \
	-I$(redex_srcdir)/opt/test_cfg \
	-I$(redex_srcdir)/opt/track_resources \
	-I$(redex_srcdir)/opt/unreferenced_interfaces \
	-I$(redex_srcdir)/opt/unterface \
	-I$(redex_srcdir)/opt/verifier \
	-I$(redex_srcdir)/opt/virtual_scope \
	-I$(redex_srcdir)/service/constant-propagation \
	-I$(redex_srcdir)/service/dataflow \
	-I$(redex_srcdir)/service/escape-analysis \
	-I$(redex_srcdir)/service/method-dedup \
	-I$(redex_srcdir)/service/reference-update \
	-I$(redex_srcdir)/service/switch-dispatch \
	-I$(redex_srcdir)/shared \
	-I$(redex_srcdir)/sparta/include \
	-I$(redex_srcdir)/tools/common \
	-I$(redex_srcdir)/tools/redexdump \
	-I$(redex_srcdir)/util \
	-I/usr/include/jsoncpp

noinst_PROGRAMS = credex-all


credex_all_SOURCES = \
	passes/remover/Remover.cpp \
	redex/libredex/DexAsm.cpp \
	redex/opt/access-marking/AccessMarking.cpp \
	redex/opt/add_redex_txt_to_apk/AddRedexTxtToApk.cpp \
	redex/opt/analysis_ref_graph/ReferenceGraphCreator.cpp \
	redex/opt/annokill/AnnoKill.cpp \
	redex/opt/basic-block/BasicBlockProfile.cpp \
	redex/opt/bridge/Bridge.cpp \
	redex/opt/check_breadcrumbs/CheckBreadcrumbs.cpp \
	redex/opt/constant-propagation/ConstantPropagation.cpp \
	redex/opt/constant-propagation/ConstantPropagationRuntimeAssert.cpp \
	redex/opt/constant-propagation/ConstantPropagationTransform.cpp \
	redex/opt/constant-propagation/IPConstantPropagation.cpp \
	redex/opt/copy-propagation/AliasedRegisters.cpp \
	redex/opt/copy-propagation/CopyPropagationPass.cpp \
	redex/opt/dead-code-elimination/DeadCodeEliminationPass.cpp \
	redex/opt/dead-code-elimination/SideEffectSummary.cpp \
	redex/opt/dead-code-elimination/UsedVarsAnalysis.cpp \
	redex/opt/dedup_blocks/DedupBlocksPass.cpp \
	redex/opt/delinit/DelInit.cpp \
	redex/opt/delsuper/DelSuper.cpp \
	redex/opt/final_inline/FinalInline.cpp \
	redex/opt/final_inline/FinalInlineV2.cpp \
	redex/opt/hotness-score/HotnessScore.cpp \
	redex/opt/inlineinit/InlineInit.cpp \
	redex/opt/instrument/Instrument.cpp \
	redex/opt/interdex/DexStructure.cpp \
	redex/opt/interdex/InterDex.cpp \
	redex/opt/interdex/InterDexPass.cpp \
	redex/opt/local-dce/LocalDce.cpp \
	redex/opt/obfuscate/Obfuscate.cpp \
	redex/opt/obfuscate/ObfuscateUtils.cpp \
	redex/opt/obfuscate/VirtualRenamer.cpp \
	redex/opt/original_name/OriginalNamePass.cpp \
	redex/opt/outliner/Outliner.cpp \
	redex/opt/peephole/Peephole.cpp \
	redex/opt/peephole/RedundantCheckCastRemover.cpp \
	redex/opt/print-members/PrintMembers.cpp \
	redex/opt/reachability_graph/ReachabilityGraphPrinter.cpp \
	redex/opt/rebindrefs/ReBindRefs.cpp \
	redex/opt/regalloc/GraphColoring.cpp \
	redex/opt/regalloc/Interference.cpp \
	redex/opt/regalloc/RegAlloc.cpp \
	redex/opt/regalloc/RegisterType.cpp \
	redex/opt/regalloc/Split.cpp \
	redex/opt/regalloc/VirtualRegistersFile.cpp \
	redex/opt/remove-builders/RemoveBuilders.cpp \
	redex/opt/remove-builders/RemoveBuildersHelper.cpp \
	redex/opt/remove-unreachable/RemoveUnreachable.cpp \
	redex/opt/remove-unread-fields/RemoveUnreadFields.cpp \
	redex/opt/remove_empty_classes/RemoveEmptyClasses.cpp \
	redex/opt/remove_gotos/RemoveGotos.cpp \
	redex/opt/remove_unused_args/RemoveUnusedArgs.cpp \
	redex/opt/renameclasses/RenameClasses.cpp \
	redex/opt/renameclasses/RenameClassesV2.cpp \
	redex/opt/reorder-interfaces/ReorderInterfaces.cpp \
	redex/opt/shorten-srcstrings/Shorten.cpp \
	redex/opt/simpleinline/Deleter.cpp \
	redex/opt/simpleinline/SimpleInline.cpp \
	redex/opt/singleimpl/SingleImpl.cpp \
	redex/opt/singleimpl/SingleImplAnalyze.cpp \
	redex/opt/singleimpl/SingleImplOptimize.cpp \
	redex/opt/singleimpl/SingleImplStats.cpp \
	redex/opt/singleimpl/SingleImplUtil.cpp \
	redex/opt/simplify_cfg/SimplifyCFG.cpp \
	redex/opt/static-sink/StaticSink.cpp \
	redex/opt/staticrelo/StaticRelo.cpp \
	redex/opt/staticrelo/StaticReloV2.cpp \
	redex/opt/string_concatenator/StringConcatenator.cpp \
	redex/opt/string_simplification/StringIterator.cpp \
	redex/opt/string_simplification/StringSimplification.cpp \
	redex/opt/strip-debug-info/StripDebugInfo.cpp \
	redex/opt/synth/Synth.cpp \
	redex/opt/test_cfg/TestCFG.cpp \
	redex/opt/track_resources/TrackResources.cpp \
	redex/opt/unreferenced_interfaces/UnreferencedInterfaces.cpp \
	redex/opt/unterface/Unterface.cpp \
	redex/opt/unterface/UnterfaceOpt.cpp \
	redex/opt/verifier/Verifier.cpp \
	redex/opt/virtual_scope/MethodDevirtualizationPass.cpp \
	redex/service/constant-propagation/ConstantEnvironment.cpp \
	redex/service/constant-propagation/ConstantPropagationAnalysis.cpp \
	redex/service/constant-propagation/ConstantPropagationWholeProgramState.cpp \
	redex/service/constant-propagation/IPConstantPropagationAnalysis.cpp \
	redex/service/constant-propagation/ObjectDomain.cpp \
	redex/service/constant-propagation/SignDomain.cpp \
	redex/service/dataflow/LiveRange.cpp \
	redex/service/escape-analysis/LocalPointersAnalysis.cpp \
	redex/service/method-dedup/ConstantLifting.cpp \
	redex/service/method-dedup/ConstantValue.cpp \
	redex/service/method-dedup/MethodDedup.cpp \
	redex/service/method-dedup/TypeTags.cpp \
	redex/service/reference-update/MethodReference.cpp \
	redex/service/reference-update/TypeReference.cpp \
	redex/service/switch-dispatch/SwitchDispatch.cpp \
	redex/tools/common/ToolsCommon.cpp \
	redex/tools/redex-all/main.cpp

credex_all_LDADD = \
	redex/libredex.la \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_THREAD_LIB) \
	-lpthread \
	-ldl

credex_all_LDFLAGS = \
	-rdynamic # function names in stack traces


#
# redex: Python driver script
#
bin_SCRIPTS = credex
CLEANFILES = credex

PYTHON_SRCS := $(redex_srcdir)/redex.py \
	$(redex_srcdir)/pyredex/__init__.py \
	$(redex_srcdir)/pyredex/logger.py \
	$(redex_srcdir)/pyredex/unpacker.py \
	$(redex_srcdir)/pyredex/utils.py

credex: credex-all $(PYTHON_SRCS)
	./build-credex.sh
