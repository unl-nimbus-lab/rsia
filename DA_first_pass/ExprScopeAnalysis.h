/*
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/**
 * \file
 *
 * \brief Interface for detecting a local variable's dependence on global or class member variables
 *
 * This file provides interface for extracting information about local variables. Weather they depend
 * on a global variable, a class member variable or not.
 *
 * Case Examples: \n
 * <b>Case 1:</b> A variable is dependent on the input parameters. It will return it's scope as <b> \c local </b> \n
 * <b>Case 2:</b> A variable is defined inside a function and is instantiated locally. Return Scope: <b> \c local </b> \n
 * <b>Case 3:</b> A variable is dependent on a function's (global or class member function) return value. Return Scope: <b> \c global </b> \n
 * <b>Case 4:</b> A variable is dependent on another variable that's global or class member variable. Return Scope: <b> \c global </b> \n
 *
 * \author Nishant Sharma
 *
 * \version 0.5
 *
 * \date 19th June, 2016
 *
 * Contact: nsharma@cse.unl.edu
 */

#ifndef LLVM_CLANG_ANALYSIS_ANALYSES_ExprScopeAnalysis_H
#define LLVM_CLANG_ANALYSIS_ANALYSES_ExprScopeAnalysis_H

#include "clang/AST/Decl.h"
#include "clang/Analysis/AnalysisContext.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/ImmutableSet.h"
#include "string"

using namespace std;

namespace clang {

	class CFG;
	class CFGBlock;
	class Stmt;
	class DeclRefExpr;
	class SourceManager;


	class ExprScopeAnalysis : public ManagedAnalysis {
	public:


		enum Scope{
			///to define that the variable is local only
					LOCAL,
			/// to define that the variable might have been modified outside or using outside infromation
					GLOBAL
		};

		class LocalScopeValues {
		public:
			/**
			 * \brief to store the results for the block terminator expressions
			 *
			 * blocks terminator expression are conditions of if, while, for, do, swtich
			 */
			std::map<const Expr *, Scope> localExpr;
			/**
			 * \brief to store the results for every VarDecl defined in the function and the function parameters
			 *
			 * to clarify, function parameters are assumed to be local variables
			 *
			 */
			std::map<const VarDecl *, Scope> localVarDecl;

			LocalScopeValues() {};

			LocalScopeValues(std::map<const Expr *, Scope> localExpr,
						   std::map<const VarDecl *, Scope> localVarDecl)
					: localExpr(localExpr), localVarDecl(localVarDecl) {};

			bool equals(const LocalScopeValues &localScopeValues) const;


			bool isLocal(const Expr *expr) const;
			bool isLocal(const VarDecl *varDecl) const;

			friend class ExprScopeAnalysis;
		};

		~ExprScopeAnalysis() override;

		/// Compute the Localness information for a given CFG.
		static ExprScopeAnalysis *computeLocalness(AnalysisDeclContext &analysisContext);

		/// Return true if a variable is local at the end of a
		/// specified block.
		bool isLocal(const CFGBlock *block, const VarDecl *varDecl);
		bool isLocal(const CFGBlock *block, const Expr *expr);

		/// Print to stderr the localness information associated with
		/// each basic block.
		void dumpBlockLocalness(const SourceManager& sourceManager);
		/// Print to stderr the localness information associated with
		/// each basic block to the input file.
		void dumpBlockLocalness(const SourceManager& sourceManager, string filename);

		static ExprScopeAnalysis *create(AnalysisDeclContext &analysisContext) {
			return computeLocalness(analysisContext);
		}

		static const void *getTag();

	private:
		ExprScopeAnalysis(void *implementation);
		void *implementation;
	};

} // end namespace clang



#endif//LLVM_CLANG_ANALYSIS_ANALYSES_ExprScopeAnalysis_H

