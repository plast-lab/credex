AUTOMAKE_OPTIONS = foreign

SUBDIRS = redex

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

credex_added_src = \
	passes/remover/Remover.cpp

credex_all_SOURCES = \
	$(credex_added_src) \
	$(redex_srcdir)/libredex/DexAsm.cpp \
	$(redex_srcdir)/opt/access-marking/AccessMarking.cpp \
	$(redex_srcdir)/opt/add_redex_txt_to_apk/AddRedexTxtToApk.cpp \
	$(redex_srcdir)/opt/analysis_ref_graph/ReferenceGraphCreator.cpp \
	$(redex_srcdir)/opt/annokill/AnnoKill.cpp \
	$(redex_srcdir)/opt/basic-block/BasicBlockProfile.cpp \
	$(redex_srcdir)/opt/bridge/Bridge.cpp \
	$(redex_srcdir)/opt/check_breadcrumbs/CheckBreadcrumbs.cpp \
	$(redex_srcdir)/opt/constant-propagation/ConstantPropagation.cpp \
	$(redex_srcdir)/opt/constant-propagation/ConstantPropagationRuntimeAssert.cpp \
	$(redex_srcdir)/opt/constant-propagation/ConstantPropagationTransform.cpp \
	$(redex_srcdir)/opt/constant-propagation/IPConstantPropagation.cpp \
	$(redex_srcdir)/opt/copy-propagation/AliasedRegisters.cpp \
	$(redex_srcdir)/opt/copy-propagation/CopyPropagationPass.cpp \
	$(redex_srcdir)/opt/dead-code-elimination/DeadCodeEliminationPass.cpp \
	$(redex_srcdir)/opt/dead-code-elimination/SideEffectSummary.cpp \
	$(redex_srcdir)/opt/dead-code-elimination/UsedVarsAnalysis.cpp \
	$(redex_srcdir)/opt/dedup_blocks/DedupBlocksPass.cpp \
	$(redex_srcdir)/opt/delinit/DelInit.cpp \
	$(redex_srcdir)/opt/delsuper/DelSuper.cpp \
	$(redex_srcdir)/opt/final_inline/FinalInline.cpp \
	$(redex_srcdir)/opt/final_inline/FinalInlineV2.cpp \
	$(redex_srcdir)/opt/hotness-score/HotnessScore.cpp \
	$(redex_srcdir)/opt/inlineinit/InlineInit.cpp \
	$(redex_srcdir)/opt/instrument/Instrument.cpp \
	$(redex_srcdir)/opt/interdex/DexStructure.cpp \
	$(redex_srcdir)/opt/interdex/InterDex.cpp \
	$(redex_srcdir)/opt/interdex/InterDexPass.cpp \
	$(redex_srcdir)/opt/local-dce/LocalDce.cpp \
	$(redex_srcdir)/opt/obfuscate/Obfuscate.cpp \
	$(redex_srcdir)/opt/obfuscate/ObfuscateUtils.cpp \
	$(redex_srcdir)/opt/obfuscate/VirtualRenamer.cpp \
	$(redex_srcdir)/opt/original_name/OriginalNamePass.cpp \
	$(redex_srcdir)/opt/outliner/Outliner.cpp \
	$(redex_srcdir)/opt/peephole/Peephole.cpp \
	$(redex_srcdir)/opt/peephole/RedundantCheckCastRemover.cpp \
	$(redex_srcdir)/opt/print-members/PrintMembers.cpp \
	$(redex_srcdir)/opt/reachability_graph/ReachabilityGraphPrinter.cpp \
	$(redex_srcdir)/opt/rebindrefs/ReBindRefs.cpp \
	$(redex_srcdir)/opt/regalloc/GraphColoring.cpp \
	$(redex_srcdir)/opt/regalloc/Interference.cpp \
	$(redex_srcdir)/opt/regalloc/RegAlloc.cpp \
	$(redex_srcdir)/opt/regalloc/RegisterType.cpp \
	$(redex_srcdir)/opt/regalloc/Split.cpp \
	$(redex_srcdir)/opt/regalloc/VirtualRegistersFile.cpp \
	$(redex_srcdir)/opt/remove-builders/RemoveBuilders.cpp \
	$(redex_srcdir)/opt/remove-builders/RemoveBuildersHelper.cpp \
	$(redex_srcdir)/opt/remove-unreachable/RemoveUnreachable.cpp \
	$(redex_srcdir)/opt/remove-unread-fields/RemoveUnreadFields.cpp \
	$(redex_srcdir)/opt/remove_empty_classes/RemoveEmptyClasses.cpp \
	$(redex_srcdir)/opt/remove_gotos/RemoveGotos.cpp \
	$(redex_srcdir)/opt/remove_unused_args/RemoveUnusedArgs.cpp \
	$(redex_srcdir)/opt/renameclasses/RenameClasses.cpp \
	$(redex_srcdir)/opt/renameclasses/RenameClassesV2.cpp \
	$(redex_srcdir)/opt/reorder-interfaces/ReorderInterfaces.cpp \
	$(redex_srcdir)/opt/shorten-srcstrings/Shorten.cpp \
	$(redex_srcdir)/opt/simpleinline/Deleter.cpp \
	$(redex_srcdir)/opt/simpleinline/SimpleInline.cpp \
	$(redex_srcdir)/opt/singleimpl/SingleImpl.cpp \
	$(redex_srcdir)/opt/singleimpl/SingleImplAnalyze.cpp \
	$(redex_srcdir)/opt/singleimpl/SingleImplOptimize.cpp \
	$(redex_srcdir)/opt/singleimpl/SingleImplStats.cpp \
	$(redex_srcdir)/opt/singleimpl/SingleImplUtil.cpp \
	$(redex_srcdir)/opt/simplify_cfg/SimplifyCFG.cpp \
	$(redex_srcdir)/opt/static-sink/StaticSink.cpp \
	$(redex_srcdir)/opt/staticrelo/StaticRelo.cpp \
	$(redex_srcdir)/opt/staticrelo/StaticReloV2.cpp \
	$(redex_srcdir)/opt/string_concatenator/StringConcatenator.cpp \
	$(redex_srcdir)/opt/string_simplification/StringIterator.cpp \
	$(redex_srcdir)/opt/string_simplification/StringSimplification.cpp \
	$(redex_srcdir)/opt/strip-debug-info/StripDebugInfo.cpp \
	$(redex_srcdir)/opt/synth/Synth.cpp \
	$(redex_srcdir)/opt/test_cfg/TestCFG.cpp \
	$(redex_srcdir)/opt/track_resources/TrackResources.cpp \
	$(redex_srcdir)/opt/unreferenced_interfaces/UnreferencedInterfaces.cpp \
	$(redex_srcdir)/opt/unterface/Unterface.cpp \
	$(redex_srcdir)/opt/unterface/UnterfaceOpt.cpp \
	$(redex_srcdir)/opt/verifier/Verifier.cpp \
	$(redex_srcdir)/opt/virtual_scope/MethodDevirtualizationPass.cpp \
	$(redex_srcdir)/service/constant-propagation/ConstantEnvironment.cpp \
	$(redex_srcdir)/service/constant-propagation/ConstantPropagationAnalysis.cpp \
	$(redex_srcdir)/service/constant-propagation/ConstantPropagationWholeProgramState.cpp \
	$(redex_srcdir)/service/constant-propagation/IPConstantPropagationAnalysis.cpp \
	$(redex_srcdir)/service/constant-propagation/ObjectDomain.cpp \
	$(redex_srcdir)/service/constant-propagation/SignDomain.cpp \
	$(redex_srcdir)/service/dataflow/LiveRange.cpp \
	$(redex_srcdir)/service/escape-analysis/LocalPointersAnalysis.cpp \
  $(redex_srcdir)/service/method-dedup/ConstantLifting.cpp \
	$(redex_srcdir)/service/method-dedup/ConstantValue.cpp \
	$(redex_srcdir)/service/method-dedup/MethodDedup.cpp \
	$(redex_srcdir)/service/method-dedup/TypeTags.cpp \
	$(redex_srcdir)/service/reference-update/MethodReference.cpp \
	$(redex_srcdir)/service/reference-update/TypeReference.cpp \
	$(redex_srcdir)/service/switch-dispatch/SwitchDispatch.cpp \
	$(redex_srcdir)/tools/common/ToolsCommon.cpp \
	$(redex_srcdir)/tools/redex-all/main.cpp

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
