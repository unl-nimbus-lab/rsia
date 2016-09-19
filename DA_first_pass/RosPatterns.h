/*
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/*
 * @author: Nishant Sharma
 */

//===- RosPatterns.h - Reachable Conditions Analysis for Source CFGs -*- C++ --*-//
//
//===----------------------------------------------------------------------===//
//
// This file implements Reachable Conditions analysis for source-level CFGs.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_ANALYSIS_ANALYSES_RosPatterns_H
#define LLVM_CLANG_ANALYSIS_ANALYSES_RosPatterns_H

#include "clang/AST/Decl.h"
#include "clang/Analysis/Analyses/ReachableConditions.h"
#include "clang/Analysis/AnalysisContext.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/ImmutableSet.h"
#include <llvm/Support/YAMLParser.h>
#include "string"
#include "vector"
#include <iostream>


using namespace std;

namespace clang
{
	class CFG;
	class CFGBlock;
	class Stmt;
	class DeclRefExpr;
	class SourceManager;

//for reading compiled node information

	struct TopicRemap{
		string from;
		string to;
	};

	struct RosNode{
		string name;
		string ns;
		string package;
		string type;
		string args;
		vector<TopicRemap> remaps;
	};


	class RosPatterns : public ManagedAnalysis
	{
	public:

		virtual ~RosPatterns() override ;

		/// Compute the reachable conditions information for a given CFG.
		static RosPatterns *computeRosPatterns(AnalysisDeclContext &analysisContext);

		/// writes to different files in the /tmp directory
		void dump(const SourceManager& M){};


		static RosPatterns* create(AnalysisDeclContext &analysisContext) {
			return computeRosPatterns(analysisContext);
		}

		static const void *getTag();

	private:
		RosPatterns(void *impl);
		void *impl;
	};


} // end namespace clang

#endif
