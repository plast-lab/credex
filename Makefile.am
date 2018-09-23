AUTOMAKE_OPTIONS = foreign

SUBDIRS = . test redex

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
	passes/explicit_inline/ExplicitInlinePass.cpp \
	passes/plast-devirt/PlastDevirtualizationPass.cpp \
	passes/plast-devirt/PlastDVParser.cpp


credex_all_LDADD = \
	redex/libredex/DexAsm.o \
	redex/opt/access-marking/AccessMarking.o \
	redex/opt/add_redex_txt_to_apk/AddRedexTxtToApk.o \
	redex/opt/analysis_ref_graph/ReferenceGraphCreator.o \
	redex/opt/annokill/AnnoKill.o \
	redex/opt/basic-block/BasicBlockProfile.o \
	redex/opt/bridge/Bridge.o \
	redex/opt/check_breadcrumbs/CheckBreadcrumbs.o \
	redex/opt/constant-propagation/ConstantPropagation.o \
	redex/opt/constant-propagation/ConstantPropagationRuntimeAssert.o \
	redex/opt/constant-propagation/ConstantPropagationTransform.o \
	redex/opt/constant-propagation/IPConstantPropagation.o \
	redex/opt/copy-propagation/AliasedRegisters.o \
	redex/opt/copy-propagation/CopyPropagationPass.o \
	redex/opt/dead-code-elimination/DeadCodeEliminationPass.o \
	redex/opt/dead-code-elimination/SideEffectSummary.o \
	redex/opt/dead-code-elimination/UsedVarsAnalysis.o \
	redex/opt/dedup_blocks/DedupBlocksPass.o \
	redex/opt/delinit/DelInit.o \
	redex/opt/delsuper/DelSuper.o \
	redex/opt/final_inline/FinalInline.o \
	redex/opt/final_inline/FinalInlineV2.o \
	redex/opt/hotness-score/HotnessScore.o \
	redex/opt/inlineinit/InlineInit.o \
	redex/opt/instrument/Instrument.o \
	redex/opt/interdex/DexStructure.o \
	redex/opt/interdex/InterDex.o \
	redex/opt/interdex/InterDexPass.o \
	redex/opt/local-dce/LocalDce.o \
	redex/opt/obfuscate/Obfuscate.o \
	redex/opt/obfuscate/ObfuscateUtils.o \
	redex/opt/obfuscate/VirtualRenamer.o \
	redex/opt/original_name/OriginalNamePass.o \
	redex/opt/outliner/Outliner.o \
	redex/opt/peephole/Peephole.o \
	redex/opt/peephole/RedundantCheckCastRemover.o \
	redex/opt/print-members/PrintMembers.o \
	redex/opt/reachability_graph/ReachabilityGraphPrinter.o \
	redex/opt/rebindrefs/ReBindRefs.o \
	redex/opt/regalloc/GraphColoring.o \
	redex/opt/regalloc/Interference.o \
	redex/opt/regalloc/RegAlloc.o \
	redex/opt/regalloc/RegisterType.o \
	redex/opt/regalloc/Split.o \
	redex/opt/regalloc/VirtualRegistersFile.o \
	redex/opt/remove-builders/RemoveBuilders.o \
	redex/opt/remove-builders/RemoveBuildersHelper.o \
	redex/opt/remove-unreachable/RemoveUnreachable.o \
	redex/opt/remove-unread-fields/RemoveUnreadFields.o \
	redex/opt/remove_empty_classes/RemoveEmptyClasses.o \
	redex/opt/remove_gotos/RemoveGotos.o \
	redex/opt/remove_unused_args/RemoveUnusedArgs.o \
	redex/opt/renameclasses/RenameClasses.o \
	redex/opt/renameclasses/RenameClassesV2.o \
	redex/opt/reorder-interfaces/ReorderInterfaces.o \
	redex/opt/shorten-srcstrings/Shorten.o \
	redex/opt/simpleinline/Deleter.o \
	redex/opt/simpleinline/SimpleInline.o \
	redex/opt/singleimpl/SingleImpl.o \
	redex/opt/singleimpl/SingleImplAnalyze.o \
	redex/opt/singleimpl/SingleImplOptimize.o \
	redex/opt/singleimpl/SingleImplStats.o \
	redex/opt/singleimpl/SingleImplUtil.o \
	redex/opt/simplify_cfg/SimplifyCFG.o \
	redex/opt/static-sink/StaticSink.o \
	redex/opt/staticrelo/StaticRelo.o \
	redex/opt/staticrelo/StaticReloV2.o \
	redex/opt/string_concatenator/StringConcatenator.o \
	redex/opt/string_simplification/StringIterator.o \
	redex/opt/string_simplification/StringSimplification.o \
	redex/opt/strip-debug-info/StripDebugInfo.o \
	redex/opt/synth/Synth.o \
	redex/opt/test_cfg/TestCFG.o \
	redex/opt/track_resources/TrackResources.o \
	redex/opt/unreferenced_interfaces/UnreferencedInterfaces.o \
	redex/opt/unterface/Unterface.o \
	redex/opt/unterface/UnterfaceOpt.o \
	redex/opt/verifier/Verifier.o \
	redex/opt/virtual_scope/MethodDevirtualizationPass.o \
	redex/service/constant-propagation/ConstantEnvironment.o \
	redex/service/constant-propagation/ConstantPropagationAnalysis.o \
	redex/service/constant-propagation/ConstantPropagationWholeProgramState.o \
	redex/service/constant-propagation/IPConstantPropagationAnalysis.o \
	redex/service/constant-propagation/ObjectDomain.o \
	redex/service/constant-propagation/SignDomain.o \
	redex/service/dataflow/LiveRange.o \
	redex/service/escape-analysis/LocalPointersAnalysis.o \
	redex/service/method-dedup/ConstantLifting.o \
	redex/service/method-dedup/ConstantValue.o \
	redex/service/method-dedup/MethodDedup.o \
	redex/service/method-dedup/TypeTags.o \
	redex/service/reference-update/MethodReference.o \
	redex/service/reference-update/TypeReference.o \
	redex/service/switch-dispatch/SwitchDispatch.o \
	redex/tools/common/ToolsCommon.o \
	redex/tools/redex-all/main.o

credex_all_LDADD += \
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
