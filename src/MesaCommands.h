/**************************************************************************
MesaCommands.h - atomic user commands that can be given to MESA

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK) Silwood
  Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://evolve.bio.ic.ac.uk>

Changes:
- (01.4.4) Created.

**************************************************************************/

#pragma once
#ifndef MESACOMMANDS_H
#define MESACOMMANDS_H


// *** INCLUDES


// *** CONSTANTS & DEFINES

enum mesaCmdId_t
//: the commands Mesa understands
{
	kCmd_Null,
	
	// the file commands
	kCmd_OpenFile = 1,
	kCmd_SaveFile,
	kCmd_ExportData,
	kCmd_CloseFile,
	kCmd_Import,
	kCmd_ImportRich,
	kCmd_NewFile,
	kCmd_Distill,

	kCmd_Summarise,	
	kCmd_Return,
	
	// those to a submenu
	kCmd_PrefsMenu,
	kCmd_SysActionMenu,
	kCmd_AnalysisMenu,
	kCmd_TaskMenu,

	// those in the manipulation submenu	
	kCmd_SelectTree,
	kCmd_DuplicateTree,
	kCmd_DeleteTree,
	kCmd_DeleteTrait,
	kCmd_NewTrait,
	kCmd_DetailedReport,
	kCmd_RulePruneFixedNum, 
	kCmd_RulePrunFixedFrac, 
	kCmd_RulePrunProb, 
	kCmd_RulePrunByTrait,
	kCmd_RulePrunIf,

	// those in the preference dlog
	kCmd_PrefVerbosity,
	kCmd_PrefLogging,
	kCmd_PrefCase,
	kCmd_PrefPadMatrix,
	kCmd_PrefAnalysisOut,
	kCmd_PrefSetRandSeed,
	kCmd_PrefSetCladeLabels,
	kCmd_PrefWriteTranslation,
	kCmd_PrefPreserveNodes,
	kCmd_PrefDeadNodes,
	kCmd_PrefWriteTaxa,
	kCmd_PrefTimeGrain,
	
	// analysis action commands
	kCmd_AnalExTaxa,	
	kCmd_AnalAllTaxa,
	kCmd_AnalGeneticDiv, 		
	kCmd_AnalPhyloDiv,
	kCmd_AnalJackknifePd,
	kCmd_AnalJackknifeGd,
	kCmd_AnalBootstrapPd,
	kCmd_AnalBootstrapGd,
	kCmd_AnalShannonDiv,
	kCmd_AnalSimpsonDiv,
	kCmd_AnalBrillDiv,
	kCmd_AnalPieDiv,
	kCmd_AnalMargelefDiv,
	kCmd_AnalMacintoshDiv,
	kCmd_AnalMehinickDiv,
	kCmd_AnalFusco,
	kCmd_AnalFuscoAll,
	kCmd_AnalFuscoWeighted,
	kCmd_AnalFuscoExtended,
	kCmd_AnalFuscoExtendedAll,
	kCmd_AnalSlowinski,
	kCmd_AnalShaosNbar,
	kCmd_AnalShaosSigmaSq,
	kCmd_AnalCollessC,
	kCmd_AnalB1,
	kCmd_AnalB2,
	kCmd_AnalPhyloAgeTree,
	kCmd_AnalNodeInfo,
	kCmd_XAnalNodeInfo,
	kCmd_AnalSiteComp,
	// kCmd_AnalListNodes,
	kCmd_AnalStemminess,
	kCmd_AnalCaic,
	kCmd_AnalMacroCaic,
	kCmd_AnalResolution,
	kCmd_AnalUltrametric,
	
	// macro action commands
	kCmd_MacroRunOnce,
	kCmd_MacroRunN,		
	kCmd_MacroRunAndRestore,	
	kCmd_EpochPopLimit,
	kCmd_EpochTimeLimit,
	kCmd_EpochSizeLimit,
	kCmd_EpochTipLimit,
	kCmd_MacroRunTree,
	
	// system action commands
	kCmd_SysActionDupTree,
	kCmd_SysActionSave,
	kCmd_SysActionSetLabels,
	kCmd_SysActionSetLengths,
	kCmd_SysActionShuffleTraits,
	kCmd_SysActionSetPreservation,
	kCmd_SysActionConsolidateTaxa,
	kCmd_SysActionDeleteDeadTaxa,
	kCmd_SysActionDeleteDeadTraits,
	kCmd_SysActionCollapseSingletonsTaxa,
	kCmd_SysActionMakeNeont,
	
	// rule commands
	kCmd_RuleMarkovSp,
   kCmd_RuleLogisticSp,
   kCmd_RuleLogisticKill,
	kCmd_RuleLatentSp,
	kCmd_RuleBiasedSp,
	kCmd_RuleBiasedKill,
	kCmd_RuleMarkovKill,
	kCmd_RuleBiasedTraitSp,
	kCmd_RuleBiasedTraitSp_New, // testing new distributions
	kCmd_RuleSymSpecTraitEv,
	kCmd_RuleAsymSpecTraitEv, 
	kCmd_RuleGradualTraitEv,
	kCmd_RuleTerminalTraitEv,	
	kCmd_RuleBiasedTraitKill,
	kCmd_RuleMassKillFixedNum,
	kCmd_RuleMassKillPercent,
	kCmd_RuleMassKillProb,
	kCmd_RuleMassKillIf,
	kCmd_RuleMassKillBiasedTrait,
	kCmd_RuleMassKillRangeTrait,
	kCmd_RuleNull,
	kCmd_StateList,
	kCmd_StateDelete, 
	kCmd_StateAdd, 
	kCmd_StateSort,
	kCmd_SchemeList, 
	kCmd_SchemeDelete,
	kCmd_SchemeNull, 
	// kCmd_SchemeBinary, 
	kCmd_SchemeDiscMarkov,
	kCmd_SchemeDiscRankedMarkov,
	kCmd_SchemeContBrownian,
	kCmd_SchemeContLogNormal,
	
	// queue manipulation commands
	kCmd_QueueDelete,
	kCmd_QueueDeleteAll,
	kCmd_QueueGo,	
	kCmd_QueueGoN,	
	kCmd_QueueGoRunAndRestore,
	kCmd_QueueGoTrees,
	kCmd_QueueList,
	kCmd_QueueProgram,

	// Detritus	
	kCmd_NewTree,
	kCmd_SysActionulate,
	kCmd_Task,
	kCmd_SetPrefs,
	kCmd_SaveTree,
	kCmd_FileTree, 
	kCmd_RestoreTree,
	kCmd_Info,
	kCmd_KillNum,
	kCmd_KillPercent,
	kCmd_KillTrait,
	kCmd_CalcDiversity,
	kCmd_RunAndRestore,
	
	kCmd_XPruneSites,
	kCmd_DEBUG // for testing
};


#endif
// *** END ***************************************************************/

