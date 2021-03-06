
libredex_opt_la_SOURCES= \
	@top_srcdir@/redex/opt/access-marking/AccessMarking.cpp \
	@top_srcdir@/redex/opt/add_redex_txt_to_apk/AddRedexTxtToApk.cpp \
	@top_srcdir@/redex/opt/analysis_ref_graph/ReferenceGraphCreator.cpp \
	@top_srcdir@/redex/opt/annokill/AnnoKill.cpp \
	@top_srcdir@/redex/opt/basic-block/BasicBlockProfile.cpp \
	@top_srcdir@/redex/opt/bridge/Bridge.cpp \
	@top_srcdir@/redex/opt/check_breadcrumbs/CheckBreadcrumbs.cpp \
	@top_srcdir@/redex/opt/constant-propagation/ConstantPropagation.cpp \
	@top_srcdir@/redex/opt/constant-propagation/ConstantPropagationRuntimeAssert.cpp \
	@top_srcdir@/redex/opt/constant-propagation/ConstantPropagationTransform.cpp \
	@top_srcdir@/redex/opt/constant-propagation/IPConstantPropagation.cpp \
	@top_srcdir@/redex/opt/copy-propagation/AliasedRegisters.cpp \
	@top_srcdir@/redex/opt/copy-propagation/CopyPropagationPass.cpp \
	@top_srcdir@/redex/opt/dead-code-elimination/DeadCodeEliminationPass.cpp \
	@top_srcdir@/redex/opt/dead-code-elimination/SideEffectSummary.cpp \
	@top_srcdir@/redex/opt/dead-code-elimination/UsedVarsAnalysis.cpp \
	@top_srcdir@/redex/opt/dedup_blocks/DedupBlocksPass.cpp \
	@top_srcdir@/redex/opt/delinit/DelInit.cpp \
	@top_srcdir@/redex/opt/delsuper/DelSuper.cpp \
	@top_srcdir@/redex/opt/final_inline/FinalInline.cpp \
	@top_srcdir@/redex/opt/final_inline/FinalInlineV2.cpp \
	@top_srcdir@/redex/opt/hotness-score/HotnessScore.cpp \
	@top_srcdir@/redex/opt/inlineinit/InlineInit.cpp \
	@top_srcdir@/redex/opt/instrument/Instrument.cpp \
	@top_srcdir@/redex/opt/interdex/DexStructure.cpp \
	@top_srcdir@/redex/opt/interdex/InterDex.cpp \
	@top_srcdir@/redex/opt/interdex/InterDexPass.cpp \
	@top_srcdir@/redex/opt/local-dce/LocalDce.cpp \
	@top_srcdir@/redex/opt/merge_interface/MergeInterface.cpp \
	@top_srcdir@/redex/opt/obfuscate/Obfuscate.cpp \
	@top_srcdir@/redex/opt/obfuscate/ObfuscateUtils.cpp \
	@top_srcdir@/redex/opt/obfuscate/VirtualRenamer.cpp \
	@top_srcdir@/redex/opt/original_name/OriginalNamePass.cpp \
	@top_srcdir@/redex/opt/outliner/Outliner.cpp \
	@top_srcdir@/redex/opt/peephole/Peephole.cpp \
	@top_srcdir@/redex/opt/peephole/RedundantCheckCastRemover.cpp \
	@top_srcdir@/redex/opt/print-members/PrintMembers.cpp \
	@top_srcdir@/redex/opt/rebindrefs/ReBindRefs.cpp \
	@top_srcdir@/redex/opt/regalloc/GraphColoring.cpp \
	@top_srcdir@/redex/opt/regalloc/Interference.cpp \
	@top_srcdir@/redex/opt/regalloc/RegAlloc.cpp \
	@top_srcdir@/redex/opt/regalloc/RegisterType.cpp \
	@top_srcdir@/redex/opt/regalloc/Split.cpp \
	@top_srcdir@/redex/opt/regalloc/VirtualRegistersFile.cpp \
	@top_srcdir@/redex/opt/remove-builders/RemoveBuilders.cpp \
	@top_srcdir@/redex/opt/remove-builders/RemoveBuildersHelper.cpp \
	@top_srcdir@/redex/opt/remove-interfaces/RemoveInterfacePass.cpp \
	@top_srcdir@/redex/opt/remove-unreachable/RemoveUnreachable.cpp \
	@top_srcdir@/redex/opt/remove-unread-fields/RemoveUnreadFields.cpp \
	@top_srcdir@/redex/opt/remove_empty_classes/RemoveEmptyClasses.cpp \
	@top_srcdir@/redex/opt/remove_gotos/RemoveGotos.cpp \
	@top_srcdir@/redex/opt/remove_unused_args/RemoveUnusedArgs.cpp \
	@top_srcdir@/redex/opt/renameclasses/RenameClasses.cpp \
	@top_srcdir@/redex/opt/renameclasses/RenameClassesV2.cpp \
	@top_srcdir@/redex/opt/reorder-interfaces/ReorderInterfaces.cpp \
	@top_srcdir@/redex/opt/shorten-srcstrings/Shorten.cpp \
	@top_srcdir@/redex/opt/simpleinline/Deleter.cpp \
	@top_srcdir@/redex/opt/simpleinline/SimpleInline.cpp \
	@top_srcdir@/redex/opt/singleimpl/SingleImpl.cpp \
	@top_srcdir@/redex/opt/singleimpl/SingleImplAnalyze.cpp \
	@top_srcdir@/redex/opt/singleimpl/SingleImplOptimize.cpp \
	@top_srcdir@/redex/opt/singleimpl/SingleImplStats.cpp \
	@top_srcdir@/redex/opt/singleimpl/SingleImplUtil.cpp \
	@top_srcdir@/redex/opt/simplify_cfg/SimplifyCFG.cpp \
	@top_srcdir@/redex/opt/static-sink/StaticSink.cpp \
	@top_srcdir@/redex/opt/staticrelo/StaticRelo.cpp \
	@top_srcdir@/redex/opt/staticrelo/StaticReloV2.cpp \
	@top_srcdir@/redex/opt/string_concatenator/StringConcatenator.cpp \
	@top_srcdir@/redex/opt/string_simplification/StringIterator.cpp \
	@top_srcdir@/redex/opt/string_simplification/StringSimplification.cpp \
	@top_srcdir@/redex/opt/strip-debug-info/StripDebugInfo.cpp \
	@top_srcdir@/redex/opt/synth/Synth.cpp \
	@top_srcdir@/redex/opt/test_cfg/TestCFG.cpp \
	@top_srcdir@/redex/opt/track_resources/TrackResources.cpp \
	@top_srcdir@/redex/opt/type-erasure/ApproximateShapeMerging.cpp \
	@top_srcdir@/redex/opt/type-erasure/ClassAssemblingUtils.cpp \
	@top_srcdir@/redex/opt/type-erasure/MergerType.cpp \
	@top_srcdir@/redex/opt/type-erasure/MethodMerger.cpp \
	@top_srcdir@/redex/opt/type-erasure/Model.cpp \
	@top_srcdir@/redex/opt/type-erasure/ModelMerger.cpp \
	@top_srcdir@/redex/opt/type-erasure/TypeErasureInterDexPlugin.cpp \
	@top_srcdir@/redex/opt/type-erasure/TypeErasurePass.cpp \
	@top_srcdir@/redex/opt/type-erasure/TypeTagUtils.cpp \
	@top_srcdir@/redex/opt/unreferenced_interfaces/UnreferencedInterfaces.cpp \
	@top_srcdir@/redex/opt/unterface/Unterface.cpp \
	@top_srcdir@/redex/opt/unterface/UnterfaceOpt.cpp \
	@top_srcdir@/redex/opt/verifier/Verifier.cpp \
	@top_srcdir@/redex/opt/virtual_scope/MethodDevirtualizationPass.cpp \
	@top_srcdir@/redex/service/constant-propagation/ConstantEnvironment.cpp \
	@top_srcdir@/redex/service/constant-propagation/ConstantPropagationAnalysis.cpp \
	@top_srcdir@/redex/service/constant-propagation/ConstantPropagationWholeProgramState.cpp \
	@top_srcdir@/redex/service/constant-propagation/IPConstantPropagationAnalysis.cpp \
	@top_srcdir@/redex/service/constant-propagation/ObjectDomain.cpp \
	@top_srcdir@/redex/service/constant-propagation/SignDomain.cpp \
	@top_srcdir@/redex/service/dataflow/LiveRange.cpp \
	@top_srcdir@/redex/service/escape-analysis/LocalPointersAnalysis.cpp \
	@top_srcdir@/redex/service/method-dedup/ConstantLifting.cpp \
	@top_srcdir@/redex/service/method-dedup/ConstantValue.cpp \
	@top_srcdir@/redex/service/method-dedup/MethodDedup.cpp \
	@top_srcdir@/redex/service/method-dedup/TypeTags.cpp \
	@top_srcdir@/redex/service/reference-update/MethodReference.cpp \
	@top_srcdir@/redex/service/reference-update/TypeReference.cpp \
	@top_srcdir@/redex/service/switch-dispatch/SwitchDispatch.cpp


